/*
CONTROL SENSOR CARDIACO
*/
#include <Arduino.h>
#include <MAX30105.h>
#include "heartRate.h"

// MAX30102
MAX30105 particleSensor;

const byte RATE_SIZE = 4;
volatile byte rates[RATE_SIZE];
volatile byte rateSpot = 0;
volatile long lastBeat = 0;

volatile float beatsPerMinute;
volatile int beatAvg;
volatile int count;
volatile int perCent;
volatile int avgIr;
////////////////////////////////

// Change this Value for Calibration
int irOffset = 1850;

long samplesTaken = 0; // Counter for calculating the Hz or read rate
long unblockedValue;   // Average IR at power up
long startTime;        // Used to calculate measurement rate

void inicioSensorCardio()
{
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
    {
        Serial.println("Sensor MAX30102 No encontrado.");
        while (1)
            ;
    }
    Serial.println("Sensor MAX3012 LISTO.");

    particleSensor.setup();
    particleSensor.setPulseAmplitudeRed(0x0A); // Enciende LED Rojo, sensor en funcionamiento.
    particleSensor.setPulseAmplitudeGreen(0);  // Apaga LED Verde.

    byte ledBrightness = 25; // Options: 0=Off to 255=50mA=0xFF hexadecimal. 100=0x64; 50=0x32 25=0x19
    byte sampleAverage = 4;  // Options: 1, 2, 4, 8, 16, 32
    byte ledMode = 2;        // Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    int sampleRate = 400;    // Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411;    // Options: 69, 118, 215, 411
    int adcRange = 2048;     // Options: 2048, 4096, 8192, 16384

    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); // Configure sensor with these settings

    // Take an average of IR readings at power up
    const byte avgAmount = 64;
    long baseValue = 0;
    for (byte x = 0; x < avgAmount; x++)
    {
        baseValue += particleSensor.getIR(); // Read the IR value
    }
    baseValue /= avgAmount;
}

// Lectura Cardio
void LectCardio()
{
    volatile long irValue = particleSensor.getIR();

    if (checkForBeat(irValue) == true)
    {
        pulso_ON();
        delay(100);
        pulso_OFF();

        // We sensed a beat!
        volatile long delta = millis() - lastBeat;
        lastBeat = millis();

        beatsPerMinute = 60 / (delta / 1000.0);

        if (beatsPerMinute < 255 && beatsPerMinute > 20)
        {
            rates[rateSpot++] = (byte)beatsPerMinute; // Store this reading in the array
            rateSpot %= RATE_SIZE;                    // Wrap variable

            // Take average of readings
            beatAvg = 0;
            for (byte x = 0; x < RATE_SIZE; x++)
                beatAvg += rates[x];
            beatAvg /= RATE_SIZE;
        }
    }
    Serial.print("IR=");
    Serial.print(irValue);
    Serial.print(", BPM=");
    Serial.print(beatsPerMinute);
    Serial.print(", Avg BPM=");
    Serial.print(beatAvg);

    if (irValue < 50000)
    {
        Serial.println("No finger?");
        beatAvg = 0;
    }
    else
    {
        count = count + 1;
        avgIr = avgIr + irValue;
    }
    if (count == 100)
    {
        avgIr = avgIr / count;

        count = 0;
        avgIr = 0;
    }
    Serial.println();
}
//////////////////////////////////
