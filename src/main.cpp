#include <Arduino.h>
#include <TM1637Display.h>
#include <Adafruit_NeoPixel.h>

#ifdef ESP8266
#define WARNING_LIGHT_PIN D6
// #define BUTTON_PIN D2
#define NEOPIXEL_PIN D4
#define DISPLAY_CLK D7
#define DISPLAY_DIO D1
#else
#define DISPLAY_CLK 2
#define DISPLAY_DIO 3
#define NEOPIXEL_PIN 4
#define WARNING_LIGHT_PIN 5
#endif

// void processConfigButton();
void processDisplay();
void displayColor(uint32_t color);

TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);
Adafruit_NeoPixel pixels(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
uint8_t emptyLabel[] = {SEG_G, SEG_G, SEG_G, SEG_G};
uint8_t topic1Label[] = {SEG_A | SEG_D | SEG_E | SEG_F | SEG_G, SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, SEG_D | SEG_E | SEG_F | SEG_G, 0x00}; // eut
uint8_t topic2Label[] = {
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,
    SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,
    SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,
    SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,
    SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,
}; // esas

long beginDisplaySequenceMs = 0;
long startWarningLight = false;
int topic1CounterValue = 0;
int topic2CounterValue = 0;

void setup()
{
  pinMode(WARNING_LIGHT_PIN, OUTPUT);
  // pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(WARNING_LIGHT_PIN, HIGH);

  Serial.begin(115200);
  while (!Serial)
    ; // wait for serial attach

  beginDisplaySequenceMs = millis();

  pixels.begin();
  display.setBrightness(0x0f);

  displayColor(pixels.Color(255, 0, 0));
  display.setSegments(emptyLabel);
}

void loop()
{
  String value;
  if (Serial.available() > 0)
  {
    value = Serial.readStringUntil('\n');
    if (value.length() == 0)
    {
      return;
    }

    Serial.println(value);

    if (value.startsWith("AT+COLOR="))
    {
      int r, g, b;
      sscanf(value.c_str(), "AT+COLOR=%d;%d;%d", &r, &g, &b);
      displayColor(pixels.Color(r, g, b));
    }
    else if (value.startsWith("AT+LAMP=ON"))
    {
      // turn the LED on (HIGH is the voltage level)
      digitalWrite(WARNING_LIGHT_PIN, LOW);
    }
    else if (value.startsWith("AT+LAMP=OFF"))
    {
      // turn the LED off by making the voltage LOW
      digitalWrite(WARNING_LIGHT_PIN, HIGH);
    }
    else if (value.startsWith("AT+COUNTER1="))
    {
      sscanf(value.c_str(), "AT+COUNTER1=%d", &topic1CounterValue);
    }
    else if (value.startsWith("AT+COUNTER2="))
    {
      sscanf(value.c_str(), "AT+COUNTER2=%d", &topic2CounterValue);
    }
    else
    {
      Serial.println("ERROR");
    }
  }

  // processConfigButton();

  processDisplay();
}

// void processConfigButton()
// {
//   if (digitalRead(BUTTON_PIN) == LOW)
//   {
//     delay(1000);
//     if (digitalRead(BUTTON_PIN) == LOW)
//     {
//       Serial.println("TODO");
//     }
//   }
// }

void processDisplay()
{
  if (topic1CounterValue == 0 && topic2CounterValue == 0)
  {
    display.setSegments(emptyLabel);
    return;
  }

  long msSinceLastBeginSequence = millis() - beginDisplaySequenceMs;
  if (msSinceLastBeginSequence < 2000)
  {
    display.setSegments(topic1Label);
  }
  else if (msSinceLastBeginSequence < 4000)
  {
    display.showNumberDec(topic1CounterValue, false);
  }
  else if (msSinceLastBeginSequence < 6000)
  {
    display.setSegments(topic2Label);
  }
  else if (msSinceLastBeginSequence < 8000)
  {
    display.showNumberDec(topic2CounterValue, false);
  }
  else
  {
    beginDisplaySequenceMs = millis();
  }
}

void displayColor(uint32_t color)
{
  pixels.setPixelColor(0, color);
  pixels.show();
}
