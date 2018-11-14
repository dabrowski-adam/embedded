#ifndef SAMPLING_MODULE_H
#define SAMPLING_MODULE_H
#include <LPC213x.H>
#include "complex.h"

extern void InitSamplingModule(void);
extern void Sampling(cplx*);
extern int GetInitialVoltage(void);
#endif
