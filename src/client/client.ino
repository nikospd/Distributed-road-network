#include <RF22.h>
#include <RF22Router.h>
#include <SPI.h>

int sensor_pin = 7;
int id = random(1, 100);
RF22Router rf22; // my broadcast object
int freq_up = 437;
int freq_down = 438;
int max_receice_time = 4000;
void setup() {
  Serial.begin(9600);
  if (!rf22.init()) {
    Serial.println("RF22 init failed");
  } // begin the rf22
  rf22.setTxPower(RF22_TXPOW_17DBM);
  if (!rf22.setModemConfig(RF22::FSK_Rb125Fd125)) {
    Serial.println("Modulation Fail");
  }
  int gps_flag = 0;
}

void loop() {
  int ok = LOW; // HIGH for a pothole, LOW for a clean road.
  int lo = 0;   // 1 if the server is ready to receive my message, 0 if not.
  int ret = 0;  // 1 if the server sent a new information, 0 if not
  phase_1_A();
  // Serial.print("my id inside the loop is : ");
  Serial.println(id);
  // delay(2000);
  delay(50);
  ret = phase_1_B();
  if (ret) {
    Serial.println("The server told me that there is a pothole here");
  }

  ok = digitalRead(sensor_pin);
  // TODO: a function for better sensing
  // TODO: read coordinates from gps. Send them on phase_2 for the network to be
  // able to map the potholes.
  if (ok) {
    // if (1){
    phase_2_A();
    lo = phase_2_B();
  }

  delay(900);
}
void phase_1_A() {
  Serial.println("1_A");
  if (!rf22.setFrequency(freq_up)) {
    Serial.println("setFrequency failed in phase_1_A");
  }
  uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
  char data[] = "hello, i am inside your range";
  memcpy(data_send, data, RF22_ROUTER_MAX_MESSAGE_LEN);
  if (rf22.sendtoWait(data_send, sizeof(data_send), RF22_BROADCAST_ADDRESS) !=
      RF22_ROUTER_ERROR_NONE) {
    Serial.println("sendtoWait failed in phase_1_A (for the first message)");
  }
  rf22.waitPacketSent();
  Serial.println("i sent the insert message, now i will send my id");
  sprintf(data, "%d", id);
  memcpy(data_send, data, RF22_ROUTER_MAX_MESSAGE_LEN);
  delay(50);
  if (rf22.sendtoWait(data, sizeof(data), RF22_BROADCAST_ADDRESS) !=
      RF22_ROUTER_ERROR_NONE) {
    Serial.println("sendtoWait failed in phase_1_A (for the id)");
  }
  rf22.waitPacketSent();
  Serial.print("my id inside the phase is : ");
  Serial.println(id);
}

int phase_1_B() {
  Serial.println("1_B");
  int ret = 0;
  if (!rf22.setFrequency(freq_down)) {
    Serial.println("setFrequency failed in phase_1_B");
  }
  uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  char ack1[RF22_ROUTER_MAX_MESSAGE_LEN];
  sprintf(ack1, "%s", "I already sent you the coordinates");
  char ack2[RF22_ROUTER_MAX_MESSAGE_LEN];
  sprintf(ack2, "%s", "Sorry mate, i have nothing for you");
  char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
  // rf22.waitAvailable();
  if (rf22.recvfromAckTimeout(buf, &len, max_receice_time)) {
    if (strcmp((char *)buf, ack1) == 0) {
      Serial.println("I already have this information");
    } else if (strcmp((char *)buf, ack2) == 0) {
      Serial.println("He has nothing to send me");
    } else {
      memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
      ret = atoi((char *)incoming);
    }
  } else {
    Serial.println("Receive failed in phase_1_B");
  }
  return ret;
}

void phase_2_A() {
  Serial.println("2_A");
  if (!rf22.setFrequency(freq_up)) {
    Serial.println("setFrequency failed in phase_2_A");
  }
  char data[] = "i have something for you! A new pothole";
  uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
  memcpy(data_send, data, RF22_ROUTER_MAX_MESSAGE_LEN);
  if (rf22.sendtoWait(data_send, sizeof(data_send), RF22_BROADCAST_ADDRESS) !=
      RF22_ROUTER_ERROR_NONE) {
    Serial.println("sendtoWait failed in phase_2_A");
  }
  rf22.waitPacketSent();
  Serial.println("i told him that i have an information!");
}

int phase_2_B() {
  Serial.println("2_B");
  int lo = 0;
  if (!rf22.setFrequency(freq_down)) {
    Serial.println("setFrequency failed in phase_2_B");
  }
  uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  char ack[RF22_ROUTER_MAX_MESSAGE_LEN];
  sprintf(ack, "%s", "I am ready to receive your message!");
  if (rf22.recvfromAckTimeout(buf, &len, max_receice_time)) {
    if (strcmp((char *)buf, ack) == 0) {
      Serial.println("He will accept my message! I am gonna send it now!");
      lo = 1; // Just for the phace_2_C
    }
  } else {
    Serial.println("For some reasson he can not accept my message");
  }
  return lo;
}
