//************************************************************
//  File: complex.h
//  Complex numbers definition and arithmetic operations
//  for ARM based microcontrollers.
//
//  Arkadiusz Antoniak, 2005
//
//************************************************************
#ifndef COMPLEX_H
#define COMPLEX_H

#include "fix.h"

typedef struct
{
        fix Re;
        fix Im;
} cplx;

//Arithmetic operations
extern cplx cplx_add(cplx a, cplx b);
extern cplx cplx_sub(cplx a, cplx b);
extern cplx cplx_mul(cplx a, cplx b);

#endif

