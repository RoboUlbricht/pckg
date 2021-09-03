/*
 * Copyright (c) 2004-2006 Ing. Robert Ulbricht
 *
 * Projekt: Optivus
 * Obsah: TRurCalendar
 * Kalendar s mesiacmi
 */

#ifndef RurCalendarH
#define RurCalendarH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include "RurCalendarBase.h"
//---------------------------------------------------------------------------
enum TCalendarTyp {cplan1, cplan5, cplan7, cplan31, cplanAll};

#define TRCD_DATE       0x0001 ///< Je to datumova polozka
#define TRCD_REDFRAME   0x0002 ///< Oznacenie dnesneho dna
#define TRCD_BLACKFRAME 0x0004 ///< Oznacenie selektnuteho dna

///
/// Jeden den v kalendari
///
struct TRurCalendarDay
{
TColor cpismo;      ///< Farba pisma
TFontStyles cstyle; ///< Styl pisma
TColor cbackground; ///< Farba podkladu
AnsiString nazov;   ///< nazov dna
AnsiString popis;   ///< Podrobny popis eventov dna
int style;          ///< Rozne flagy

void Clear() {cpismo=clBlack;cbackground=clWhite;style=0;cstyle=TFontStyles();popis="";}
};

///
/// Jeden mesiac v kalendari
///
struct TRurCalendarMonth
{
TDateTime dat_s;         ///< Zaciatok mesiaca
TDateTime dat_k;         ///< Konec mesiaca
TRurCalendarDay day[49]; ///< pole dni  x*stlpce+y
bool prepared;           ///< Uz bolo predpocitane

TRurCalendarMonth() {prepared=false;}

};

///
/// Kalendar s mesiacmi
///
class PACKAGE TRurCalendar : public TGraphicControl
{
private:
  bool is_init;
  int FRiadkov;
  int FStlpcov;
  TDate FDatum;
  TDate FDatum_od;
  TDate FDatum_do;
  TDate FDatum_min; // iba tieto dve rozpatia na datumy
  TDate FDatum_max; // dokazem zobrazit (pri naplneni polozkami)
  int FRok;
  int FMesiac;
  int _M,_R;
  int FMonthOffset;
  RurCalendarArray rca; ///< Pole dni s terminom
  RurCalendarArray sviatky; ///< Pole sviatkov
  bool FCiary;          ///< Kresli ciary medzi bunkami
  bool FVLinka;         ///< Vodorovna linka pod dnami
  bool FMove;           ///< Umoznuje posun mesiacov sipkami
  bool FColorInactive;  ///< Farbi na zlto neaktivne mesiace
  TCalendarTyp FTyp;
  TRect rect_all;       ///< Predpocitana oblast celeho aktivneho pola
  TRect rect_l,rect_r;  ///< Predpocitane oblasti na posun mesiaca
  Graphics::TBitmap *bmp;
  TNotifyEvent FOnMoveClick,FOnCellClick;
  TRurCalendarMonth *month; ///< Pole urychlovacich mesiacov pre vykreslenie

  MESSAGE void __fastcall CMHintShow(Messages::TMessage &Message);
  void __fastcall SetRiadkov(int value);
  int __fastcall GetRiadkov();
  void __fastcall SetStlpcov(int value);
  int __fastcall GetStlpcov();
  void __fastcall SetDatum(TDate value);
  TDate __fastcall GetDatum();
  void __fastcall SetRok(int value);
  int __fastcall GetRok();
  void __fastcall SetMesiac(int value);
  int __fastcall GetMesiac();
  void PrepareMonth(TRurCalendarMonth &mesiac, int R, int M);
  int DaysThisMonth();
  int DaysPerMonth(int AYear, int AMonth);
  bool IsLeapYear(int AYear);
  AnsiString GetCellText(int ACol, int ARow,int &n);
  void CalcRozmery();
  void CalcOdDo();
  void CalcMinMax();
  void __fastcall SetCiary(bool value);
  bool __fastcall GetCiary();
  void __fastcall SetVLinka(bool value);
  bool __fastcall GetVLinka();
  void __fastcall SetEnableMove(bool value);
  void __fastcall SetColorInactive(bool value);
  void __fastcall SetTyp(TCalendarTyp Value);
  DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
  DYNAMIC void __fastcall DblClick(void);
  DYNAMIC void __fastcall Resize(void);
  void __fastcall SetOnMoveClick(TNotifyEvent value);
  TNotifyEvent __fastcall GetOnMoveClick();
  void __fastcall SetOnCellClick(TNotifyEvent value);
  TNotifyEvent __fastcall GetOnCellClick();

protected:
public:
  __fastcall TRurCalendar(TComponent* Owner);
  __fastcall ~TRurCalendar();
  virtual void __fastcall Paint();
  void AddItem(RurCalendarItem &ri);
  void DeleteItem(long hash);
  void ClearItems();
  void AddSviatok(TDateTime d, AnsiString nazov);
  void ClearSviatky();
  void Today();
  TDate GetOd() {return FDatum_od;}
  TDate GetDo() {return FDatum_do;}
  TDate GetMin() {return FDatum_min;}
  TDate GetMax() {return FDatum_max;}
  TDateTime GetXYDate(int X, int Y);
  AnsiString GetXYDateDescription(int X, int Y);
  void Dirty();
  void MoveUp();
  void MoveDown();

__published:
  __property int Riadkov  = { read=GetRiadkov, write=SetRiadkov };
  __property int Stlpcov  = { read=GetStlpcov, write=SetStlpcov };
  __property TDate Datum  = { read=GetDatum, write=SetDatum };
  __property int Rok  = { read=GetRok, write=SetRok };
  __property int Mesiac  = { read=GetMesiac, write=SetMesiac };
  __property Align;
  __property bool Ciary  = { read=GetCiary, write=SetCiary };
  __property bool VLinka  = { read=GetVLinka, write=SetVLinka };
  __property bool EnableMove  = { read=FMove, write=SetEnableMove, default=true };
  __property bool ColorInactive  = { read=FColorInactive, write=SetColorInactive, default=false };
  __property TCalendarTyp Typ = {read=FTyp, write=SetTyp, default=0};
  __property OnClick;
  __property OnDblClick;
  __property PopupMenu;
  __property OnDragOver;
  __property OnDragDrop;
  __property ShowHint;
  __property TNotifyEvent OnMoveClick  = { read=GetOnMoveClick, write=SetOnMoveClick };
  __property TNotifyEvent OnCellClick  = { read=GetOnCellClick, write=SetOnCellClick };

BEGIN_MESSAGE_MAP
    VCL_MESSAGE_HANDLER(CM_HINTSHOW, TMessage, CMHintShow);
END_MESSAGE_MAP(TGraphicControl);

};
//---------------------------------------------------------------------------
#endif
