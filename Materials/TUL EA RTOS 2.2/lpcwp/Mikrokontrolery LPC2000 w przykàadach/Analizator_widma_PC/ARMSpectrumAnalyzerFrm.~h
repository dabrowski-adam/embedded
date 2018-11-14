//---------------------------------------------------------------------------

#ifndef ARMSpectrumAnalyzerFrmH
#define ARMSpectrumAnalyzerFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>

#include "CommInterface.h"
#include "SpectrumListeningThread.h"
#include "WindowType.h"
//---------------------------------------------------------------------------
class TfrmARMSpectrumAnalyzer : public TForm
{
__published:	// IDE-managed Components
        TPanel *pnlFFT;
        TPaintBox *paintboxFFT;
        TPanel *pnlXY;
        TLabel *lbY;
        TLabel *lbX;
        TSpeedButton *speedbtnBkgColors;
        TColorDialog *colordialogColors;
        TSpeedButton *speedbtnLinesColor;
        TPanel *pnlWindowType;
        TSpeedButton *speedbtnRun;
        TLabel *lbF;
        TLabel *lbV;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall paintboxFFTPaint(TObject *Sender);
        void __fastcall paintboxFFTMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall BtnColorsClick(TObject *Sender);
        void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall speedbtnRunClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall paintboxFFTMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
private:
        TColor clBackgroung;	// User declarations
        TColor clLine;
        CCommInterface* pCom;
        TSpectrumListeningThread* pSpectrumThread;
        unsigned int FreqMarkerPos;	// User declarations
        unsigned int VoltageMarkerPos;
        TColor clVoltageMarker;
        TColor clFreqMarker;
        void DrawMarker(int marker, TPenMode pm);	// User declarations
public:
        unsigned long* DataTable;
        CWindowType WinType;
        static const unsigned long NUMBER_OF_POINTS=256;
        static const unsigned long q=11;
        __fastcall TfrmARMSpectrumAnalyzer(TComponent* Owner);
        void DataReady(void);

        //Friend thread functions
        friend void TSpectrumListeningThread::Execute(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmARMSpectrumAnalyzer *frmARMSpectrumAnalyzer;
//---------------------------------------------------------------------------
#endif
