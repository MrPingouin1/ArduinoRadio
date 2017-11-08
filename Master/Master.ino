#include <Message.h>
#include <RFM69.h>
#include <SPI.h>

// Output
#define SERIAL_BAUD 115200
#define RFM69_RST 4
#define LED 13

#define RFM69_CS 8
#define RFM69_IRQ 7
#define IS_RFM69HCW true
#define RFM69_IRQN 4

#define NETWORKID 17 
#define NODEID 1 
#define FREQUENCY RF69_433MHZ // RF69_868MHZ RF69_915MHZ
#define ENCRYPTKEY "sampleEncryptKey"

int16_t packetnum = 0;
RFM69 radio = RFM69(RFM69_CS, RFM69_IRQ, IS_RFM69HCW, RFM69_IRQN);
Message message;

void setup() {
  while (!Serial);

  //Start
  Serial.begin(SERIAL_BAUD);
  Serial.println("Uart init done. Starting Init.");
  
  // Hard Reset the RFM module
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, HIGH);
  digitalWrite(RFM69_RST, LOW);
    
  radio.initialize(FREQUENCY,NODEID,NETWORKID); 
  if (IS_RFM69HCW)
    radio.setHighPower(); // Only for RFM69HCW & HW!    
  radio.setPowerLevel(31); // power output ranges from 0 (5dBm) to 31 (20dBm)
  radio.encrypt(ENCRYPTKEY);
  message.setRadio(&radio);
  
  pinMode(LED, OUTPUT);
  Serial.print("Radio Init done. Transmitting at ");
  Serial.print(FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(" MHz");
}


void loop() {
  static unsigned int loopCounter = 0;
  //static int receiver =  11; 

  message.receiveMessage();  
  
  //Tous les 2000 passages
  if (loopCounter > 2000)
  {
    packetnum ++; 
    loopCounter= 0;
  
    // On construit une trame arbitraire pour test
    message.radiopacket[0] = 01;
    message.radiopacket[1] = radio.readTemperature(4); 
    message.radiopacket[2] = packetnum >> 8;  // Les poids forts
    message.radiopacket[3] = packetnum ;      // Les poids faibles
    message.radiopacket[4] = 00;
    message.radiopacket[5] = 255;
  
    message.radiopacket[6] = 11;
    message.radiopacket[7] = 22;
    message.radiopacket[8] = 33;
    message.radiopacket[9] = 0;
    message.radiopacket[10] = 0;
    message.radiopacket[11] = 0;
    message.radioPacketLen = 15;

    message.sendMessage(11);
    message.printMessage();    
  }
  
  Serial.flush(); //make sure all serial data is clocked out before sleeping the MCU
  loopCounter ++;
  delay(1);
}

void blinkLED (byte PIN, byte DELAY_MS, byte loops)
{
  for (byte i=0; i<loops; i++)
  {
    digitalWrite(PIN,HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN,LOW);
    delay(DELAY_MS);
  }
}

