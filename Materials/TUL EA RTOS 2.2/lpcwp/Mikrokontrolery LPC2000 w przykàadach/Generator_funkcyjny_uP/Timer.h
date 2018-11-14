#ifndef _TIMER_H
#define _TIMER_H
#include <LPC213x.H>
#include "SignalType.h"

extern void InitTimer0(void);
extern void SetSignalType(SignalType ST);
extern void SetAmplitude(int A);
extern void SetConstValue(int C);
extern void SetFrequency(int F);
extern void SetDuty(int D);

extern SignalType GetSignalType(void);
extern int GetAmplitude(void);
extern int GetConstValue(void);
extern int GetFrequency(void);
extern int GetDuty(void);
#endif
