//----------------------------------------------------------------------------
#ifndef RurDatumDlgH
#define RurDatumDlgH
//----------------------------------------------------------------------------

#include "RurCalendar.h"
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
//----------------------------------------------------------------------------
class TDatumDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
        TRurCalendar *cal;
        TButton *ZBtn;
        void __fastcall calCellClick(TObject *Sender);
        void __fastcall OKBtnClick(TObject *Sender);
        void __fastcall ZBtnClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
private:
public:
	virtual __fastcall TDatumDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TDatumDlg *DatumDlg;
//----------------------------------------------------------------------------
#endif    
