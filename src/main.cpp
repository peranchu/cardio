/*
PALABRAS PARA EL CORAZÓN
Honorino García. Febrero 2023
------------------------------------

MAX30102  I2C SCL: D1 SDA: D2
LED     D5
Volumen A0
TX      D6
RX      D7
Busy    D4
*/

#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <TaskScheduler.h>

#include "LED.h"
#include "Cardio.h"
#include "DFPLAYER.h"

#define Volumen A0
#define Busy D4

// Potemciometro Volumen
int potCState = 0;
int potPState = 0;
int valorCState = 0;
int valorPState = 0;
int potVar = 0;

const int TIMEOUT = 300;
const int varThreshold = 30;
bool potMoving = true;
unsigned long PTime = 0;
unsigned long timer = 0;
///////////////////////////////////////////

volatile bool EstadoReproAct = true;
volatile bool EstadoReproAnt = true;
volatile bool Reproduciendo = false;
/////////////////////////////////////////

volatile bool PresenciaSensorAct = true;
volatile bool PresenciaSensorAnt = true;
volatile bool LeyendoSensor = false;
int cuenta;
int ValorSensor;
int tiempo;
//////////////////////////////////////

void PotVol();
void EstadoReproductor();
void fingerSensor();
////////////////////////////

// Tareas
Scheduler tareas;
Task T_reproductor(100, TASK_FOREVER, &EstadoReproductor);
Task T_Volumen(300, TASK_FOREVER, &PotVol);

Task T_Presencia(100, TASK_FOREVER, &fingerSensor);
Task T_Cardio(5, TASK_FOREVER, &LectCardio);

Task T_Latido(100, TASK_FOREVER, &latido);
Task T_Breath(50, TASK_FOREVER, &BreathUP);
///////////////////////////////////////////

void setup()
{
  Serial.begin(115200);
  delay(100);

  WiFi.mode(WIFI_OFF);
  delay(300);

  pinMode(Busy, INPUT);

  // Inicio Reproductor DFPLAYER
  inicioDFPLAYER();

  // inicio Sensor MAX30102
  inicioSensorCardio();

  // Secuencia Inicio LEDS
  inicioLEDS();

  cuenta = 0;

  // Inicio Tareas
  tareas.addTask(T_reproductor);
  tareas.addTask(T_Volumen);

  tareas.addTask(T_Presencia);
  tareas.addTask(T_Cardio);

  tareas.addTask(T_Latido);
  tareas.addTask(T_Breath);

  T_reproductor.enable();
  T_Volumen.enable();

  T_Presencia.enable();
  T_Cardio.disable();

  T_Latido.disable();
  T_Breath.enable();
}

void loop()
{
  tareas.execute();
}
/////////////////////////

// Presencia Sensor Cardio
void fingerSensor()
{
  volatile long ValorIR = particleSensor.getIR();

  if (ValorIR < 5000) // No hay dedo
  {
    PresenciaSensorAct = false;
    if (PresenciaSensorAct != PresenciaSensorAnt)
    {
      Serial.println("Sin dedo");

      T_Cardio.disable();
      T_Breath.enable();

      beatAvg = 0;
      PresenciaSensorAnt = PresenciaSensorAct;
    }
  }

  if (ValorIR > 5000) // Dedo detectado
  {
    PresenciaSensorAct = true;
    if (PresenciaSensorAct != PresenciaSensorAnt)
    {
      Serial.println("dedo");

      T_Breath.disable();
      T_Latido.disable();

      T_Cardio.enable();

      T_Volumen.disable();
      clearLED();

      PresenciaSensorAnt = PresenciaSensorAct;
    }
    cuenta = cuenta + 1;

    if (cuenta == 200) // iteraciones de lectura del sensor
    {
      cuenta = 0;
      ValorSensor = beatAvg;
      Serial.println(ValorSensor);
      T_Cardio.disable();

      tiempo = 60000 / ValorSensor; // Convierte BPM a Milisegundos

      T_Latido.setInterval(tiempo);
      T_Latido.enable();

      sendCommand(0x03, 0, 1);
    }
  }
}
//////////////////////////////////////

// Estado REPRODUCTOR
void EstadoReproductor()
{
  EstadoReproAct = digitalRead(Busy);

  if (EstadoReproAnt != EstadoReproAct)
  {
    if (EstadoReproAct == LOW)
    {
      Serial.println("Reproduciendo");

      Reproduciendo = true;
      T_Volumen.enable();
      T_Presencia.disable();
      EstadoReproAnt = EstadoReproAct;
    }
    if (EstadoReproAct == HIGH)
    {
      Serial.println("PARADO");

      T_Presencia.enable();
      T_Latido.disable();
      clearLED();

      T_Breath.enable();

      Reproduciendo = false;
      EstadoReproAnt = EstadoReproAct;
    }
  }
}
///////////////////////////////////////

// Lectura Potenciometro
void PotVol()
{
  potCState = analogRead(Volumen);
  valorCState = map(potCState, 0, 1023, 0, 30);

  potVar = abs(potCState - potPState);

  if (potVar > varThreshold)
  {
    PTime = millis();
  }
  timer = millis() - PTime;

  if (timer < TIMEOUT)
  {
    potMoving = true;
  }
  else
  {
    potMoving = false;
  }

  if (potMoving == true)
  {
    Serial.println(valorCState);
    sendCommand(0x06, 0, valorCState);

    potPState = potCState;
  }
}