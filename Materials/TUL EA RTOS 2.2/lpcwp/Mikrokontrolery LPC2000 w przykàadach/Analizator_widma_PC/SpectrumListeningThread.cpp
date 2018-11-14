//---------------------------------------------------------------------------
// ClassExplorer Pro generated source file
// Created by Administrator on 2005-05-03, 18:57:30
//---------------------------------------------------------------------------
#include "SpectrumListeningThread.h"

#include "ARMSpectrumAnalyzerFrm.h"
#include "WindowType.h"
//---------------------------------------------------------------------------


__fastcall TSpectrumListeningThread::TSpectrumListeningThread(void * pfrm) : TThread(true)
{
        this->pMainFrm=pfrm;
        bRun=false;
}


void __fastcall TSpectrumListeningThread::Execute()
{
        TfrmARMSpectrumAnalyzer *p_frm=(TfrmARMSpectrumAnalyzer*)pMainFrm;
        unsigned long Error,InQue,BRead;
        unsigned char *Input;

        while(bRun==true)
        {
                //Send request
                unsigned char byte=0x01;
                p_frm->pCom->Write(&byte,1);

                //Break time
                Sleep(300);

                //Check response
                p_frm->pCom->CheckCommInput(&Error,&InQue);
                if(Error!=0)
                {
                        //Application->MessageBoxA("Comunication error","Error",0);
                        p_frm->pCom->ClearComm();
                        //Beep(2000,50);
                        continue;
                }

                if(InQue>=1027)
                {
                        Input=new unsigned char[InQue];
                        p_frm->pCom->Read(Input,&BRead,InQue);

                        //Find 0xAA
                        unsigned long i;
                        for(i=0;i<InQue;i++)
                                if(Input[i]==0xAA) break;

                        //If 0xAA on right position
                        if((InQue-i)>=1027)
                                this->ProcessReceivedData(Input,i);

                        delete [] Input;
                }
                else
                {
                        //No response
                        p_frm->WinType=wtNone;
                        for(int i=0;i<TfrmARMSpectrumAnalyzer::NUMBER_OF_POINTS;i++)
                                p_frm->DataTable[i]=0;
                        Synchronize(DataReady);
                }
        }
}


void __fastcall TSpectrumListeningThread::DataReady(void)
{
        TfrmARMSpectrumAnalyzer *p_frm=(TfrmARMSpectrumAnalyzer*)pMainFrm;
        p_frm->DataReady();
}


void TSpectrumListeningThread::ProcessReceivedData(unsigned char* Input, unsigned long i)
{
        TfrmARMSpectrumAnalyzer *p_frm=(TfrmARMSpectrumAnalyzer*)pMainFrm;

        unsigned long j;
        unsigned char summa=0;

        //Checking checksum
        for(j=i;j<(i+1027);j++)
                summa+=Input[j];

        if((summa&0xFF)!=0)
                return;

        //Checksum OK
        for(j=i;j<(i+1026);j++)
        {
                unsigned char ch=Input[j];
                if(j==(i+1))
                {
                        p_frm->WinType=(CWindowType)ch;
                        continue;
                }
                if(j>(i+1))
                {
                        unsigned long k=j-(i+2);
                        unsigned long WordNr=k/4;
                        unsigned long ByteNr=3-(k%4);

                        p_frm->DataTable[WordNr]&=~(0xFF<<(8*ByteNr));
                        p_frm->DataTable[WordNr]|=ch<<(8*ByteNr);
                }
        }
        Synchronize(DataReady);
}

