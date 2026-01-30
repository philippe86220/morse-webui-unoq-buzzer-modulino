#ifndef MORSE_H
#define MORSE_H

#include <Arduino.h>
#include "Modulino.h"

class Morse {
  public:
    Morse(ModulinoBuzzer& buzzer, char* s, int vitesse);
    void joue(void);
    ~Morse();

  private:
    ModulinoBuzzer& _buzzer;
    char* _s;
    int _vitesse;

    static const int NOTE_C4 = 262;

    void point(int duree, int frequence);
    void tiret(int duree, int frequence);
    void caractere(int vitesse, int frequence, int index);
};

#endif

