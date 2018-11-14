//************************************************************
//  File: fft.c
//  Fast Fourier Transform implementation
//  for ARM based microcontrollers.
//
//  Arkadiusz Antoniak, 2005
//
//************************************************************
#include "fft.h"

//Real and immaginary values for twiddle factors
fix Wn_Re[N]={
#include "Wn256_Re.dat"
};

fix Wn_Im[N]={
#include "Wn256_Im.dat"
};

//Complex twiddle factors for k=0...N-1
//Wn[k]=exp(-j*2*PI*k/N)
cplx Wn[N];

//Hanning window
fix HanningWin[N]={
#include "Hanning256.dat"
};

//Hamming window
fix HammingWin[N]={
#include "Hamming256.dat"
};

//*****************************************
//  Hanning window function
//*****************************************
void Hanning(cplx *x)
{
        int k;

        //Multiplying signal's factors by Hanning's window
        for(k=0;k<N;k++)
        {
       			x[k].Re=FMUL(x[k].Re,HanningWin[k],q);
        		x[k].Im=FMUL(x[k].Im,HanningWin[k],q);
        }
}

//*****************************************
//  Hamming window function
//*****************************************
void Hamming(cplx *x)
{
        int k;

        //Multiplying signal's factors by Hamming's window
        for(k=0;k<N;k++)
        {
                x[k].Re=FMUL(x[k].Re,HammingWin[k],q);
                x[k].Im=FMUL(x[k].Im,HammingWin[k],q);
        }
}

//*****************************************
//  InitFFT
//*****************************************
void InitFFT(void)
{
        int k;

        //Filling twiddle factors
        for(k=0;k<N;k++)
        {
                Wn[k].Re=Wn_Re[k];
                Wn[k].Im=Wn_Im[k];
        }
}

//*****************************************
//  Bit shifting
//*****************************************
int BitShift(int num, int MaxBitNr)
{
        int i,result=0;
        for(i=0;i<=MaxBitNr;i++)
                if(((1<<i) & num)!=0)
                        result|=(1<<(MaxBitNr-i));

        return result;
}

//*****************************************
//  Butterfly function
//  Input, output: x, y
//  x <- x + Wn[k]*y
//  y <- x - Wn[k]*y
//*****************************************
void Butterfly(cplx *x, cplx *y, int k)
{
        cplx tx,ty,tWy;

        tWy=cplx_mul(Wn[k],*y);
        tx=cplx_add(*x,tWy);
        ty=cplx_sub(*x,tWy);

        *x=tx;
        *y=ty;
}

//*****************************************
//  FFT function
//  x - input N complex values
//  X - output N complex values
//*****************************************
void FFT(cplx *x, cplx *X)
{
        int stc;        //stages counter
        int blc;        //blocks of butterflys in stage counter
        int bfc;        //butterflys in block counter

        int NumOfStages;        //Number of stages = log2(N)
        int NumOfBlocks;        //in stage
        int NumOfButterflys;    //in block

        //Determining number of stages - counting log2(N)
        for(NumOfStages=0;NumOfStages<32;NumOfStages++)
                if(N==(1<<NumOfStages))
                        break;

        //Copying input data to output buffer
        //to perform in-place computing
        for(stc=0;stc<N;stc++)
                X[stc]=x[BitShift(stc,NumOfStages-1)];

        //Main FFT loop
        for(stc=0;stc<NumOfStages;stc++)
        {
                NumOfButterflys=(1<<stc);
				NumOfBlocks=N>>(stc+1);

                for(blc=0;blc<NumOfBlocks;blc++)
                {
                        int base=(1<<(stc+1))*blc;
                        for(bfc=0;bfc<NumOfButterflys;bfc++)
                                Butterfly(X+base+bfc,X+base+bfc+NumOfButterflys,NumOfBlocks*bfc);
                }
        }
}

//*****************************************
//  IFFT function (Inverse FFT)
//  x - output N complex values
//  X - input N complex values
//*****************************************
void IFFT(cplx *x, cplx *X)
{
        cplx tX[N];
        int i;

        for(i=0;i<N;i++)
        {
                tX[i].Re=X[i].Re;
                tX[i].Im=-X[i].Im;
        }

        //Count FFT for tX (input). Result in x
        FFT(tX,x);

        for(i=0;i<N;i++)
        {
                x[i].Im=-x[i].Im;
                x[i].Re/=N;
                x[i].Im/=N;
        }
}

