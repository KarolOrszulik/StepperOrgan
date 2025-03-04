#include <Arduino.h>

class Stepper
{
public:
    void setPins(byte step, byte en);
    void stop();
    void playNote(byte midiNote);
    void update();

private:
    byte stepPin;
    byte enPin;

    unsigned long stepInterval;
    unsigned long lastStepTime;
};