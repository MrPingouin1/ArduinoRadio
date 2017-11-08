#ifndef DEF_RADIO
#define DEF_RADIO

#include <RFM69.h>
#include "Message.h"

class Radio{
  private:
    static RFM69 radio;
  public:
    void start(int nodeid);
    void printMessage();
    void blinkLED(byte PIN, byte DELAY_MS, byte loops);
    void sendMessage(Message message, int receiver);
    bool receiveMessage(int *sender, Message *message);
    int readTemperature();
};    

#endif    
