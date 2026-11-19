#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PIN_BUTTON     12  
#define PIN_SENSOR     13  
#define PIN_TACHO      2   
#define PIN_GREEN_LED  14  
#define PIN_BLUE_LED   16  
#define PIN_ORANGE_LED 0   

#define OLED_SDA       4   
#define OLED_SCL       5  

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool systemActive = false;
bool lastButtonState = HIGH;

volatile unsigned long countFC51 = 0;
volatile unsigned long countTacho = 0;
volatile unsigned long lastInterruptTimeFC51 = 0;

const unsigned long DEBOUNCE_DELAY_FC51 = 10;

unsigned long lastMillis = 0;
const unsigned long MEASUREMENT_INTERVAL = 1000; 

float rpmFC51 = 0;
float rpmTacho = 0;
float absError = 0;
float relError = 0;

void IRAM_ATTR isrFC51(){
    unsigned long currentMillis = millis();

    if ((currentMillis - lastInterruptTimeFC51) > DEBOUNCE_DELAY_FC51){
        countFC51++;
        lastInterruptTimeFC51 = currentMillis;
    }
}

void IRAM_ATTR isrTacho(){
    countTacho++;
}

void updateDisplayActive();
void updateDisplayStandby();

void setup() {
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);

  pinMode(PIN_GREEN_LED, OUTPUT);
  pinMode(PIN_BLUE_LED, OUTPUT);
  pinMode(PIN_ORANGE_LED, OUTPUT);
  
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  
  pinMode(PIN_SENSOR, INPUT); 
  pinMode(PIN_TACHO, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  digitalWrite(PIN_ORANGE_LED, LOW);
  
  updateDisplayStandby();
}

void loop() {
    bool currentButtonState = digitalRead(PIN_BUTTON);

    if (lastButtonState == HIGH && currentButtonState == LOW){
        systemActive = !systemActive;

        if (systemActive){
            countFC51 = 0;
            countTacho = 0;
            lastMillis = millis();

            attachInterrupt(digitalPinToInterrupt(PIN_SENSOR), isrFC51, FALLING);
            attachInterrupt(digitalPinToInterrupt(PIN_TACHO), isrTacho, FALLING);
        } else {
            detachInterrupt(digitalPinToInterrupt(PIN_SENSOR));
            detachInterrupt(digitalPinToInterrupt(PIN_TACHO));
        }
        delay(200);
    }
    lastButtonState = currentButtonState;

    if (systemActive){
        digitalWrite(PIN_GREEN_LED, HIGH);
        digitalWrite(PIN_BLUE_LED, LOW);

        unsigned long currentMillis = millis();
        if ((currentMillis - lastMillis) >= MEASUREMENT_INTERVAL){
            noInterrupts();

            unsigned long snapFC51 = countFC51;
            unsigned long snapTacho = countTacho;
            countFC51 = 0;
            countTacho = 0;
            interrupts();

            unsigned long deltaTime = currentMillis - lastMillis;
            lastMillis = currentMillis;

            rpmFC51 = (snapFC51 * 60000.0) / deltaTime;
            rpmTacho = (snapTacho * 60000.0) / (deltaTime * 2.0);
            absError = abs(rpmFC51 - rpmTacho);

            if (rpmTacho > 0) {
                relError = (absError / rpmTacho) * 100.0;
            } else {
                relError = 0;
            }

            digitalWrite(PIN_ORANGE_LED, (snapFC51 > 0) ? HIGH : LOW);
            updateDisplayActive();
        }
    } else {
        digitalWrite(PIN_GREEN_LED, LOW);
        digitalWrite(PIN_BLUE_LED, HIGH);
        digitalWrite(PIN_ORANGE_LED, LOW);
        
        lastMillis = millis();
    }
}

void updateDisplayActive() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("OPT (FC-51):");
  
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print((int)rpmFC51);
  display.print(" RPM");
  
  display.setTextSize(1);
  display.setCursor(0, 30);
  display.print("TACHO (INT):");
  
  display.setTextSize(2);
  display.setCursor(0, 40);
  display.print((int)rpmTacho);
  display.print(" RPM");
  
  display.setTextSize(1);
  display.setCursor(0, 57);
  display.print("Err: ");
  display.print((int)absError);
  display.print(" | ");
  display.print(relError, 1);
  display.print("%");
  
  display.display();
}

void updateDisplayStandby() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("NULP | DC MOTOR");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  
  display.setTextSize(2);
  display.setCursor(35, 20);
  display.print("READY");
  
  display.setTextSize(1);
  display.drawLine(0, 42, 128, 42, SSD1306_WHITE);
  display.setCursor(0, 48);
  display.print("Last Abs Err: ");
  display.print((int)absError);
  
  display.setCursor(0, 57);
  display.print("Last Rel Err: ");
  display.print(relError, 1);
  display.print("%");
  
  display.display();
}