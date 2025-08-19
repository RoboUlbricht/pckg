/*
 * Copyright (c) 2005, Ing. Robert Ulbricht
 *
 * Projekt: AKontakt
 * Obsah: TRurDBGridConfigurator, TRurDBGridColumnsDlg
 * Nastavovanie stlpcov na gride
 */

#include <vcl.h>
#pragma hdrstop

#include "RuDBGridColumns.h"
#include "RuDBGrid.h"
#include "XMLTools.h"
#include <xmldoc.hpp>
#include "xmltemplates.h"
#include "CommonFunctions.h"
#include <clipbrd.hpp>
#include <strutils.hpp>

#pragma resource "*.dfm"
TRurDBGridColumnsDlg *RurDBGridColumnsDlg;

///
/// Konstruktor
///
TRurDBGridConfigurator::TRurDBGridConfigurator()
{
xml=NULL;
user=NULL;
save=NULL;
}

///
/// Destruktor
///
TRurDBGridConfigurator::~TRurDBGridConfigurator()
{
#if defined(RUR_GRID_USE_DATASET)

#else
if(save)
  delete save;
#endif;
if(user)
  delete user;
if(xml)
  delete xml;
}

///
/// Otvorenie konfiguratora
///
void TRurDBGridConfigurator::Init(AnsiString path)
{
xml=new XMLConfig(path);
}

///
/// Zatvorenie konfiguratora
///
void TRurDBGridConfigurator::Close()
{

}

///
/// Zostroji stlpceky v gride.
/// Do saved vrati true, ak pouzil uzivatelske nastavenie stlpcekov.
///
bool TRurDBGridConfigurator::CreateColumns(TRurDBGrid *grid, AnsiString name, bool &saved)
{
saved=false;
if(xml==0) return false;
if(xml->Locate("/columns/grid","id",name,"",true))
  {
  grid->Columns->Clear();
  int poc=xml->Count();
  if(poc) // vytvorim zoznam pomenovanych stlpcov
    {
    if(grid->names) delete[] grid->names;
    grid->names=new TRurDBGridColumnName[poc];
    grid->namescount=poc;
    }

  int i=0;
  while(poc)
    {
    xml->NextNode();
    AnsiString scaption=xml->ReadLocatePropertyString("caption");
    if(TranslateGrid)
      scaption=TranslateGrid(scaption);
    AnsiString sfieldname=xml->ReadLocatePropertyString("fieldname");
    AnsiString sname=xml->ReadLocatePropertyString("name");
    if(sname.Length()==0) // nepomenovane pomenujem podla fieldu
      sname=sfieldname;
    AnsiString ssort = xml->ReadLocatePropertyString("sort");
    AnsiString swidth=xml->ReadLocatePropertyString("width");
    AnsiString svisible=xml->ReadLocatePropertyString("visible");
    AnsiString smail=xml->ReadLocatePropertyString("mail");
    AnsiString swww=xml->ReadLocatePropertyString("www");

    TColumn *c=grid->Columns->Add();
    c->Title->Caption=scaption;
    c->FieldName=sfieldname;
    c->Width=swidth.ToIntDef(64);
    c->Visible=svisible=="1";
    if(c->Visible && grid->FOnColEnable)
      c->Visible=grid->FOnColEnable(grid,c->Title->Caption,c->FieldName);
    if(smail=="1" || swww=="1") // maily farbim na modro
      {
      c->Font->Color=clBlue;
      }

    // pomenovane stlpce
    grid->names[i].col = c;
    grid->names[i].name = sname;
    grid->names[i].sort = ssort;

    poc--;
    i++;
    }

  // extended columns
  if(grid->ExtendedColumns.Length())
    {
    _di_IXMLDocument xml=NewXMLDocument();
    xml->LoadFromXML(String(UTF8Encode(WideString(grid->ExtendedColumns))));
    rur::XMLNode root=xml->DocumentElement;
    TRurDBGridColumnName *nold=grid->names;
    grid->names=new TRurDBGridColumnName[grid->namescount+root.Items.Count];
    for(int k=0;k<grid->namescount;k++)
      {
      grid->names[k].col=nold[k].col;
      grid->names[k].name=nold[k].name;
      }
    grid->namescount=grid->namescount+root.Items.Count;
    delete[] nold;

    for(int k=0;k<root.Items.Count;k++)
      {
      rur::XMLNode row=root.Items[k];
      bool def=row["default"];
      AnsiString scaption=row["caption"];
      AnsiString sfieldname=row["fieldname"];
      AnsiString sname=row["name"];
      if(sname.Length()==0) // nepomenovane pomenujem podla fieldu
        sname=sfieldname;
      AnsiString swidth=row["width"];
      AnsiString svisible=row["visible"];
      AnsiString smail=row["mail"];
      AnsiString swww=row["www"];

      TColumn *c=grid->Columns->Add();
      c->Title->Caption=scaption;
      c->FieldName=sfieldname;
      c->Width=swidth.ToIntDef(64);
      c->Visible=svisible=="1";
      if(c->Visible && grid->FOnColEnable)
        c->Visible=grid->FOnColEnable(grid,c->Title->Caption,c->FieldName);
      if(smail=="1" || swww=="1") // maily farbim na modro
        {
        c->Font->Color=clBlue;
        }

      // pomenovane stlpce
      grid->names[i].col=c;
      grid->names[i].name=sname;

      i++;
      }
    }

  // uzivatelske zmeny sledujeme
  if(save)
    {
    if(save->Locate("nazov",name,TLocateOptions())) // a naslo uzivatelsky zaznam
      {
      saved=true;
      if(user) delete user;
      user=new XMLConfig;
      user->LoadXML(save->FieldByName("hodnota")->AsString);
      user->LocateList("/user");
      poc=user->Count();
      int por=0;
      while(poc)
        {
        user->NextNode();
        AnsiString sfieldname=user->ReadLocatePropertyString("fieldname");
        AnsiString swidth=user->ReadLocatePropertyString("width");
        AnsiString svisible=user->ReadLocatePropertyString("visible");

        int p=grid->Columns->Count;
        for(int i=0;i<p;i++)
          {
          TColumn *col=grid->Columns->operator [](i);
          if(col->FieldName==sfieldname)
            {
            int w=swidth.ToIntDef(64);
            if(w!=-1)
              col->Width=w;
            col->Visible=svisible=="1";
            if(col->Visible && grid->FOnColEnable)
              col->Visible=grid->FOnColEnable(grid,col->Title->Caption,col->FieldName);
            try {
            if(por<p)
              col->Index=por++; // presunieme poradie
            } catch(...) {}
            }
          }
        poc--;
        }
      }
    }
  return true;
  }
return false;
}

bool TRurDBGridConfigurator::CreateColumns(Vcl::Dbgrids::TDBGrid *grid, AnsiString name, bool &saved)
{
saved = false;
if(xml==0) return false;
if(xml->Locate("/columns/grid", "id", name, "", true))
  {
  grid->Columns->Clear();
  int poc = xml->Count();

  int i = 0;
  while(poc)
    {
    xml->NextNode();
    AnsiString scaption = xml->ReadLocatePropertyString("caption");
    if(TranslateGrid)
      scaption = TranslateGrid(scaption);
    AnsiString sfieldname = xml->ReadLocatePropertyString("fieldname");
    AnsiString sname = xml->ReadLocatePropertyString("name");
    if(sname.Length()==0) // nepomenovane pomenujem podla fieldu
      sname = sfieldname;
    AnsiString ssort = xml->ReadLocatePropertyString("sort");
    AnsiString swidth = xml->ReadLocatePropertyString("width");
    AnsiString svisible = xml->ReadLocatePropertyString("visible");
    AnsiString smail = xml->ReadLocatePropertyString("mail");
    AnsiString swww = xml->ReadLocatePropertyString("www");

    TColumn *c = grid->Columns->Add();
    c->Title->Caption = scaption;
    c->FieldName = sfieldname;
    c->Width = swidth.ToIntDef(64);
    c->Visible = svisible=="1";
    if(smail=="1" || swww=="1") // maily farbim na modro
      c->Font->Color = clBlue;

    poc--;
    i++;
    }

  // uzivatelske zmeny sledujeme
  if(save)
    {
    if(save->Locate("nazov", name, TLocateOptions())) // a naslo uzivatelsky zaznam
      {
      saved = true;
      if(user) delete user;
      user = new XMLConfig;
      user->LoadXML(save->FieldByName("hodnota")->AsString);
      user->LocateList("/user");
      poc = user->Count();
      int por = 0;
      while(poc)
        {
        user->NextNode();
        AnsiString sfieldname = user->ReadLocatePropertyString("fieldname");
        AnsiString swidth = user->ReadLocatePropertyString("width");
        AnsiString svisible = user->ReadLocatePropertyString("visible");

        int p = grid->Columns->Count;
        for(int i=0; i<p; i++)
          {
          TColumn *col = grid->Columns->operator [](i);
          if(col->FieldName==sfieldname)
            {
            int w = swidth.ToIntDef(64);
            if(w!=-1)
              col->Width = w;
            col->Visible = svisible=="1";
            try {
            if(por<p)
              col->Index = por++; // presunieme poradie
            } catch(...) {}
            }
          }
        poc--;
        }
      }
    }
  return true;
  }
return false;
}

///
/// Natavenie ukladania konfiguracie do databazy
///
#if defined(RUR_GRID_USE_DATASET)
void TRurDBGridConfigurator::Init(AnsiString table, TDataSet *_save, int _user_id)
{
if(save)
  return;
save=_save;
save->Open();
user_id=_user_id;
}

#else
void TRurDBGridConfigurator::Init(AnsiString table,TFDConnection *c, int _user_id)
{
if(save) delete save;
save=new TFDQuery(NULL);
save->Connection=c;
save->SQL->Text="select * from "+table+" where user_id="+_user_id;
save->OpenOrExecute();
user_id=_user_id;
}
#endif

///
/// Ulozenie aktualnej pozicie gridu
///
void TRurDBGridConfigurator::SaveGrid(Vcl::Dbgrids::TDBGrid *grid, AnsiString name)
{
if(save && save->Active)
  {
  XMLConfig s;
  IXMLDOMNodePtr user;
  user=s.AppendElement("user");
  int poc=grid->Columns->Count;
  for(int i=0;i<poc;i++)
    {
    TColumn *col=grid->Columns->operator [](i);
    IXMLDOMElementPtr c;
    c=s.AppendChild(user,"column");
    s.AppendAttribute(c,"fieldname",col->FieldName);
    s.AppendAttribute(c,"width",col->Width);
    s.AppendAttribute(c,"visible",col->Visible ? "1" : "0");
    }
  if(save->Locate("nazov",name,TLocateOptions()))
    save->Edit();
  else
    save->Append();
  save->FieldByName("nazov")->AsString=name;
  save->FieldByName("user_id")->AsInteger=user_id;
  save->FieldByName("hodnota")->AsString=s.Xml();
  save->Post();
  }
}

///
/// Vymazanie tejto uzivatelskej konfiguracie
///
void TRurDBGridConfigurator::ResetGrid(AnsiString name)
{
if(save && save->Active)
  {
  if(save->Locate("nazov",name,TLocateOptions())) // a naslo uzivatelsky zaznam
    save->Delete();
  }
}

void TRurDBGridConfigurator::SaveData(String name, String data)
{
if(save && save->Active) {
  if(save->Locate("nazov", name, TLocateOptions()))
    save->Edit();
  else
    save->Append();
  save->FieldByName("nazov")->AsString = name;
  save->FieldByName("user_id")->AsInteger = user_id;
  save->FieldByName("hodnota")->AsString = data;
  save->Post();
  }
}

String TRurDBGridConfigurator::LoadData(String name)
{
if(save && save->Active) {
  if(save->Locate("nazov", name, TLocateOptions()))
    return save->FieldByName("hodnota")->AsString;
  }
return "";
}

///
/// Natavenie ukladania konfiguracie do databazy
///
#if defined(RUR_GRID_USE_DATASET)
void RurGridConfiguratorInit(AnsiString table, TDataSet *save, int user_id)
{
rdgc.Init(table, save, user_id);
}
#else
void RurGridConfiguratorInit(AnsiString table,TFDConnection *c, int user_id)
{
rdgc.Init(table,c,user_id);
}
#endif

///
/// Konstruktor
///
TRurDBGridMenu::TRurDBGridMenu(Vcl::Dbgrids::TDBGrid *g, int f)
: grid(g), flag(f)
{

}

///
/// Vytvori menu pre grid
///
void TRurDBGridMenu::CreateMenu()
{
menu = grid->PopupMenu;
if(menu==NULL)
  menu = new TPopupMenu(grid);
else
  menu->Items->Add(NewLine());

TMenuItem *mi;
if(flag&rdgmTABULKA) {
  mi = NewItem("Kopíruj tabu¾ku do schránky", 0, false, true, OnTabulka, 0, "gItem1");
  mi->Hint = "Skopírovanie celej tabu¾ky";
  menu->Items->Add(mi);
  }
if(flag&rdgmBUNKA) {
  mi = NewItem("Kopíruj bunku do schránky", 0, false, true, OnBunka, 0, "gItem2");
  mi->Hint = "Skopírovanie oznaèenej bunky";
  menu->Items->Add(mi);
  }
if(flag&rdgmSTLPCE) {
  mi = NewItem("Ståpce...", 0, false, true, OnSetup, 0, "gItem3");
  mi->Hint = "Upravenie ståpcov v tabu¾ke";
  menu->Items->Add(mi);
  }
}

AnsiString TRurDBGridMenu::DoGetAsText(Vcl::Dbgrids::TDBGrid *g)
{
AnsiString tmp;
for(int i=0; i<g->Columns->Count; i++)
  {
  if(g->Columns->Items[i]->Visible)
    tmp += g->Columns->Items[i]->Title->Caption+"\t";
  }
tmp+="\r\n";

TDataSet *s = g->DataSource->DataSet;
s->First();
while(!s->Eof)
  {
  for(int i=0; i<g->Columns->Count; i++)
    {
    if(g->Columns->Items[i]->Visible==false) continue;
    TField *f = g->Columns->Items[i]->Field;
    if(f==NULL)
      tmp += "\t";
    else if(f->DataType==ftMemo)
      tmp += f->AsString + "\t";
    else
      tmp += f->Text+"\t";
    }
  tmp+="\r\n";
  s->Next();
  }
return tmp;
}

AnsiString TRurDBGridMenu::DoGetAsHTML(Vcl::Dbgrids::TDBGrid *g)
{
AnsiString tmp;
tmp+="<table>\r\n";
tmp+="<tr>\r\n";
for(int i=0; i<g->Columns->Count; i++)
  {
  if(g->Columns->Items[i]->Visible)
    tmp+="<td>"+AnsiToUtf8(g->Columns->Items[i]->Title->Caption)+"</td>";
  }
tmp+="\r\n";
tmp+="</tr>\r\n";

TDataSet *s = g->DataSource->DataSet;
s->First();
while(!s->Eof)
  {
  tmp+="<tr>\r\n";
  for(int i=0;i<g->Columns->Count;i++)
    {
    if(g->Columns->Items[i]->Visible==false) continue;
    TField *f=g->Columns->Items[i]->Field;
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

void TRurDBGridMenu::DoCopyTable(Vcl::Dbgrids::TDBGrid *grid)
{
UINT CF_HTML = RegisterClipboardFormatA("HTML Format");
AnsiString tmp;
TClipboard *c = Clipboard();
c->Open();
// text
tmp = DoGetAsText(grid);
HGLOBAL gMem = GlobalAlloc(GMEM_DDESHARE + GMEM_MOVEABLE, tmp.Length()+1);
void *g = GlobalLock(gMem);
CopyMemory(g, tmp.c_str(), tmp.Length()+1);
GlobalUnlock(gMem);
c->SetAsHandle(CF_TEXT, (THandle)gMem);

// html
tmp = "Version:0.9\r\n"
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
tmp+=DoGetAsHTML(grid);
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
/// Tabulka do schranky
///
void __fastcall TRurDBGridMenu::OnTabulka(TObject *Sender)
{
DoCopyTable(grid);
}

///
/// Bunka do schranky
///
void __fastcall TRurDBGridMenu::OnBunka(TObject *Sender)
{
DoCopyBunka(grid);
}

void TRurDBGridMenu::DoCopyBunka(Vcl::Dbgrids::TDBGrid *g)
{
TDataSet *s = g->DataSource->DataSet;
if(s) {
  int idx = g->SelectedIndex;
  String value = g->Columns->operator [](idx)->Field->AsString;
  TClipboard *c = Clipboard();
  c->AsText = value;
  }
}

///
/// Vyvolanie nastavenia stlpcov
///
void __fastcall TRurDBGridMenu::OnSetup(TObject *Sender)
{
rur::ptr<TRurDBGridColumnsDlg> col(new TRurDBGridColumnsDlg(grid));
col->grid = grid;
int vys = col->ShowModal();
}

///
/// Konstruktor
///
__fastcall TRurDBGridColumnsDlg::TRurDBGridColumnsDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}

///
/// Inicializacia dialogu
///
void __fastcall TRurDBGridColumnsDlg::FormShow(TObject *Sender)
{
int poc = grid->Columns->Count;
for(int i=0; i<poc; i++)
  {
  int pos = lb->Items->AddObject(
    grid->Columns->operator [](i)->Title->Caption,
    (TObject*)grid->Columns->operator [](i));
  if(grid->Columns->operator [](i)->Visible)
    lb->Checked[pos] = true;
  }
}

///
/// Roztiahnut stlpce na sirku
///
void __fastcall TRurDBGridColumnsDlg::Roztiahnut(TObject *Sender)
{
//
}

///
/// Posun stlpec hore
///
void __fastcall TRurDBGridColumnsDlg::Hore(TObject *Sender)
{
if(lb->Items->Count>1 && lb->ItemIndex>0)
  lb->Items->Exchange(lb->ItemIndex, lb->ItemIndex-1);
}

///
/// Posun stlpec dole
///
void __fastcall TRurDBGridColumnsDlg::Dolu(TObject *Sender)
{
if(lb->Items->Count>1 && lb->ItemIndex<lb->Items->Count-1)
  lb->Items->Exchange(lb->ItemIndex, lb->ItemIndex+1);
}

///
/// Stlacenie OK
///
void __fastcall TRurDBGridColumnsDlg::OKBtnClick(TObject *Sender)
{
int poc = lb->Items->Count;
for(int i=0; i<poc; i++)
  {
  TColumn *col = (TColumn*)lb->Items->Objects[i];
  col->Visible = lb->Checked[i];
  TRurDBGrid *rg = dynamic_cast<TRurDBGrid*>(grid);
  if(col->Visible && rg && rg->FOnColEnable)
    col->Visible =rg->FOnColEnable(rg, col->Title->Caption, col->FieldName);
  col->Index = i;
  }
}

