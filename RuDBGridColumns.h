/*
 * Copyright (c) 2005, Ing. Robert Ulbricht
 *
 * Projekt: AKontakt
 * Obsah: TRurDBGridConfigurator, TRurDBGridColumnsDlg
 * Nastavovanie stlpcov na gride
 */

#ifndef RuDBGridColumnsH
#define RuDBGridColumnsH
//----------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.CheckLst.hpp>
#include <Vcl.StdCtrls.hpp>
#if defined(RUR_GRID_USE_DATASET)
  #include <Data.DB.hpp>
#else
  #include <FireDAC.Comp.Client.hpp>
#endif
//----------------------------------------------------------------------------

class XMLConfig;
class TRurDBGrid;

namespace Vcl {
namespace Dbgrids {
class TDBGrid;
}
}

///
/// Konfiguracny program
///
class TRurDBGridConfigurator
{
XMLConfig *xml;   ///< Defaultna konfiguracia
XMLConfig *user;  ///< Uzivatelska konfiguracia
#if defined(RUR_GRID_USE_DATASET)
TDataSet *save;  ///< Sem ukladam uzivatelske nastavenie pre kazdy jeho grid
#else
TFDQuery *save;  ///< Sem ukladam uzivatelske nastavenie pre kazdy jeho grid
#endif
int user_id;      ///< id uzivatela

public:
  TRurDBGridConfigurator();
  ~TRurDBGridConfigurator();
  void Init(AnsiString path);
  void Close();
  bool CreateColumns(TRurDBGrid *grid, AnsiString name, bool &saved);
  bool CreateColumns(Vcl::Dbgrids::TDBGrid *grid, AnsiString name, bool &saved);
#if defined(RUR_GRID_USE_DATASET)
  void Init(AnsiString table, TDataSet *_save, int _user_id);
#else
  void Init(AnsiString table,TFDConnection *c, int _user_id);
#endif
  void SaveGrid(Vcl::Dbgrids::TDBGrid *grid, AnsiString name);
  void ResetGrid(AnsiString name);
  void SaveData(String name, String data);
  String LoadData(String name);
};

#if defined(RUR_GRID_USE_DATASET)
void RurGridConfiguratorInit(AnsiString table, TDataSet *save, int user_id);
#else
void RurGridConfiguratorInit(AnsiString table,TFDConnection *c, int user_id);
#endif
extern TRurDBGridConfigurator rdgc;

#define rdgmTABULKA 0x0001
#define rdgmBUNKA   0x0002
#define rdgmSTLPCE  0x0004

///
/// Menu pre obycajne gridy
///
class TRurDBGridMenu
{
  Vcl::Dbgrids::TDBGrid *grid;
  TPopupMenu *menu;
  int flag;

  void __fastcall OnSetup(TObject *Sender);
  void __fastcall OnTabulka(TObject *Sender);
  void __fastcall OnBunka(TObject *Sender);

public:
  TRurDBGridMenu(Vcl::Dbgrids::TDBGrid *g, int f = rdgmTABULKA|rdgmBUNKA|rdgmSTLPCE);

  void CreateMenu();

  static AnsiString DoGetAsText(Vcl::Dbgrids::TDBGrid *g);
  static AnsiString DoGetAsHTML(Vcl::Dbgrids::TDBGrid *g);
  static void DoCopyTable(Vcl::Dbgrids::TDBGrid *g);
  static void DoCopyBunka(Vcl::Dbgrids::TDBGrid *g);
};

///
/// Dialog na nastavenie viditelnosti stlpcov
///
class TRurDBGridColumnsDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
  TCheckListBox *lb;
  TButton *Button1;
  TButton *Button2;
  TButton *Button3;
  void __fastcall FormShow(TObject *Sender);
  void __fastcall Roztiahnut(TObject *Sender);
  void __fastcall Hore(TObject *Sender);
  void __fastcall Dolu(TObject *Sender);
  void __fastcall OKBtnClick(TObject *Sender);
private:
public:
	virtual __fastcall TRurDBGridColumnsDlg(TComponent* AOwner);

  Vcl::Dbgrids::TDBGrid *grid;
};
//----------------------------------------------------------------------------
extern PACKAGE TRurDBGridColumnsDlg *RurDBGridColumnsDlg;
//----------------------------------------------------------------------------
#endif
