/*
CONTROL MÓDULO DFPLAYER
*/
#include <Arduino.h>
#include <SoftwareSerial.h>

// Puerto de comunicacion DFPLAYER
SoftwareSerial mp3(D7, D6); // RX, TX

// Parámetros tráma conexión DFPLAYER
#define startByte 0x7E
#define endByte 0xEF
#define versionByte 0xFF
#define dataLength 0x06
#define infoReq 0x01
#define isDebug false
///////////////////////////////////////////////

//////// Función Envio comandos al DFPlayer ////////////////////
void sendCommand(byte Command, byte Param1, byte Param2)
{
    // Calcula Checksum
    unsigned int checkSum = -(versionByte + dataLength + Command + infoReq + Param1 + Param2);

    // Construct Command Line
    byte commandBuffer[10] = {startByte, versionByte, dataLength, Command, infoReq,
                              Param1, Param2, highByte(checkSum), lowByte(checkSum), endByte};

    for (int cnt = 0; cnt < 10; cnt++)
    {
        mp3.write(commandBuffer[cnt]);
    }
    // Espera entre sucesivos comandos
    delay(30);
}
////////////////////////////////////////

// Inicio DFPLAYER
void inicioDFPLAYER()
{
    mp3.begin(9600);
    delay(100);

    sendCommand(0x3F, 0x00, 0x00); // Inicion parámetros DFPlayer
    sendCommand(0x06, 0, 20);      // Volumen
    sendCommand(0x07, 0, 5);       // Equalizador
}
/////////////////////////////////////
