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
#include <System.Actions.hpp>
#include <Vcl.ActnList.hpp>
#include "RuDBGrid.h"

class TRurDBGridViewsDlg : public TForm
{
__published:	// IDE-managed Components
  TButton *btnOk;
  TButton *btnCancel;
  TLabel *lblZoznam;
  TListBox *lbZoznam;
  TLabel *lblStlpce;
  TListBox *lbStlpce;
  TLabel *Label1;
  TListBox *lbUser;
  TLabel *lblPStlpce;
  TListBox *lbPStlpce;
  TButton *btnSRight;
  TButton *btnURight;
  TActionList *actlst1;
  TAction *aViewAdd;
  TAction *aViewRename;
  TAction *aViewDelete;
  TButton *btnViewAdd;
  TButton *btnViewRename;
  TButton *btnViewDelete;
  TAction *aSRight;
  TAction *aURight;
  TButton *btnMoveUp;
  TButton *btnMoveDown;
  TButton *btnDelete;
  TAction *aMoveUp;
  TAction *aMoveDown;
  TAction *aDelete;
  TLabel *lblShowHelp;
  void __fastcall FormShow(TObject *Sender);
  void __fastcall lbZoznamClick(TObject *Sender);
  void __fastcall aViewAddExecute(TObject *Sender);
  void __fastcall aSRightExecute(TObject *Sender);
  void __fastcall aSRightUpdate(TObject *Sender);
  void __fastcall aURightExecute(TObject *Sender);
  void __fastcall aURightUpdate(TObject *Sender);
  void __fastcall aMoveUpExecute(TObject *Sender);
  void __fastcall aMoveDownExecute(TObject *Sender);
  void __fastcall aMoveUpUpdate(TObject *Sender);
  void __fastcall aMoveDownUpdate(TObject *Sender);
  void __fastcall aViewRenameExecute(TObject *Sender);
  void __fastcall aViewRenameUpdate(TObject *Sender);
  void __fastcall aViewDeleteExecute(TObject *Sender);
  void __fastcall aViewDeleteUpdate(TObject *Sender);
  void __fastcall aDeleteExecute(TObject *Sender);
  void __fastcall aDeleteUpdate(TObject *Sender);
  void __fastcall lblShowHelpClick(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
private:	// User declarations
  vColumns ucolumns;

  void OznacZoznamStlpcov();

public:		// User declarations
  __fastcall TRurDBGridViewsDlg(TComponent* Owner);

  TRurDBGrid *grid;
  vViews user_views;
};
//---------------------------------------------------------------------------
extern PACKAGE TRurDBGridViewsDlg *RurDBGridViewsDlg;
//---------------------------------------------------------------------------
#endif
