#include "lpc214x.h"
#include "uart.h"
#include "armint.h"

//Bufor odbiornika
static unsigned char rxbuf[TXBUF_SIZE +10];	
//Bufor nadajnika
static unsigned char txbuf[RXBUF_SIZE+10];	
//Liczniki nadajnika i odbiornika
volatile static unsigned short txcnt, rxpos, txpos, rxcnt;
//Znacznik zajetosci nadajnika
volatile static unsigned char busy;

//Definicje ustawien pinow RXD i TXD
//Ustawienia kontrolera VIC
#define TXD0_P00_SEL (1<<0) 
#define RXD0_P01_SEL (1<<2)

//Zapelnienie bufora fifo odbiornika
#define U0IIR_RDA_INT  0x04
//Przez 3,5 znaku nie odebrano danych
#define U0IIR_CTI_INT 0x0C
//Bufor fifo nadajnika pusty
#define U0IIR_THRE_INT 0x02

//8 bitow danych 
#define U0LCR_8Bit_Data 3
//1 bit stopu
#define U0LCR_1Bit_Stop 0
//Brak bitu parzystosci
#define U0LCR_No_Parity 0
//Bufor FIFO na 14 znakow
#define U0FCR_14Char_Fifo (3<<6) 
#define U0LSR_RDR 0x1
#define U0LCR_Divisor_Latch_Access_Bit 0x80
#define U0FCR_FIFO_Enable 0x1
#define U0IER_THRE_Interrupt_Enable 0x2
#define U0IER_RBR_Interrupt_Enable 0x1

#define U0_VIC (1<<6)
#define U0_VIC_BIT 6
#define VIC_IRQSLOT_EN (1<<5) 

//Definicja naglowku przerwania
static void Uart0Int(void) __attribute__ ((interrupt("IRQ")));


//Funkcja przerwania
void Uart0Int(void)
{
	unsigned char irqstat = U0IIR & 0x0F;	
	unsigned char tmp;
	if(irqstat==U0IIR_RDA_INT || irqstat==U0IIR_CTI_INT)
	{
		//odczytuj dane z fifo dopuki sa
		while(U0LSR & U0LSR_RDR)
		{
			if(rxcnt < RXBUF_SIZE) 
			{
				rxbuf [(rxpos + rxcnt++) % RXBUF_SIZE] = U0RBR;
			}
			else
			{
				tmp = U0RBR;
				break;					
			}
		}
	}
	if(irqstat==U0IIR_THRE_INT)
	{
		//Mozna nadawac nowy znak
		if(txcnt) 
		{
			busy = 1;
			txcnt--;
      		U0THR = txbuf[txpos++];
	        if(txpos >= TXBUF_SIZE) txpos = 0;
		}
		else
		{ 
			tmp = U0IIR;
			busy = 0;
		}
	}
	//Informacja dla kontrolera przerwan
	VICVectAddr = 0;
}

/* Inicjalizacja Uart0 */
void Uart0Init(unsigned short BaudRate)
{
	//Funkcje wyjsciowe UART
	PINSEL0 |= TXD0_P00_SEL | RXD0_P01_SEL;
	//DLAB = 1
	U0LCR = U0LCR_Divisor_Latch_Access_Bit;
	//Ustaw predkosci transmisji
	U0DLL = (unsigned char)BaudRate;
	U0DLM = (unsigned char)(BaudRate>>8);
	//Ustawienie 8,n,1
	U0LCR = U0LCR_8Bit_Data | U0LCR_1Bit_Stop | U0LCR_No_Parity;
	//Wlacz FIFO na 14 znakow
	U0FCR = U0FCR_14Char_Fifo | U0FCR_FIFO_Enable;
	//Funkcja przerwania wektora 1
	VICVectAddr1 = (int)Uart0Int;
	//Zalaczenie slotu 1
	VICVectCntl1 = U0_VIC_BIT | VIC_IRQSLOT_EN;
	//Kasuj ewentualne znaczniki odbioru nadania znaku
	U0LSR = U0IIR = 0;
	//Wlaczenie przerwan od RX TX
	U0IER = U0IER_RBR_Interrupt_Enable | U0IER_THRE_Interrupt_Enable;
	//Zalaczenie przerwania od UART0
	VICIntEnable = U0_VIC;
	//Odblokuj przerwania
	enable_irq();
}


//Nadawanie znaku
void Uart0PutChar(char c)
{
	 //Gdy wszystkie bajty, czekaj na zwolnienie 
	 while(txcnt >= TXBUF_SIZE);   
	 //Wylacz na moment przerwanie od RS
	 cpu_t irqs = disable_irq();
	 U0IER = 0;
	 restore_irq(irqs);
	 if(busy)
	 {
	 	txbuf[(txpos + txcnt++) % TXBUF_SIZE] = c;
	 }
	 else
	 {
	 	U0THR = c;
	 	busy = 1;
	 }
	 //Wlaczenie przerwan od RX TX
	 irqs = disable_irq();
	 U0IER = U0IER_RBR_Interrupt_Enable | U0IER_THRE_Interrupt_Enable;
	 restore_irq(irqs);
}

//Odebranie znaku
char Uart0GetChar(void)
{
	int c;
	while (!rxcnt);		//Czekaj az bedzie znak
	//Wylacz na moment przerwanie od RS
	cpu_t irqs = disable_irq();
	U0IER = 0;
	restore_irq(irqs);
	rxcnt--;
  	c = rxbuf[rxpos++];
  	if (rxpos >= RXBUF_SIZE) rxpos = 0; 
	//Wlaczenie przerwan od RX TX
	irqs = disable_irq();
	U0IER = U0IER_RBR_Interrupt_Enable | U0IER_THRE_Interrupt_Enable;
	restore_irq(irqs);
	return c;
}

//Nadawanie lancucha znakow
void Uart0Puts(const char *str)
{
	//Wysylaj kolejne znaki
	while(*str)
	{
		//Wyslij znak i przejdz do nast znaku
		Uart0PutChar(*str++);
	}
}

//Odbior lancucha znakow
void Uart0Gets(char *str)
{
	char c;
	do
	{
		c = Uart0GetChar();
		//Zapisz znak do bufora
		*str++ = c;
		//Nadaj zwrotnie znak
		Uart0PutChar(c);
	}
	while(c != '\r');
	*(str-1) = 0;
}
