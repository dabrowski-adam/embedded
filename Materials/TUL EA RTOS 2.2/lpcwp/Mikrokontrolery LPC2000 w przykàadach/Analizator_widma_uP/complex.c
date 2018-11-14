//************************************************************
//  File: complex.c
//  Complex numbers
//  for ARM based microcontrollers.
//
//  Arkadiusz Antoniak, 2005
//
//************************************************************
#include "complex.h"

//Add two complex numbers: a+b
cplx cplx_add(cplx a, cplx b)
{
        cplx result;
        result.Re=FADD(a.Re,b.Re);
        result.Im=FADD(a.Im,b.Im);
        return result;
}

//Sub two complex numbers: a-b
cplx cplx_sub(cplx a, cplx b)
{
        cplx result;
        result.Re=FSUB(a.Re,b.Re);
        result.Im=FSUB(a.Im,b.Im);
        return result;
}

//Mul two complex numbers: a*b
cplx cplx_mul(cplx a, cplx b)
{
        cplx result;
        result.Re=FSUB(FMUL(a.Re,b.Re,q),FMUL(a.Im,b.Im,q));
        result.Im=FADD(FMUL(a.Re,b.Im,q),FMUL(a.Im,b.Re,q));
        return result;
}

//Div two complex numbers: a/b
// NOT NEEDED

