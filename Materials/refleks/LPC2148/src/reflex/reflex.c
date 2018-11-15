/******************************************************************************
 *
 * Pawe³ Tarasiuk, 15121
 *
 * Based on "main.c" from the test program copyrighted by
 * Embedded Atrists AB.
 *
 *****************************************************************************/


#include "../pre_emptive_os/api/osapi.h"
#include "../pre_emptive_os/api/general.h"
#include <printf_P.h>
#include <ea_init.h>
#include <lpc2xxx.h>
#include <consol.h>

#define PROC_MAIN_STACK_SIZE 2048
#define PROC_RAND_STACK_SIZE 2048
#define PROC_JOY_STACK_SIZE 2048
#define PROC_LEDS_STACK_SIZE 2048
#define INIT_STACK_SIZE  400

static tU8 procMainStack[PROC_MAIN_STACK_SIZE];
static tU8 pidMain;
static void procMain(void* arg);

static tU8 procRandStack[PROC_RAND_STACK_SIZE];
static tU8 pidRand;
static void procRand(void* arg);

static tU8 procJoyStack[PROC_JOY_STACK_SIZE];
static tU8 pidJoy;
static void procJoy(void* arg);

static tU8 procLedsStack[PROC_LEDS_STACK_SIZE];
static tU8 pidLeds;
static void procLeds(void* arg);


static tU8 initStack[INIT_STACK_SIZE];
static void initProc(void* arg);

void testLedMatrix(void);
void messageOnLCD(char *str, tU8 important);
void testMotor(void);
void testRGB(tU8 r, tU8 g, tU8 b);
void testI2C(void);
void testAdc(void);

tU8 getRandBit(void);
tU32 getRandBits(int count);

tU8 inGame;
tU16 random = 0x667d; /* chosen by fair dice roll, guaranteed to be random */
tU8 request = 8; /* 0: none, 1: up, 2: right, 3: bottom, 4: left, 5: push,
                    8: player not ready */

tU32 ticks = 0; /* multiply of 2ms since the timer starts */
tU32 requestTime;
tU8 score = 0;
tU8 lives = 2;
tU32 sumResult = 0;
tU8 state = 0; /* 0: new game, 1: after success, 2: after failure */


/*****************************************************************************
 *
 * Description:
 *    Calculate hash of a string (the transmission protocol for console)
 *
 ****************************************************************************/
unsigned int
calculateHash(char *buffer)
{
  static unsigned char map[256];
  static unsigned char initialized;
  int i;
  char *p;
  unsigned int result = 0xf50a;
  
  if(initialized == 0)
  {
    for(i = 0; i < 256; i++) map[i] = 1;
    for(i = 'A'; i < 'Z'; i++) map[i] = i + 2 - 'A';
    for(i = 'a'; i < 'z'; i++) map[i] = i + 28 - 'a';
    for(i = '0'; i < '9'; i++) map[i] = i + 54 - '0';
    initialized = 1;
  }
  
  for(p = buffer; *p; p++)
  {
    if(*p != '\r')
    {
      result = ((result * 0x1111) ^ map[(unsigned int) *p]) & 0xffff;
    }
  }
  
  return result;
}

/*****************************************************************************
 *
 * Description:
 *    Send a text message via console;
 *
 ****************************************************************************/
void
consoleMessage(char *message)
{
  printf("%s$", message);
  printf("%u$", calculateHash(message));
}


/*****************************************************************************
 *
 * Description:
 *    Return the pseudo-random bit
 *
 ****************************************************************************/
tU8
getRandBit(void)
{	
	/* 269*241=64829; 269, 241 - primes*/
	random = (tU16) ((random * (tU32) random) % 64829);
	return random & 1;
}

/*****************************************************************************
 *
 * Description:
 *    Return the pseudo-random 32-bit number
 *
 ****************************************************************************/
tU32
getRandBits(int count)
{
  tU32 ret = 0;
  if(count > 32)
  {
    count = 32;
  }
  while(count --> 0) {
    ret = (ret << 1) | getRandBit();
  }
  return ret;
}

/*****************************************************************************
 *
 * Description:
 *    The first function to execute 
 *
 ****************************************************************************/
int
main(void)
{
  tU8 error;

  // immediately turn off buzzer (if connected)
  IODIR0 |= 0x00000080;
  IOSET0  = 0x00000080;
  
  osInit();
  osCreateProcess(initProc, initStack, INIT_STACK_SIZE, &pidMain, 1, NULL, &error);
  osStartProcess(pidMain, &error);
  
  osStart();
  return 0;
}

/*****************************************************************************
 *
 * Description:
 *    Updates LCD to inform about number of lives left
 *
 * Params:
 *    none
 *
 ****************************************************************************/
void
updateLCD(void)
{
  char LCDMessage[] = "Lives left: 0";
  LCDMessage[12] += lives;
  messageOnLCD(LCDMessage, FALSE);
}

void
winnerMessage(void)
{
  char text[] = "Avg:      . ms\nP0.14 = new game";
  char console[] = "Average time:     . ms\n";
  tU32 tmp = sumResult << 1;
  tU8 i = 9;
  
  text[11] = '0' + (tmp % 10);
  console[19] = text[11];
  tmp /= 10;
  
  while(tmp) {
    text[i--] = '0' + (tmp % 10);
    console[i + 9] = text[i + 1];
    tmp /= 10;
  }
  
  messageOnLCD(text, TRUE);
  consoleMessage(console);
}

/*****************************************************************************
 *
 * Description:
 *    A process entry function 
 *
 * Params:
 *    [in] arg - This parameter is not used in this application. 
 *
 ****************************************************************************/
static void
procMain(void* arg)
{
  tU8 error;
  
  osCreateProcess(procRand, procRandStack, PROC_RAND_STACK_SIZE, &pidRand, 3, NULL, &error);
  osStartProcess(pidRand, &error);
  
  
  while(TRUE)
  {
    
    if(state == 0)
    {
      testRGB(0, 0, 0);
      messageOnLCD("Press P0.14 \nto start game.", TRUE);
    }
    else if(state == 1)
    {
      testRGB(0, 0, 255);
      winnerMessage();
      testMotor();
    }
    else if(state == 2)
    {
      testRGB(255, 0, 0);
      messageOnLCD("Game over!\nP0.14 = new game", TRUE);
      consoleMessage("GAME OVER!\n");
    }
    
    consoleMessage("\nWaiting for a new game (press P0.14 button).\n");
	
	/* read input from P0.14 */
	IODIR &= ~0x00004000;
	
	/* wait to start game */
    while(inGame == FALSE)
    {
      if ((IOPIN & 0x00004000) == 0)
      {
	    score = 0;
	    request = 8;
	    lives = 2;
	    sumResult = 0;
	    testRGB(0, 255, 0);
	    updateLCD();
	    inGame = TRUE;
      }

      osSleep(2);
    }
    
    osCreateProcess(procLeds, procLedsStack, PROC_RAND_STACK_SIZE, &pidLeds, 3, NULL, &error);
    osStartProcess(pidLeds, &error);
    
    osCreateProcess(procJoy, procJoyStack, PROC_RAND_STACK_SIZE, &pidJoy, 3, NULL, &error);
    osStartProcess(pidJoy, &error);
    
    while(inGame)
    {
      osSleep(2);
    }
    
    osSleep(200);
  }
}

/*****************************************************************************
 *
 * Description:
 *    A process entry function 
 *
 * Params:
 *    [in] arg - This parameter is not used in this application. 
 *
 ****************************************************************************/
static void
procRand(void* arg)
{
  while(TRUE)
  {
    random = (random ^ getRandBits(16)) + 1;
    osSleep(43); /* 43 - just a nice small prime */
  }
}
/*****************************************************************************
 *
 * Description:
 *    Print a single reaction time.
 *
 * Params:
 *    [in] r - reaction time in ticks.
 *
 ****************************************************************************/
void
printSingleResult(tU32 result)
{
  tU32 millis = result << 1;
  char template[] = "Single reaction time:     ms\n";
  tU8 i = 25;
  
  while(millis)
  {
    template[i] = (millis % 10) + '0';
    millis /= 10;
    i--;
  }
  
  consoleMessage(template);
}

/*****************************************************************************
 *
 * Description:
 *    Reaction for a joystick moves.
 *
 * Params:
 *    [in] r - the state suggested by joystick.
 *
 ****************************************************************************/
void
handleCorrectRequest(tU8 r)
{
  tU32 result;
  if(request == r)
  {
    result = ticks - requestTime;
    printSingleResult(result);
    score += 1;
    sumResult += result;
    request = 8;
    if(score >= 10)
    {
      state = 1;
      inGame = FALSE;
    }
  }
  else
  {
    lives -= 1;
    if(lives == 0)
    {
      testRGB(255, 0, 0);
      state = 2;
      inGame = FALSE;
    }
    else
    {
      consoleMessage("Whoops! Lives left: 1.\n");
      testRGB(255, 255, 0);
      updateLCD();
    }
  }
}

/*****************************************************************************
 *
 * Description:
 *    A process entry function 
 *
 * Params:
 *    [in] arg - This parameter is not used in this application. 
 *
 ****************************************************************************/
static void
procJoy(void* arg)
{
  tU32 oldIOPIN, tmp;
  
  /* read input from joystick */
  IODIR &= ~0x001f0000;
  
  /* joystick support */
  while(inGame)
  {
    oldIOPIN = IOPIN;
    osSleep(2);
    
    tmp = (oldIOPIN & ~IOPIN) & 0x001f0000;
    
    if(tmp != 0x00000000) {
      if(tmp == 0x00020000) handleCorrectRequest(1);
      else if(tmp == 0x00040000) handleCorrectRequest(2);
      else if(tmp == 0x00100000) handleCorrectRequest(3);
      else if(tmp == 0x00080000) handleCorrectRequest(4);
      else if(tmp == 0x00010000) handleCorrectRequest(5);
    }
  }
  
  osDeleteProcess();
}

/*****************************************************************************
 *
 * Description:
 *    A process entry function 
 *
 * Params:
 *    [in] arg - This parameter is not used in this application. 
 *
 ****************************************************************************/
static void
procLeds(void* arg)
{
  testLedMatrix();
  osDeleteProcess();
}

/*****************************************************************************
 *
 * Description:
 *    The entry function for the initialization process. 
 *
 * Params:
 *    [in] arg - This parameter is not used in this application. 
 *
 ****************************************************************************/
static void
initProc(void* arg)
{
  tU8 error;

  eaInit();
  osCreateProcess(procMain, procMainStack, PROC_MAIN_STACK_SIZE, &pidMain, 3, NULL, &error);
  osStartProcess(pidMain, &error);

  osDeleteProcess();
}

/*****************************************************************************
 *
 * Description:
 *    The timer tick entry function that is called once every timer tick
 *    interrupt in the RTOS. Observe that any processing in this
 *    function must be kept as short as possible since this function
 *    execute in interrupt context.
 *
 * Params:
 *    [in] elapsedTime - The number of elapsed milliseconds since last call.
 *
 ****************************************************************************/
void
appTick(tU32 elapsedTime)
{
}
