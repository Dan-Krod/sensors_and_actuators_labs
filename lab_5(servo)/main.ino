#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#define PIN_BUTTON     12  
#define SERVO_PIN      2   
#define POT_PIN        A0  
#define PIN_GREEN_LED  14  
#define PIN_BLUE_LED   16  

#define OLED_SDA       4   
#define OLED_SCL       5   

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64

const int POT_MIN = 50;
const int POT_MAX = 1000;
const int ADC_DEADBAND = 30;       
const int SERVO_DEADBAND = 2;      
const float EMA_ALPHA = 0.05f;     

const unsigned long CONTROL_INTERVAL_MS = 10;  
const unsigned long DISPLAY_INTERVAL_MS = 150;  
const unsigned long DEBOUNCE_DELAY_MS   = 50;   

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Servo myServo;

bool systemActive = false;
bool standbyDrawn = false;

bool buttonState = HIGH;
bool lastButtonReading = HIGH;
unsigned long lastDebounceTime = 0;

int lastStableADC = 0;
float smoothedPot = 0.0f;
int lastServoAngle = -1;
float currentAlpha = 0.0f;
float currentKi = 0.0f;
int currentDisplayAngle = 0;

unsigned long lastControlMs = 0;
unsigned long lastDisplayMs = 0;

void handleButton();
void processControlLoop();
void updateDisplayActive(float alpha, float ki, int ang);
void displayStandby();

void setup() {
    Serial.begin(115200);
    Wire.begin(OLED_SDA, OLED_SCL);

    pinMode(PIN_GREEN_LED, OUTPUT);
    pinMode(PIN_BLUE_LED, OUTPUT);
    pinMode(PIN_BUTTON, INPUT_PULLUP);

    myServo.attach(SERVO_PIN, 500, 2500);
    myServo.write(90);
    lastServoAngle = 90;

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    lastStableADC = analogRead(POT_PIN);
    int initMapped = constrain(map(lastStableADC, POT_MIN, POT_MAX, 0, 1023), 0, 1023);
    smoothedPot = (float)initMapped;

    displayStandby();
    standbyDrawn = true;
}

void loop() {
    handleButton();
    unsigned long currentMillis = millis();

    if (systemActive) {
        standbyDrawn = false;
        digitalWrite(PIN_GREEN_LED, HIGH);
        digitalWrite(PIN_BLUE_LED, LOW);

        if (currentMillis - lastControlMs >= CONTROL_INTERVAL_MS) {
        lastControlMs = currentMillis;
        processControlLoop();
        }

        if (currentMillis - lastDisplayMs >= DISPLAY_INTERVAL_MS) {
        lastDisplayMs = currentMillis;
        updateDisplayActive(currentAlpha, currentKi, currentDisplayAngle);

        Serial.print(F("ADC: ")); Serial.print(lastStableADC);
        Serial.print(F(" | a: ")); Serial.print(currentAlpha, 2);
        Serial.print(F(" | Ki: ")); Serial.print(currentKi, 2);
        Serial.print(F(" | Ang: ")); Serial.println(currentDisplayAngle);
        }
    } else {
        digitalWrite(PIN_GREEN_LED, LOW);
        digitalWrite(PIN_BLUE_LED, HIGH);

        if (!standbyDrawn) {
        displayStandby();
        standbyDrawn = true;
        }
    }
}

void handleButton() {
    bool reading = digitalRead(PIN_BUTTON);

    if (reading != lastButtonReading) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY_MS) {
        if (reading != buttonState) {
        buttonState = reading;
        if (buttonState == LOW) {
            systemActive = !systemActive;
        }
        }
    }
    lastButtonReading = reading;
}

void processControlLoop() {
    int currentRawADC = analogRead(POT_PIN);

    if (abs(currentRawADC - lastStableADC) > ADC_DEADBAND) {
        lastStableADC = currentRawADC;
    }

    int mappedPot = map(lastStableADC, POT_MIN, POT_MAX, 0, 1023);
    mappedPot = constrain(mappedPot, 0, 1023);

    smoothedPot = (EMA_ALPHA * mappedPot) + ((1.0f - EMA_ALPHA) * smoothedPot);

    currentAlpha = smoothedPot / 1023.0f;
    currentKi = currentAlpha / (1.0f + 2.0f * (currentAlpha - currentAlpha * currentAlpha));

    currentDisplayAngle = constrain((int)(currentKi * 180.0f), 0, 180);

    int targetServoAngle = 180 - currentDisplayAngle;

    if (abs(targetServoAngle - lastServoAngle) >= SERVO_DEADBAND) {
        myServo.write(targetServoAngle);
        lastServoAngle = targetServoAngle;
    }
}

void updateDisplayActive(float alpha, float ki, int ang) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(15, 0);
    display.print(F("NULP: POT -> SERVO"));
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

    display.setCursor(0, 15);
    display.print(F("a:")); display.print(alpha, 2);
    display.setCursor(64, 15);
    display.print(F("Ki:")); display.print(ki, 2);

    display.setTextSize(2);
    display.drawLine(0, 27, 128, 27, SSD1306_WHITE);
    display.setCursor(0, 32);
    display.print(F("ANG: ")); display.print(ang); display.write(247); 

    int barWidth = map(ang, 0, 180, 0, 128);
    display.fillRect(0, 52, barWidth, 12, SSD1306_WHITE);
    display.display();
}

void displayStandby() {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(35, 20);
    display.print(F("READY"));

    display.setTextSize(1);
    display.drawLine(0, 45, 128, 45, SSD1306_WHITE);
    display.setCursor(32, 52);
    display.print(F("VAR 3 (0.5R)"));
    display.display();
}