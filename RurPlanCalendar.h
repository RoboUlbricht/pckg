/*
 * Copyright (c) 2004-2006,2007 Ing. Robert Ulbricht
 *
 * Projekt: Optivus
 * Obsah: TRurPlanCalendar
 * Planovaci kalendar
 */

#ifndef RurPlanCalendarH
#define RurPlanCalendarH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include "RurCalendarBase.h"
#include <vector>

//---------------------------------------------------------------------------
enum TPlanCalendarTyp {pcplan1, pcplan5, pcplan7, pcplan31, pcplanU};
enum TPlanCalendarScale {pcps60, pcps30, pcps15, pcps10, pcps6, pcps5};

typedef void __fastcall (__closure *TPlanEditEvent)(System::TObject* Sender,RurCalendarItem *ri);
typedef void __fastcall (__closure *TPlanMoveTerm)(System::TObject* Sender,RurCalendarItem *ri,TDateTime t1,TDateTime t2);

class PACKAGE TRurPlanCalendarHeader;
class PACKAGE TRurPlanCalendar;

///
/// Pomocny vypocet pri vykreslovani
///
struct TRurPlanCalendarHeaderItem
{
TRect r;
RurCalendarItem *ri;
void DrawItem(TCanvas *c,TRurPlanCalendar *plan);
};

typedef std::vector<TRurPlanCalendarHeaderItem> v_items;

///
/// Predpocitane hodnoty pre konkretny den
///
struct TRurPlanCalendarDay
{
TRect r;        ///< Jeho plocha
TColor c;       ///< Jeho farba
TDateTime d;    ///< Den
v_items items; ///< Zoznam predpocitanych poloziek
};

///
/// Predstavuje jedneho usera v rezime kalendara U
///
struct TRurPlanCalendarUser
{
int id;          ///< ID uzivatela
AnsiString meno; ///< Jeho meno
TRect r;         ///< Jeho obdlznik na obrazovke
std::vector<RurCalendarItem*> items;   ///< Zoznam jeho poloziek
std::vector<RurCalendarItem*> time;   ///< Zoznam jeho poloziek na hodinu
std::vector<RurCalendarItem*> days;    ///< Zoznam jeho poloziek, ktory je priradeny na den
v_items gitems; ///< Zoznam predpocitanych poloziek

TRurPlanCalendarUser() : id(-1) {}
TRurPlanCalendarUser(int _id, AnsiString _meno) : id(_id),meno(_meno) {}
TRurPlanCalendarUser(const TRurPlanCalendarUser &i) : id(i.id),meno(i.meno) {}
};

typedef std::vector<TRurPlanCalendarUser> v_users;

///
/// Zapuzdrenie prace s uzivatelmi
///
class TRurPlanCalendarUsers : public v_users
{
  TRurPlanCalendar *parent;

public:
  TRurPlanCalendarUsers(TRurPlanCalendar *_parent) : parent(_parent) {}
  TRurPlanCalendarUser* AddUser(int id, AnsiString meno);
  bool DeleteUser(int id);
};

///
/// Planovaci kalendar
///
class PACKAGE TRurPlanCalendar : public TScrollingWinControl
{
private:
  Graphics::TCanvas* FCanvas;
  TColor FColorNoc;
  TColor FColorDen;
  TPlanCalendarTyp FTyp;         ///< Stanovuje vzhlad kalendara
  TPlanCalendarScale FScale;     ///< Mierka kalendara
  int FTypDays;                  ///< Pocet dni v danom type
  int FBunkaHeight;              ///< Vyska jednej bunky
  int FBunkaCount;               ///< Pocet buniek
  int FBunkaMultiply;            ///< Pocet buniek, ktore tvoria hodinu
  int FBunkaMinuty;              ///< Pocet minut, ktore tvoria jednu bunku
  TDateTime FDatum;              ///< Aktualny datum
  TDateTime FDatum_od,FDatum_do; ///< Rozpatie datumov celeho zobrazeneho kalendara
  int FPDobaZaciatok;        ///< Bunka zacina pracovnu dobu
  int FPDobaKoniec;          ///< Bunka konci pracovnu dobu
  TDateTime FPDobaTZaciatok; ///< Zaciatok pracovnej doby
  TDateTime FPDobaTKoniec;   ///< Koniec pracovnej doby

  TNotifyEvent FOnRightMouse;
  TNotifyEvent FOnSelTerm;
  TNotifyEvent FOnDblClick;
  TPlanEditEvent FOnEditTerm;
  TPlanEditEvent FOnSelTerm2;    ///< Event po selektnuti polozky
  TPlanMoveTerm FOnMoveTerm;

  //TDate SelDatum,SelDatum2;
  int SelOriginX, SelOriginY; ///< Presny pociatok kliknutia
  int   SelBunka, SelBunka2;
  int   SelSirka;
  bool hidedate;
  bool hidemeniny;
  RurCalendarArray rca; ///< Tu sa skladuju kalendarove polozky
  std::vector<RurCalendarItem*> v_time[31]; ///< Predpocitany zoznam pre kazdy den
  std::vector<RurCalendarItem*> v_date[5]; ///< Predpocitany zoznam pre 5 pracovnych dni, co sa nevztahuje na cas
  TRurPlanCalendarDay days[42];       ///< Pole na predpocitanie mesacneho kalendara
  TRurPlanCalendarDay *days_selected; ///< Posledny klik siel na tento den - pointer
  int days_position;                  ///< Posledny klik siel na tento den - poradie v poli
  TRurPlanCalendarHeaderItem *days_selecteditem; ///< Selektnuta polozka
  //v_items items; ///< Zoznam predpocitanych poloziek
  TRurPlanCalendarUsers *users; ///< Zoznam uzivatelov

  bool drag,needrecalc;
  int dragy;
  int dragmode; // co bude robit dragovacka (1 cely termin, 2 horny okraj, 3 dolny okraj)
  int draggeditem;
  TRect dragrect;
  AnsiString FMeniny;
  Graphics::TBitmap *bpruzokh;
  int BarHeight;  ///< Rezervovana vyska pre pruzok
  TRurPlanCalendarHeader *pbh; ///< Sem si niekedy vykresluje veci, ktore nemaju rolovat
  Word FCommand;
  TPopupMenu *FVlastneMenu;

  MESSAGE void __fastcall WMPaint(Messages::TWMPaint &Message);
  void __fastcall SetColorNoc(TColor value);
  TColor __fastcall GetColorNoc();
  void __fastcall SetColorDen(TColor value);
  TColor __fastcall GetColorDen();
  void __fastcall SetTyp(TPlanCalendarTyp Value);
  void __fastcall SetScale(TPlanCalendarScale Value);
  void __fastcall SetDatum(TDate value);
  TDate __fastcall GetDatum();
  DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
  DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
  DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
  DYNAMIC void __fastcall DblClick(void);
  DYNAMIC void __fastcall Resize(void);
  void LMouseDown(int X,int Y);
  void LMouseUp(int X,int Y);
  void RMouseUp(int X,int Y);
  void StartDC(int &x,int &y);
  void EndDC();
  void __fastcall SetPDobaZaciatok(int value);
  int __fastcall GetPDobaZaciatok();
  void __fastcall SetPDobaKoniec(int value);
  int __fastcall GetPDobaKoniec();
  void __fastcall SetMeniny(AnsiString value);
  bool NeedPBH();
  bool FindDayXY(int X,int Y);
  bool FindDayItemXY(int X,int Y);
  MESSAGE void __fastcall CMHintShow(Messages::TMessage &Message);
  void UpdateRange();
  int TimeToCol(TDateTime t);
  TDateTime ColToTime(int c);

protected:
  virtual void __fastcall CreateParams(TCreateParams &Params);
  void PaintRecalculate();
    void PaintRecalculate1();
    void PaintRecalculate5();
    void PaintRecalculate7();
    void PaintRecalculate31();
    void PaintRecalculateU();
  void PaintBackground();
    void PaintBackground1();
    void PaintBackground5();
    void PaintBackground7();
    void PaintBackground31();
    void PaintBackgroundU();
  void PaintForeground();
    void PaintForeground1();
    void PaintForeground5();
    void PaintForeground7();
    void PaintForeground31();
    void PaintForegroundU();
  void DrawDebugRectangle(TRect r);

  void DrawGrid(int x,int ww,TDateTime d, int vect);
  void DrawPruzok();
  void CalcOdDo();
  void CalcTerminPos();
  void DrawSelection();
  void DrawSelTermin();
  void DrawDraggedItem();
  void DrawItem(TCanvas *c,RurCalendarItem *r1);

  DYNAMIC bool __fastcall DoMouseWheelDown(Classes::TShiftState Shift, const TPoint &MousePos);
  DYNAMIC bool __fastcall DoMouseWheelUp(Classes::TShiftState Shift, const TPoint &MousePos);
  DYNAMIC TPopupMenu* __fastcall GetPopupMenu(void);
  void __fastcall MMenuExecute(TObject *Sender);

public:
  __fastcall TRurPlanCalendar(TComponent* Owner);
  __fastcall ~TRurPlanCalendar();
  virtual void __fastcall Paint();
  virtual void __fastcall PaintHeader();
  void AddItem(RurCalendarItem &ri);
  void DeleteItem(long hash);
  void DeleteItem(RurCalendarItem *ri);
  void ClearItems();
  bool GetSel(TDateTime &from, TDateTime &to);
  void GetWorkTime(TDateTime &from, TDateTime &to);
  void SetWorkTime(TDateTime from, TDateTime to);
  TDateTime GetMin() {return FDatum_od;}
  TDateTime GetMax() {return FDatum_do;}
  long GetSelHash();
  RurCalendarItem* GetSelItem();
  void HideDate();
  void HideMeniny();
  AnsiString GetDateMeniny();
  AnsiString GetMenoMeniny(AnsiString _meno);
  void NeedRecalc() {needrecalc=true;}
  void SetHeader(TRurPlanCalendarHeader *pb);
  TRurPlanCalendarUsers* __fastcall GetUsers() {return users;}

  __property Graphics::TCanvas* Canvas = {read=FCanvas};

__published:
  __property TColor ColorNoc  = { read=GetColorNoc, write=SetColorNoc };
  __property TColor ColorDen  = { read=GetColorDen, write=SetColorDen };
  __property TPlanCalendarTyp Typ = {read=FTyp, write=SetTyp, default=0};
  __property TPlanCalendarScale Scale = {read=FScale, write=SetScale, default=1};
  __property Align;
  __property TDateTime Datum  = { read=GetDatum, write=SetDatum };
  __property TNotifyEvent OnRightMouse = {read=FOnRightMouse, write=FOnRightMouse};
  __property TNotifyEvent OnSelTerm = {read=FOnSelTerm, write=FOnSelTerm};
  __property TPlanEditEvent OnSelTerm2 = {read=FOnSelTerm2, write=FOnSelTerm2};
  __property TNotifyEvent OnDblClick = {read=FOnDblClick, write=FOnDblClick};
  __property TPlanEditEvent OnEditTerm = {read=FOnEditTerm, write=FOnEditTerm};
  __property TPlanMoveTerm OnMoveTerm = {read=FOnMoveTerm, write=FOnMoveTerm};
  __property TabStop;
  __property TabOrder;
  __property Hint;
  __property ShowHint;
  __property PopupMenu;
  __property int PDobaZaciatok  = { read=GetPDobaZaciatok, write=SetPDobaZaciatok };
  __property int PDobaKoniec  = { read=GetPDobaKoniec, write=SetPDobaKoniec };
  __property AnsiString Meniny  = { read=FMeniny, write=SetMeniny };
  __property OnMouseDown;
  __property OnMouseUp;
  __property Visible;

BEGIN_MESSAGE_MAP
    VCL_MESSAGE_HANDLER(WM_PAINT, TWMPaint, WMPaint);
    VCL_MESSAGE_HANDLER(CM_HINTSHOW, TMessage, CMHintShow);
END_MESSAGE_MAP(TScrollingWinControl);

  friend TRurPlanCalendarHeader;
  friend TRurPlanCalendarHeaderItem;
};

///
/// Hlavicka ku planovaciemu kalendaru
///
class PACKAGE TRurPlanCalendarHeader : public TScrollingWinControl
{
private:
  Graphics::TCanvas *FCanvas;  ///< Sem kreslim
  Graphics::TBitmap *bpruzokh; ///< Bitmapa na pruzky
  TRurPlanCalendar *master;    ///< Moj majitel
  v_items items; ///< Zoznam predpocitanych poloziek
  TRect plochy[5];             ///< Plochy do ktorych mozno umiestnit objekty
  TRurPlanCalendarHeaderItem *sel; ///< Oznaceny prvok
  TNotifyEvent FOnDblClick;
  TPlanEditEvent FOnEditTerm;
  TPlanEditEvent FOnSelTerm;

  MESSAGE void __fastcall WMPaint(Messages::TWMPaint &Message);
  virtual void __fastcall Paint();
  void DrawPruzok();
  void DrawGrid(int x,int ww,TDateTime d);
  void DrawItem(TCanvas *c,TRurPlanCalendarHeaderItem *i);
  DYNAMIC void __fastcall Resize(void);
  MESSAGE void __fastcall CMHintShow(Messages::TMessage &Message);

protected:
  virtual void __fastcall CreateParams(TCreateParams &Params);
  DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
  DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
  DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
  DYNAMIC void __fastcall DblClick(void);
  void StartDC(int &x,int &y);
  void EndDC();

public:
  __fastcall TRurPlanCalendarHeader(TComponent* Owner);
  __fastcall ~TRurPlanCalendarHeader();
  void Prepocitaj();
  RurCalendarItem* GetSelItem();

  __property Graphics::TCanvas* Canvas = {read=FCanvas};

__published:
  __property Align;
  __property TabStop;
  __property TabOrder;
  __property PopupMenu;
  __property Visible;
  __property Constraints;
  __property TNotifyEvent OnDblClick = {read=FOnDblClick, write=FOnDblClick};
  __property TPlanEditEvent OnEditTerm = {read=FOnEditTerm, write=FOnEditTerm};
  __property TPlanEditEvent OnSelTerm = {read=FOnSelTerm, write=FOnSelTerm};

BEGIN_MESSAGE_MAP
    VCL_MESSAGE_HANDLER(WM_PAINT, TWMPaint, WMPaint);
    VCL_MESSAGE_HANDLER(CM_HINTSHOW, TMessage, CMHintShow);
END_MESSAGE_MAP(TScrollingWinControl);

  friend TRurPlanCalendar;
};

//---------------------------------------------------------------------------
#endif



