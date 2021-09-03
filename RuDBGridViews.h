/*
 * Copyright (c) 2021 Ing. Robert Ulbricht
 *
 * Projekt: Optivus
 * Obsah: TRurDBGridViewsDlg
 * Nastavovanie stlpcov na gride
 */

#ifndef RuDBGridViewsH
#define RuDBGridViewsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>

class TRurDBGrid;

class TRurDBGridViewsDlg : public TForm
{
__published:	// IDE-managed Components
  TButton *btnOk;
  TButton *btnCancel;
private:	// User declarations
public:		// User declarations
  __fastcall TRurDBGridViewsDlg(TComponent* Owner);

  TRurDBGrid *grid;
};
//---------------------------------------------------------------------------
extern PACKAGE TRurDBGridViewsDlg *RurDBGridViewsDlg;
//---------------------------------------------------------------------------
#endif
