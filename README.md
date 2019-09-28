# Token-Ring-between-three-ECUs-using-FreeRTOS
CAN protocol is used between the three ECUs to communicate with each others. ECU should enter startup state when power is turned on.
•	Each ECU has identifier (Node ID 1, 2 and 3).
•	Each ECU has led to turn ON when receive the token for 1 sec. 
•	Each ECU transmit token PDU which contains the source node address and destination address.
•	Tiva C Cortex M-4 is used.
