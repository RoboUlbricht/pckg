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
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
