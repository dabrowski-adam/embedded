
#ifndef UART
#define UART 1

#include "FreeRTOSConfig.h"

void InitSerial0(void);

void testUART(void *pvParameters);
void UARTWrStr (char*str);
#define printf UARTWrStr
void  SendNumber(unsigned char base,
                 unsigned char noDigits,
                 unsigned char sign,
                 char          pad,
                 int           number);
int GetIntNum(void);

#endif
