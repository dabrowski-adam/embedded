//---------------------------------------------------------------------------
// ClassExplorer Pro generated header file
// Created by Administrator on 2005-05-03, 18:57:30
//---------------------------------------------------------------------------
#ifndef SpectrumListeningThreadH
#define SpectrumListeningThreadH
#include <Classes.hpp>

//---------------------------------------------------------------------------
class TSpectrumListeningThread : public TThread
{
private:
        void * pMainFrm;
        void __fastcall DataReady(void);
        void ProcessReceivedData(unsigned char* Input, unsigned long i);
protected:
        void __fastcall Execute();
public:
        bool bRun;
        __fastcall TSpectrumListeningThread(void * pdbgm);
};

//---------------------------------------------------------------------------
#endif
