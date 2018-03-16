#include <SPI.h>
#include <RF22.h>
#include <RF22Router.h>

RF22Router rf22; // The broadcast object.
int freq_up = 437;
int freq_down = 438;
int max_receice_time = 4000;
int id_count = 0;
int gps_flag = 1; //TODO: not flag but the real coordinates in a 2x1 vector.
int id[50];

int temp = 0;
void setup() {
  Serial.begin(9600);
  if (!rf22.init()){Serial.println("RF22 init failed");} // Starting rf22
  rf22.setTxPower(RF22_TXPOW_17DBM);
  if (!rf22.setModemConfig(RF22::FSK_Rb125Fd125)){Serial.println("Modulation Fail");}
  for (int i=0;i<50;i++){
    id[i] = 0;
  }
  
  
}

void loop() { 
  if (!rf22.setFrequency(freq_up)){Serial.println("setFrequency failed in loop");}
  int id_temp = 0;//To save the id of the client
  int temp2 = 0;//1 if i sent before the same message to a client, 2 if i dont have an information, 0 otherwise
  int lo = 0;//1 if the client send an information, 0 otherwise

  //TODO: the id_temp, temp2 and lo, must be vectors for the network to serve multiple clients at the same time. 
  
  char ack1[RF22_ROUTER_MAX_MESSAGE_LEN];
  sprintf(ack1,"%s","hello, i am inside your range");
  char ack2[RF22_ROUTER_MAX_MESSAGE_LEN];
  sprintf(ack2,"%s","i have something for you! A new pothole");
  char ack3[RF22_ROUTER_MAX_MESSAGE_LEN];
  sprintf(ack3,"%s","Sorry mate, i have nothing for you");
  uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
  if(rf22.recvfromAckTimeout(buf,&len,max_receice_time)){
    if (strcmp((char*)buf , ack1) == 0){
      memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
      Serial.println("i had this message");
      Serial.println(incoming);
      id_temp = phase_1_A();
      Serial.print("the id of the user is :  ");
      Serial.println(id_temp);
      Serial.print("the id_count is : ");
      Serial.println(id_count);
      if (!id_count){
        id[id_count] = id_temp;
        id_count ++;      
      }
      
      else{
        for (int i = 0;i<id_count;i++){
          if (id[i] == id_temp){temp2 = 1;}
        }
      }
      if (!gps_flag){temp2 = 2;}
      Serial.print("the temp2 is : ");
      Serial.println(temp2);
      phase_1_B(temp2,gps_flag);  
      
    }
    else if (strcmp((char*)buf , ack2) == 0){
      lo = phase_2_A();
    }
    else{Serial.println("Something wrong happened here!"); }
    if (lo){
      phase_2_B();
    }
  }
  else{
    Serial.println("recvfromAckTimeout failed in Loop");
  }
  delay(1000);
}

int phase_1_A(){
  Serial.println("1_A");
  int id1 = 0;
  uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
  if (rf22.recvfromAckTimeout(buf,&len,max_receice_time)){
    memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
    id1 = atoi((char*)incoming);
    Serial.println(id1);
  }
  else {
    Serial.println("recvfromAckTimeout failed in phase_1_A");
  }
  return id1;
}


void phase_1_B(int temp2, int gps_flag){
  
  Serial.println("1_B");
  if (!rf22.setFrequency(freq_down)){Serial.println("setFrequency failed in phase_1_B");}
  uint8_t data_send1[RF22_ROUTER_MAX_MESSAGE_LEN];
  uint8_t data_send2[RF22_ROUTER_MAX_MESSAGE_LEN];
  char data1[] = "I already sent you the coordinates";
  char data2[] = "Sorry mate, i have nothing for you";
  memcpy(data_send1,data1,RF22_ROUTER_MAX_MESSAGE_LEN);
  memcpy(data_send2,data2,RF22_ROUTER_MAX_MESSAGE_LEN);
  switch (temp2){
    case 0:
      char data_read[RF22_ROUTER_MAX_MESSAGE_LEN];
      uint8_t data_send3[RF22_ROUTER_MAX_MESSAGE_LEN];
      sprintf(data_read, "%d", gps_flag);
      memcpy(data_send3,data_read,RF22_ROUTER_MAX_MESSAGE_LEN);
      if (rf22.sendtoWait(data_send3,sizeof(data_send3),RF22_BROADCAST_ADDRESS) != RF22_ROUTER_ERROR_NONE){Serial.println("sendtoWait failed in phase_1_B (gia to alt)");}
      rf22.waitPacketSent();
      Serial.println("i sent him the flag of the gps");
    case 1:
      if (rf22.sendtoWait(data_send1,sizeof(data_send1),RF22_BROADCAST_ADDRESS) != RF22_ROUTER_ERROR_NONE){Serial.println("sendtoWait failed in phase_1_B (gia to minima pou ton exw ksana)");}
      rf22.waitPacketSent();
      Serial.println("i allready sent him once");
    case 2:
      if (rf22.sendtoWait(data_send2,sizeof(data_send2),RF22_BROADCAST_ADDRESS) != RF22_ROUTER_ERROR_NONE){Serial.println("sendtoWait failed in phase_1_B (gia to minima pou den exw data)");}
      rf22.waitPacketSent();
      Serial.println("i have no information for him");
  }
}



int phase_2_A(){
  Serial.println("2_A");
  int lo = 0;
  Serial.println("He has something for me. A new pothole!!! :)");
  lo = 1;
  return lo;
}

void phase_2_B(){
  Serial.println("2_B");
  if (!rf22.setFrequency(freq_down)){Serial.println("setFrequency failed in phase_2_B");}
  char data[] = "I am ready to receive your message!";
  uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
  memcpy(data_send, data, RF22_ROUTER_MAX_MESSAGE_LEN);
  if (rf22.sendtoWait(data_send,sizeof(data_send),RF22_BROADCAST_ADDRESS) != RF22_ROUTER_ERROR_NONE){Serial.println("sendtoWait failed in phase_2_B ");}
  rf22.waitPacketSent();
}



