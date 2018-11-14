//************************************************************
//  File: fft.h
//  Fast Fourier Transform header
//  for ARM based microcontrollers.
//
//  Arkadiusz Antoniak, 2005
//
//************************************************************
#ifndef FFT_H
#define FFT_H

#include "complex.h"

#define N 256

extern void InitFFT(void);
extern void FFT(cplx *x, cplx *X);
extern void IFFT(cplx *x, cplx *X);
extern void Hanning(cplx *x);
extern void Hamming(cplx *x);
#endif
