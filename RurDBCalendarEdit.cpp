//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "RurDBCalendarEdit.h"
#include "RurDatumDlg.h"
#include <dateutils.hpp>
//#pragma resource "*.res"
#pragma resource "sipka.res"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TRurDBCalendarEdit *)
{
        new TRurDBCalendarEdit(NULL);
}
//---------------------------------------------------------------------------
__fastcall TRurDBCalendarEdit::TRurDBCalendarEdit(TComponent* Owner)
        : TCustomEdit(Owner)
{
Width=121;
Height=21;
TabStop=true;
FSipka=new TSpeedButton(this);
FSipka->Transparent=false;
FSipka->Align=alRight;
FSipka->Parent=this;
FSipka->Glyph->LoadFromResourceName((int)HInstance,"SIPKA");
FSipka->Width=16;
FSipka->OnClick=SipkaClick;
FDataLink=new TFieldDataLink();
FDataLink->Control=this;
FDataLink->OnDataChange=DataChange;
FDataLink->OnUpdateData=UpdateData;
//FReadOnly=false;
}

__fastcall TRurDBCalendarEdit::~TRurDBCalendarEdit()
{
FDataLink->Control=NULL;
FDataLink->OnDataChange=NULL;
FDataLink->OnUpdateData=NULL;
delete FDataLink;
}

void __fastcall TRurDBCalendarEdit::SetEditRect(void)
{
TRect Loc;
SendMessage(Handle, EM_GETRECT, 0, long(&Loc));
//Loc.Bottom = ClientHeight + 1;  // +1 is workaround for windows paint bug
Loc.Right = ClientWidth - FSipka->Width - 2;
Loc.Top = 0;
Loc.Left = 0;
SendMessage(Handle, EM_SETRECTNP, 0, long(&Loc));
}

void __fastcall TRurDBCalendarEdit::CreateParams(TCreateParams &Params)
{
TCustomEdit::CreateParams(Params);
Params.Style |=  ES_MULTILINE | WS_CLIPCHILDREN;
}

void __fastcall TRurDBCalendarEdit::CreateWnd()
{
TCustomEdit::CreateWnd();
SetEditRect();
}

MESSAGE void __fastcall TRurDBCalendarEdit::WMSize(TWMSize &Message)
{
if (FSipka != NULL) {
  if (NewStyleControls)
    FSipka->SetBounds(Width - FSipka->Width - 5, 0, FSipka->Width, Height - 5);
  else FSipka->SetBounds (Width - FSipka->Width, 0, FSipka->Width, Height);
  SetEditRect();
};
}

// zobrazim dialog
void __fastcall TRurDBCalendarEdit::SipkaClick(TObject *Sender)
{
TDateTime t1=DateOf(FDataLink->Field->AsDateTime);
TDateTime t2=TimeOf(FDataLink->Field->AsDateTime);
TDatumDlg *dd=new TDatumDlg(this);
dd->cal->Datum=t1;
if(dd->ShowModal()==mrOk)
  FDataLink->Field->AsDateTime=dd->cal->Datum+t2;
delete dd;
}

TField* __fastcall TRurDBCalendarEdit::GetField()
{
return FDataLink->Field;
}

AnsiString __fastcall TRurDBCalendarEdit::GetDataField()
{
return FDataLink->FieldName;
}

TDataSource* __fastcall TRurDBCalendarEdit::GetDataSource()
{
return FDataLink->DataSource;
}

void __fastcall TRurDBCalendarEdit::SetDataField(AnsiString Value)
{
FDataLink->FieldName=Value;
}

void __fastcall TRurDBCalendarEdit::SetDataSource(TDataSource *Value)
{
if(Value!=NULL)
  Value->FreeNotification(this);
FDataLink->DataSource=Value;
}

void __fastcall TRurDBCalendarEdit::DataChange(TObject *Sender)
{
if(FDataLink->Field==NULL)
  {
  if(ComponentState.Contains(csDesigning))
    Text="Field: none";
  else
    Text="";
  }
else
  {
  if(ComponentState.Contains(csDesigning))
    Text="Field: "+FDataLink->Field->FieldName;
  else
    Text=FDataLink->Field->AsString;
  }
}

void __fastcall TRurDBCalendarEdit::UpdateData(TObject *Sender)
{
FDataLink->Field->Text=Text;
}

void __fastcall TRurDBCalendarEdit::Change(void)
{
if(FDataLink != NULL)
  FDataLink->Modified();
TCustomEdit::Change();  
}

void __fastcall TRurDBCalendarEdit::CMExit(TWMNoParams Message)
{
try {
  UpdateData(NULL);
  FDataLink->UpdateRecord();
} catch(...) {
  SetFocus();
  throw;
}
}

//---------------------------------------------------------------------------
namespace Rurdbcalendaredit
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TRurDBCalendarEdit)};
                 RegisterComponents("Rur", classes, 0);
        }
}
//---------------------------------------------------------------------------
