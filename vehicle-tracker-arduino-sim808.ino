#include <SoftwareSerial.h>

char lat[12];
char lon[12];
char tosend[300];

#define PIN_TX 10
#define PIN_RX 13
SoftwareSerial mySerial(PIN_TX,PIN_RX);

void setup() {
  // put your setup code here, to run once:
 mySerial.begin(9600);
    Serial.begin(9600);
    Serial.println("Starting...");
    delay(5000);
    //init
    while(!sendATcommand("AT", "OK", 2000)){ }

    Serial.println("init GPS...");
   
    //attach gps
    while (!sendATcommand("AT+CGNSPWR=1", "OK", 2000)){}
    while (!sendATcommand("AT+CGPSPWR=1", "OK", 2000)){}

    Serial.println("init GPS done.");

    //register on network (+CREG: 0,1 is home network, +CREG: 0,5 is roaming)
    while (!sendATcommand("AT+CREG?", "+CREG: 0,1", 2000)){}

    //check connection status
    while (!sendATcommand("AT+CMEE=1", "OK", 2000)){}
    delay(10000);
    Serial.println("Setup done.");
}

void loop() {
  // put your main code here, to run repeatedly:

  delay(2000);
  getgps();
  delay(2000);
  sendgps();
  delay(5000);
}

void sendgps()
{

    //shut connections
    while (!sendATcommand("AT+CIPSHUT", "OK", 2000)){}

    //Attaching to network
    while (!sendATcommand("AT+CGATT=1", "OK", 2000)){}

    //Setting up APN for TCP connection
    while (!sendATcommand("AT+CSTT=\"mobilinternet.upc.hu\"", "OK", 2000)){}

    //Bring up GPRS Connection
    while (!sendATcommand("AT+CIICR", "OK", 2000)){}

    //get ip
    sendATcommand("AT+CIFSR", "OK", 2000);

    //TCP connection
    while (!sendATcommand("AT+CIPSTART=\"TCP\",\"18.196.102.49\",\"80\"", "CONNECT OK", 5000)){}

    //TCP send
    //sendATcommand("AT+CIPSEND", "SEND OK", 2000);
    while (!sendATcommand("AT+CIPSEND", ">", 2000)){}

    //itemid is the name of the tracked item, pass is the password to write new location to the database
    snprintf(tosend, sizeof(tosend), "GET /gps/%s/%s/itemid/itempass HTTP/1.1\r\nHost: 18.196.102.49\r\nUser-Agent: aws-sdk-go\r\n\r\n", lat, lon);
    sendATcommand(tosend, "SEND OK", 2000);
}

void getgps()
{
    char response[300];
    int i = 0;
    char *token[300];
    unsigned long previous;
    unsigned int timeout = 2000;
    uint8_t x=0;
    previous = millis();

        mySerial.println("AT+CGNSINF");    // Send the AT command

         while(((millis() - previous) < timeout)){
              if(mySerial.available() != 0){
                  // if there are data in the UART input buffer, reads it and checks for the asnwer
                  response[x] = mySerial.read();
                  //Serial.print(response[x]);
                  x++;
            }
        }
        response[x] = '\0';

        token[0] = strtok(response, ",");
        while (token[i] != NULL) {
            i++;
            token[i] = strtok(NULL, ",");
        }

        strcpy(lat, token[3]);
        strcpy(lon, token[4]);
        Serial.println(lat);
        Serial.println(lon);
}

  int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialize the string

    delay(100);

    while( mySerial.available() > 0) mySerial.read();    // Clean the input buffer

    mySerial.println(ATcommand);    // Send the AT command
    Serial.println(ATcommand);

    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        if(mySerial.available() != 0){
            // if there are data in the UART input buffer, reads it and checks for the asnwer
            response[x] = mySerial.read();
            x++;
            // check if the desired answer  is in the response of the module
            if (strstr(response, expected_answer) != NULL)
            {
                answer = 1;
            }
        }
         // Waits for the asnwer with time out
    }while((answer == 0) && ((millis() - previous) < timeout));
     Serial.println("---RESPONSE START---");
    Serial.println(response);
    Serial.println("---RESPONSE END---");
    return answer;
}
