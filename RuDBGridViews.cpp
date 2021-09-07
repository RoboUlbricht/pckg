/*
 * Copyright (c) 2021 Ing. Robert Ulbricht
 *
 * Projekt: Optivus
 * Obsah: TRurDBGridViewsDlg
 * Nastavovanie stlpcov na gride
 */

#include <vcl.h>
#pragma hdrstop

#include "RuDBGridViews.h"
#include <xmldoc.hpp>
#include "xmltemplates.h"

#pragma package(smart_init)
#pragma resource "*.dfm"
TRurDBGridViewsDlg *RurDBGridViewsDlg;

///
/// Kostruktor
///
__fastcall TRurDBGridViewsDlg::TRurDBGridViewsDlg(TComponent* Owner)
  : TForm(Owner)
{
}

///
/// Inicializacia dialogu
///
void __fastcall TRurDBGridViewsDlg::FormShow(TObject *Sender)
{
// Zoznam pohladov
user_views = grid->user_views;
for(vViews::iterator i = user_views.begin(); i!=user_views.end(); i++) {
  int x = lbZoznam->Items->Add(i->name + (i->is_system ? " [sys]" : ""));
  if(i->name==grid->last_view)
    lbZoznam->ItemIndex = x;
}

// Zoznam stlpcov
for(vColumns::iterator i = grid->def_columns.begin(); i!=grid->def_columns.end(); i++) {
  lbStlpce->Items->Add(i->caption);
}

if(grid->ExtendedColumns.Length()) {
  _di_IXMLDocument xml = NewXMLDocument();
  xml->LoadFromXML(String(UTF8Encode(WideString(grid->ExtendedColumns))));
  rur::XMLNode root = xml->DocumentElement;
  for(int k=0; k<root.Items.Count; k++) {
    rur::XMLNode row = root.Items[k];
    TRurDBGridColumn c;
    c.caption = row["caption"];
    c.fieldname = row["fieldname"];
    c.width = row["width"];
    ucolumns.push_back(c);
    lbUser->Items->Add(c.caption);
  }
}

if(lbZoznam->ItemIndex==-1)
  lbZoznam->ItemIndex = 0;
lbZoznamClick(NULL);
}

///
/// Zobrazenie stlpcov
///
void __fastcall TRurDBGridViewsDlg::lbZoznamClick(TObject *Sender)
{
int pos = lbZoznam->ItemIndex;
TRurDBGridView &v = user_views[pos];
lbPStlpce->Items->Clear();
for(vColumns::iterator i=v.columns.begin(); i!=v.columns.end(); i++) {
  lbPStlpce->Items->Add(i->caption);
  }
}

///
/// Prida novy pohlad
///
void __fastcall TRurDBGridViewsDlg::aViewAddExecute(TObject *Sender)
{
String newname;
if(InputQuery("N�zov pre poh�ad", "N�zov", newname) && newname.Length()) {
  int pos = lbZoznam->ItemIndex;
  TRurDBGridView &v = user_views[pos];
  TRurDBGridView nv = v;
  nv.name = newname;
  nv.is_system = false;
  user_views.push_back(nv);

  int x = lbZoznam->Items->Add(nv.name);
  lbZoznam->ItemIndex = x;
  lbZoznamClick(NULL);
  }
}

///
/// Prida stlpce
///
void __fastcall TRurDBGridViewsDlg::aSRightExecute(TObject *Sender)
{
int pos = lbZoznam->ItemIndex;
int idx = lbStlpce->ItemIndex;
if(pos==-1 || idx==-1)
  return;

TRurDBGridView &v = user_views[pos];
if(v.is_system==true) {
  Application->MessageBox(L"Do syst�mov�ho poh�adu nemo�no prid�va� st�pce.", L"Syst�mov� poh�ad", MB_OK|MB_ICONEXCLAMATION);
  return;
}
TRurDBGridColumn &c = grid->def_columns[idx];
for(vColumns::iterator i=v.columns.begin(); i!=v.columns.end(); i++) {
  if(i->fieldname==c.fieldname) {
    Application->MessageBox(L"Tak�to st�pec u� je v zozname.", L"Duplicita st�pca", MB_OK|MB_ICONEXCLAMATION);
    return;
    }
  }
v.columns.push_back(c);
int x = lbPStlpce->Items->Add(c.caption);
lbPStlpce->ItemIndex = x;
}

///
/// Enablovanie
///
void __fastcall TRurDBGridViewsDlg::aSRightUpdate(TObject *Sender)
{
bool ena = false;
if(lbStlpce->ItemIndex!=-1 && lbZoznam->ItemIndex!=-1) {
  int pos = lbZoznam->ItemIndex;
  TRurDBGridView &v = user_views[pos];

  if(v.is_system==false)
    ena = true;
  }
((TAction*)Sender)->Enabled = ena;
}

///
/// Prida uzivatelske stlpce
///
void __fastcall TRurDBGridViewsDlg::aURightExecute(TObject *Sender)
{
int pos = lbZoznam->ItemIndex;
int idx = lbUser->ItemIndex;
if(pos==-1 || idx==-1)
  return;

TRurDBGridView &v = user_views[pos];
if(v.is_system==true) {
  Application->MessageBox(L"Do syst�mov�ho poh�adu nemo�no prid�va� st�pce.", L"Syst�mov� poh�ad", MB_OK|MB_ICONEXCLAMATION);
  return;
}
TRurDBGridColumn &c = ucolumns[idx];
for(vColumns::iterator i=v.columns.begin(); i!=v.columns.end(); i++) {
  if(i->fieldname==c.fieldname) {
    Application->MessageBox(L"Tak�to st�pec u� je v zozname.", L"Duplicita st�pca", MB_OK|MB_ICONEXCLAMATION);
    return;
    }
  }
v.columns.push_back(c);
int x = lbPStlpce->Items->Add(c.caption);
lbPStlpce->ItemIndex = x;
}

///
/// Enablovanie
///
void __fastcall TRurDBGridViewsDlg::aURightUpdate(TObject *Sender)
{
bool ena = false;
if(lbUser->ItemIndex!=-1 && lbZoznam->ItemIndex!=-1) {
  int pos = lbZoznam->ItemIndex;
  TRurDBGridView &v = user_views[pos];

  if(v.is_system==false)
    ena = true;
  }
((TAction*)Sender)->Enabled = ena;
}

///
/// Posun stlpec hore
///
void __fastcall TRurDBGridViewsDlg::aMoveUpExecute(TObject *Sender)
{
if(lbPStlpce->Items->Count>1 && lbPStlpce->ItemIndex>0) {
  int pos = lbZoznam->ItemIndex;
  TRurDBGridView &v = user_views[pos];
  TRurDBGridColumn c = v.columns[lbPStlpce->ItemIndex];
  v.columns[lbPStlpce->ItemIndex] = v.columns[lbPStlpce->ItemIndex-1];
  v.columns[lbPStlpce->ItemIndex-1] = c;

  lbPStlpce->Items->Exchange(lbPStlpce->ItemIndex, lbPStlpce->ItemIndex-1);
  }
}

///
/// Enablovanie
///
void __fastcall TRurDBGridViewsDlg::aMoveUpUpdate(TObject *Sender)
{
bool ena = false;
if(lbZoznam->ItemIndex!=-1) {
  int pos = lbZoznam->ItemIndex;
  TRurDBGridView &v = user_views[pos];
  if(v.is_system==false && lbPStlpce->Items->Count>1 && lbPStlpce->ItemIndex>0)
    ena = true;
  }
((TAction*)Sender)->Enabled = ena;
}

///
/// Posun stlpec dole
///
void __fastcall TRurDBGridViewsDlg::aMoveDownExecute(TObject *Sender)
{
if(lbPStlpce->Items->Count>1 && lbPStlpce->ItemIndex>-1 && lbPStlpce->ItemIndex<lbPStlpce->Items->Count-1) {
  int pos = lbZoznam->ItemIndex;
  TRurDBGridView &v = user_views[pos];
  TRurDBGridColumn c = v.columns[lbPStlpce->ItemIndex];
  v.columns[lbPStlpce->ItemIndex] = v.columns[lbPStlpce->ItemIndex+1];
  v.columns[lbPStlpce->ItemIndex+1] = c;

  lbPStlpce->Items->Exchange(lbPStlpce->ItemIndex, lbPStlpce->ItemIndex+1);
  }
}

///
/// Enablovanie
///
void __fastcall TRurDBGridViewsDlg::aMoveDownUpdate(TObject *Sender)
{
bool ena = false;
if(lbZoznam->ItemIndex!=-1) {
  int pos = lbZoznam->ItemIndex;
  TRurDBGridView &v = user_views[pos];
  if(v.is_system==false && lbPStlpce->Items->Count>1 && lbPStlpce->ItemIndex>-1 && lbPStlpce->ItemIndex<lbPStlpce->Items->Count-1)
    ena = true;
  }
((TAction*)Sender)->Enabled = ena;
}

///
/// Premenovanie pohladu
///
void __fastcall TRurDBGridViewsDlg::aViewRenameExecute(TObject *Sender)
{
int pos = lbZoznam->ItemIndex;
TRurDBGridView &v = user_views[pos];

String newname = v.name;
if(InputQuery("Nov� n�zov pre poh�ad", "N�zov", newname) && newname.Length()) {
  v.name = newname;
  lbZoznam->Items->Strings[pos] = newname;
  }
}

///
/// Enablovanie premenovania
///
void __fastcall TRurDBGridViewsDlg::aViewRenameUpdate(TObject *Sender)
{
bool ena = false;
if(lbZoznam->ItemIndex!=-1) {
  int pos = lbZoznam->ItemIndex;
  TRurDBGridView &v = user_views[pos];

  if(v.is_system==false)
    ena = true;
  }
((TAction*)Sender)->Enabled = ena;
}

///
/// Mazanie pohladu
///
void __fastcall TRurDBGridViewsDlg::aViewDeleteExecute(TObject *Sender)
{
if(Application->MessageBox(L"M��e sa ozna�en� polo�ka vymaza�?", L"Vymazanie polo�ky", MB_YESNO|MB_ICONQUESTION)==mrYes)
  {
  int pos = lbZoznam->ItemIndex;
  user_views.erase(user_views.begin() + pos);
  lbZoznam->Items->Delete(pos);
  }
}

///
/// Enablovanie mazania
///
void __fastcall TRurDBGridViewsDlg::aViewDeleteUpdate(TObject *Sender)
{
bool ena = false;
if(lbZoznam->ItemIndex!=-1) {
  int pos = lbZoznam->ItemIndex;
  TRurDBGridView &v = user_views[pos];

  if(v.is_system==false)
    ena = true;
  }
((TAction*)Sender)->Enabled = ena;
}

///
/// Mazanie stlpca
///
void __fastcall TRurDBGridViewsDlg::aDeleteExecute(TObject *Sender)
{
if(lbPStlpce->Items->Count>1) {
  int pos = lbZoznam->ItemIndex;
  TRurDBGridView &v = user_views[pos];
  v.columns.erase(v.columns.begin() + lbPStlpce->ItemIndex);
  lbPStlpce->Items->Delete(lbPStlpce->ItemIndex);
  }
}

///
/// Enablovanie mazania
///
void __fastcall TRurDBGridViewsDlg::aDeleteUpdate(TObject *Sender)
{
bool ena = false;
if(lbZoznam->ItemIndex!=-1) {
  int pos = lbZoznam->ItemIndex;
  TRurDBGridView &v = user_views[pos];

  if(v.is_system==false && lbPStlpce->ItemIndex!=-1)
    ena = true;
  }
((TAction*)Sender)->Enabled = ena;
}

