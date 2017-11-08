#include <Message.h>
#include <Arduino.h> 

char Message::radiopacket[65];
unsigned char Message::radioPacketLen;
        
void Message::printMessage(){
    Serial.print("Message : "); 
    for (int i=0; i< radioPacketLen; i++)
    {
        Serial.print((unsigned char) radiopacket[i], DEC); 
        Serial.print(",");
    }
    Serial.println("]");
}