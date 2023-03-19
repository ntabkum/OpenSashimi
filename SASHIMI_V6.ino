//CURRENT VERSION 6 SASHIMI - 03/15/2022
//
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <DHT.h>
#include <PubSubClient.h>


BlynkTimer T1, T2, T3, T4, T5;

bool reset1 = false;
bool reset2 = false;
bool flag1 = false;
bool flag2 = false;
bool flag3 = false;
bool flag4 = false;
bool firstBoot=true;
bool g1_open, g2_open;

int gpi5_d1 = 5;  //16, 5  to Relay 1 
int gpi4_d2 = 4;  //2, 4 to Relay 2
int gpi14_d5 = 14; //from magnetic sw1 
int gpi12_d6 = 12; //from magnetic sw2
String displaytime;
String displaydate;  
#define DHTPIN 13       // What digital pin we're connected
#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321

DHT dht(DHTPIN, DHTTYPE);

WidgetLCD lcd1(V0);
WidgetLCD lcd2(V15);
// WidgetBridge bridge2(V20);
WidgetRTC rtc;  // real time clock

char auth[] = "5bc4da55826f4dd2b745a6ace2ef29d3"; // home VM n.tabkum@gmail.com
// char auth[] = "820226fcd81e4346941e5fa3303f331d"; // home VM ntabkum@gmail.com
char ssid[] = "IOTDEVICE2";
char pass[] = "1e2d3c4b5a";

WidgetLED led1(V1); // ack 1 LED  Assign virtual pin 1 to widget led1 
WidgetLED led2(V2); // open LED
WidgetLED led3(V3); // close LED
WidgetLED led4(V4); // ack 2 LED
WidgetLED led5(V5); // open LED
WidgetLED led6(V6); // close LED

int counter3 = 0;
int counter4 = 0;
IPAddress sashimi_ip ( 172,  17,   18,  101);
IPAddress dns_ip     (  8,   8,   8,   8);
IPAddress gateway_ip ( 172,  17,   18,   1);
IPAddress s_mask(255, 255, 255,   0);

// #define KEY "bA2LqeAMQFEi8XCuaHgI6f"  
// #define EVENT_NAME "FRONT LIGHTS" 
WiFiClient esp1;
PubSubClient client(IPAddress(172,17,18,100),1964, esp1);

void setup() {
     pinMode(5, OUTPUT);
     pinMode(4, OUTPUT);
     pinMode(2, OUTPUT);
     digitalWrite(gpi5_d1, HIGH); // to relay 1 initial off mode
     digitalWrite(gpi4_d2, HIGH); // to relay 2 initial off mode
     pinMode(gpi14_d5, INPUT_PULLUP); // from contact sensor 1
     pinMode(gpi12_d6, INPUT_PULLUP); // from contact sensor 2
    // Serial.begin(115200);
      WiFi.mode(WIFI_STA);
     // Blynk.begin(auth, ssid, pass, IPAddress(68,111,139,135), 8080); // 68,111,139,135 | 
      WiFi.config(sashimi_ip, s_mask, gateway_ip, dns_ip); 
      WiFi.begin(ssid, pass);
      
      Blynk.config(auth, IPAddress(172,17,18,100), 8080); 
      
      while (Blynk.connect(1000) == false) { 
                  } 
     lcd1.clear();
     lcd2.clear();
     dht.begin();
     setSyncInterval(3600); // Sync interval in seconds (every hour)
  // Setup a function to be called (get temp, humidity) every 10 second
    T1.setInterval(10000L, sendSensor);
    T2.setInterval(30000L, lcd2Display);
    T3.setInterval(1000L, blinkLED2);
    T4.setInterval(1000L, v7count);
    T5.setInterval(1000L, v8count);
    
          }
  BLYNK_CONNECTED() 
  {
    // Blynk.syncAll();
    Blynk.syncVirtual(V11, V12);  // this will put zero/  initialize V11, V12
    rtc.begin(); // Synchronize time on connection
    
  }          

void blinkLED2() { 
  digitalWrite(2, !digitalRead(2));  
             }
             
BLYNK_WRITE(V9) // reset counter 1 / V7
       { int pinData = param.asInt();
         if (pinData == 1) { reset1 = true; }
       }  
BLYNK_WRITE(V10) // reset counter 2 / V8
       { int pinData = param.asInt();
         if (pinData == 1) { reset2 = true; }
       }  
        
       
BLYNK_WRITE(V11)        
       { int pinData = param.asInt();
        if (pinData == 1) {  
        Serial.println(pinData);
        digitalWrite(gpi5_d1, LOW);
        led1.on(); //acknowledge led on
        delay(250); // keep d1 pin low for 200ms for relay to engage
        digitalWrite(gpi5_d1, HIGH); // release relay 
        led1.off(); // acknowledge led off
        Blynk.virtualWrite(V11, 0); // clear push button
        
         }
        }
        
BLYNK_WRITE(V12)
      { int pinData = param.asInt();
        if (pinData == 1) {
         Serial.println(pinData);  
         digitalWrite(gpi4_d2, LOW);
         led4.on();
         delay(250); 
         digitalWrite(gpi4_d2, HIGH);
         led4.off();
         Blynk.virtualWrite(V12, 0); // clear push button
         
       }
      }
/* BLYNK_WRITE(V31) {
  float pinData = param.asFloat();
  // Serial.println(pinData);
  Blynk.virtualWrite(V16, pinData);
  
}
BLYNK_WRITE(V32) {
  float pinData = param.asFloat();
  // Serial.println(pinData);
  Blynk.virtualWrite(V17, pinData);
  
}      
BLYNK_WRITE(V33) {
  float pinData = param.asFloat();
  // Serial.println(pinData);
  Blynk.virtualWrite(V18, pinData);
  
}
BLYNK_WRITE(V34) {
  float pinData = param.asFloat();
  // Serial.println(pinData);
  Blynk.virtualWrite(V19, pinData);
  
}
*/      
void g1g2Led(){
    //Routine for garage 1
    if (firstBoot){
                  String B = WiFi.localIP().toString();
                  gettime();
                  getdate();
                  String A = "Power Up!" + String(" @ ") + displaytime + "  " + displaydate + " IP: " + B;
                  Blynk.notify(A);
                  firstBoot=false;
                  } 
    if (digitalRead(gpi14_d5)) { 
        led2.on(); led3.off(); reset1 = false; g1_open = true; 
        
        Blynk.virtualWrite(V7, counter3);
        if (counter3 % 500 == 0 || counter3 == 1) // notify when door first open and notify if door left open every 500 counts (approx 5 minutes)
                              {    
              gettime();
              String A = "Garage 1 Open!!!" + String("   ==>  ") + displaytime;
              client.publish("Garage_L","on"); 
              Blynk.notify(A);
              
              
                              }
        if (!flag1) { 
              flag1=true; 
              flag3=false;
              lcd1.print(0, 0, "Garage 1 Opened");
              }          
      } // V2 led on - garage 1 open
        else { 
              g1_open = false;
              led2.off(); if (reset1) {Blynk.virtualWrite(V7, 0); reset1 = false;}
              
              if (!flag3){
              led3.on();lcd1.print(0, 0, "Garage 1 Closed");
              gettime();
              String A = "Garage 1 Closed!!!" + String("   ==>  ") + displaytime;
              client.publish("Garage_L","off"); 
              Blynk.notify(A); 
              flag1=false; 
              flag3=true;
              
                        }
         counter3 = 0;
              } // V3 led on - garage 1 close
    // Routine for garage 2
    if (digitalRead(gpi12_d6)) 
        { 
        led5.on(); led6.off(); reset2 = false; g2_open = true;
        
        Blynk.virtualWrite(V8, counter4);
        if (counter4 % 500 == 0 || counter4 == 1) 
                              {
        gettime();
        String A = "Garage 2 Open!!!" + String("   ==>  ") + displaytime;
        client.publish("Garage_R","on");
        Blynk.notify(A);
                              }
        if (!flag2) {
            flag2=true; 
            flag4=false;
            lcd1.print(0, 1, "Garage 2 Opened"); 
            
                      }
        }
      else { 
            led5.off(); 
            led6.on();
            
            
            if (reset2) {Blynk.virtualWrite(V8, 0); reset2 = false;}
        if (!flag4)
            {
              lcd1.print(0, 1, "Garage 2 Closed"); 
              client.publish("Garage_R","off"); 
              
              gettime();
              String A = "Garage 2 Closed!!!" + String("   ==>  ") + displaytime;
              Blynk.notify(A); 
              flag2=false; 
              flag4=true;
              
              
            }
              
         counter4 = 0;
           }
    if (!digitalRead(gpi5_d1)) {led1.on();} // read pin d1 for state
      else {led1.off();}
    if (!digitalRead(gpi4_d2)) {led4.on();} // read pin d2 for state
      else {led4.off();}
    }     

void sendSensor()  // get temp and humidity data
    {
        float h = dht.readHumidity();
        float t = dht.readTemperature(true); // or dht.readTemperature(true) for Fahrenheit

    if (isnan(h) || isnan(t)) 
          {
    // Serial.println("Failed to read from sensor!");
            return;
          } 
      Blynk.virtualWrite(V13, t);
      Blynk.virtualWrite(V14, h);
    }

void lcd2Display()
{ 
  gettime();
  
  lcd2.print(0, 0, "Time: "+ displaytime);

  getdate();
  
  lcd2.print(0, 1, "Date: "+ displaydate);
   
}
void gettime() 
{
int gmthour = hour();
  if (gmthour == 24){
     gmthour = 0;
  }
  String displayhour =   String(gmthour, DEC);
  int hourdigits = displayhour.length();
  if(hourdigits == 1){
    displayhour = "0" + displayhour;
  }
  String displayminute = String(minute(), DEC);
  int minutedigits = displayminute.length();  
  if(minutedigits == 1){
    displayminute = "0" + displayminute;
  }  
  /*  
  String displaysecond = String(second(), DEC);
  int seconddigits = displaysecond.length();  
  if(seconddigits == 1){
  displaysecond = "0" + displaysecond;
  }    
  */
  
  displaytime = displayhour + ":" + displayminute; //+ ":" + displaysecond;
  
}
void getdate() 
{
String displaymonth =   String(month(), DEC);
  int monthdigits = displaymonth.length();
  if(monthdigits == 1){
    displaymonth = "0" + displaymonth;
  }
  String displayday = String(day(), DEC);
  int daydigits = displayday.length();  
  if(daydigits == 1){
    displayday = "0" + displayday;
  }  
    
   displaydate = displaymonth + "/" + displayday + "/" + year();
}

void v7count() {
  if(g1_open) {
    counter3++;
  }
}
void v8count() {
  if(g2_open) {
    counter4++;
  }
  
}
void mqtt() {
    if (client.connected())
      {client.loop();}
      else
      {client.connect("NAVA2");
      }
      
}


void loop() 
  { 
  Blynk.run();
  T1.run();
  T2.run();
  T3.run();
  T4.run();
  T5.run();
  g1g2Led(); 
  mqtt();         
  }
