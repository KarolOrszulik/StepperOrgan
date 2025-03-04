#pragma once

struct StepperPins
{
    byte step;
    byte en;
};

constexpr StepperPins STEPPER_PINS[] = {
    { 3, 12 },
    { 5, 39 },
    { 7, 37 },
    { 9, 35 },
    { 11, 33 }
};

constexpr int numSteppers()
{
    return sizeof(STEPPER_PINS) / sizeof(STEPPER_PINS[0]);
}