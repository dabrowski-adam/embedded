#ifndef _USERSETTINGS_H
#define _USERSETTINGS_H
#include <LPC213x.H>
#include "SignalType.h"
#include "switches_pins.h"

extern void ChangeSignalType(void);
extern void ChangeAmplitude(void);
extern void ChangeConstValue(void);
extern void ChangeFrequency(void);
extern void ChangeDuty(void);
#endif
