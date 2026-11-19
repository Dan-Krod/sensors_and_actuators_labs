#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PIN_BUTTON     12  
#define PIN_SENSOR     13  
#define PIN_GREEN_LED  14  
#define PIN_BLUE_LED   16  
#define OLED_SDA       4   
#define OLED_SCL       5   

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool systemActive = false;
bool buttonState = HIGH;
bool lastButtonReading = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

String currentSysMode = "";
String currentSensorStatus = "";

void updateDisplayIfNeeded(String sysMode, String sensorStatus);

void setup() {
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);

  pinMode(PIN_GREEN_LED, OUTPUT);
  pinMode(PIN_BLUE_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  pinMode(PIN_SENSOR, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  updateDisplayIfNeeded("OFF", "READY");
}

void loop() {
  bool curReading = digitalRead(PIN_BUTTON);

  if (curReading != lastButtonReading){
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay){
    if (curReading != buttonState){
      buttonState = curReading;

      if (buttonState == LOW){
        systemActive = !systemActive;
      }
    }
  }
  lastButtonReading = curReading;

  String displaySysMode = "";
  String displaySensorStatus = "";

  if (systemActive){
    displaySysMode = "ON";

    bool isObstacle = (digitalRead(PIN_SENSOR)==LOW);

    if (isObstacle){
      digitalWrite(PIN_GREEN_LED, LOW);
      digitalWrite(PIN_BLUE_LED, HIGH);
      displaySensorStatus = "OBSTACLE!";
    } else {
      digitalWrite(PIN_GREEN_LED, HIGH);
      digitalWrite(PIN_BLUE_LED, LOW);
      displaySensorStatus = "CLEAR";
    }
  } else {
    digitalWrite(PIN_GREEN_LED, LOW);
    digitalWrite(PIN_BLUE_LED, HIGH); 
    displaySysMode = "OFF";
    displaySensorStatus = "READY";
  }

  updateDisplayIfNeeded(displaySysMode, displaySensorStatus);
}

void updateDisplayIfNeeded(String sysMode, String sensorStatus) {
  if (sysMode == currentSysMode && sensorStatus == currentSensorStatus) {
    return; 
  }

  currentSysMode = sysMode;
  currentSensorStatus = sensorStatus;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("NULP | Group IP-34"); //
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  
  display.setTextSize(2);
  display.setCursor(0, 15);
  display.print("SYS: ");
  display.println(sysMode);
  
  display.drawLine(0, 35, 128, 35, SSD1306_WHITE);
  
  display.setCursor(0, 43);
  display.println(sensorStatus);
  display.display();
}
