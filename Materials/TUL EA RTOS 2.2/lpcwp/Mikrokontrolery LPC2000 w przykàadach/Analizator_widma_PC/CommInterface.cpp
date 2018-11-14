//---------------------------------------------------------------------------
// ClassExplorer Pro generated source file
// Created by Administrator on 2005-02-20, 13:24:57
//---------------------------------------------------------------------------
#include "CommInterface.h"
//#include <vcl.h>
#include <vcl\Windows.hpp>

//---------------------------------------------------------------------------


CCommInterface::CCommInterface()
{
        //TODO: Add your source code here
        this->cbInQueue=4096;
        this->cbOutQueue=1024;
        this->lpFileName="COM1";
        this->BaudRate=CBR_9600;
        this->hCommDev=NULL;
}


bool CCommInterface::Open(string CommID, unsigned long Baud)
{
        DCB     dcb;

        this->lpFileName=CommID.begin();
        this->BaudRate=Baud;

        hCommDev = CreateFile(lpFileName, GENERIC_READ |
                          GENERIC_WRITE, 0, NULL,
                          OPEN_EXISTING, 0, NULL);

        if (hCommDev != INVALID_HANDLE_VALUE)
        {
                dcb.DCBlength = sizeof(dcb);    // aktualny rozmiar dcb

                GetCommState(hCommDev, &dcb);
                dcb.BaudRate=this->BaudRate;
                //--parametry komunikacyjne-------
                dcb.Parity = NOPARITY;       // ustawienie parzystoœci
                dcb.StopBits = ONESTOPBIT;    // bity stopu
                dcb.ByteSize = 8;             // bity danych
                //--przyk³adowe ustawienia flag steruj¹cych DCB----
                dcb.fParity = FALSE;            // sprawdzanie parzystoœci
                dcb.fDtrControl = DTR_CONTROL_DISABLE;
                // sygna³ DTR - stan nieaktywny
                dcb.fRtsControl = RTS_CONTROL_DISABLE;
                // RTS - stan nieaktywny
                dcb.fOutxCtsFlow = FALSE;
                dcb.fOutxDsrFlow = FALSE;
                dcb.fDsrSensitivity = FALSE;
                dcb.fAbortOnError = FALSE;
                dcb.fOutX = FALSE;
                dcb.fInX = FALSE;
                dcb.fErrorChar = FALSE;
                dcb.fNull = FALSE;
                SetCommState(hCommDev,&dcb);
                SetupComm(hCommDev, cbInQueue, cbOutQueue);
        }
        else
                return false;

        return true;
}


void CCommInterface::Close(void)
{
        if((hCommDev != INVALID_HANDLE_VALUE) && (hCommDev > 0))
                CloseHandle(hCommDev);
}


unsigned long CCommInterface::Write(const void* Buffer, unsigned long NumberOfBytesToWrite)
{
        unsigned long NumberOfBytesWritten;
        WriteFile(hCommDev,Buffer,NumberOfBytesToWrite, &NumberOfBytesWritten, NULL);
        return NumberOfBytesWritten;
}


bool CCommInterface::Read(void * Buffer, unsigned long * lpNumberOfBytesRead, unsigned long NumberOfBytesToRead)
{
        unsigned long nNumberOfBytesToRead;

        if (NumberOfBytesToRead>this->cbInQueue)
                nNumberOfBytesToRead=this->cbInQueue;
        else
                nNumberOfBytesToRead=NumberOfBytesToRead;
        return ReadFile(hCommDev,Buffer,nNumberOfBytesToRead,lpNumberOfBytesRead, NULL);
}


void CCommInterface::CheckCommInput(unsigned long * lpErrors, unsigned long * lpInQue)
{
        COMSTAT Stat;
        ClearCommError(hCommDev,lpErrors,&Stat);
        *lpInQue=Stat.cbInQue;
}


void CCommInterface::ClearComm(void)
{
        PurgeComm(hCommDev,PURGE_RXCLEAR);
        PurgeComm(hCommDev,PURGE_TXCLEAR);
}

