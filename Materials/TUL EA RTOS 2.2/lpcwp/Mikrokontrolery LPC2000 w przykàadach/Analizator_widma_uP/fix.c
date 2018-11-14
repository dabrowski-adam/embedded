//************************************************************
//  File: fix.c
//  Fixed point numbers
//  for ARM based microcontrollers.
//
//  number = n*2^(-q)
//  n - mantisa (signed integer - int)
//  q - exponent (here fixed: q=const. value)
//
//  Arkadiusz Antoniak, 2005
//
//************************************************************
#include "fix.h"

const int q=11;
