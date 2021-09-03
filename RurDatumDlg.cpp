//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "RurDatumDlg.h"
#include <dateutils.hpp>
//---------------------------------------------------------------------
#pragma link "RurCalendar"
#pragma resource "*.dfm"
TDatumDlg *DatumDlg;
//--------------------------------------------------------------------- 
__fastcall TDatumDlg::TDatumDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------
void __fastcall TDatumDlg::calCellClick(TObject *Sender)
{
cal->Invalidate();
ModalResult=mrOk;
}
//---------------------------------------------------------------------------



void __fastcall TDatumDlg::OKBtnClick(TObject *Sender)
{
cal->Today();
}
//---------------------------------------------------------------------------

void __fastcall TDatumDlg::ZBtnClick(TObject *Sender)
{
cal->Datum=IncDay(Date());
ModalResult=mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TDatumDlg::FormShow(TObject *Sender)
{
OKBtn->Hint=Date().DateString();
ZBtn->Hint=TDateTime(Date()+1).DateString();
}
//---------------------------------------------------------------------------

