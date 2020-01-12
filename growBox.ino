//Importing libraries
#include <Adafruit_NeoPixel.h> // library that allows control over the grow light
#include <DHT.h> //library for interpreting readings from the DHT humidity and temperature sensor

#ifdef __AVR__
#include <avr/power.h>
#endif
//#define sensorPin A0 //Moisture sensor pin
#define PIN 6 // grow light connected to pin 6
#define NUMPIXELS 256 // number of LEDs in the grow light
#define DHTPIN 7 // temperature sensor connected to pin 2
#define DHTTYPE DHT22 // type of the sensor
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(DHTPIN, DHTTYPE); //initialising of the temperature sensor

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); // initialising the grow light
#define DELAYVAL 500

int photocellPin = 0; //photocell pin connected to A0
const int sensorPin1 = A1; // soil moisture sensor 1 connected  to pin A1
const int sensorPin2 = A2; // soil moisture sensor 2 connected to pin A2
const int sensorPin3  = A3; // soil moisture sensor 3 connected to pin A3
const int sensorPin4 =  A4; // soil moisture sensor 4 connected to pin A4
float moisture = 0; //initialising the moisture variable
int photocellReading;
int temperature;
int humidity;
void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  Serial.begin(115200);
  dht.begin();
  pixels.begin();
  pinMode(2, OUTPUT);
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(sensorPin3, INPUT);
  pinMode(sensorPin4, INPUT);
}

void loop() {

  humidity = dht.readHumidity(); //readings from the DHT sensor for the humidity in the box
  temperature = dht.readTemperature(); //data from the DHT sensor for the temperature in the box
  updateDisplay(humidity, temperature); //calling the function to display the data
  photocellReading = analogRead(photocellPin); // readings from the light sensor/photoresistor
  adjustGrowLight(photocellReading); // calling the function to adjust the light with the data
  moisture = analogRead((sensorPin1 + sensorPin2 + sensorPin3 + sensorPin4) / 4); // readings from the moisture soil sensors and taking
  //calculating the average
  //if statement to check if the soil is wet or dry
  if (moisture > 500) {
    digitalWrite(2, HIGH); // if the moisture is over 500, which shows that is dry, switch on the relay/watering
  } else {
    digitalWrite(2, LOW); // otherwise switch the relay off/watering
  }

  //if statement to check the humidity and call the function for the warning whether it is above or below the required
  if (humidity < 40 ) {
    lowHumidityWarning();
  } else if (humidity > 98) {
    highHumidityWarning();
  }
  //if statement to check the temperature and call the function for the warning whether it is above or below the required
  if ( temperature < 22 ) {
    lowTemperatureWarning();
  } else if (temperature > 27) {
    highTemperatureWarning();
  }
}

// turns the led strip on with a given brightness
void lightsOn(int brightness) {
  pixels.setBrightness(brightness); \
  //turns on each LED of the grow light
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(200, 0, 255, 0));
  }
  pixels.show();
}

//turns off the grow light
void lightsOff() {
  //turns off each led of the grow light
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
}

//adjusting the brightness of the grow light depending on the light source in the room
void adjustGrowLight(int photocellReading) {
  if (photocellReading < 800 && photocellReading > 570) {
    lightsOn(10);
  } else if (photocellReading < 569 && photocellReading > 401) {
    lightsOn(25);
  } else if (photocellReading < 400 && photocellReading > 201) {
    lightsOn(30);
  } else if (photocellReading < 200) {
    lightsOn(40);
  } else {
    lightsOff();
  }
}

void printReadings(float humidity, float temperature) {
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C ");
  delay(1000);
}
//function that displays the temperature and humidity of the box
void updateDisplay(int humidity, int temperature) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Temp:");
  display.print(temperature);
  display.print((char)247);
  display.print("C");
  display.setCursor(0, 20);
  display.print("Hum:");
  display.print(humidity);
  display.println("%");
  display.display();
}

//temperature warning for temperature over 27 degrees
void highTemperatureWarning() {
  while (temperature > 27) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(40, 5);
    display.println("High");
    display.setCursor(30, 30);
    display.print("Temp!");
    display.display();
    photocellReading = analogRead(photocellPin);
    adjustGrowLight(photocellReading);
  }
}
//function that displays warning if the temperature is below 22 degrees
void lowTemperatureWarning() {
  while (temperature < 22) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(40, 35);
    display.println("Low");
    display.setCursor(30, 50);
    display.print("Temp!");
    display.display();
    photocellReading = analogRead(photocellPin);
    adjustGrowLight(photocellReading);
  }
}
//function that displays warning if the humidity is over 60%
void highHumidityWarning() {
  while (humidity > 60) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(40, 0);
    display.println("High");
    display.setCursor(15, 20);
    display.print("Humidity!");
    display.display();
    photocellReading = analogRead(photocellPin);
    adjustGrowLight(photocellReading);
  }
}
//function that displays warning if the humidity is below 40%
void lowHumidityWarning() {
  while (humidity < 40) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(40, 0);
    display.println("Low");
    display.setCursor(15, 15);
    display.print("Humidity!");
    display.display();
    photocellReading = analogRead(photocellPin);
    adjustGrowLight(photocellReading);
  }
}
