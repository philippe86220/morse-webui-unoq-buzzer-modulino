
#include "Morse.h"
#include <ctype.h>

const byte MAXCODE = 6 + 1;

struct t_code {
  char representation[MAXCODE];
  char lettre;
};

static const t_code codeMorse[] = {
  {"--..--", ','},
  {"-....-", '-'},
  {".-.-.-", '.'},
  {"-..-.",  '/'},
  {"-----",  '0'},
  {".----",  '1'},
  {"..---",  '2'},
  {"...--",  '3'},
  {"....-",  '4'},
  {".....",  '5'},
  {"-....",  '6'},
  {"--...",  '7'},
  {"---..",  '8'},
  {"----.",  '9'},
  {"---...", ':'},
  {"-.-.-.", ';'},
  {"",       '<'},
  {"-...-",  '='},
  {"",       '>'},
  {"..--..", '?'},
  {".--.-.", '@'},
  {".-",     'A'},
  {"-...",   'B'},
  {"-.-.",   'C'},
  {"-..",    'D'},
  {".",      'E'},
  {"..-.",   'F'},
  {"--.",    'G'},
  {"....",   'H'},
  {"..",     'I'},
  {".---",   'J'},
  {"-.-",    'K'},
  {".-..",   'L'},
  {"--",     'M'},
  {"-.",     'N'},
  {"---",    'O'},
  {".--.",   'P'},
  {"--.-",   'Q'},
  {".-.",    'R'},
  {"...",    'S'},
  {"-",      'T'},
  {"..-",    'U'},
  {"...-",   'V'},
  {".--",    'W'},
  {"-..-",   'X'},
  {"-.--",   'Y'},
  {"--..",   'Z'},
};

static const byte nbMorseSymboles = sizeof(codeMorse) / sizeof(codeMorse[0]);

static int trouveIndex(char c)
{
  for (byte i = 0; i < nbMorseSymboles; i++) {
    if (codeMorse[i].lettre == c) return (int)i;
  }
  return -1;
}

Morse::Morse(ModulinoBuzzer& buzzer, char* s, int vitesse)
  : _buzzer(buzzer),
    _s(s),
    _vitesse((60 * 1000L) / (50 * vitesse))
{
}

void Morse::point(int duree, int frequence)
{
  _buzzer.tone(frequence, duree);
  delay(duree);
}

void Morse::tiret(int duree, int frequence)
{
  int d = 3 * duree;
  _buzzer.tone(frequence, d);
  delay(d);
}

