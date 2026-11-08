#include <DHT.h>
#include <Arduino.h>
#include <math.h>
#include <Wire.h>
#include <U8g2lib.h>

class dhtSensor {
  private: 

   const uint8_t sensorPin;
   const uint8_t sensorType;
  DHT dht;

  public:
  dhtSensor(uint8_t pin, uint8_t type) : sensorPin(pin), sensorType(type), dht(pin, type) {}

  void init() { dht.begin(); }

  float humidityReading() { return dht.readHumidity(); }

  float temperatureCelsiusReading() { return dht.readTemperature(); }
   
  float temperatureFahrenheitReading() { return dht.readTemperature(true); }

  void verifyReadings(float h, float t, float f) { if (isnan(h) || isnan(t) || isnan(f)) { Serial.println(F("Failed to read")); return; } } 

                                                  // STRINGS FOR OLED
                                                
  String humidityVal() { return "Humidity Level: " + String(humidityReading()); }
  String tempCelsiusVal() { return "Celsius Level: " + String(temperatureCelsiusReading()); }
  String tempFahrenheitVal() { return "Fahrenheit Level: " + String(temperatureFahrenheitReading()); }

};

struct tempReadings {
  float _cHumidity;
  float _cTempCelsius;
  float _cTempFahrenheit;
};

void printDhtVal(const tempReadings &_data) {

  Serial.print(F("Current Humidity: "));
   Serial.print(_data._cHumidity);

  Serial.print(F(" Current Celsius: "));
   Serial.print(_data._cTempCelsius);

  Serial.print(F(" Current Fahrenheit: "));
   Serial.println(_data._cTempFahrenheit);

}

class oled {
  private:

  static const uint8_t width = 128;
  static const uint8_t height = 64;

  U8G2_SSD1306_128X64_NONAME_1_HW_I2C oled_ssd1306;

  public:
  
  oled() : oled_ssd1306(U8G2_R0, U8X8_PIN_NONE /*Reset Pin*/) {} 

  void init() { oled_ssd1306.begin(); }

  void test() { 
    
    oled_ssd1306.firstPage();
    do {

      oled_ssd1306.setFont(u8g2_font_6x10_tf);
      oled_ssd1306.drawStr(32, 16, "Hello, World!");
    
    } while( oled_ssd1306.nextPage() );


   }

   void displayData(dhtSensor const &sensor) { 
    String humStr = sensor.humidityVal();
    String celStr = sensor.tempCelsiusVal();
    String fahStr = sensor.tempFahrenheitVal();

    oled_ssd1306.firstPage();
    do {
      oled_ssd1306.setFont(u8g2_font_6x10_tf); 

      int humX = (width - oled_ssd1306.getStrWidth(humStr.c_str())) / 2;
      oled_ssd1306.drawStr(humX, 18, humStr.c_str());

  
      int celX = (width - oled_ssd1306.getStrWidth(celStr.c_str())) / 2;
      oled_ssd1306.drawStr(celX, 36, celStr.c_str());

      oled_ssd1306.setFont(u8g2_font_5x7_tf); 


      int fahX = (width - oled_ssd1306.getStrWidth(fahStr.c_str())) / 2;
      oled_ssd1306.drawStr(fahX, 54, fahStr.c_str());
    
    } while( oled_ssd1306.nextPage() ); 
  }

};
class buzzer {
  private:
   uint8_t buzzerPin;
   uint8_t state = 0;

  public:
   buzzer(uint8_t pin) { buzzerPin = pin; }

   void init() { pinMode(buzzerPin, OUTPUT); }

   void on() { digitalWrite(buzzerPin, HIGH); }
   void off() { digitalWrite(buzzerPin, LOW); }

};

dhtSensor DHT(2, DHT11);
oled Oled;
buzzer Buzzer(5);
void setup() {
  Buzzer.init();
  DHT.init();
  Oled.init();
  Serial.begin(9600);

}

void loop() {


  tempReadings tempreadings;
  float Humidity = DHT.humidityReading();
  float tempCelsius = DHT.temperatureCelsiusReading();
  float tempFahrenheit = DHT.temperatureFahrenheitReading();

  DHT.verifyReadings(Humidity, tempCelsius, tempFahrenheit);

  tempreadings._cHumidity = Humidity;
  tempreadings._cTempCelsius = tempCelsius;
  tempreadings._cTempFahrenheit = tempFahrenheit;

  printDhtVal(tempreadings);
  Buzzer.off();

  Oled.displayData(DHT);

  delay(2000);
  Buzzer.on();

}
