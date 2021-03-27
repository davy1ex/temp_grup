/*

/temp - кидает жсон с 'time', 'date', 'temp'  
 
*/

#include <ArduinoJson.h>

#include <SPI.h>
#include <Ethernet.h>

#include <OneWire.h>
#include <DallasTemperature.h>


#define ONE_WIRE_BUS      8
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 0, 78);
EthernetServer server(80);
String readString;


void setup() {
  sensors.begin(); // start temp sensor
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Ethernet WebServer: Hello");

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();

  //------------------------------блок обработки данных датчика температуры--------------------------------------------
  sensors.requestTemperatures();
  int Temp = sensors.getTempCByIndex(0);
  //-----------------------------в итоге в переменной Temp хранится целое число с показателем температуры------------ 
  
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100) {

          //store characters to string
          readString += c;
          //Serial.print(c);
          
        }
        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        
        if (c == '\n' ) {
          if(readString.indexOf("/temp") >0)//checks for temp
          {
            StaticJsonDocument<200> doc;
            doc["time"] = __TIME__;
            doc["date"] = __DATE__;
            doc["temp"] = Temp;            
            
            client.println(F("HTTP/1.0 200 OK"));
            client.println(F("Content-Type: application/json"));
            client.println(F("Connection: close"));
            client.print(F("Content-Length: "));
            client.println(measureJsonPretty(doc));            
            client.println();
            
            serializeJsonPretty(doc, client);
            
          }
          // send a standard http response header
//          client.println("HTTP/1.1 200 OK");
//          client.println("Content-Type: text/html");
//          client.println("Connection: close");  // the connection will be closed after completion of the response
//          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
//          client.println();
//          client.println("<!DOCTYPE HTML>");
//          client.println("<html>");
//          client.println("</html>");
          break;
        }
        if (c == '\n') {
          Serial.println(readString);
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
