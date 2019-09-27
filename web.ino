#define SS_PIN 4  //D2
#define RST_PIN 3 //D1

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <ESP8266WiFi.h>

const char* ssid = "N Y A Y";
const char* password = "as1234df";
WiFiServer server(80);

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int statuss = 0;
int out = 0;

Servo servo;

void setup() 
{
  Serial.begin(115200);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  servo.attach(2); //D4
  servo.write(0); 
  delay(2000);

  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
}

int rfid()
{
  //Show UID on serial monitor
  //Serial.println();
  //Serial.print(" UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  if (content.substring(1) == "A6 45 2E 1A " ) //change UID of the card that you want to give access
  {
    servo.write(90);
    delay(3000);
    servo.write(0);
    delay(1000);
    statuss = 1;
    return 1;
  }
  else
  {
    return 0;
  }
}

void loop() 
{
  int chck;
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  // Listenning for new clients
  WiFiClient client = server.available();
  if (client)
  {
    Serial.println("New client");
    // bolean to locate when the http request ends
    boolean blank_line = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        if (c == '\n' && blank_line)
        {
          //Serial.print(content);
          client.println();
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          // your actual web page that displays temperature
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head></head><body><h1>ESP8266 - Auto Door Lock</h1>");
          chck = rfid();
          if(chck == 1)
          {
            client.println("<h3> Access Granted");
            client.println("</h3><h3>Welcome Mr.Ankit ");
            client.println("</h3><h3>Have FUN ");
            client.println("</h3></body></html>");
            client.println();
            break;
          }
          else
          {
            client.println("<h3>Access Denied ");
            client.println("</h3></body></html>");
            break;
          }
        }
        if (c == '\n')
        {
          // when starts reading a new line
          blank_line = true;
        }
        else if (c != '\r')
        {
          // when finds a character on the current line
          blank_line = false;
        }
      }
    }  
    // closing the client connection
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}
