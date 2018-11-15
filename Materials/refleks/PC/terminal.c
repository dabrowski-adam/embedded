#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

/* User dialog needed to choose the right COM port. */
unsigned int
portChoiceDialog()
{
  unsigned int portNumber;
  unsigned char loop = 1;
  printf("Put the COM port number (1 for COM1, 2 for COM2, etc.): ");
  fflush(stdout);
  
  while(loop)
  {
    scanf("%u", &portNumber);
    if(1 <= portNumber && portNumber <= 255)
    {
      loop = 0;
    }
    else
    {
      printf("Put the correct COM port number again [1-255]: ");
    }
  }
  
  printf("\n");
  
  return portNumber;
}

/* Setting the connection up. */
unsigned int
connect(HANDLE *handlePtr, char *portName)
{
  unsigned int ret;
  DCB controlBlock;
  COMMTIMEOUTS timeouts;
  
  (*handlePtr) = CreateFile(portName, GENERIC_WRITE | GENERIC_READ,
      0, NULL, OPEN_EXISTING, 0, NULL);
  
  GetCommState(*handlePtr, &controlBlock);
  controlBlock.DCBlength = sizeof(DCB);
  controlBlock.fParity = NOPARITY;
  controlBlock.StopBits = ONESTOPBIT;
  controlBlock.fBinary = FALSE;
  controlBlock.BaudRate = CBR_38400;
  controlBlock.fDtrControl = DTR_CONTROL_DISABLE;
  controlBlock.fDsrSensitivity = FALSE;
  controlBlock.fOutxCtsFlow = FALSE; 
  controlBlock.fOutxDsrFlow = FALSE;
  controlBlock.fTXContinueOnXoff = TRUE;
  controlBlock.fOutX = FALSE;
  controlBlock.fInX = FALSE;
  controlBlock.fErrorChar = FALSE;
  controlBlock.fNull = FALSE;
  controlBlock.fRtsControl = RTS_CONTROL_DISABLE;
  controlBlock.fAbortOnError = FALSE;
  SetCommState(*handlePtr, &controlBlock);
  
  GetCommTimeouts(*handlePtr, &timeouts);
  timeouts.WriteTotalTimeoutConstant = 0;
  timeouts.WriteTotalTimeoutMultiplier = 0;
  timeouts.ReadTotalTimeoutConstant = 1;
  timeouts.ReadTotalTimeoutMultiplier = 0;
  timeouts.ReadIntervalTimeout = MAXDWORD;
  SetCommTimeouts(*handlePtr, &timeouts);
  
  ret = SetCommState(*handlePtr, &controlBlock);
  
  EscapeCommFunction(*handlePtr, CLRDTR);
  EscapeCommFunction(*handlePtr, CLRRTS);
  Sleep(500);
  
  return ret;
}

/* Receive some data. */
void
receiveData(HANDLE *handlePtr, char **bufferPtr, size_t *bufferSizePtr)
{
  char ch;
  unsigned int loop = 1;
  unsigned int pos = 0;
  DWORD count = 0;
  while(loop)
  {
    ReadFile(*handlePtr, &ch, 1, &count, NULL);
    
    if(pos >= *bufferSizePtr)
    {
      (*bufferSizePtr) = ((*bufferSizePtr) << 1);
      (*bufferPtr) = realloc(*bufferPtr, *bufferSizePtr);
    }
      
    if(count != 0)
    {
      if(ch == '$' && pos > 0 && (*bufferPtr)[pos - 1])
      {
        loop = 0;
        (*bufferPtr)[pos] = '\0';
      }
      else
      {
        (*bufferPtr)[pos] = ch;
        pos++;
      }
    }
    else
    {
      Sleep(10);
    }
  }
}

/* Hash function. */
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

/* String receiving protocol. */
void
receiveString(HANDLE *handlePtr, char **bufferPtr, size_t *bufferSizePtr)
{
  unsigned int localHash, remoteHash;
  char *hashBuffer = malloc(1);
  size_t hashBufferSize = 1;
  
  receiveData(handlePtr, bufferPtr, bufferSizePtr);
  receiveData(handlePtr, &hashBuffer, &hashBufferSize);
  sscanf(hashBuffer, "%u", &remoteHash);
  localHash = calculateHash(*bufferPtr);
  
  if(localHash != remoteHash)
  {
    fprintf(stderr, "CRITICAL: Data transmission error! (%u != %u).\n", localHash, remoteHash);
    exit(EXIT_FAILURE);
  }
  
  free(hashBuffer);
  
  printf("%s", *bufferPtr);
}

/* The main procedure. */
int
main(void)
{
  HANDLE handle;
  int portNumber;
  char portName[7];
  char *buffer = malloc(1);
  size_t bufferSize = 1;
  
  portNumber = portChoiceDialog();
  sprintf(portName, "COM%u", portNumber);
  
  if(connect(&handle, portName))
  {
    while(1)
    {
      receiveString(&handle, &buffer, &bufferSize);
    }
  }
  else
  {
    fprintf(stderr, "Connection failed.\n");
  }
  
  CloseHandle(handle);
  
  free(buffer);
  
  return EXIT_SUCCESS;
}
