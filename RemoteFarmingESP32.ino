
#include <Adafruit_AHTX0.h>
#include <HardwareSerial.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED width,  in pixels
#define SCREEN_HEIGHT 64 // OLED height, in pixels

// create an OLED display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Adafruit_AHTX0 aht;

const int Photocell = 4;
const int Valve = 18;
const int Cooler = 5;
const int Steam = 1;
const int Light = 23;
const int Heater = 13;
const String Phone = "+XXzzzzzzzzzz";
int Brightness;

boolean Automatic = true;
String Valveopen;



//Create software serial object to communicate with SIM800L
HardwareSerial sim800(2);  //SIM800C Tx & Rx is connected to ESP32 #16 & #17




void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit AHT10/AHT20 demo!");
  
  pinMode(Photocell, INPUT);
  pinMode(Cooler, OUTPUT);
  pinMode(Steam, OUTPUT);
  pinMode(Light, OUTPUT);
  pinMode(Valve, OUTPUT);
  pinMode(Heater, OUTPUT);
  


  if (!aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }

  // initialize OLED display with I2C address 0x3C
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("failed to start SSD1306 OLED"));
    while (1);
  }


  Serial.begin(115200);

  //Begin serial communication with Arduino and SIM800L
  sim800.begin(115200);

  Serial.println("Initializing...");
  delay(1000);

  sim800.println("AT");  //Once the handshake test is successful, it will back to OK
  updateSerial();

  sim800.println("AT+CMGF=1");  // Configuring TEXT mode
  updateSerial();
  sim800.println("AT+CNMI=1,2,0,0,0");  // Decides how newly arrived sms messages should be handled
  updateSerial();

  delay(2000);

  oled.clearDisplay(); // clear display
  oled.setTextSize(1);         // set text size
  oled.setTextColor(WHITE);    // set text color
  oled.setCursor(0, 20);       // set position to display
  oled.println("Hello, Welcome to:"); // set text
  oled.setCursor(0, 30);       // set position to display
  oled.println("ESP32 greenhouse control"); // set text
  oled.display();
  delay(3000);
}

void loop() {

// showInfo();

  if (Automatic) {
    
    Auto_control();
    //          Reply("Automatic control is active");
  } else {
    


    Manual_control();
    //          Reply("Manual control is active");
  }
}

void Auto_control() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);  // populate temp and humidity objects with fresh data
  Brightness = map(analogRead(Photocell),0,4095,0,100);
  
  //  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  //  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
  //  Serial.print("Light: "); Serial.print(Brightness); Serial.println(" LMS");

showInfo();
  

  if (temp.temperature * 100 > 2650) {
    digitalWrite(Cooler, HIGH);
    digitalWrite(Heater, LOW);
  } else if (temp.temperature * 100 < 2400) {
    digitalWrite(Cooler, LOW);
    digitalWrite(Heater, HIGH);
  } else {
    digitalWrite(Cooler, LOW);
    digitalWrite(Heater, LOW);
  }
  if (Brightness < 15) {
    digitalWrite(Light, HIGH);
  } else {
    digitalWrite(Light, LOW);
  }
  if (humidity.relative_humidity * 100 > 6500) {
    digitalWrite(Steam, HIGH);
  } else {
    digitalWrite(Steam, LOW);
  }
  if (millis() % 10000 <= 3000) {
      
    digitalWrite(Valve, HIGH);
    Valveopen = "Open";
  } else {
    digitalWrite(Valve, LOW);
    Valveopen = "Close";
  }
  

  while (sim800.available()) {
    String sms = sim800.readString();
    if (sms.indexOf(Phone) > -1) {
      if (sms.indexOf("Status") > -1) {
        show_SMS("Status");          
        Reply("Temp: " + String(temp.temperature) + " Celsius\n Humidity: " + String(humidity.relative_humidity) + " %\n " + "Light: " + String(Brightness) + " %\n Valve: " + Valveopen);

      } else if (sms.indexOf("Temp") > -1) {
        show_SMS("Temp");                  
        Reply("Temp: " + String(temp.temperature) + " Celsius");
      } else if (sms.indexOf("Humidity") > -1) {
        show_SMS("Humidity");                
        Reply("Humidity: " + String(humidity.relative_humidity) + " %");
      } else if (sms.indexOf("Brightness") > -1) {
        show_SMS("Brightness");
        Reply("Brightness: " + String(Brightness) + " %");
      } else if (sms.indexOf("Valve") > -1) {
        show_SMS("Valve");                  
        Reply("Valve: " + Valveopen);
      } else if (sms.indexOf("Manual") > -1) {
        show_SMS("Manual");
        Reply("Manual control is active");
        Automatic = false;
      }
    }
  }
}

void Manual_control() {
  showInfo();

  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);  // populate temp and humidity objects with fresh data
  Brightness = map(analogRead(Photocell),0,4095,0,100);
  
  while (sim800.available()) {
    String sms = sim800.readString();
    if (sms.indexOf(Phone) > -1) {
      if (sms.indexOf("CoolerON") > -1) {
        digitalWrite(Cooler, HIGH);
        show_SMS("CoolerON");
        Reply("Cooler is on");

      } else if (sms.indexOf("CoolerOFF") > -1) {
        digitalWrite(Cooler, LOW);
        show_SMS("CoolerOFF");
        Reply("Cooler is off");

      }else if(sms.indexOf("HeaterON") > -1){
        digitalWrite(Heater, HIGH);
        show_SMS("HeaterON");
        Reply("Heater is on");

      } else if(sms.indexOf("HeaterOFF") > -1){
        digitalWrite(Heater, LOW);
        show_SMS("HeaterOFF");
        Reply("Heater is off");

      }else if (sms.indexOf("SteamON") > -1) {
        digitalWrite(Steam, HIGH);
        show_SMS("SteamON");
        Reply("Steam is on");

      } else if (sms.indexOf("SteamOFF") > -1) {
        digitalWrite(Steam, LOW);
        show_SMS("SteamOFF");
        Reply("Steam is off");

      } else if (sms.indexOf("LightON") > -1) {
        digitalWrite(Light, HIGH);
        show_SMS("LightON");        
        Reply("Light is on");

      } else if (sms.indexOf("LightOFF") > -1) {
        digitalWrite(Light, LOW);
        show_SMS("LightOFF");
        Reply("Light is off");

      } else if (sms.indexOf("WaterON") > -1) {
        digitalWrite(Valve, HIGH);
        Valveopen = "Open"; 
        show_SMS("WaterON");       
        Reply("Valve is open");

      } else if (sms.indexOf("WaterOFF") > -1) {
        digitalWrite(Valve, LOW);
        Valveopen = "Close";
        show_SMS("WaterOFF");
        Reply("Valve is closd");

      } else if (sms.indexOf("AllON") > -1) {
        digitalWrite(Cooler, HIGH);
        digitalWrite(Steam, HIGH);
        digitalWrite(Light, HIGH);
        digitalWrite(Valve, HIGH);
        digitalWrite(Heater, HIGH);
        Valveopen = "Open";
        show_SMS("AllON");        
        Reply("All tools are on");

      } else if (sms.indexOf("AllOFF") > -1) {
        digitalWrite(Cooler, LOW);
        digitalWrite(Steam, LOW);
        digitalWrite(Light, LOW);
        digitalWrite(Valve, LOW);
        digitalWrite(Heater, LOW);
        Valveopen = "Close";
        show_SMS("AllOFF");
        Reply("All tools are off");

      }else if (sms.indexOf("Status") > -1) {
        show_SMS("Status");          
        Reply("Temp: " + String(temp.temperature) + " Celsius\n Humidity: " + String(humidity.relative_humidity) + " %\n " + "Light: " + String(Brightness) + " %\n Valve: " + Valveopen);

      } else if (sms.indexOf("Temp") > -1) {
        show_SMS("Temp");
        Reply("Temp: " + String(temp.temperature) + " Celsius");
      } else if (sms.indexOf("Humidity") > -1) {
        show_SMS("Humidity");
        Reply("Humidity: " + String(humidity.relative_humidity) + " %");
      } else if (sms.indexOf("Brightness") > -1) {
        show_SMS("Brightness");        
        Reply("Brightness: " + String(Brightness) + " %");
      } else if (sms.indexOf("Valve") > -1) {
        show_SMS("Valve");                  
        Reply("Valve: " + Valveopen);
      } 

      else if (sms.indexOf("Auto") > -1) {
        Automatic = true;
        show_SMS("Auto");        
        Reply("Automatic control is active");
      }
    }
  }
}

void updateSerial() {
  delay(500);
  while (Serial.available()) {
    sim800.write(Serial.read());  //Forward what Serial received to Software Serial Port
  }
  while (sim800.available()) {
    Serial.write(sim800.read());  //Forward what Software Serial received to Serial Port
  }
}
void Reply(String text) {
  sim800.println("AT+CMGF=1");
  updateSerial();
  sim800.println("AT+CMGS=\"+ZZxxxxxxxxxx\"");
  updateSerial();
  sim800.print(text);
  updateSerial();
  sim800.write(26);
}
void showInfo(){

sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);  // populate temp and humidity objects with fresh data
  Brightness = map(analogRead(Photocell),0,4095,0,100);
  
        
  oled.clearDisplay(); // clear display
  oled.setTextSize(1);         // set text size
  oled.setTextColor(WHITE);    // set text color
  oled.setCursor(0, 20);       // set position to display
  oled.println("Temp: " + String(temp.temperature) + " Celsius"); // set text
  oled.setCursor(0, 30);
  oled.println("Humidity: " + String(humidity.relative_humidity) + " %");   
  oled.setCursor(0, 40);
  oled.println("Brightness: " + String(Brightness) + " %");     
  oled.setCursor(0, 50);
  oled.println("Valve: " + Valveopen); 
  oled.display();              // display on OLED     
}

void show_SMS(String SMS){

oled.clearDisplay(); // clear display
oled.setTextSize(1);         // set text size
oled.setTextColor(WHITE);    // set text color
oled.setCursor(0, 20);       // set position to display
oled.println("SMS Recieved:"); // set text
oled.setCursor(0, 30);       // set position to display
oled.println(SMS); // set text
oled.display();  
delay(2500);
showInfo();
  
}
