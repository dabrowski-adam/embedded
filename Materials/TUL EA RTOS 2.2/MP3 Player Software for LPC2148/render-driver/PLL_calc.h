#ifndef __PLL_CALC_H
#define __PLL_CALC_H

/**********************************************************************/
/* __PLL_CALC_H                                                       */
/*                                                                    */
/* In this file the parameter M and P for the PLL are calculated      */
/* during pre-compiling. A macro called 'PLLCFGVAL' is set to the     */
/* value, which can be downloaded directly into the PLLCFG-register.  */
/*                                                                    */
/* Lutz Pape                                                          */
/* 14.08.2003                                                         */
/* Philips Semiconductors                                             */
/* Microcontroller                                                    */
/**********************************************************************/

//parameters to set by the software developer

#define Fosc 12000000
#define cclk 60000000

//#define Fosc 14745600
//#define cclk 58982400

// Calculations of the parameters M and P for the PLL. Please see the datasheet
//for details.
//*** M ***
#define M (cclk/Fosc)
#if (M>6)
  #error Invalid value for PLL parameter M in 'PLL_calc.H'. Maximum is 6.
#endif
#if (M<1)
  #error Invalid value for PLL parameter M in 'PLL_calc.H'. Minimum is 1.
#endif

#define MSEL (M-1)

//*** P ***
#define Fcco1 cclk*2   //P=1
#define Fcco2 cclk*2*2 //P=2
#define Fcco4 cclk*2*4 //P=4
#define Fcco8 cclk*2*8 //P=8

//check on P=1
#if (Fcco1>=156000000)
  #if (Fcco1<=320000000)
    #define PSEL 0
  #endif
#endif

//check on P=2
#ifndef PSEL
  #if (Fcco2>=156000000)
    #if (Fcco2<=320000000)
      #define PSEL 1
    #endif
  #endif
#endif

//check on P=4
#ifndef PSEL
  #if (Fcco4>=156000000)
    #if (Fcco4<=320000000)
      #define PSEL 2
    #endif
  #endif
#endif

//check on P=8
#ifndef PSEL
  #if (Fcco8>=156000000)
    #if (Fcco8<=320000000)
      #define PSEL 3
    #endif
  #endif
#endif

#ifndef PSEL
  #error No value for 'P' was found during pre-calculation of the PLL-value in 'PLL_calc.H'
#endif

//**** config register value *****//
#define PLLCFGVAL (PSEL*32)+MSEL


#endif
