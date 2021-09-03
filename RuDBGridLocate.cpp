//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "RuDBGridLocate.h"
#include "RuDBGrid.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TLocateDlg *LocateDlg;
static inline void ValidCtrCheck(TLocateDlg *)
{
        new TLocateDlg(NULL);
}

///
/// Konstruktor
///
__fastcall TLocateDlg::TLocateDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}

///
/// Inicializacia dialogu
///
void __fastcall TLocateDlg::FormShow(TObject *Sender)
{
for(int i=0;i<rd->Columns->Count;i++)
  {
  c->Items->Add(rd->Columns->operator [](i)->Title->Caption);
  }
c->ItemIndex = ii;
e->SetFocus();
e->SelStart = 1;
e->SelLength = 0;
eChange(Sender);
}

///
/// Vyhladanie v stlpci po zmene editu
///
void __fastcall TLocateDlg::eChange(TObject *Sender)
{
try {
  if(e->Text.Length() && c->ItemIndex>-1)
    {
    String key = rd->Columns->operator [](c->ItemIndex)->Field->FieldName;
    rd->DataSource->DataSet->Locate(key, e->Text, TLocateOptions()<<loPartialKey<<loCaseInsensitive);
    }
  } catch(...) {}  
}

///
/// Zmena v combe
/// Focus ide rovno na edit, aby mohol zase pisat
///
void __fastcall TLocateDlg::cChange(TObject *Sender)
{
eChange(Sender);
e->SetFocus();
}

