# Distributed-road-network

Arduino code for a Distributed road network with purpose to inform the drivers about the road condition.

The server.ino has to be uploaded to each static ground station and the client.ino to each vehicle.
We use the Arduino uno and the RFM22 shield to communicate at the ISM band.
The communication protocol that shown below is custom and use 2 phases.  


  +---------+                                                    +---------+
 |   Client  |                    Phase 1                       |   Server  |
	+---------+                                                    +---------+
	     |                                                              |
	     |  Send broadcast verification message                         |
	     |------------------------------------------------------------->|                                                            
	     |                                                              |
	     |                Send information data (if doesn't had before) |
	     |<-------------------------------------------------------------|
	     |                                                              |
	     |  Confirm or else the server repeat the mission               |
	     |------------------------------------------------------------->|
	     |                                                              |

This phase occurs periodically as the client is trying to find a server and get informed about the road.






  +---------+                                                    +---------+
 |  Server   |                      Phase 2                     |   Client  |
  +---------+                                                    +---------+
     	 |                                                              |
     	 |                                    Send information data     |
     	 |<-------------------------------------------------------------|                                                            
     	 |                                                              |
     	 |  Confirm or else the client repeat the mission               |
     	 |<-------------------------------------------------------------|
     	 |                                                              |

This phase occurs when the a client find a pothole that doesn't know about before.

TODO: update the communication protocol as this one drains a lot of energy from the Client. The goal is to have low energy consumption at the clients because they will be mobile.  
