#ifndef DEF_MESSAGE
#define DEF_MESSAGE

class Message{
    public:
        static char radiopacket[65];
        static unsigned char radioPacketLen;
        
        void printMessage();
};    

#endif