#include "testRGB.h"
/*****************************************************************************
 *
 * Description:
 *    A process entry function.
 *
 ****************************************************************************/
void testRGB(void *pvParameters)
{
	typedef struct {
		tBool Rozjasnia_Sie;
		tU32  Jasnosc;   // w zakresie 0 - 0x1000
		tU32  O_ile;
	} Dioda;
	Dioda Diody[3] = {{TRUE, 0, 0x49},{TRUE,0, 0x53},{TRUE,0,0x5b}};
	tU8 led;
	#define MAX_WART 0x1000

  	PINSEL0 &= 0xfff03fff;  //Enable PWM2 on P0.7, PWM4 on P0.8, and PWM6 on P0.9
  	PINSEL0 |= 0x000a8000;  //Enable PWM2 on P0.7, PWM4 on P0.8, and PWM6 on P0.9

  	//PULSE WIDTH MODULATION INIT*********************************************
  	PWM_PR  = 0x00;    // Prescale Register
  	PWM_MCR = 0x02;    // Match Control Register
  	PWM_MR0 = 0x1000;    // TOTAL PERIODTID   T
  	PWM_MR2 = 0x0000;    // HÖG SIGNAL        t
  	PWM_MR4 = 0x0000;    // HÖG SIGNAL        t
  	PWM_MR6 = 0x0000;    // HÖG SIGNAL        t
  	PWM_LER = 0x55;    // Latch Enable Register
  	PWM_PCR = 0x5400;  // Prescale Counter Register PWMENA2, PWMENA4, PWMENA6
  	PWM_TCR = 0x09;    // Counter Enable och PWM Enable
  	//************************************************************************

	for (;;)
  	{
		for (led=0; led<3; led++)
		{
			if ( Diody[led].Rozjasnia_Sie)
			{
				if ( Diody[led].Jasnosc <= MAX_WART - Diody[led].O_ile)
					Diody[led].Jasnosc += Diody[led].O_ile;
				else
				{
					Diody[led].Jasnosc -= Diody[led].O_ile;
					Diody[led].Rozjasnia_Sie = FALSE;
				}
			}
			else
			{
				if ( Diody[led].Jasnosc >= Diody[led].O_ile)
					Diody[led].Jasnosc -= Diody[led].O_ile;
				else
				{
					Diody[led].Jasnosc += Diody[led].O_ile;
					Diody[led].Rozjasnia_Sie = TRUE;
				}
			}
		}

		PWM_MR2 = Diody[0].Jasnosc;
		PWM_MR4 = Diody[1].Jasnosc;
		PWM_MR6 = Diody[2].Jasnosc;
		PWM_LER = (0x40|0x10|0x4); // zastosuj;
    	vTaskDelay(30);
  	}
}
