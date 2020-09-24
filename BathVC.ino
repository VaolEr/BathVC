/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on NodeMCU.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right NodeMCU module
  in the Tools -> Board menu!

  For advanced settings please follow ESP examples :
   - ESP8266_Standalone_Manual_IP.ino
   - ESP8266_Standalone_SmartConfig.ino
   - ESP8266_Standalone_SSL.ino

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include "PinDefines.h"
#include <GyverButton.h>

#include <Wire.h>
#include "Adafruit_SHT31.h"

// ------------------- ТИПЫ --------------------
BlynkTimer timer; // Announcing the timer
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);

// ----------------- ПЕРЕМЕННЫЕ ------------------
// ----- Режим управления вентилятором
byte Control_Mode; // 1 - Ручной, 2 - Автоматический

// -------- КНОПКА BLYNK --------
bool bButtonOldState;
int  iButtonOldState = 3;

bool bButtonState;
int  iButtonState = 0; // 0 - Вентилятор выключен, 10 - вентилятор включен

// -------- Датчик SHT31-DIS-B -------
Adafruit_SHT31 sht31 = Adafruit_SHT31();
bool sensorState = false;

// -------- ВЛАЖНОСТЬ -------
int ustavka_Humidity = 30;  // Порог срабатывания реле включения вентилятора, %
float Humidity = 60;          // Текущее значение влажности, полученное от датчика, %

// -------- ТЕМПЕРАТУРА -------
float Temperature = 60;       // Текущее значение температуры, полученное от датчика, С

// -------- КНОПКА -------
#define USE_BUTTON 1    // 1 - использовать кнопку, 0 - нет
bool TouchButtonOn = false;


// ----------------- Настрйоки WiFi ------------------
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "8In7YQfi6oSalH0cUWrEZJ-wuMTRY6Wu";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "netis_2.4G";
char pass[] = "Valentin";



int i = 0;
void myTimerEvent()
{
  checkSHT31connection();
  myI2C_Humidity_request();
  myHumidityControl();
  //if(i < 40) i++;
  //else i = 0; 
  Blynk.virtualWrite(Blynk_Temperature, Temperature);
  
  Blynk.virtualWrite(Blynk_Humidity, Humidity);

  Blynk.virtualWrite(Blynk_Manual_Vent_OnOff, iButtonState);
}

void myI2C_Humidity_request()
{
  /*Wire.requestFrom(8, 1); /* запрашиваем и считываем данные с 8 адреса slave устройства 1 байта
  while(Wire.available())
  {
    Serial.println("Wire available"); 
    Humidity = Wire.read();
  }
  Serial.println(Humidity);*/
  if(sensorState){ 
    Temperature = sht31.readTemperature();
    Humidity = sht31.readHumidity();
  
    if (! isnan(Temperature)) {  // check if 'is not a number'
      Serial.print("Temp *C = "); Serial.print(Temperature); Serial.print("\t\t");
    } else { 
      Serial.println("Failed to read temperature");
      Temperature = 111;
    }
    
    if (! isnan(Humidity)) {  // check if 'is not a number'
      Serial.print("Hum. % = "); Serial.println(Humidity);
    } else { 
      Serial.println("Failed to read humidity");
      Humidity = 111;
    }
  }
  else{
    Temperature = 222;
    Humidity = 222;
  }
}

void myHumidityControl()
{ 
    if(TouchButtonOn == true)
    { 
      Blynk.virtualWrite(Blynk_Touch_Indicator, 255);
      
      if(Control_Mode == ManualControlMode)
      {

      }
  
      if(Control_Mode == AutomaticControlMode)
      {
        if(Humidity >= ustavka_Humidity)
        {
          iButtonState = 10;
          digitalWrite(Vent_Relay, true);
          Blynk.virtualWrite(Blynk_Vent_ON_LED, 255);  
        }
  
        if(Humidity < ustavka_Humidity)
        {
          iButtonState = 0;
          digitalWrite(Vent_Relay, false);
          Blynk.virtualWrite(Blynk_Vent_ON_LED, 0);
        }
      }
    }
    if(TouchButtonOn == false)
    {
      digitalWrite(Vent_Relay, false);
      Blynk.virtualWrite(Blynk_Vent_ON_LED, 0);
      iButtonState = 0;
      Blynk.virtualWrite(Blynk_Touch_Indicator, 0);
    }
}

void buttonTick()
{
  touch.tick();
  if(USE_BUTTON)
  {
    if (touch.isSingle()) 
    {
      TouchButtonOn = !TouchButtonOn; 
    }
  }
  else
  {
    TouchButtonOn = true;
  }
}

void checkSHT31connection(){
  Serial.println("SHT31 test");
  sensorState = true;
  Blynk.virtualWrite(Blynk_SHT31_LED, 255);
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    sensorState = false;
    Blynk.virtualWrite(Blynk_SHT31_LED, 0);
    while (1) delay(1);
  }
}

void setup()
{
  //Wire.begin(D2,D1);
  
  pinMode(BTN_PIN, INPUT);
  touch.setStepTimeout(100);
  touch.setClickTimeout(500);
  
  // Debug console
  Serial.begin(115200);

  //Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  
  Blynk.begin(auth, ssid, pass, IPAddress(139,59,206,133), 8080); // Connect to Blynk Server

  timer.setInterval(4000L, myTimerEvent);
  
  pinMode(Vent_Relay, OUTPUT);      //Set the Relay Control pin mode to output
  digitalWrite(Vent_Relay, false);  //Set the Relay Control pin lvl to LOW

  Control_Mode = ManualControlMode;
  Blynk.virtualWrite(Blynk_Control_Mode, ManualControlMode);
  Blynk.setProperty(Blynk_Control_Mode, "color", "#23C48E");
}

BLYNK_WRITE(Blynk_Control_Mode)
{
  Control_Mode = param.asInt();
  switch (Control_Mode){
    case ManualControlMode:{
      Blynk.setProperty(Blynk_Control_Mode, "color", "#23C48E");
      Blynk.setProperty(Blynk_Manual_Vent_OnOff, "onBackColor", "#23C48E");
      Blynk.setProperty(Blynk_Manual_Vent_OnOff, "offBackColor", "#1F473D");}
    break;
    case AutomaticControlMode:{
      Blynk.setProperty(Blynk_Control_Mode, "color", "#04C0F8");
      Blynk.setProperty(Blynk_Manual_Vent_OnOff, "onBackColor", "#2C2D31");
      Blynk.setProperty(Blynk_Manual_Vent_OnOff, "onColor", "#17181B");
      Blynk.setProperty(Blynk_Manual_Vent_OnOff, "offColor", "#17181B");
      Blynk.setProperty(Blynk_Manual_Vent_OnOff, "offBackColor", "#2C2D31");}
    break;
  }
  myHumidityControl();
}

BLYNK_WRITE(Blynk_Manual_Vent_OnOff)   
{
  if(TouchButtonOn)
  {
    iButtonOldState = iButtonState;
  
    if(Control_Mode == ManualControlMode)
    { 
      iButtonState = param.asInt(); // read value from virtual pin
      if(iButtonState == 10) 
      {
        Blynk.virtualWrite(Blynk_Vent_ON_LED, 255);
        digitalWrite(Vent_Relay, true);
      }
      if(iButtonState == 0)
      {
        Blynk.virtualWrite(Blynk_Vent_ON_LED, 0);
        digitalWrite(Vent_Relay, false);
      }
    }
    if(Control_Mode == AutomaticControlMode)
    {
      Blynk.notify(String("Для независимого управления вентилятором смените режим на Ручной."));
      Blynk.virtualWrite(Blynk_Manual_Vent_OnOff, iButtonState);
    }
  }
  else
  {
    Blynk.notify(String("Включите работу устройства одиночным нажатием кнопки на корпусе устройства."));
  }
}

BLYNK_WRITE(Blynk_Humidity_Ustavka)
{
    ustavka_Humidity = param.asInt();
    //myHumidityControl();
}

void loop()
{
  Blynk.run();
  timer.run();
  buttonTick();
}
