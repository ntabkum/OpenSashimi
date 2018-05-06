//  Created by Nava - Garage doors IOT "Open Sashimi" // Version 3 final
//  3/1/2018

// #define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <DHT.h>
#include <DHT_U.h>
BlynkTimer T1, T2, T3, T4;


bool reset1 = false;
bool reset2 = false;
bool flag1 = false;
bool flag2 = false;
bool flag3 = false;
bool flag4 = false;
bool firstBoot=true;
int gpi5_d1 = 5;  // Relay 1 
int gpi4_d2 = 4;  // Relay 2
int gpi14_d5 = 14; //from magnetic sw1 
int gpi12_d6 = 12; //from magnetic sw2

#define DHTPIN 13       // Data
#define DHTTYPE DHT22   // AM2320

DHT dht(DHTPIN, DHTTYPE);

WidgetLCD lcd1(V0);
WidgetLCD lcd2(V15);

WidgetRTC rtc;  // real time clock

char auth[] = "your blynk auth code";
char ssid[] = "your SSID";
char pass[] = "your passphrase";

WidgetLED led1(V1); // ack 1 LED  
WidgetLED led2(V2); // open LED
WidgetLED led3(V3); // close LED
WidgetLED led4(V4); // ack 2 LED
WidgetLED led5(V5); // open LED
WidgetLED led6(V6); // close LED
// int counter1 = 0;
// int counter2 = 0;
int counter3 = 0;
int counter4 = 0;

void setup() {
     pinMode(2, OUTPUT); // LED on board
     pinMode(16, OUTPUT); //LED on board
     pinMode(5, OUTPUT);
     pinMode(4, OUTPUT);
     digitalWrite(gpi5_d1, HIGH); // to relay 1 initial off
     digitalWrite(gpi4_d2, HIGH); // to relay 2 initial off
     pinMode(gpi14_d5, INPUT_PULLUP); // from contact sensor 1
     pinMode(gpi12_d6, INPUT_PULLUP); // from contact sensor 2
     
     Blynk.begin(auth, ssid, pass, IPAddress(172,27,0,100), 8080); // 68,111,139,135 | 10,0,1,100 | 172,27,0,100
     
     lcd1.clear();
     dht.begin();
    setSyncInterval(3600); // Sync interval in seconds (every hour)
 
    T1.setInterval(10000L, sendSensor);
    T2.setInterval(1000L, lcd2Display);
    T3.setInterval(1000L, LED16);
    T4.setInterval(30000L, CHECKBLYNK);
    }
          
  BLYNK_CONNECTED() {
    // Blynk.syncAll();
    Blynk.syncVirtual(V11, V12);  // this will put zero/  initialize V11, V12
    rtc.begin(); // Synchronize time on connection
    
                    }          
void CHECKBLYNK() {
              bool A = Blynk.connected();
              if (A == false){
                LED0();
                delay(500);
                LED0();
                delay(500);
              }
}
void LED16() { 
  digitalWrite(2, !digitalRead(2));  
             }

void LED0() { 
  digitalWrite(16, !digitalRead(16));  
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
        // Serial.println(pinData);
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
         // Serial.println(pinData);  
         digitalWrite(gpi4_d2, LOW);
         led4.on();
         delay(250); 
         digitalWrite(gpi4_d2, HIGH);
         led4.off();
         Blynk.virtualWrite(V12, 0); // clear push button
         
       }
      }
void g1g2Led(){
    //Routine for garage 1
    if (firstBoot){
                  
                  String currentTime = String(hour()) + ":" + minute() + ":" + second();
                  String currentDate = String(month()) + "/" + day() + "/" + year();
                  String B = WiFi.localIP().toString();
                  String A = "Power Up!" + String(" @ ") + currentTime + "  " + currentDate + " IP: " + B;
                  Blynk.notify(A);
                  firstBoot=false;
                  
                  } 
    if (digitalRead(gpi14_d5)) { 
        led2.on(); led3.off(); reset1 = false;
        Blynk.virtualWrite(V7, ++counter3);
        if (counter3 % 300 == 0 || counter3 == 1) // notify when door first open and notify if door left open every 300 counts (approx 5 minutes)
                              {    
              String currentTime = String(hour()) + ":" + minute() + ":" + second();
              String A = "Garage 1 Open!!!" + String("   ==>  ") + currentTime;
              Blynk.notify(A);
                              }
        if (!flag1) { 
              
              lcd1.print(0, 0, "Garage 1 Opened");
              flag1=true; 
              flag3=false;} 
            
             
      } // V2 led on - garage 1 open
        else { 
              led2.off(); if (reset1) {Blynk.virtualWrite(V7, 0); reset1 = false;}
              if (!flag3){
              led3.on();lcd1.print(0, 0, "Garage 1 Closed");
              String currentTime = String(hour()) + ":" + minute() + ":" + second();
              String A = "Garage 1 Closed!!!" + String("   ==>  ") + currentTime;
              Blynk.notify(A); 
              flag1=false; 
              flag3=true;
                        }
         counter3 = 0;
              } // V3 led on - garage 1 close
    // Routine for garage 2
    if (digitalRead(gpi12_d6)) 
        { 
        led5.on(); led6.off(); reset2 = false;
        Blynk.virtualWrite(V8, ++counter4);
        if (counter4 % 300 == 0 || counter4 == 1) 
                              {
        String currentTime = String(hour()) + ":" + minute() + ":" + second();
        String A = "Garage 2 Open!!!" + String("   ==>  ") + currentTime;
        Blynk.notify(A);
                              }
        if (!flag2) {
            
            lcd1.print(0, 1, "Garage 2 Opened");
            flag2=true; 
            flag4=false;
                      }
        }
      else { 
            led5.off(); 
            led6.on();
            if (reset2) {Blynk.virtualWrite(V8, 0); reset2 = false;}
        if (!flag4)
            {
              lcd1.print(0, 1, "Garage 2 Closed");
              String currentTime = String(hour()) + ":" + minute() + ":" + second();
              String A = "Garage 2 Closed!!!" + String("   ==>  ") + currentTime;
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
    // Serial.println("Failed to read from DHT sensor!");
            return;
          } 
      Blynk.virtualWrite(V13, t);
      Blynk.virtualWrite(V14, h);
    }

BLYNK_WRITE(V31) {
  float t = param.asFloat();
  Blynk.virtualWrite(V31, t);
  
}
BLYNK_WRITE(V32) {
  float h = param.asFloat();
  Blynk.virtualWrite(V32, h);
  
}

void lcd2Display()
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
  String displaysecond = String(second(), DEC);
  int seconddigits = displaysecond.length();  
  if(seconddigits == 1){
    displaysecond = "0" + displaysecond;
  }
  String displaytime = displayhour + ":" + displayminute + ":" + displaysecond;
  // Blynk.virtualWrite(0, displaycurrenttime);
  lcd2.print(0, 0, "Time: "+ displaytime);

  
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
    
  String displaydate = displaymonth + "/" + displayday + "/" + year();
  lcd2.print(0, 1, "Date: "+ displaydate);
   
}


void loop() { 
  
  Blynk.run();
  T1.run();
  T2.run();
  T3.run();
  T4.run();
  g1g2Led();          
  
  }



