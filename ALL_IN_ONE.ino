

#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
int pin = 28;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
float maxcon = 0;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

IPAddress ip(192, 168, 1, 177); //공유기 ip


EthernetServer server(80);

void setup() {
  // Open serial communications and wait for port to open:
   pinMode(8,INPUT);
 starttime = millis();
 lcd.begin(16, 2);
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  int sen = analogRead(1);
   duration = pulseIn(pin, LOW);
 lowpulseoccupancy = lowpulseoccupancy+duration;
 if ((millis()-starttime) > sampletime_ms)
 {
 ratio = lowpulseoccupancy/(sampletime_ms*10.0); // Integer percentage 0=>100
 concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62;
 if (concentration > maxcon){ maxcon = concentration;}
  lcd.setCursor(0,0);
  lcd.print("CO2=");
  lcd.setCursor(4,0);
  lcd.print(sen);
  lcd.setCursor(7,0);
  lcd.print("ppm");
  lcd.setCursor(0,1);
   lcd.print("Dust=");
 lcd.setCursor(5,1);
 lcd.print(concentration);
 lcd.setCursor(9,1);
 lcd.print("ug/m3");
   delay(1000);
}
if(concentration>400){pinMode(13, HIGH);} //relay control for air cleaner
if(sen>400){pinMode(12, HIGH);}
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
         
            client.print("CO2= ");
  
            client.print(" is ");
            client.print(sen);
            client.print("PPM");

            client.print("Dust=");
             client.print(" is ");
            client.print(concentration);
            client.print("ug/m3");
            
          }
          client.println("</html>");
          break;
        }
        
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }


