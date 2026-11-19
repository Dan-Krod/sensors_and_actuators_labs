#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PIN_BUTTON     12  
#define TRIG_PIN       13  
#define ECHO_PIN       2   
#define PIN_GREEN_LED  14 
#define PIN_BLUE_LED   16 
#define PIN_ORANGE_LED 0  

#define OLED_SDA       4   
#define OLED_SCL       5  

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool systemActive = false;

float airTemp = 20.0;
float soundCreepCmUs = 0;

const int SAMPLES = 10;
float readings[SAMPLES];
int readIndex = 0;
int validSampleCount = 0;
float total = 0;
float averageDistance = 0;

unsigned long lastMeasureTime = 0;
const unsigned long MEASURE_INTERVAL = 60; 

bool lastButtonState = HIGH;
unsigned long buttonPressTime = 0;
bool isButtonPressed = false;
const unsigned long LONG_PRESS_THRESHOLD = 1000;

void updateSoundSpeed();
float getRawDistance();
void addReadingToFilter(float newRawDistance);
void updateDisplayActive();
void displayStandby();

void setup(){
    Serial.begin(115200);
    Wire.begin(OLED_SDA, OLED_SCL);

    pinMode(PIN_GREEN_LED, OUTPUT);
    pinMode(PIN_BLUE_LED, OUTPUT);
    pinMode(PIN_ORANGE_LED, OUTPUT);
    
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    for (int i = 0; i < SAMPLES; i++){
        readings[i] = 0;
    }

    updateSoundSpeed();

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
    }

    displayStandby();
}

void loop(){
    if (Serial.available() > 0){
        float newTemp = Serial.parseFloat();
        if (newTemp != 0.0 || Serial.read() == '0') { 
            airTemp = newTemp;
            updateSoundSpeed();
        }
    }

    bool currentButtonState = digitalRead(PIN_BUTTON);
    unsigned long currentMillis = millis();

    if (lastButtonState == HIGH && currentButtonState == LOW){
        buttonPressTime = currentMillis;
        isButtonPressed = true;
    }

    if (lastButtonState == LOW && currentButtonState == HIGH && isButtonPressed){
        isButtonPressed = false;
        unsigned long pressDuration = currentMillis - buttonPressTime;

        if (pressDuration >= LONG_PRESS_THRESHOLD) {
            if (airTemp < 10.0) airTemp = 20.0;
            else if (airTemp < 25.0) airTemp = 30.0;
            else airTemp = 0.0;
            updateSoundSpeed();
        } else if (pressDuration > 50) { 
            systemActive = !systemActive;
            
            if (systemActive) {
                total = 0;
                validSampleCount = 0;
                readIndex = 0;
            } else {
                displayStandby();
            }
        }
    }

    lastButtonState = currentButtonState;

    if (systemActive){
        digitalWrite(PIN_GREEN_LED);
        digitalWrite(PIN_BLUE_LED);

        if (currentMillis - lastMeasureTime >= MEASURE_INTERVAL){
            lastMeasureTime = currentMillis;

            float rawDist = getRawDistance();

            if (rawDist > 0) {
                digitalWrite(PIN_ORANGE_LED, HIGH); 
                addReadingToFilter(rawDist);
            } else {
                digitalWrite(PIN_ORANGE_LED, LOW);
            }

            updateDisplayActive();
        }
    } else {
        digitalWrite(PIN_GREEN_LED, LOW);
        digitalWrite(PIN_BLUE_LED, HIGH);
        digitalWrite(PIN_ORANGE_LED, LOW);
    }
}

void updateSoundSpeed(){
    float soundSpeedMs = 331.3 + (0.606 * airTemp);

    soundSpeedCmUs = soundSpeedMs / 10000.0; 
    Serial.print("Temp: "); Serial.print(airTemp); Serial.print(" C | ");
    Serial.print("Speed: "); Serial.print(soundSpeedMs); Serial.println(" m/s");
}

float getRawDistance(){
    igitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10); 
    digitalWrite(TRIG_PIN, LOW);

    unsigned long duration = pulseIn(ECHO_PIN, HIGH, 25000); 
    
    if (duration == 0) return 0; 
    
    float dist = (duration * soundSpeedCmUs) / 2.0; 
    
    if (dist > 400.0 || dist < 2.0) return 0; 
    
    return dist;
}

void addReadingToFilter(float newRawDistance) {
    if (validSampleCount == SAMPLES) {
        total -= readings[readIndex];
    }
    
    readings[readIndex] = newRawDistance;
    total += newRawDistance;
    
    readIndex = (readIndex + 1) % SAMPLES;
    
    if (validSampleCount < SAMPLES) {
        validSampleCount++; 
    }
    
    averageDistance = total / validSampleCount;
}

void updateDisplayActive() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("T:"); display.print(airTemp, 1); display.print("C");
    display.setCursor(64, 0);
    display.print("V:"); display.print(soundSpeedCmUs * 10000.0, 0); display.print("m/s");
    
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
    display.setCursor(0, 15);
    display.print("MEASURED DISTANCE:");[cite: 3]
    
    display.setTextSize(2);
    display.setCursor(20, 30);
    if (validSampleCount > 0) {
        display.print(averageDistance, 1);
        display.print(" cm");
    } else {
        display.print("--.- cm");
    }
    
    int barWidth = map(constrain((int)averageDistance, 0, 200), 0, 200, 0, 128);
    display.drawRect(0, 52, 128, 10, SSD1306_WHITE); 
    if (validSampleCount > 0) {
        display.fillRect(0, 52, barWidth, 10, SSD1306_WHITE); 
    }
    
    display.display();
}

void displayStandby() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("NULP | HC-SR04");[cite: 3]
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    display.setTextSize(2);
    display.setCursor(30, 20);
    display.print("READY");[cite: 3]
    
    display.setTextSize(1);
    display.setCursor(0, 45);
    display.print("Speed of Sound:");[cite: 3]
    display.setCursor(0, 55);
    display.print(soundSpeedCmUs * 10000.0, 1);
    display.print(" m/s at ");
    display.print(airTemp, 1);
    display.print("C");
    
    display.display();
}