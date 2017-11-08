#ifndef DEF_MESSAGE
#define DEF_MESSAGE

#include <RFM69.h>

class Message{
	public:
		static char radiopacket[65];
		static unsigned char radioPacketLen;
		static RFM69* radio;
		
		void setRadio(RFM69* r);
		void printMessage();
		void sendMessage(int receiver);
		void receiveMessage();
};    

#endif     