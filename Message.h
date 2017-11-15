#ifndef DEF_MESSAGE
#define DEF_MESSAGE

class Message{
	public:
		static char contenu[65];
		static unsigned char longueur;
    
		void printMessage();
    char joinResponseOK_getID();
    
    void joinRequest();
    void joinResponseOK(char deviceID);
    void joinResponseNotOK();
    void unknownDeviceResponse();
    void dataInformation(char data);

    bool isjoinRequest();
    bool isjoinResponseOK();
    bool isjoinResponseNotOK();
    bool isunknownDeviceResponse();
    bool isdataInformation();
};    

#endif     
