// ESP8266 pins defines //
#define Vent_Relay  D0
//------------------//
#define I2C_SCL     D1
#define I2C_SDA     D2
#define I2C_addr    8
//------------------//
#define BTN_PIN     D8
//------------------//
#define SPI_SCK     D5
#define SPI_MISO    D6
#define SPI_MOSI    D7
#define SPI_CS      D8
//------------------//

// Blynk Virtual Pins defines //
#define Blynk_Control_Mode      V3 // Select the Control Mode ( Manual or Automatic )
#define Blynk_Temperature       V6 // Temperature Gauge value
#define Blynk_Humidity          V5 // Humidity Gauge value
#define Blynk_Vent_ON_LED       V0 // Ventilator ON OFF LED value
#define Blynk_SHT31_LED         V4 // SHT31 sensor status LED value
#define Blynk_Humidity_Ustavka  V2 // Humidity on/off ventilation ustavka
#define Blynk_Manual_Vent_OnOff V1 // Manual ventilator relay control
#define Blynk_Touch_Indicator	 V10 // Manual ventilator relay control

#define ManualControlMode 1
#define AutomaticControlMode 2
