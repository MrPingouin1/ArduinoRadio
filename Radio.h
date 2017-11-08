#ifndef DEF_RADIO
#define DEF_RADIO

#include <RFM69.h>
#include "Message.h"

class Radio{
  private:
    static RFM69 radio;
  public:
    void start(int nodeid);
    bool chercherMaitre();
    void printMessage();
    void blinkLED(byte PIN, byte DELAY_MS, byte loops);
    void sendMessage(Message message, int receiver);
    uint8_t receiveMessage(Message *message);
    int readTemperature();
    bool isJoinRequest(Message message);
};    

#endif    
