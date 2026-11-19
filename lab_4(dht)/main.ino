#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define PIN_BUTTON     12  
#define DHT11_PIN      13  
#define DHT22_PIN      2   
#define PIN_GREEN_LED  14  
#define PIN_BLUE_LED   16  

#define OLED_SDA       4   
#define OLED_SCL       5  

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht11(DHT11_PIN, DHT11);
DHT dht22(DHT22_PIN, DHT22);

struct SensorReading
{
    float temp;
    float hum;
    bool valid;
};

bool systemActive = false;
bool standbyDrawn = false;

bool buttonState = HIGH;
bool lastButtonReading = HIGH;

unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 50;

unsigned long lastSampleTime = 0;
const unsigned long SAMPLE_INTERVAL = 2000;

void handleButton();
SensorReading readSensor(DHT &sensor);
void logTelemetryCSV(const SensorReading &s11, const SensorReading &s22);
void updateDisplayActive(const SensorReading &s11, const SensorReading &s22);
void displayStandby();

void setup() {
    Serial.begin(115200);
    Wire.begin(OLED_SDA, OLED_SCL);

    pinMode(PIN_GREEN_LED, OUTPUT);
    pinMode(PIN_BLUE_LED, OUTPUT);
    pinMode(PIN_BUTTON, INPUT_PULLUP);

    dht11.begin();
    dht22.begin();

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    Serial.println(F("Timestamp_ms,DHT11_Temp_C,DHT11_Hum_%,DHT22_Temp_C,DHT22_Hum_%"));
    displayStandby();
    standbyDrawn = true;
}

void loop() {
    handleButton();

    if (systemActive) {
        standbyDrawn = false;
        digitalWrite(PIN_GREEN_LED, HIGH);
        digitalWrite(PIN_BLUE_LED, LOW);

        unsigned long currentMillis = millis();
        if (currentMillis - lastSampleTime >= SAMPLE_INTERVAL) {
        lastSampleTime = currentMillis;

        SensorReading s11 = readSensor(dht11);
        SensorReading s22 = readSensor(dht22);

        logTelemetryCSV(s11, s22);
        updateDisplayActive(s11, s22);
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

    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
        if (reading != buttonState) {
        buttonState = reading;
        if (buttonState == LOW) {
            systemActive = !systemActive;
            if (systemActive) {
            lastSampleTime = millis() - SAMPLE_INTERVAL;
            }
        }
        }
    }
    lastButtonReading = reading;
}

SensorReading readSensor(DHT &sensor) {
    SensorReading data;
    data.temp = sensor.readTemperature();
    data.hum = sensor.readHumidity();
    data.valid = !isnan(data.temp) && !isnan(data.hum);
    return data;
}

void logTelemetryCSV(const SensorReading &s11, const SensorReading &s22) {
    Serial.print(millis());
    Serial.print(',');
    if (s11.valid) {
        Serial.print(s11.temp, 1); Serial.print(','); Serial.print(s11.hum, 1);
    } else {
        Serial.print(F("NaN,NaN"));
    }
    Serial.print(',');
    if (s22.valid) {
        Serial.print(s22.temp, 1); Serial.print(','); Serial.println(s22.hum, 1);
    } else {
        Serial.println(F("NaN,NaN"));
    }
}


void updateDisplayActive(const SensorReading &s11, const SensorReading &s22) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(0, 0);
    display.print(F("LAB 4: DHT11 vs DHT22"));
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

    display.setCursor(0, 18);
    display.print(F("DHT11: "));
    if (s11.valid) {
        display.print(s11.temp, 1); display.print(F("C "));
        display.print(s11.hum, 0);  display.print(F("%"));
    } else {
        display.print(F("SENSOR ERR"));
    }

    display.setCursor(0, 32);
    display.print(F("DHT22: "));
    if (s22.valid) {
        display.print(s22.temp, 1); display.print(F("C "));
        display.print(s22.hum, 0);  display.print(F("%"));
    } else {
        display.print(F("SENSOR ERR"));
    }

    display.drawLine(0, 46, 128, 46, SSD1306_WHITE);
    display.setCursor(0, 52);
    if (s11.valid && s22.valid) {
        display.print(F("Delta T: "));
        display.print(abs(s22.temp - s11.temp), 1);
        display.print(F(" C"));
    } else {
        display.print(F("Delta T: --.- C"));
    }

    display.display();
}

void displayStandby() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(F("NULP | DHT BENCHMARK"));
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

    display.setTextSize(2);
    display.setCursor(35, 28);
    display.print(F("READY"));

    display.display();
}
