//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RuDBGrid.h"
//#include "RurTable.h"
#include <inifiles.hpp>
#include <clipbrd.hpp>
#if defined(RUR_DB_USE_ADO)
  #include <adodb.hpp>
  #define RUR_DB_DATASET Data::Win::Adodb::TADODataSet
#else
  #define RUR_DB_DATASET Firedac::Comp::Dataset::TFDDataSet
#endif
#include "RuDBGridLocate.h"
#include "RuDBGridColumns.h"
#include "RuDBGridViews.h"
#if defined(GRID_EVIDENCIE)
  #include "evidencie_tbl_cfg.h"
#endif
#include <strutils.hpp>
#include <System.IOUtils.hpp>
#include "JsonTemplates.h"
#include <xmldoc.hpp>
#include "xmltemplates.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TRurDBGrid *)
{
        new TRurDBGrid(NULL);
}



TRurDBGridConfigurator rdgc;
int GRID_RK_DELAY = 0;
String TRurDBGrid::view_path;

TRurDBGridView::TRurDBGridView()
{
is_system = false;
}

TRurDBGridView::TRurDBGridView(const TRurDBGridView &m)
{
name = m.name;
shortcut = m.shortcut;
columns = m.columns;
is_system = m.is_system;
}

void TRurDBGridView::operator=(const TRurDBGridView &m)
{
name = m.name;
shortcut = m.shortcut;
columns = m.columns;
is_system = m.is_system;
}

void TRurDBGridView::SetColumnWidth(String fieldname, int width)
{
for(vColumns::iterator i = columns.begin(); i!=columns.end(); i++) {
  if(i->fieldname==fieldname)
    i->width = width;
  }
}

///
/// \brief Konstruktor
///
__fastcall TRurDBGrid::TRurDBGrid(TComponent* Owner)
        : TSMDBGrid(Owner)
{
FTitleSort=true;
FKeyLocateRecord=true;
FSort=0;
FCommand=-1;
FVlastneMenu=NULL;
FOnAfterKeyLocate=NULL;
FOnBeforeKeyLocate=NULL;
FOnDelayedScroll=NULL;
FOnColumnsChanged = NULL;
FOnTitleClickSort = NULL;
FOnEmail=NULL;
FOnWWW=NULL;
FOnCellCheckClick=NULL;
FOnCellClick2=NULL;
FOnColEnable=NULL;
FOnGroupCreate = NULL;
FOnGroupEdit = NULL;
FOnSetupColumns = NULL;
trt=0; // nemam table
cfg=0; // nemam konfiguraciu
ds_timer=NULL;
roll_koliesko=0;
configcolumns=false;
loadedcolumns=false;
names=NULL;
namescount=0;
hide_indicator=false;
FOnExport=NULL;
for(int i=0;i<4;i++)
  mih[i] = NULL;
viewsItem = NULL;
user_views_activity = 0;
}

///
/// \brief Destruktor
///
__fastcall TRurDBGrid::~TRurDBGrid()
{
XMLSave();
if(ds_timer) {
  delete ds_timer;
  ds_timer=NULL;
  }
if(FVlastneMenu) {PopupMenu=NULL;delete FVlastneMenu;}
if(names)
  delete[] names;
}

// do ini v table si ulozi sirky
void TRurDBGrid::SaveSirky()
  {
#if defined(GRID_EVIDENCIE)
  if(trt==0 && cfg==0) return;
  if(Columns->State==csDefault) return; // nie je nastaveny
  if(cfg)
    {
    for(int i=0;i<Columns->Count;i++) // ulozim sirky
      {
      AnsiString a=Columns->Items[i]->FieldName;
      if(a.Length()) // obcas sa funkcia zavola, ked nie su columns
        cfg->ini->WriteInteger(RIniName,a,Columns->Items[i]->Width);
                     // na NT to sposobilo padnutie programu, ked bolo a 0
      }
    return;
    }
  TIniFile *ini=new TIniFile(trt->GetIniString());
  for(int i=0;i<Columns->Count;i++) // ulozim sirky
    {
    AnsiString a=Columns->Items[i]->FieldName;
    if(a.Length()) // obcas sa funkcia zavola, ked nie su columns
      ini->WriteInteger(RIniName,a,Columns->Items[i]->Width);
                   // na NT to sposobilo padnutie programu, ked bolo a 0
    }
  delete ini;
#endif
  }
// podla table si vytvori stlpce
void TRurDBGrid::CreateRColumns(TRurTable *tb,bool all)
{
#if defined(GRID_EVIDENCIE)
trt=tb; // aj si poznacim pointer
cfg=0;
SaveSirky();
Columns->Clear();
TRurTableSource *ts=trt->GetSource();
TRurTableSource *ts2=trt->GetAddTable() ? trt->GetAddTable()->GetSource() : 0;
TIniFile *ini=new TIniFile(trt->GetIniString());
bool robil_nieco=false;
if(all)
  goto iii;
for(int i=0;i<10;i++)
  {
  int n=tb->GetDispCol(i);
  if(n==-1) // sme na konci
    continue;
  robil_nieco=true;
  TColumn *cc=Columns->Add();
  RurTablePolozka &rp=(n>1000) ? (*ts2)[n-1000] : (*ts)[n];
  cc->Title->Caption=(*(rp.mpokec)) ? rp.mpokec : rp.pokec;
  cc->FieldName=rp.meno;
  int ss=ini->ReadInteger(RIniName,cc->FieldName,-1);
  if(ss!=-1) // ak uz nastavil
    cc->Width=ss;
  }
iii:
if(!robil_nieco) // hodim prvych 6 pouzitelnych
  {
  for(int i=0;i<ts->GetItemsInContainer() ;i++)
    {
    RurTablePolozka &rp=(*ts)[i];
    if(rp.visible)
      {
      TColumn *cc=Columns->Add();
      cc->Title->Caption=(*(rp.mpokec)) ? rp.mpokec : rp.pokec;
      cc->FieldName=rp.meno;
      int ss=ini->ReadInteger(RIniName,cc->FieldName,-1);
      if(ss!=-1) // ak uz nastavil
        cc->Width=ss;
      }
    }
  if(ts2)
  for(int i=0;i<ts2->GetItemsInContainer() ;i++)
    {
    RurTablePolozka &rp=(*ts2)[i];
    if(rp.visible)
      {
      TColumn *cc=Columns->Add();
      cc->Title->Caption=(*(rp.mpokec)) ? rp.mpokec : rp.pokec;
      cc->FieldName=rp.meno;
      int ss=ini->ReadInteger(RIniName,cc->FieldName,-1);
      if(ss!=-1) // ak uz nastavil
        cc->Width=ss;
      }
    }
  }
delete ini;
#endif
}

void TRurDBGrid::CreateRColumns(TDataSetCfg *_cfg,bool all)
{
#if defined(GRID_EVIDENCIE)
cfg=_cfg;
trt=0;
SaveSirky();
Columns->Clear();
TRurTableSource *ts=cfg->GetSource();
TIniFile *ini=cfg->ini;
bool robil_nieco=false;
if(all)
  goto iii;
for(int i=0;i<10;i++)
  {
  int n=cfg->GetDispCol(i);
  if(n==-1) // sme na konci
    continue;
  robil_nieco=true;
  TColumn *cc=Columns->Add();
  RurTablePolozka &rp=(*ts)[n];
  cc->Title->Caption=(*(rp.mpokec)) ? rp.mpokec : rp.pokec;
  cc->FieldName=rp.meno;
  int ss=ini->ReadInteger(RIniName,cc->FieldName,-1);
  if(ss!=-1) // ak uz nastavil
    cc->Width=ss;
  }
iii:
if(!robil_nieco) // hodim prvych 6 pouzitelnych
  {
  for(int i=0;i<ts->GetItemsInContainer() ;i++)
    {
    RurTablePolozka &rp=(*ts)[i];
    if(rp.visible)
      {
      TColumn *cc=Columns->Add();
      cc->Title->Caption=(*(rp.mpokec)) ? rp.mpokec : rp.pokec;
      cc->FieldName=rp.meno;
      int ss=ini->ReadInteger(RIniName,cc->FieldName,-1);
      if(ss!=-1) // ak uz nastavil
        cc->Width=ss;
      }
    }
  }
#endif
}

///
/// \brief Tabulka v podobe textu oddeleneho tabelatormi
///
AnsiString TRurDBGrid::GetAsText()
{
AnsiString tmp;
for(int i=0;i<Columns->Count;i++)
  {
  if(Columns->Items[i]->Visible)
    tmp+=Columns->Items[i]->Title->Caption+"\t";
  }
tmp+="\r\n";
TDataSet *s=DataSource->DataSet;
s->First();
while(!s->Eof)
  {
  for(int i=0;i<Columns->Count;i++)
    {
    if(Columns->Items[i]->Visible==false) continue;
    TField *f=Columns->Items[i]->Field;
    if(f==NULL)
      tmp+="\t";
    else if(f->DataType==ftMemo)
      tmp+=f->AsString+"\t";
    else
      tmp+=f->Text+"\t";
    }
  tmp+="\r\n";
  s->Next();
  }
return tmp;
}

///
/// \brief Tabulka v podobe HTML kodu
///
AnsiString TRurDBGrid::GetAsHtml()
{
AnsiString tmp;
tmp+="<table>\r\n";
tmp+="<tr>\r\n";
for(int i=0;i<Columns->Count;i++)
  {
  if(Columns->Items[i]->Visible)
    tmp+="<td>"+AnsiToUtf8(Columns->Items[i]->Title->Caption)+"</td>";
  }
tmp+="\r\n";
tmp+="</tr>\r\n";

TDataSet *s=DataSource->DataSet;
s->First();
while(!s->Eof)
  {
  tmp+="<tr>\r\n";
  for(int i=0;i<Columns->Count;i++)
    {
    if(Columns->Items[i]->Visible==false) continue;
    TField *f=Columns->Items[i]->Field;
    if(f==NULL)
      tmp+="<td></td>";
    else if(f->DataType==ftMemo)
      tmp+="<td>"+AnsiToUtf8(f->AsString)+"</td>";
    else
      tmp+="<td>"+AnsiToUtf8(f->Text)+"</td>";
    }
  tmp+="</tr>\r\n";
  s->Next();
  }
tmp+="</table>\r\n";
return tmp;
}

void TRurDBGrid::Copy2Clipboard()
{
UINT CF_HTML=RegisterClipboardFormatA("HTML Format");
AnsiString tmp;
TClipboard *c=Clipboard();
c->Open();
// text
tmp=GetAsText();
HGLOBAL gMem=GlobalAlloc(GMEM_DDESHARE + GMEM_MOVEABLE, tmp.Length()+1);
void *g=GlobalLock(gMem);
CopyMemory(g,tmp.c_str(),tmp.Length()+1);
GlobalUnlock(gMem);
c->SetAsHandle(CF_TEXT,(THandle)gMem);

// html
tmp="Version:0.9\r\n"
  "StartHTML:<<<<<1\r\n"
  "EndHTML:<<<<<2\r\n"
  "StartFragment:^^^^^^\r\n"
  "EndFragment:°°°°°°\r\n"
  "StartSelection:^^^^^^\r\n"
  "EndSelection:°°°°°°\r\n";
AnsiString s;
s.printf("%06d",tmp.Length());
tmp=AnsiReplaceText(tmp,"<<<<<1",s);
tmp+="<html>\r\n";
tmp+="<head>\r\n";
tmp+="<meta http-equiv=Content-Type content=\"text/html; charset=utf-8\">\r\n";
tmp+="<meta name=Generator content=\"TRurDBGrid\">\r\n";
tmp+="</head>\r\n";
tmp+="<body>\r\n";
tmp+="<!--StartFragment-->";
AnsiString sf,ef;
sf.printf("%06d",tmp.Length());
tmp=AnsiReplaceText(tmp,"^^^^^^",sf);
tmp+=GetAsHtml();
ef.printf("%06d",tmp.Length());
tmp=AnsiReplaceText(tmp,"°°°°°°",ef);
tmp+="<!--EndFragment-->\r\n";
tmp+="</body>\r\n";
tmp+="</html>";
s.printf("%06d",tmp.Length());
tmp=AnsiReplaceText(tmp,"<<<<<2",s);
gMem=GlobalAlloc(GMEM_DDESHARE + GMEM_MOVEABLE, tmp.Length()+1);
g=GlobalLock(gMem);
CopyMemory(g,tmp.c_str(),tmp.Length()+1);
GlobalUnlock(gMem);
c->SetAsHandle(CF_HTML,(THandle)gMem);
c->Close();
}

///
/// Skopirovanie konkretnej bunky
///
void TRurDBGrid::CopyCell2Clipboard()
{
TDataSet *s = DataSource->DataSet;
if(s) {
  int idx = this->SelectedIndex;
  String value = this->Columns->operator [](idx)->Field->AsString;
  TClipboard *c = Clipboard();
  c->AsText = value;
  }
}

void __fastcall TRurDBGrid::SetRIniName(AnsiString value)
{
        if(FRIniName != value) {
                FRIniName = value;
        }
}
AnsiString __fastcall TRurDBGrid::GetRIniName()
{
        return FRIniName;
}

void __fastcall TRurDBGrid::MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
  {
  if(tmpb==NULL)
    {
    tmpb=new TButton(this);
    tmpb->Width=10;
    tmpb->Height=10;
    tmpb->Parent=this;
    }
  tmpb->Visible=true;
  tmpb->SetFocus();
  tmpb->Visible=false;
  TGridCoord co;
  co= MouseCoord(X,Y);
//  if(OnMouseDown && co.Y>=1)
//    OnMouseDown(this,Button, Shift, X, Y);
  TSMDBGrid::MouseDown(Button, Shift, X, Y);
  if(Button==mbLeft && Options.Contains(dgMultiSelect) && DataLink->Active)
    {
//    if(Shift.Contains(ssCtrl))
//      ;//SelectedRows->CurrentRowSelected=!SelectedRows->CurrentRowSelected;
//    else
//      SelectedRows->Clear();
    }
  }

void __fastcall TRurDBGrid::MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
  {
  TGridCoord cell;
  TSMDBGrid::MouseUp(Button, Shift, X, Y);
  cell=MouseCoord(X,Y);
  if(Button==mbLeft && cell.X>=IndicatorOffset && cell.Y>=TitleOffset)
    {
    int csel=cell.X-IndicatorOffset;
    TColumn *col=Columns->operator [](csel);
    if(col && FOnCellClick2)
      FOnCellClick2(col);
    if(col && col->Field && col->Field->DataType==ftBoolean && FOnCellCheckClick)
      {
      CancelDrag();
      //EndDrag(false);
      FOnCellCheckClick(col);
      unsigned long lp;
      lp=0;//(Y<<16)+(0);
      PostMessage(tmpb->Handle,WM_LBUTTONDOWN,0,lp);
      PostMessage(tmpb->Handle,WM_LBUTTONUP,0,lp);
      }
    if(col && col->Font->Color==clBlue)
      {
      AnsiString bunka=col->Field->AsString;
      if(bunka.Length())
        {
        if(bunka.Pos("@")) // e-mail
          {
          if(FOnEmail) FOnEmail(bunka);
          }
        else               // www
          {
          if(FOnWWW) FOnWWW(bunka);
          }
        }
      }
    }
  }

void __fastcall TRurDBGrid::TitleClick(TColumn* Column) {

    TSMDBColumn *col = dynamic_cast<TSMDBColumn*>(Column);
    FSort++;

    if (FSort == 3) {

      FSort = 0;
    }

#if defined(RUR_DB_USE_ADO)
  if(FTitleSort && DataSource && DataSource->DataSet && dynamic_cast<TCustomADODataSet*>(DataSource->DataSet))
    {
    try {
      TCustomADODataSet *cad=dynamic_cast<TCustomADODataSet*>(DataSource->DataSet);
      for(int i=0;i<Columns->Count;i++)
        {
        Columns->operator [](i)->Title->Font->Color=clBlack;
        dynamic_cast<TSMDBColumn*>(Columns->operator [](i))->SortType=(TSMSortType)0;
        }
      switch(FSort)
        {
        case 0:cad->Sort="";col->SortType=(TSMSortType)0;break;
        case 1:cad->Sort=Column->FieldName+" ASC";Column->Title->Font->Color=clBlue;col->SortType=Smdbgrid::stAscending;break;
        case 2:cad->Sort=Column->FieldName+" DESC";Column->Title->Font->Color=clRed;col->SortType=Smdbgrid::stDescending;break;
        }
      } catch(...) {}
    }
#else
    if (FTitleSort && DataSource && DataSource->DataSet && dynamic_cast<TFDDataSet*>
          (DataSource->DataSet)) {

      try {

        TFDDataSet* dataSet = dynamic_cast<TFDDataSet*>(DataSource->DataSet);

        for (int i = 0; i < Columns->Count; i++) {

          Columns->Items[i]->Title->Font->Color = clBlack;
          dynamic_cast<TSMDBColumn*>(Columns->Items[i])->SortType = (TSMSortType)0;
        }

        switch (FSort) {

        case 0:
          if(FOnTitleClickSort)
            FOnTitleClickSort(col);
          else
            dataSet->IndexFieldNames = "";
          col->SortType            = (TSMSortType)0;
          break;
        case 1:
          if(FOnTitleClickSort)
            FOnTitleClickSort(col);
          else
            dataSet->IndexFieldNames   = Column->FieldName + ":A";
          Column->Title->Font->Color = clBlue;
          col->SortType              = Smdbgrid::stAscending;
          break;
        case 2:
          if(FOnTitleClickSort)
            FOnTitleClickSort(col);
          else
            dataSet->IndexFieldNames   = Column->FieldName + ":D";
          Column->Title->Font->Color = clRed;
          col->SortType              = Smdbgrid::stDescending;
          break;
        }
      }
      catch (...) {
      }
    }
#endif

    TDBGrid::TitleClick(Column);
  }

  // vyvolava jednotny impulz, oneskoreny o urcity cas
void TRurDBGrid::InternalDelayedScroll()
{
ds_time=Now()+TDateTime(0,0,0,500);
if(ds_timer==NULL && FOnDelayedScroll)
  {
  ds_timer=new TTimer(this);
  ds_timer->OnTimer=DelayedTimer;
  ds_timer->Interval=100;
  }
if(ds_timer)
  ds_timer->Enabled = true;
}

void __fastcall TRurDBGrid::DelayedTimer(TObject *Sender)
{
if(Now()>ds_time) // uz je viac, ako pozadovany cas
  {
  //delete ds_timer;
  //ds_timer=NULL;
  ds_timer->Enabled = false;
  if(FOnDelayedScroll) FOnDelayedScroll(this);
  }
}


void __fastcall TRurDBGrid::KeyDown(Word &Key, Classes::TShiftState Shift)
{
TSMDBGrid::KeyDown(Key,Shift);
switch(Key)
  {
  case VK_UP:
  case VK_PRIOR:
  case VK_DOWN:
  case VK_NEXT:
  case VK_HOME:
  case VK_END:InternalDelayedScroll();break;
  }
Key=0;
}

void __fastcall TRurDBGrid::KeyUp(Word &Key, Classes::TShiftState Shift)
{
Word K=::MapVirtualKey(Key,2);
TSMDBGrid::KeyUp(Key,Shift);
if(Shift.Contains(ssCtrl) && Key==77)  // Ctrl-M
  {
  if(ExOptions.Contains(eoCheckBoxSelect))
    {
    ExOptions=ExOptions>>eoCheckBoxSelect;
    Options=Options>>dgMultiSelect;
    if(hide_indicator)
      {
      Options=Options>>dgIndicator;
      hide_indicator=false;
      }
    }
  else
    {
    ExOptions=ExOptions<<eoCheckBoxSelect;
    Options=Options<<dgMultiSelect;
    WidthOfIndicator=24;
    if(Options.Contains(dgIndicator)==false)
      {
      Options=Options<<dgIndicator;
      hide_indicator=true;
      }
    }
  return;
  }
if(DataSource && DataSource->DataSet && DataSource->DataSet->Active && FKeyLocateRecord /*&& dynamic_cast<TFDDataSet*>(DataSource->DataSet)*/)
  {
  char KK=K;
  if(K==0) return;
  //if(KK=='\r') return;
  if(KK=='\t') return;
  if(KK=='\x1b') return;
  //if(Key==223 && K==180) return; // asi diakritika
  if(Shift.Contains(ssAlt) || Shift.Contains(ssCtrl)) return;
  if(FOnBeforeKeyLocate) FOnBeforeKeyLocate(this);
  BYTE kb[256];
  GetKeyboardState(kb);
  WCHAR uc[5] = {};
  int x = ToUnicode(Key, MapVirtualKey(Key, MAPVK_VK_TO_VSC), kb, uc, 4, 0);
  if(x==2) {
    dead_key = uc;
    return;
  }
  if(uc[0]=='\r')
    return;

  TLocateDlg *ld=new TLocateDlg(this);
  ld->rd=this;
  if(this->Options.Contains(dgIndicator))
    ld->ii=this->Col-1;
  else
    ld->ii=this->Col;
  if(x==1) {
    if(dead_key=="´´") {
      switch(uc[0]) {
        case 'a': ld->e->Text = "á";break;
        case 'e': ld->e->Text = "é";break;
        case 'i': ld->e->Text = "í";break;
        case 'o': ld->e->Text = "ó";break;
        case 'u': ld->e->Text = "ú";break;
        case 'y': ld->e->Text = "ý";break;
        case 'l': ld->e->Text = "å";break;
      }
    }
    else if(dead_key=="¡¡") {
      switch(uc[0]) {
        case 'z': ld->e->Text = "ž";break;
        case 'c': ld->e->Text = "è";break;
        case 's': ld->e->Text = "š";break;
        case 'n': ld->e->Text = "ò";break;
        case 'l': ld->e->Text = "¾";break;
        case 'd': ld->e->Text = "ï";break;
        case 'r': ld->e->Text = "ø";break;
        case 't': ld->e->Text = "";break;
      }
    }
    else
      ld->e->Text = uc;
  }
  else
    ld->e->Text=KK;
  ld->e->SelStart=1;
  int vys=ld->ShowModal();
  if(vys==mrOk && FOnAfterKeyLocate) FOnAfterKeyLocate(this);
  delete ld;
  dead_key = "";
  }
}

void __fastcall TRurDBGrid::KeyPress(TCHAR &Key)
{
if(Key=='\r') {TDBGrid::KeyPress(Key);return;}
if(Key=='\t') {TDBGrid::KeyPress(Key);return;}
if(Key=='\x1b') {TDBGrid::KeyPress(Key);return;}
if(!DataSource) return;
if(!DataSource->DataSet) return;
if(!DataSource->DataSet->Active) return;
if(FKeyLocateRecord && DataSource && DataSource->DataSet && dynamic_cast<RUR_DB_DATASET*>(DataSource->DataSet))
  {
  if(FOnBeforeKeyLocate) FOnBeforeKeyLocate(this);
  TLocateDlg *ld=new TLocateDlg(this);
  ld->rd=this;
  if(this->Options.Contains(dgIndicator))
    ld->ii=this->Col-1;
  else
    ld->ii=this->Col;
  ld->e->Text=Key;
  ld->e->SelStart=1;
  int vys=ld->ShowModal();
  if(vys==mrOk && FOnAfterKeyLocate) FOnAfterKeyLocate(this);
  delete ld;
  }
}

///
/// \brief Ziskanie popupmenu
///
TPopupMenu* __fastcall TRurDBGrid::GetPopupMenu(void)
{
TPopupMenu *tmp = TDBGrid::GetPopupMenu();
TMenuItem *mi;
if(!tmp)
  {
  tmp = new TPopupMenu(this);
  FVlastneMenu = tmp;
  PopupMenu = tmp;
  }
else
  {
  if(tmp->FindItem(FCommand,fkCommand))
    {
    mih[0]->Visible = ExOptions.Contains(eoCheckBoxSelect);
    mih[1]->Visible = ExOptions.Contains(eoCheckBoxSelect);
    mih[2]->Visible = ExOptions.Contains(eoCheckBoxSelect);
    mih[3]->Visible = ExOptions.Contains(eoCheckBoxSelect) && FOnGroupCreate;
    mih[4]->Visible = ExOptions.Contains(eoCheckBoxSelect) && FOnGroupEdit;
    return tmp;
    }
  mi=NewLine();
  tmp->Items->Add(mi);
  }
mi=NewItem("Kopíruj tabu¾ku do schránky", 0, false, true, MMenuExecute, 0, "RGridItem1");
mi->Hint="Skopírovanie celej tabu¾ky";
mi->Tag=-1;
tmp->Items->Add(mi);
FCommand=mi->Command;

mi = NewItem("Kopíruj bunku do schránky", 0, false, true, MMenuExecute, 0, "RGridItem10");
mi->Hint = "Skopírovanie oznaèenej bunky";
mi->Tag = -10;
tmp->Items->Add(mi);

if(FOnExport)
  {
  mi=NewItem("Export...", 0, false, true, MMenuExecute, 0, "RGridItem5");
  mi->Hint="Export údajov z databázy";
  mi->Tag=-5;
  tmp->Items->Add(mi);
  }
UpdateViewsMenu(tmp);
if(configcolumns)
  {
  mi=NewItem("Ståpce...", 0, false, true, MMenuExecute, 0, "RGridItem2");
  mi->Hint="Upravenie ståpcov v tabu¾ke";
  mi->Tag=-2;
  tmp->Items->Add(mi);
  }

// multiselekty
mih[0]=NewItem("Oznaèi všetko", 0, false, true, MMenuExecute, 0, "RGridItem3");
mih[0]->Hint="Zaškrtne všetky riadky";
mih[0]->Tag=-3;
tmp->Items->Add(mih[0]);

mih[1]=NewItem("Oznaèi niè", 0, false, true, MMenuExecute, 0, "RGridItem4");
mih[1]->Hint="Žiadne riadky nebudú zaškrtnuté";
mih[1]->Tag=-4;
tmp->Items->Add(mih[1]);

mih[2]=NewItem("Invertova výber", 0, false, true, MMenuExecute, 0, "RGridItem5");
mih[2]->Hint="Invertujú sa zaškrtnuté riadky";
mih[2]->Tag=-6;
tmp->Items->Add(mih[2]);

mih[3]=NewItem("Vytvori skupinu...", 0, false, true, MMenuExecute, 0, "RGridItem6");
mih[3]->Hint="Vytvorí sa skupina položiek a nastaví sa vo filtri";
mih[3]->Tag=-7;
tmp->Items->Add(mih[3]);

mih[4]=NewItem("Spravova skupinu...", 0, false, true, MMenuExecute, 0, "RGridItem7");
mih[4]->Hint="Upravovanie existujúcej skupiny";
mih[4]->Tag=-8;
tmp->Items->Add(mih[4]);

mih[0]->Visible = ExOptions.Contains(eoCheckBoxSelect);
mih[1]->Visible = ExOptions.Contains(eoCheckBoxSelect);
mih[2]->Visible = ExOptions.Contains(eoCheckBoxSelect);
mih[3]->Visible = ExOptions.Contains(eoCheckBoxSelect) && FOnGroupCreate;
mih[4]->Visible = ExOptions.Contains(eoCheckBoxSelect) && FOnGroupEdit;
return tmp;
}

///
/// Aktualizovanie zoznamu pohladov
///
void TRurDBGrid::UpdateViewsMenu(TPopupMenu *tmp)
{
if(user_views.size()) {
  if(viewsItem==NULL) {
    viewsItem = NewItem("Poh¾ady", 0, false, true, NULL, 0, "RGridItem2");
    viewsItem->Hint = "Zoznam poh¾adov";
    viewsItem->Tag = -2;
    tmp->Items->Add(viewsItem);
    }
  else {
    viewsItem->Clear();
    }

  int ii = 0;
  for(vViews::iterator i=user_views.begin(); i!=user_views.end(); i++) {
    TMenuItem *mi2;
    mi2 = NewItem(i->name, 0, false, true, MMenuExecute, 0, String("view_") + ii);
    mi2->GroupIndex = 101;
    mi2->RadioItem = true;
    mi2->AutoCheck = true;
    mi2->Checked = i->name==last_view;
    mi2->Tag = -100 + ii;
    //if(i->shortcut.Length())
    //  mi2->ShortCut = TextToShortCut(i->shortcut);
    viewsItem->Add(mi2);
    ii++;
    }

  viewsItem->Add(NewLine());
  TMenuItem *mi;
  mi = NewItem("Nastavenie...", 0, false, true, MMenuExecute, 0, "RGridItem11");
  mi->Hint = "Upravenie ståpcov v tabu¾ke";
  mi->Tag = -11;
  viewsItem->Add(mi);
  }
}

///
/// Najde aktivny pohlad
///
TRurDBGridView* TRurDBGrid::GetActiveView()
{
return GetView(last_view);
}

///
/// Najde pohlad podla mena
///
TRurDBGridView* TRurDBGrid::GetView(String name)
{
for(vViews::iterator i=user_views.begin(); i!=user_views.end(); i++)
  if(i->name==name)
    return &*i;
return NULL;
}

void __fastcall TRurDBGrid::SetExtendedColumns(String ex)
{
FExtendedColumns = ex;
ext_columns.clear();
if(ex.Length()==0)
  return;
try {
  _di_IXMLDocument xml = NewXMLDocument();
  xml->LoadFromXML(String(UTF8Encode(WideString(FExtendedColumns))));
  rur::XMLNode root = xml->DocumentElement;
  for(int k=0; k<root.Items.Count; k++) {
    rur::XMLNode row = root.Items[k];
    TRurDBGridColumn c;
    c.caption = row["caption"];
    c.fieldname = row["fieldname"];
    c.width = row["width"];
    ext_columns.push_back(c);
    }
} catch (...) {
  }
}

///
/// Pripoji svoju reakciu na menu polozku
///
void TRurDBGrid::ReagujNaMenu(TMenuItem *menu)
{
menu->OnClick=MMenuExecute;
}

void __fastcall TRurDBGrid::MMenuExecute(TObject *Sender)
{
int tag = ((TMenuItem*)(Sender))->Tag;
switch(tag)
  {
  case -1:Copy2Clipboard();
    break;
  case -2:SetupColumns();
    break;
  case -3:SelectAllClick(this);
    break;
  case -4:UnSelectAllClick(this);
    break;
  case -5:
    if(FOnExport)
      FOnExport(this);
    break;
  case -6:InvertSelectAllClick();
    break;
  case -7:CreateGroupClick();
    break;
  case -8:EditGroupClick();
    break;
  case -10:CopyCell2Clipboard();
    break;
  case -11:
    SetupViews();
    break;
  case -90:
  case -91:
  case -92:
  case -93:
  case -94:
  case -95:
  case -96:
  case -97:
  case -98:
  case -99:
  case -100:
    ((TMenuItem*)(Sender))->Checked = true;
    LoadView(100 + tag);
    break;
  }
}

void TRurDBGrid::DisableAutosave()
{
xmlname = "";
}

///
/// Prepnutie pohladu
///
void TRurDBGrid::LoadView(int id)
{
TRurDBGridView &v = user_views[id];
LoadView(&v);
}

///
/// Prepnutie pohladu
///
void TRurDBGrid::LoadView(String name)
{
TRurDBGridView *v = GetView(name);
if(v==NULL)
  v = &*user_views.begin();
LoadView(v);
}

///
/// Prepnutie pohladu
///
void TRurDBGrid::LoadView(TRurDBGridView *v)
{
try {
  user_views_activity++;
  last_view = v->name;

  Columns->Clear();
  for(vColumns::iterator i=v->columns.begin(); i!=v->columns.end(); i++) {
    TColumn *c = Columns->Add();
    c->Title->Caption = i->caption;
    c->FieldName = i->fieldname;
    c->Width = i->width;
    c->Visible = true;
    if(c->Visible && this->FOnColEnable)
      c->Visible = this->FOnColEnable(this, c->Title->Caption, c->FieldName);
    }
} __finally {
  user_views_activity--;
  if(FOnColumnsChanged)
    FOnColumnsChanged(this);
  }
}

///
/// Invertovanie zaskrtnutych poloziek
///
void TRurDBGrid::InvertSelectAllClick()
{
this->SMSelectionChanging();
TBookmark position = this->DataLink->DataSet->GetBookmark();
this->DataLink->DataSet->DisableControls();
try {
  this->DataLink->DataSet->First();
  while(!this->DataLink->DataSet->Eof) {
    this->SelectedRows->CurrentRowSelected = !this->SelectedRows->CurrentRowSelected;
    this->DataLink->DataSet->Next();
  }
} __finally {
  this->DataLink->DataSet->GotoBookmark(position);
  this->DataLink->DataSet->EnableControls();
  this->SMSelectionChanged();
  }
}

///
/// Vytvorenie skupiny
///
void TRurDBGrid::CreateGroupClick()
{
if(FOnGroupCreate) {
  FOnGroupCreate(this);
  }
}

///
/// Editovanie skupiny
///
void TRurDBGrid::EditGroupClick()
{
if(FOnGroupEdit) {
  FOnGroupEdit(this);
  }
}

///
/// Dialog na zmenu stlpcov
///
void TRurDBGrid::SetupColumns()
{
if(FOnSetupColumns) {
  FOnSetupColumns(this);
  return;
}
rur::ptr<TRurDBGridColumnsDlg> col(new TRurDBGridColumnsDlg(this));
col->grid = this;
int vys = col->ShowModal();
if(vys==mrOk && FOnColumnsChanged)
  FOnColumnsChanged(this);
}

///
/// Dialog na zmenu stlpcov
///
void TRurDBGrid::SetupViews()
{
rur::ptr<TRurDBGridViewsDlg> col(new TRurDBGridViewsDlg(this));
col->grid = this;
int vys = col->ShowModal();
if(vys==mrOk) {
  user_views = col->user_views;
  last_view = user_views[col->lbZoznam->ItemIndex].name;
  UpdateViewsMenu(NULL);
  LoadView(col->lbZoznam->ItemIndex);
  }
//if(vys==mrOk && FOnColumnsChanged)
//  FOnColumnsChanged(this);
}

///
/// Upravenie sirky stlpcov, avy optimalne vyplnila sirku gridu
///
void TRurDBGrid::PrisposobSirky(int zaciatok)
{
if(user_views.size()) // pohladove sa neprisposobuju
  return;
if(configcolumns && loadedcolumns) // stlpce su
  return;

int sirka=Width-GetSystemMetrics(SM_CYVTHUMB)-4;
if(Options.Contains(dgIndicator))
  sirka-=ColWidths[0]+1;
int usirka=0;
int max=0;
for(int i=0;i<Columns->Count;i++)
  {
  TColumn *c=Columns->operator [](i);
  if(c->Visible)
    {
    max++;
    usirka+=c->Width+1;
    }
  }

max-=zaciatok;

if(usirka<sirka)
  {
  int medzera=sirka-usirka;
  int asirka=medzera/max;
  int dsirka=medzera-asirka*max;
  for(int i=zaciatok;i<Columns->Count;i++)
    {
    TColumn *c=Columns->operator [](i);
    if(c->Visible)
      c->Width=c->Width+asirka;
    }
  Columns->operator [](max-1)->Width=
  Columns->operator [](max-1)->Width+dsirka;
  }
}

// posun kolienkom dole
bool __fastcall TRurDBGrid::DoMouseWheelDown(Classes::TShiftState Shift, const TPoint &MousePos)
{
if(DataLink->Active)
  {
  if(roll_koliesko<0)
    roll_koliesko=0;
  roll_koliesko++;
  if(roll_koliesko>GRID_RK_DELAY)
    {
    DataLink->DataSet->MoveBy(1);
    InternalDelayedScroll();
    roll_koliesko-=GRID_RK_DELAY;
    }
  }
return false;
}

// posun kolieskom hore
bool __fastcall TRurDBGrid::DoMouseWheelUp(Classes::TShiftState Shift, const TPoint &MousePos)
{
if(DataLink->Active)
  {
  if(roll_koliesko>0)
    roll_koliesko=0;
  roll_koliesko--;
  if(roll_koliesko<-GRID_RK_DELAY)
    {
    DataLink->DataSet->MoveBy(-1);
    InternalDelayedScroll();
    roll_koliesko+=GRID_RK_DELAY;
    }
  }
return false;
}

void __fastcall TRurDBGrid::WMVScroll(TWMVScroll& Message)
{
//TDBGrid::WMVScroll(Message);
InternalDelayedScroll();
}

void __fastcall TRurDBGrid::Scroll(int Distance)
{
TDBGrid::Scroll(Distance);
if(Distance)
  InternalDelayedScroll();
}

///
/// Reakcia na presun stlpca
///
void __fastcall TRurDBGrid::ColumnMoved(int FromIndex, int ToIndex)
{
TSMDBGrid::ColumnMoved(FromIndex, ToIndex);
if(user_views.size() && user_views_activity==0) {
  TRurDBGridView *av = GetActiveView();
  TRurDBGridColumn c = av->columns[FromIndex];
  av->columns[FromIndex] = av->columns[ToIndex];
  av->columns[ToIndex] = c;
  }
}

///
/// Reakcia na zmenu sirky stlpca
///
void __fastcall TRurDBGrid::ColWidthsChanged(void)
{
TSMDBGrid::ColWidthsChanged();
if(user_views.size() && user_views_activity==0) {
  TRurDBGridView *av = GetActiveView();
  int poc = Columns->Count;
  for(int i=0; i<poc; i++) {
    TColumn *col = Columns->operator [](i);
    av->SetColumnWidth(col->FieldName, col->Width);
    }
  }
}

///
/// Nastavenie cesty k VIEW suborom
///
void TRurDBGrid::ViewInitPath(String path)
{
view_path = path;
}

///
/// Otvorenie konfiguratora
///
void TRurDBGrid::XMLInitPath(AnsiString path)
{
rdgc.Init(path);
}

///
/// Zatvorenie konfiguratora
///
void TRurDBGrid::XMLClosePath()
{
rdgc.Close();
}

///
/// Inicializovanie podla konfiguratora
///
void TRurDBGrid::XMLInitName(AnsiString name)
{
if(TDirectory::Exists(view_path)) {
  String view = view_path + AnsiReplaceText(name, "::", ".") + ".json";

  if(TFile::Exists(view)) {
  try {
    json_name = name;
    //json_name = "test"; // iba ked testujes
    TBytes bt = TFile::ReadAllBytes(view);
    rur::json::RObject jn;
    jn.Parse(bt);
    String nm = jn.value("name");
    if(nm==name) {
      // stlpce
      rur::json::RArray json_items(jn.array("columns"), false);
      for(unsigned i=0; i<json_items->Count; i++) {
        rur::json::RObject item(json_items.item(i), false);
        TRurDBGridColumn c;
        c.caption = item.value("caption");
        c.fieldname = item.value("fieldname");
        c.width = item.tvalue<int>("width");
        def_columns.push_back(c);
      }

      // pohlady
      rur::json::RArray views(jn.array("views"), false);
      for(unsigned i=0; i<views->Count; i++) {
        rur::json::RObject item(views.item(i), false);
        TRurDBGridView v;
        v.is_system = true;
        v.name = item.value("name");
        v.shortcut = item.value("shortcut");
        rur::json::RArray columns(item.array("columns"), false);
        for(unsigned j=0; j<columns->Count; j++) {
          String s = columns.stritem(j);
          for(vColumns::iterator k=def_columns.begin(); k!=def_columns.end(); k++) {
            if(k->fieldname == s)
              v.columns.push_back(*k);
          }
        }
        def_views.push_back(v);
      }
    user_views = def_views;

    // uzivatelske udaje
    String user = rdgc.LoadData(json_name);
    if(user.Length() && user[1]==L'{') { // je to JSON
      rur::json::RObject j1((TJSONObject*)TJSONObject::ParseJSONValue(user));
      String lv = j1.value("last_view");
      last_view = lv; // zapnem posledny pohlad
      rur::json::RArray views(j1.array("views"), false);
      for(unsigned i=0; i<views->Count; i++) {
        rur::json::RObject item(views.item(i), false);
        String name = item.value("name");
        TRurDBGridView *old = GetView(name);
        if(old && old->is_system) { // iba aktualizujem
          rur::json::RArray columns(item.array("columns"), false);
          for(unsigned j=0; j<columns->Count; j++) {
            rur::json::RObject item(columns.item(j), false);
            String fieldname = item.value("fieldname");
            int width = item.tvalue<int>("width");
            old->SetColumnWidth(fieldname, width);
          }
        }
        else { // pridam ho
          TRurDBGridView v;
          v.is_system = false;
          v.name = name;
          rur::json::RArray columns(item.array("columns"), false);
          for(unsigned j=0; j<columns->Count; j++) {
            rur::json::RObject item(columns.item(j), false);
            TRurDBGridColumn c;
            c.fieldname = item.value("fieldname");
            for(vColumns::iterator k=def_columns.begin(); k!=def_columns.end(); k++) {
              if(k->fieldname == c.fieldname)
                c.caption = k->caption;
              }
            for(vColumns::iterator k=ext_columns.begin(); k!=ext_columns.end(); k++) {
              if(k->fieldname == c.fieldname)
                c.caption = k->caption;
              }
            c.width = item.tvalue<int>("width");
            v.columns.push_back(c);
            }
          user_views.push_back(v);
        }
      }
    }

    LoadView(last_view);
    return;
    }

  } catch (Exception &e) {
    e.Message += "\r\n" + view;
    Application->ShowException(&e);
    }
  }
}
if(rdgc.CreateColumns(this, name, loadedcolumns))
  {
  xmlname = name;
  configcolumns = true;
  }
}

///
/// Ulozenie uzivatelskej konfiguracie
///
void TRurDBGrid::XMLSave(bool force)
{
if(user_views.size()) {
  rur::json::RObject jn(new TJSONObject());
  jn.set("last_view", last_view);
  jn.set("version", 1);
  rur::json::RArray jviews(new TJSONArray(), false);
  for(vViews::iterator i=user_views.begin(); i!=user_views.end(); i++) {
    rur::json::RObject v(new TJSONObject(), false);
    v.set("name", i->name);
    rur::json::RArray jcolumns(new TJSONArray(), false);
    for(vColumns::iterator j=i->columns.begin(); j!=i->columns.end(); j++) {
      rur::json::RObject c(new TJSONObject(), false);
      c.set("fieldname", j->fieldname);
      c.set("width", j->width);
      jcolumns->AddElement(c.get());
      }
    v->AddPair("columns", jcolumns.get());
    jviews->AddElement(v.get());
    }
  jn->AddPair("views", jviews.get());
  String json = jn->ToJSON();
  rdgc.SaveData(json_name, json);
}
if(xmlname.Length())
  rdgc.SaveGrid(this,xmlname);
}

///
/// Na vyhladavanie pomenovanych stlpcov.
/// Interne si program pri inicializacii z XML pomenuje tieto stlpce.
///
TColumn* TRurDBGrid::FindNamedColumn(AnsiString name)
{
for(int i=0;i<namescount;i++)
  if(name==names[i].name)
    return names[i].col;
return NULL;
}

///
/// Najde meno daneho stlpca
///
AnsiString TRurDBGrid::FindColumnName(TColumn *col)
{
for(int i=0;i<namescount;i++)
  if(col==names[i].col)
    return names[i].name;
return "";
}

///
/// Nazov fieldu, podla ktoreho sa ma ziradit
///
String TRurDBGrid::GetColOrder(TColumn *col, String def)
{
if(FSort==0)
  return def;
for(int i=0;i<namescount;i++)
  if(col==names[i].col) {
    String s = names[i].sort;
    if(s.Length()==0)
      s = col->FieldName;
    return FSort==1 ? s + " ASC" : s + " DESC";
  }
return def;
}

AnsiString (*TranslateGrid)(AnsiString s)=NULL;

///
/// \brief Pomocna funkcia na prekladanie
///
AnsiString TRurDBGrid::TranslateText(AnsiString s)
{
if(TranslateGrid)
  return TranslateGrid(s);
return s;
}

//---------------------------------------------------------------------------
namespace Rudbgrid
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TRurDBGrid)};
                 RegisterComponents("Rur", classes, 0);
        }
}
//---------------------------------------------------------------------------
