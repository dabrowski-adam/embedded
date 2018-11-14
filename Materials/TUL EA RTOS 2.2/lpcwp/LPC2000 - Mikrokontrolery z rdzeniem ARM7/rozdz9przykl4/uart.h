#ifndef __SERIAL_H
#define __SERIAL_H

//Rozmiar bufora nadajnika
#define TXBUF_SIZE 256
//Rozmiar bufora odbiornika
#define RXBUF_SIZE 256
//Czetotliwosc PCLK
#define PCLK 60000000 

//Przelicznik predkosci portu szeregowego
#define UART_BAUD(baud) (unsigned short)(PCLK/(baud*16.0) + 0.5)

/* Inicjalizacja Uart0 */
void Uart0Init(unsigned short BaudRate);
//Nadawanie znaku
void Uart0PutChar(char c);
//Odebranie znaku
char Uart0GetChar(void);
//Nadawanie lancucha znakow
void Uart0Puts(const char *str);
//Odbior lancucha znakow
void Uart0Gets(char *str);


#endif
