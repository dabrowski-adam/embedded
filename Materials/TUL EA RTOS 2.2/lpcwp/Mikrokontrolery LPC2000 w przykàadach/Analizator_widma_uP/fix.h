//************************************************************
//  File: fix.h
//  Fixed point numbers definition and arithmetic operations
//  for ARM based microcontrollers.
//
//  number = n*2^(-q)
//  n - mantisa (signed integer - int)
//  q - exponent (here fixed: q=const. value)
//
//  Arkadiusz Antoniak, 2005
//
//************************************************************
#ifndef FIX_H
#define FIX_H

extern const int q;     //exponent

//fixed point number type
typedef int fix;        //mantissa (interpreted as fixed point number with exponent q)

//arithmetic operations
#define FADD(a,b)       ((a)+(b))
#define FSUB(a,b)       ((a)-(b))
#define FMUL(a,b,q)     (((a)*(b))>>(q))
#define FDIV(a,b,q)     (((a)<<(q))/(b))

#endif

