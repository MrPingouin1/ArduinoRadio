
#include <RFM69.h> //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>
#include <Message.h>

#define NETWORKID 17 // The same on all nodes that talk to each other
//#define NODEID 11 // The unique identifier of this node
#define FREQUENCY RF69_433MHZ // RF69_868MHZ RF69_915MHZ
#define ENCRYPTKEY "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HCW true // set to 'true' if you are using an RFM69HCW module
#define SERIAL_BAUD 115200
#define RFM69_CS 8
#define RFM69_IRQ 7
#define RFM69_IRQN 4 // Pin 7 is IRQ 4!
#define RFM69_RST 4
#define LED 13 // onboard blinky

int16_t packetnum = 0; // packet counter, we increment per xmission
RFM69 radio = RFM69(RFM69_CS, RFM69_IRQ, IS_RFM69HCW, RFM69_IRQN);

uint8_t node_id = 11; // node ID de base
uint8_t maitre_id; // le maitre du node

void setup() {
  while (!Serial); // wait until serial console is open, remove if not tethered to computer. Delete this line on ESP8266
  Serial.begin(SERIAL_BAUD);
  Serial.println("Uart init done. Starting Init.");
  // Hard Reset the RFM module
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, HIGH);
  delay(100);
  digitalWrite(RFM69_RST, LOW);
  delay(100);
  
  radio.initialize(FREQUENCY,node_id,NETWORKID);
  if (IS_RFM69HCW)
    radio.setHighPower(); // Only for RFM69HCW & HW!
    
  radio.setPowerLevel(31); // power output ranges from 0 (5dBm) to 31 (20dBm)
  radio.encrypt(ENCRYPTKEY);
  pinMode(LED, OUTPUT);
  Serial.print("Radio Init done. Transmitting at ");
  Serial.print(FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(" MHz");
  Serial.println(radio.getFrequency());
  //chercherMaitre();
}

bool chercherMaitre() {
  maitre_id = 1;
  Message mess;
  static unsigned int loopCounter;
  bool succes;
  do {
    loopCounter = 0;
    delay(1000);
    mess.radiopacket[0] = 255; 
    mess.radioPacketLen = 1;

    mess.printMessage();
    sendMessage(mess, maitre_id);

    do{
      succes = receiveMessage() && radio.SENDERID == maitre_id;
      if (succes){
        // Le maitre contacter nous a répondu
        Serial.print("Reponse du maitre :");
        afficherRep();
      }
      Serial.flush(); //make sure all serial data is clocked out before sleeping the MCU
      loopCounter ++;
      delay(1);
    } while (loopCounter < 2000);
    

    if (!succes){
      maitre_id ++;
    }
  } while (maitre_id <10 && !succes);
  maitre_id =1;
  return succes;
}


void loop() {
  Message mess;
  static unsigned int loopCounter = 0;

  //static int receiver =  11; // The recipient of packets

  receiveMessage();
  
  //Tous les 2000 passages
  if (loopCounter > 2000)
  {
    packetnum ++; 
    loopCounter= 0;
  
    //==== On construit une trame arbitraire pour test
    mess.radiopacket[0] = 01;
    mess.radiopacket[1] = radio.readTemperature(4); 
    mess.radiopacket[2] = packetnum >> 8;  // Les poids forts
    mess.radiopacket[3] = packetnum ;      // Les poids faibles
    mess.radiopacket[4] = 00;
    mess.radiopacket[5] = 255;
  
    mess.radiopacket[6] = 11;
    mess.radiopacket[7] = 22;
    mess.radiopacket[8] = 33;
    mess.radiopacket[9] = 0;
    mess.radiopacket[10] = 0;
    mess.radiopacket[11] = 0;
    mess.radioPacketLen = 15;

    sendMessage(mess, maitre_id);
    mess.printMessage();    
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

void sendMessage (Message message, int receiver){
  if (radio.sendWithRetry(receiver, message.radiopacket, message.radioPacketLen)) 
  { 
    Serial.print("OK > ");
  }
  else
  {
    Serial.print("FAIL > ");        
  }
  blinkLED(LED, 40, 1); //blink LED 3 times, 50ms between blinks 
}

bool receiveMessage (){
  bool ret = radio.receiveDone();
  if (ret)
  {
    Serial.print("Avant ACK : ");
    afficherRep();
    Serial.print(F("Received Frame "));
    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.println("Sending ACK");
    }
    else 
    {
      Serial.println("NO ACK"); 
    }
    Serial.print("Après ACK : ");
    afficherRep();
    
    blinkLED(LED, 40, 3);  
  } 
  return ret;
}

void afficherRep(){
  Serial.print("Réponse recu de ");
  Serial.println(radio.SENDERID);
  Serial.print("Message : "); 
  for (int i=0; i< radio.DATALEN -1; i++){
      Serial.print((unsigned char) radio.DATA[i], DEC); 
      Serial.print(",");
  }
  Serial.println((unsigned char) radio.DATALEN -1, DEC);
}




