#include "Message.h"
#include <Arduino.h>

char Message::contenu[65];
unsigned char Message::longueur;

void Message::printMessage(){
	Serial.print("-> Message : ["); 
	for (int i=0; i< longueur -1; i++)
	{
		Serial.print((unsigned char) contenu[i], DEC); 
		Serial.print(",");
	}
  Serial.print((unsigned char) contenu[longueur -1], DEC);
	Serial.print("]");
}

void Message::joinRequest(){
  contenu[0] = 118;
  contenu[1] = 218;
  longueur = 2;
}

bool Message::isjoinRequest(){
  return longueur == 2 && contenu[0] == (char) 118 && contenu[1] == (char) 218;
}

void Message::joinResponseOK(char deviceID){
  contenu[0] = 218;
  contenu[1] = deviceID;
  longueur = 2;
}

bool Message::isjoinResponseOK(){
  return longueur == 2 && contenu[0] == (char) 218 && contenu[1] != (char) 0;
}

char Message::joinResponseOK_getID(){
  if(isjoinResponseOK())
    return contenu[1];
  else
    return -1;
}

void Message::joinResponseNotOK(){
  contenu[0] = 218;
  contenu[1] = 0; 
  longueur = 2;  
}

bool Message::isjoinResponseNotOK(){
  return longueur == 2 && contenu[0] == (char) 218 && contenu[1] == (char) 0;
}

void Message::unknownDeviceResponse(){
  contenu[0] = 40;
  contenu[1] = 4; 
  longueur = 2;  
}

bool Message::isunknownDeviceResponse(){
  return longueur == 2 && contenu[0] == (char) 40 && contenu[1] == (char) 4;
}

void Message::dataInformation(char data){
  contenu[0] = 1;
  contenu[1] = data; 
  longueur = 2;
}

bool Message::isdataInformation(){
  return longueur == 2 && contenu[0] == (char) 1;
}


