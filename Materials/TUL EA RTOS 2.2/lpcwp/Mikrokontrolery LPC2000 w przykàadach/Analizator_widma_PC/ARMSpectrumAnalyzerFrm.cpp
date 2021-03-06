//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ARMSpectrumAnalyzerFrm.h"

///
#include <math.h>
///
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmARMSpectrumAnalyzer *frmARMSpectrumAnalyzer;
//---------------------------------------------------------------------------
__fastcall TfrmARMSpectrumAnalyzer::TfrmARMSpectrumAnalyzer(TComponent* Owner)
        : TForm(Owner)
{
        this->pCom=new CCommInterface();
        
        this->DataTable=new unsigned long[NUMBER_OF_POINTS];
        this->clBackgroung=clWhite;
        this->clLine=clNavy;
        this->clVoltageMarker=clLime;
        this->clFreqMarker=clRed;
        this->speedbtnBkgColors->Font->Color=this->clBackgroung;
        this->speedbtnLinesColor->Font->Color=this->clLine;

        this->FreqMarkerPos=0;
        this->VoltageMarkerPos=0;

        //Clear DataTable
        for(unsigned long i=0;i<NUMBER_OF_POINTS;i++)
                this->DataTable[i]=0;
}

//---------------------------------------------------------------------------
void __fastcall TfrmARMSpectrumAnalyzer::FormCreate(TObject *Sender)
{
        this->Caption=Application->Title;

        if(this->pCom->Open("COM1",/*57600*/115200)==false)
        {
                Application->MessageBoxA("Port opening error.\nApplication will terminate.","Error",0);
                Application->Terminate();
        }

        this->pSpectrumThread=new TSpectrumListeningThread(this);
        this->pSpectrumThread->Priority=tpHighest;
        this->pSpectrumThread->FreeOnTerminate=true;

        //Start communication thread
        this->pSpectrumThread->bRun=true;
        this->pSpectrumThread->Resume();

        if(this->pSpectrumThread->Suspended==true)
                this->speedbtnRun->Down=false;
        else
                this->speedbtnRun->Down=true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmARMSpectrumAnalyzer::paintboxFFTPaint(TObject *Sender)
{
        this->pnlFFT->Color=this->clBackgroung;
        //Draw outer frame
        /*this->paintboxFFT->Canvas->Pen->Color=clWhite;
        this->paintboxFFT->Canvas->MoveTo(0,0);
        this->paintboxFFT->Canvas->LineTo(this->paintboxFFT->Width-1,0);

        this->paintboxFFT->Canvas->MoveTo(this->paintboxFFT->Width-1,0);
        this->paintboxFFT->Canvas->LineTo(this->paintboxFFT->Width-1,this->paintboxFFT->Height-1);

        this->paintboxFFT->Canvas->MoveTo(0,this->paintboxFFT->Height-1);
        this->paintboxFFT->Canvas->LineTo(this->paintboxFFT->Width-1,this->paintboxFFT->Height-1);

        this->paintboxFFT->Canvas->MoveTo(0,0);
        this->paintboxFFT->Canvas->LineTo(0,this->paintboxFFT->Height-1);*/

        //Draw function
        this->paintboxFFT->Canvas->Pen->Color=this->clLine;;
        for(unsigned long i=0;i<NUMBER_OF_POINTS/2;i++)
        {
                unsigned long valY_pix=(this->DataTable[i]*paintboxFFT->Height)/(1<<q);
                if(i<(NUMBER_OF_POINTS/2-1))
                {
                        unsigned long valY_pix_next=(this->DataTable[i+1]*paintboxFFT->Height)/(1<<q);
                        this->paintboxFFT->Canvas->MoveTo(4*i,this->paintboxFFT->Height-1-valY_pix);
                        this->paintboxFFT->Canvas->LineTo(4*(i+1),this->paintboxFFT->Height-1-valY_pix_next);
                }
        }

        this->DrawMarker(3,pmNotXor);
}
//---------------------------------------------------------------------------

void __fastcall TfrmARMSpectrumAnalyzer::paintboxFFTMouseMove(
      TObject *Sender, TShiftState Shift, int X, int Y)
{
        if(X<0) X=0;
        if(X>paintboxFFT->Width) X=paintboxFFT->Width;

        float fYpix=this->paintboxFFT->Height-1-Y;
        float fX=20.0*X/512;
        this->lbX->Caption="X: "+AnsiString::FormatFloat("0.00",fX)+"kHz";
        this->lbY->Caption="Y: "+AnsiString::FormatFloat("0.00",fYpix/paintboxFFT->Height)+"V";

        if(Shift.Contains(ssLeft))
        {
                //Freq marker
                this->DrawMarker(2,pmNotXor);
                this->FreqMarkerPos=X;
                this->DrawMarker(2,pmNotXor);
        }

        if(Shift.Contains(ssRight))
        {
                //Voltage marker
                this->DrawMarker(1,pmNotXor);
                this->VoltageMarkerPos=this->paintboxFFT->Height-1-Y;
                this->DrawMarker(1,pmNotXor);
        }

        float fVpix=(float)VoltageMarkerPos;
        float fF=20.0*FreqMarkerPos/512;
        this->lbF->Caption="F: "+AnsiString::FormatFloat("0.00",fF)+"kHz";
        this->lbV->Caption="V: "+AnsiString::FormatFloat("0.00",fVpix/paintboxFFT->Height)+"V";
}
//---------------------------------------------------------------------------

void __fastcall TfrmARMSpectrumAnalyzer::BtnColorsClick(
      TObject *Sender)
{
        this->pSpectrumThread->Suspend();
        if(this->colordialogColors->Execute())
        {
                TSpeedButton *button=(TSpeedButton*)Sender;
                TColor clr=this->colordialogColors->Color;

                if(button->Name=="speedbtnLinesColor")
                        this->clLine=clr;
                else if(button->Name=="speedbtnBkgColors")
                        this->clBackgroung=clr;

                button->Font->Color=clr;
                this->paintboxFFT->Repaint();
        }
        this->pSpectrumThread->Resume();
}
//---------------------------------------------------------------------------


void __fastcall TfrmARMSpectrumAnalyzer::FormMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
        this->lbX->Caption="X: ";
        this->lbY->Caption="Y: ";
}
//---------------------------------------------------------------------------



void TfrmARMSpectrumAnalyzer::DataReady(void)
{
        //Update information about window
        if(this->WinType==wtNone)
                this->pnlWindowType->Caption="Window: None";
        else if(this->WinType==wtHanning)
                this->pnlWindowType->Caption="Window: Hanning";
        else if(this->WinType==wtHamming)
                this->pnlWindowType->Caption="Window: Hamming";

        //Computing sqare roots of DataTable
        for(int i=0;i<NUMBER_OF_POINTS;i++)
        {
                float fl_data=(float)DataTable[i];
                float fl_sqrt=sqrt(fl_data*(1<<q));
                DataTable[i]=(int)fl_sqrt;
        }
        this->paintboxFFT->Repaint();
}

void __fastcall TfrmARMSpectrumAnalyzer::speedbtnRunClick(TObject *Sender)
{
        if(this->speedbtnRun->Down==true)
                this->pSpectrumThread->Resume();
        else
                this->pSpectrumThread->Suspend();
}
//---------------------------------------------------------------------------


//marker.0 = 1 -> VoltageMarker
//marker.1 = 1 -> FreqMarker
void TfrmARMSpectrumAnalyzer::DrawMarker(int marker, TPenMode pm)
{
        TColor PenClrMem=this->paintboxFFT->Canvas->Pen->Color;

        this->paintboxFFT->Canvas->Pen->Mode=pm;

        if(marker&1)
        {
                //Voltage marker
                this->paintboxFFT->Canvas->Pen->Color=clVoltageMarker;
                this->paintboxFFT->Canvas->MoveTo(0,this->paintboxFFT->Height-1-VoltageMarkerPos);
                this->paintboxFFT->Canvas->LineTo(this->paintboxFFT->Width-1,this->paintboxFFT->Height-1-VoltageMarkerPos);
        }

        if(marker&2)
        {
                //Frequency marker
                this->paintboxFFT->Canvas->Pen->Color=clFreqMarker;
                this->paintboxFFT->Canvas->MoveTo(FreqMarkerPos,0);
                this->paintboxFFT->Canvas->LineTo(FreqMarkerPos,this->paintboxFFT->Height-1);
        }

        this->paintboxFFT->Canvas->Pen->Mode=pmCopy;
        this->paintboxFFT->Canvas->Pen->Color=PenClrMem;
}

void __fastcall TfrmARMSpectrumAnalyzer::FormClose(TObject *Sender,
      TCloseAction &Action)
{
        if(this->pSpectrumThread->bRun==true)
                this->pSpectrumThread->bRun=false;
        this->pCom->Close();
        
        Action=caFree;
}
//---------------------------------------------------------------------------

void __fastcall TfrmARMSpectrumAnalyzer::paintboxFFTMouseDown(
      TObject *Sender, TMouseButton Button, TShiftState Shift, int X,
      int Y)
{
        this->paintboxFFTMouseMove(Sender,Shift,X,Y);
}
//---------------------------------------------------------------------------

