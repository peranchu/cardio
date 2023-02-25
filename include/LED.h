/*
CONTROL DE LED
*/

#include <FastLED.h>

#define FASTLED_ESP8266_RAW_PIN_ORDER

#define NUM_LEDS 5
#define PIN_LED D5
#define COLOR_ORDER GRB
#define CHIPSET WS2812B
#define MIN_BRIGTNESS 8
#define BRIGHTNESS 190
#define VOLTS 5
#define MAX_AMPS 500

int brillo = 0;
int ValorPausa = 0;

CRGB leds[NUM_LEDS];

// Inicio secuencia LEDS
void inicioLEDS()
{
    Serial.println("Dentro LEDS");

    FastLED.addLeds<CHIPSET, PIN_LED, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MAX_AMPS);
    FastLED.setBrightness(BRIGHTNESS);

    // Secuencia inicio LEDS
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB::Aqua;
        FastLED.show();
        delay(100);
    }

    for (int i = NUM_LEDS; i >= 0; i--)
    {
        leds[i] = CRGB::Black;
        FastLED.show();
        delay(100);
    }
    delay(300);
}
////////////////////////////////

// Limpieza LEDS
void clearLED()
{
    FastLED.clear();
    FastLED.show();
}
/////////////////////////////////////

// Con Reproductor OFF
//  Breath Effect
void BreathUP()
{
    fill_solid(leds, NUM_LEDS, CRGB::Purple);

    brillo = exp((sin(millis() / 2000.0 * PI)) - 0.36787944) * 108.0;
    FastLED.setBrightness(brillo);
    FastLED.show();
}
///////////////////////////////////

// LATIDO
void latido()
{
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.show();
    delay(100);
    clearLED();
}
/////////////////////////////

// Pulso
void pulso_ON()
{
    fill_solid(leds, NUM_LEDS, CRGB::DarkGreen);
    FastLED.show();
}
////////////////////
// Pulso
void pulso_OFF()
{
    clearLED();
}
/// ///////////////////////////
