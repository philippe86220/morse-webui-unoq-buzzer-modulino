#include <Arduino.h>
#include <Arduino_RouterBridge.h>
#include "Modulino.h"
#include "Morse.h"

static const size_t TAILLE = 250;
static char message[TAILLE];

ModulinoBuzzer buzzer;

String ping_mcu(String s)
{
  (void)s;
  return "pong";
}

String morse_play(String txt, int speed)
{
  txt.replace("\r", "");
  txt.trim();

  if (txt.length() == 0) {
    Monitor.println("Texte vide");
    return "EMPTY";
  }

  size_t n = (size_t)txt.length();
  if (n > (TAILLE - 1)) n = (TAILLE - 1);

  for (size_t i = 0; i < n; i++) {
    message[i] = txt[(unsigned int)i];
  }
  message[n] = '\0';

  Monitor.print("morse_play recu: ");
  Monitor.println(message);

  Morse code(buzzer, message, speed);
  code.joue();

  return "OK";
}

void setup()
{
  // Init Bridge + console
  Bridge.begin();
  Monitor.begin();

  // Init Modulino buzzer
  Modulino.begin();
  buzzer.begin();

  // Preuve de vie
  buzzer.tone(1000, 200);
  delay(300);
  buzzer.tone(1500, 200);
  delay(400);

  // Expose Bridge methods
  Bridge.provide("ping_mcu", ping_mcu);
  Bridge.provide("morse_play", morse_play);

  Monitor.println("MCU pret: ping_mcu + morse_play");
}

void loop()
{
  delay(20);
}

