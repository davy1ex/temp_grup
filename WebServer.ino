#include<SPI.h>
#include <Ethernet.h>

#include <OneWire.h>
#include <DallasTemperature.h>


#define ONE_WIRE_BUS      8

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "api.pushingbox.com";   //YOUR SERVER
IPAddress ip(192, 168, 137, 177);
EthernetClient client;     


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

byte id_temp[3][3];
byte i;
int j=0;


// the setup function runs once when you press reset or power the board
void setup(){
    sensors.begin(); // start temp sensor
    Serial.begin(9600);

    Serial.println("Sensor started");
    SPI.begin();
    
    if (Ethernet.begin(mac) == 0) {
      Serial.println("Failed to configure Ethernet using DHCP");
      Ethernet.begin(mac, ip);
    }

    delay(1000);
    Serial.println("connecting...");
 }

// the loop function runs over and over again forever
void loop(){
    int Temp = sensors.getTempCByIndex(0);
    Sending_To_spreadsheet(Temp);
    delay(25*60*1000);
}

 void Sending_To_spreadsheet(int temp)   {
    if (client.connect(server, 80)) {
        Serial.println("Sending request");
        // Make a HTTP request:
        client.print("GET /pushingbox?devid=<devid>&time=");     //YOUR URL
        client.print(__TIME__);
        client.print("&temp=");
        client.print(temp);
        client.print(" ");      //SPACE BEFORE HTTP/1.1
        client.print("HTTP/1.1");
        client.println();
        client.println("Host: api.pushingbox.com");
        client.println("Connection: close");
        client.println();
    } 
    
    else {
        // if you didn't get a connection to the server:
        Serial.println("connection failed");
    }
 }
