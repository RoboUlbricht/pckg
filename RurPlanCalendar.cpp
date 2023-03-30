/*
 * Copyright (c) 2004-2006,2007 Ing. Robert Ulbricht
 *
 * Projekt: Optivus
 * Obsah: TRurPlanCalendar
 * Planovaci kalendar
 */

#include <vcl.h>
#pragma hdrstop

#include "RurPlanCalendar.h"
#pragma package(smart_init)
#include <algorithm>
#include <stdio.h>
#include <dateutils.hpp>

#define DEBUG(s)
//#define DEBUG(s) DebugString(s);

void Paint3dRamik(TCanvas *CC,TRect r1,AnsiString as);

//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TRurPlanCalendar *)
{
new TRurPlanCalendar(NULL);
}

static inline void ValidCtrCheck(TRurPlanCalendarHeader *)
{
new TRurPlanCalendarHeader(NULL);
}

namespace Rurplancalendar
{
  void __fastcall PACKAGE Register()
    {
    TComponentClass classes[2] = {__classid(TRurPlanCalendar),__classid(TRurPlanCalendarHeader)};
    RegisterComponents("Rur", classes, 1);
    }
}


struct RRect : public TRect
  {
  RRect() {left=0; top=0; right=0; bottom=0;}
  RRect(TRect r) {left=r.left;top=r.top;right=r.right;bottom=r.Bottom;}
  RRect(int a,int b,int c,int d) {left=a;top=b;right=c;bottom=d;}
  void Okraj(int a,int b) {left-=a;right+=a;top-=b;bottom+=b;}
  };

///
/// Vykreslenie polozky
///
void TRurPlanCalendarHeaderItem::DrawItem(TCanvas *c,TRurPlanCalendar *plan)
{
RRect rr(r);
RRect rrcolor=rr;rrcolor.right=rrcolor.left+4;
RRect rrwhite=rr;rrwhite.Left+=4;
c->Brush->Color=clWhite;
c->FillRect(rrwhite);
c->Brush->Color=ri->color;
c->FillRect(rrcolor);
InflateRect(&rrwhite,-2,-2);
c->Brush->Color=clWhite;
if(ri->splnene)
  c->Font->Style=TFontStyles()<<fsStrikeOut;
else
  c->Font->Style=TFontStyles();
::DrawTextA(c->Handle,ri->text.c_str(),-1,&rrwhite,DT_LEFT|DT_WORDBREAK);
if(plan->days_selecteditem==this)
  c->Brush->Color = clRed;
else
  c->Brush->Color = clBlack;
c->FrameRect(rr);
}

/////////////////////////////////////////////////////
// Meniny
/////////////////////////////////////////////////////

struct meniny
  {
  int den;
  int mesiac;
  AnsiString meno;
  };

class Meniny
  {
public:
  int count;
  meniny meniny[500];
public:
  Meniny() {count=0;}
  AnsiString Find(int d,int m);
  AnsiString Find(TDateTime d);
  AnsiString FindMeno(AnsiString _meno);
  };

AnsiString Meniny::Find(int d,int m)
  {
  AnsiString tmp;
  for(int i=0;i<count;i++)
    if(meniny[i].den==d && meniny[i].mesiac==m)
      {
      tmp=meniny[i].meno;
      for(int j=i+1;j<count;j++)
        if(meniny[j].den==d && meniny[j].mesiac==m)
          {
          if(tmp.Length()) tmp+=", ";
          tmp+=meniny[j].meno;
          }
      return tmp;
      }
  return tmp;
  }

AnsiString Meniny::Find(TDateTime d)
  {
  unsigned short rr,mm,dd;
  d.DecodeDate(&rr,&mm,&dd);
  return Find(dd,mm);
  }

AnsiString Meniny::FindMeno(AnsiString _meno)
  {
  for(int i=0;i<count;i++)
    if(meniny[i].meno==_meno)
      return AnsiString(meniny[i].den)+"."+meniny[i].mesiac+".";
  return "";
  }

Meniny men;

/////////////////////////////////////////////////////
// RPRecalcator
/////////////////////////////////////////////////////

class RPRecalcator
  {
  RurCalendarItem *pole[240][8];
public:
  RPRecalcator();
  bool FindFreeSpace(RurCalendarItem *rci,int top,int bottom,int &kde);
  void SetSirky(int top,int bottom,int last,int left, int sirka);
  };

RPRecalcator::RPRecalcator()
{
for(int i=0;i<240;i++)
  for(int j=0;j<8;j++)
    pole[i][j]=NULL;
}

bool RPRecalcator::FindFreeSpace(RurCalendarItem *rci,int top,int bottom,int &kde)
{
for(int i=0;i<8;i++)
  {
  if(pole[top][i]==NULL) // tu je volne
    {
    for(int j=top;j<bottom;j++)
      if(pole[j][i]!=NULL) // nie je volne
        goto dalej;
    kde=i;
    goto naslisme;
    }
dalej:
  }
return false;
naslisme:
for(int j=top;j<bottom;j++)
  pole[j][kde]=rci;
// v stlpci kde sme to nasli
return true;
}

void RPRecalcator::SetSirky(int top,int bottom,int last,int left, int sirka)
{
for(int i=top;i<bottom;i++)
  for(int j=0;j<8;j++)
    if(pole[i][j] && j>last)
      last=j;
int w=sirka/(last+1); // sirka jedneho objektu
for(int i=top;i<bottom;i++)
  for(int j=0;j<=last;j++)
    if(pole[i][j])
      {
      pole[i][j]->r.Left=left+j*w;
      pole[i][j]->r.Right=left+j*w+w;
      }
}

///
/// \brief Porovnanie dvoch poloziek
///
bool IsCalendarUserLess(TRurPlanCalendarUser &a, TRurPlanCalendarUser &b)
{
return a.meno<b.meno;
}

///
/// Pridanie noveho uzivatela
///
TRurPlanCalendarUser* TRurPlanCalendarUsers::AddUser(int id, AnsiString meno)
{
push_back(TRurPlanCalendarUser(id, meno));
selected = id;
std::sort(this->begin(), this->end(), IsCalendarUserLess);
parent->NeedRecalc();
parent->Invalidate();
for(v_users::iterator i=begin(); i!=end(); i++)
  {
  if(id==i->id)
    return &(*i);
  }
return NULL;
}

///
/// Vymazanie uzivatela
///
bool TRurPlanCalendarUsers::DeleteUser(int id)
{
for(v_users::iterator i=begin();i!=end();i++)
  {
  if(id==i->id)
    {
    erase(i);
    parent->Invalidate();
    parent->NeedRecalc();
    return true;
    }
  }
return false;
}

///
/// Oznacenie uzivatela
///
void TRurPlanCalendarUsers::SetSelected(int id)
{
for(v_users::iterator i=begin(); i!=end(); i++) {
  if(id==i->id) {
    selected = id;
    return;
    }
  }
selected = 0;
}

///
/// Ziskanie oznaceneho uzivatela
///
TRurPlanCalendarUser* TRurPlanCalendarUsers::GetSelectedUser()
{
for(v_users::iterator i=begin(); i!=end(); i++) {
  if(selected==i->id) {
    return &*i;;
    }
  }
return NULL;
}

/////////////////////////////////////////////////////
// TRurPlanCalendar
/////////////////////////////////////////////////////

///
/// Konstruktor
///
__fastcall TRurPlanCalendar::TRurPlanCalendar(TComponent* Owner)
        : TScrollingWinControl(Owner), dc_count(0)
{
hidedate=false;
hidemeniny=false;
Width=200;
Height=300;
FCommand=-1;
FVlastneMenu=NULL;
FBunkaHeight=20;
FBunkaCount=48;
FBunkaMultiply=2;
FBunkaMinuty=30;
BarHeight=0;
pbh=NULL;
ColorDen=TColor(0x00D2FFFF);
ColorNoc=TColor(0x0000AEAE);
FTyp=pcplan1;
FScale=pcps30;
FTypDays=1;
UpdateRange();
//FPDobaZaciatok=8*2;
//FPDobaKoniec=17*2;
SetWorkTime(TDateTime(8,0,0,0),TDateTime(16,30,0,0));
FCanvas=new TCanvas();
bpruzokh=new Graphics::TBitmap();
AnsiString tmp=ExtractFilePath(ParamStr(0))+"img\\pruzokh.bmp";
if(FileExists(tmp))
  bpruzokh->LoadFromFile(tmp);
SelBunka=SelBunka2=-1; // nic neselektovane
drag=false;
needrecalc=true;
dragmode=0; // selektovanie
days_selected=NULL;
days_selecteditem=NULL;
FOnSelTerm2=NULL;
users =new TRurPlanCalendarUsers(this);
FOnUserSelect = NULL;
}

///
/// Destruktor
///
__fastcall TRurPlanCalendar::~TRurPlanCalendar()
{
if(FVlastneMenu) {PopupMenu=NULL;delete FVlastneMenu;}
delete bpruzokh;
delete FCanvas;
delete users;
}

///
/// Nastavenie XP okrajov
///
void __fastcall TRurPlanCalendar::CreateParams(TCreateParams &Params)
{
TScrollingWinControl::CreateParams(Params);
//Params.Style   |= WS_BORDER;
Params.ExStyle |= WS_EX_CLIENTEDGE;
}

void TRurPlanCalendar::DebugString(String s)
{
OutputDebugString(s.c_str());
}

///
/// Vykreslenie hlavicky
///
void __fastcall TRurPlanCalendar::PaintHeader()
{
if(BarHeight==0) return;
RRect r8(0,0,Width,BarHeight);
// biele pozadie a vnorenie do plochy
Canvas->Brush->Color=clWhite;
Canvas->FillRect(r8);
}

///
/// Vykreslenie objektu
///
void __fastcall TRurPlanCalendar::Paint()
{
Canvas->Font->Style=TFontStyles();
Canvas->Font->Color=clBlack;
// ak treba prepocitat, resetnem predpocitane vektory
if(needrecalc)
  {
  DEBUG(L"Reset vectors");
  for(int i=0;i<31;i++) v_time[i].clear();
  for(int i=0;i<5;i++) v_date[i].clear();
  PaintRecalculate();
  }
int WW;
switch(FTyp)
  {
  case pcplan1:
    WW=ClientWidth-40-1;
    SelSirka=WW;
    DrawGrid(40,WW,Datum,0);
    if(SelBunka!=-1)
      {
      RRect r8(40,SelBunka*20,40+WW,SelBunka2*20);
      ::InvertRect(Canvas->Handle,&r8);
      }
    DrawPruzok();
    break;
  case pcplan5:
    {
    WW = (Width-8-40-16)/5; SelSirka = WW;
    for(int i=0; i<5; i++) {
      days[i].d = FDatum_od + i;
      DrawGrid(40+WW*i+2*i, WW, days[i].d, i);
      days[i].r = Rect(40+WW*i+2*i, 0, 40+WW*i+2*i+WW, 24*2*20);
      }
    DrawPruzok();
    }
    break;
  };
if(needrecalc && pbh) // zobrazenie alebo vypnutie hedera
  {
  pbh->Visible=NeedPBH();
  }
needrecalc=false;
PaintBackground();
PaintForeground();
}

///
/// Prepocitanie umiestnenia
///
void TRurPlanCalendar::PaintRecalculate()
{
switch(FTyp)
  {
  case pcplan1:PaintRecalculate1();break;
  case pcplan5:PaintRecalculate5();break;
  case pcplan7:PaintRecalculate7();break;
  case pcplan31:PaintRecalculate31();break;
  case pcplanU:PaintRecalculateU();break;
  };
}

///
/// Prepocitanie umiestnenia
///
void TRurPlanCalendar::PaintRecalculate1()
{

}

///
/// Prepocitanie umiestnenia
///
void TRurPlanCalendar::PaintRecalculate5()
{

}

///
/// Prepocitanie umiestnenia
///
void TRurPlanCalendar::PaintRecalculate7()
{
int cx=ClientWidth;
int x=ClientWidth/2;
int y=Height/3;
days[0].r=Rect(0,0,x,y);
days[1].r=Rect(0,y,x,2*y);
days[2].r=Rect(0,2*y,x,ClientHeight);
days[3].r=Rect(x,0,cx,y);
days[4].r=Rect(x,y,cx,2*y);
days[5].r=Rect(x,2*y,cx,2*y+y/2);
days[6].r=Rect(x,2*y+y/2,cx,ClientHeight);
TDateTime d=FDatum_od;
for(int i=0;i<7;i++)
  {
  days[i].d=d++;
  days[i].items.clear();
  }
// rozmiestnenie terminov
for(int i=0;i<rca.GetItemsInContainer();i++)
  {
  RurCalendarItem &r1=rca[i];
  TDateTime ts=DateOf(r1.from);
  TDateTime tc=DateOf(r1.to);
  // ak sa trafim do dna, dam ho do zoznamu
  for(int j=0;j<7;j++)
    {
    if(ts<=days[j].d && days[j].d<=tc)
      {
      TRurPlanCalendarHeaderItem hi;
      hi.ri=&r1;
      days[j].items.push_back(hi);
      }
    }
  }
#define VV 17
// usporiadanie poloziek v kazdom dni
for(int j=0;j<7;j++)
  {
  TRect r=days[j].r;
  r.top+=VV;
  int poc=days[j].items.size();
  if(poc==0) continue;
  int vi=r.Height()/VV; // tolko sa zmesti na vysku
  if(vi==0) // prilis male, do bunky nic nevykreslim
    {
    days[j].items.clear();
    poc=0;
    }
  int cols=0;
  while(poc>0 && cols<5) {cols++;poc-=vi;} // tolko stlpcov mam
  poc=days[j].items.size();
  int x=0;
  for(v_items::iterator i=days[j].items.begin();i!=days[j].items.end();i++,x++)
    {
    int x1=x/vi;
    int y1=x%vi;
    int s=r.Width()/cols;
    (*i).r=Rect(r.left+x1*s,r.top+y1*VV,r.left+x1*s+s,r.top+y1*VV+VV+1);
    }
  }
}

///
/// Prepocitanie umiestnenia
///
void TRurPlanCalendar::PaintRecalculate31()
{
TDateTime d=FDatum_od;
int x=DayOfTheWeek(d)-1;
int _m=DaysInMonth(d);
int _rows=5;
if(_m==28 && x==0) _rows=4;
if(_m==31 && x==5) _rows=6;
if(_m>=30 && x==6) _rows=6;
int bx=ClientWidth/6;
int by=ClientHeight/_rows;
// predpocitanie dat
d-=x;
for(int i=0;i<x;i++)
  {
  days[i].d=d++;
  days[i].c=ColorNoc;
  }
for(int i=x;i<42;i++)
  {
  days[i].d=d++;
  days[i].c=ColorDen;
  }
d=FDatum_do;
x=DayOfTheWeek(d)-1;
for(int i=(_rows-1)*7+1+x;i<42;i++)
  {
//  days[i].d=d++;
  days[i].c=ColorNoc;
  }

for(int i=0;i<6;i++)
  {
  for(int j=0;j<42;j+=7)
    {
    days[j+i].r.left=bx*i;days[j+i].r.right=bx*i+bx;
    if(i==5)
      {
      days[j+i+1].r.left=bx*i;
      days[j+i+1].r.right=ClientWidth;
      days[j+i].r.right=ClientWidth;
      }
    }
  }
for(int i=0;i<_rows;i++)
  {
  for(int j=0;j<6;j++)
    {
    days[j+i*7].r.top=by*i;days[j+i*7].r.bottom=by*i+by;
    if(j==5)
      {
      days[j+i*7].r.bottom=by*i+by/2;
      days[j+i*7+1].r.top=by*i+by/2;
      days[j+i*7+1].r.bottom=by*i+by;
      }
    if(i==(_rows-1) && j!=5) days[j+i*7].r.bottom=ClientHeight;
    if(i==(_rows-1) && j==5) days[j+i*7+1].r.bottom=ClientHeight;
    }
  }
for(int j=0;j<42;j++)
  days[j].items.clear();
// rozmiestnenie terminov
for(int i=0;i<rca.GetItemsInContainer();i++)
  {
  RurCalendarItem &r1=rca[i];
  TDateTime ts=DateOf(r1.from);
  TDateTime tc=DateOf(r1.to);
  // ak sa trafim do dna, dam ho do zoznamu
  for(int j=0;j<42;j++)
    {
    if(ts<=days[j].d && days[j].d<=tc)
      {
      TRurPlanCalendarHeaderItem hi;
      hi.ri=&r1;
      days[j].items.push_back(hi);
      }
    }
  }
#define VV 17
// usporiadanie poloziek v kazdom dni
for(int j=0;j<42;j++)
  {
  TRect r=days[j].r;
//  r.top+=VV;
  int poc=days[j].items.size();
  if(poc==0) continue;
  int vi=r.Height()/VV; // tolko sa zmesti na vysku
  if(vi==0) // prilis male, do bunky nic nevykreslim
    {
    days[j].items.clear();
    poc=0;
    }
  int cols=0;
  while(poc>0 && cols<5) {cols++;poc-=vi;} // tolko stlpcov mam
  poc=days[j].items.size();
  int x=0;
  for(v_items::iterator i=days[j].items.begin();i!=days[j].items.end();i++,x++)
    {
    int x1=x/vi;
    int y1=x%vi;
    int s=r.Width()/cols;
    (*i).r=Rect(r.left+x1*s,r.top+y1*VV,r.left+x1*s+s,r.top+y1*VV+VV+1);
    }
  }
}

///
/// Prepocitanie umiestnenia
///
void TRurPlanCalendar::PaintRecalculateU()
{
int poc=users->size();
if(poc==0) return;
int xx=40;
int sirka=ClientWidth-xx;
int s1=sirka/poc;
SelSirka = s1 - 1;
int por=0;
int diff=ClientWidth-xx-s1*poc+1;
for(v_users::iterator i=users->begin();i!=users->end();i++)
  {
  i->r=Rect(xx+s1*por,0,xx+s1*por+s1-2,48*20);
  i->time.clear();
  i->days.clear();
  i->items.clear();
  por++;
  }
(*users)[poc-1].r.Right+=diff;

// rozhodenie poloziek medzi jednotlive stlpce s uzivatelmi
for(int i=0; i<rca.GetItemsInContainer(); i++)
  {
  RurCalendarItem &r1 = rca[i];
  int id = r1.uid;
  for(v_users::iterator i=users->begin();i!=users->end();i++)
    {
    if(i->id==id)
      i->items.push_back(&r1);
    }
  }
// spocitanie polohy uloh kazdeho uzivatela v zozname
for(v_users::iterator j=users->begin();j!=users->end();j++)
  {
  RPRecalcator rrc;
  TDateTime d=FDatum;
  TDateTime d_end=EndOfTheDay(d);
  int d_int=d;
  DEBUG("Need recalc grid");
  for(std::vector<RurCalendarItem*>::iterator i=j->items.begin();i!=j->items.end();i++)
    {
    RurCalendarItem &r1=*(*i);
    int from_int=r1.from;
    if(r1.from<d_end && d<r1.to && r1.from<d) // zaciatok alebo koniec je v inom dni
      {
      j->days.push_back(&r1);
      }
    else if(r1.from<d_end && d<r1.to && r1.to>d_end) // zaciatok alebo koniec je v inom dni
      {
      j->days.push_back(&r1);
      }
    else if(from_int==d_int && r1.from==r1.to) // bez hodiny v dni
      {
      j->days.push_back(&r1);
      }
    else if(from_int==d_int) // prepocitam iba v mojom stlpci
      {
      int _f=TimeToCol(TimeOf(r1.from));
      int _t=TimeToCol(TimeOf(r1.to));
      if(_t<=_f)
        _t=_f+1;
      r1.r.Top=_f*20;
      r1.r.Bottom=_t*20+1;
      int kde;
      if(rrc.FindFreeSpace(&r1,_f,_t,kde))
        { // este poposuvam sirky
        rrc.SetSirky(_f,_t,kde,j->r.Left+5,j->r.Right-j->r.Left-10);
        }
      j->time.push_back(&r1);
      DEBUG(r1.Debug());
      }
    }

  }
}

///
/// Vykreslenie pozadia
///
void TRurPlanCalendar::PaintBackground()
{
TRect r=Rect(0,0,ClientWidth,ClientHeight);
//DrawDebugRectangle(r);
switch(FTyp)
  {
  case pcplan1:PaintBackground1();break;
  case pcplan5:PaintBackground5();break;
  case pcplan7:PaintBackground7();break;
  case pcplan31:PaintBackground31();break;
  case pcplanU:PaintBackgroundU();break;
  };
}

///
/// Vykreslenie pozadia
///
void TRurPlanCalendar::PaintBackground1()
{

}

///
/// Vykreslenie pozadia
///
void TRurPlanCalendar::PaintBackground5()
{

}

///
/// Vykreslenie pozadia
///
void TRurPlanCalendar::PaintBackground7()
{
RRect rr(0,0,Width,Height);
Canvas->Brush->Color=ColorDen;
Canvas->FillRect(rr);
Canvas->Pen->Color=TColor(0);
//int cx=ClientWidth;
int x=ClientWidth/2;
int y=Height/3;

Canvas->MoveTo(0,0);Canvas->LineTo(0,Height);
Canvas->MoveTo(x,0);Canvas->LineTo(x,Height);
Canvas->MoveTo(Width-1,0);Canvas->LineTo(Width-1,Height);

Canvas->MoveTo(0,0);Canvas->LineTo(Width,0);
Canvas->MoveTo(0,y);Canvas->LineTo(Width,y);
Canvas->MoveTo(0,y*2);Canvas->LineTo(Width,y*2);
Canvas->MoveTo(0,Height-1);Canvas->LineTo(Width,Height-1);
Canvas->MoveTo(x,y*2+y/2);Canvas->LineTo(Width,y*2+y/2);
TRect r1;
for(int i=0;i<7;i++)
  {
  r1=days[i].r;
  r1.bottom=r1.top+17;
  if(FDatum==days[i].d)
    Canvas->Font->Style=TFontStyles()<<fsBold;
  else
    Canvas->Font->Style=TFontStyles();
  Paint3dRamik(Canvas,r1,days[i].d.FormatString("d.mmmm yyyy"));
  }
//for(int i=0;i<7;i++)
//  DrawDebugRectangle(days[i].r);
}

///
/// Vykreslenie pozadia
///
void TRurPlanCalendar::PaintBackground31()
{
TDateTime d=FDatum_od;
int x=DayOfTheWeek(d)-1;
int _m=DaysInMonth(d);
int _rows=5;
if(_m==28 && x==0) _rows=4;
if(_m==31 && x==5) _rows=6;
if(_m>=30 && x==6) _rows=6;
RRect rr(0,0,ClientWidth,ClientHeight);
Canvas->Brush->Color=ColorDen;
Canvas->FillRect(rr);
int bx=ClientWidth/6;
int by=ClientHeight/_rows;
// vykreslenie stvorcov
for(int i=0;i<7*_rows;i++)
  {
  Canvas->Brush->Color=days[i].c;
  TRect r=days[i].r;
  Canvas->FillRect(r);
  InflateRect(&r,-3,-2);
  if(FDatum==days[i].d)
    Canvas->Font->Style=TFontStyles()<<fsBold;
  else
    Canvas->Font->Style=TFontStyles();
  ::DrawText(Canvas->Handle,days[i].d.FormatString("d.m.").c_str(),-1,&r,DT_RIGHT|DT_BOTTOM|DT_SINGLELINE);
  }
for(int i=0;i<6;i++)
  {
  Canvas->MoveTo(bx*i,0);Canvas->LineTo(bx*i,ClientHeight);
  }
for(int i=0;i<_rows;i++)
  {
  Canvas->MoveTo(0,by*i);Canvas->LineTo(ClientWidth,by*i);
  Canvas->MoveTo(5*bx,by*i+by/2);Canvas->LineTo(ClientWidth,by*i+by/2);
  }
//for(int i=0;i<35;i++)
//  DrawDebugRectangle(days[i].r);
}

///
/// Vykreslenie pozadia
///
void TRurPlanCalendar::PaintBackgroundU()
{
DrawPruzok();
if(users->size()==0)
  {
  RRect r(40,0,ClientWidth,24*2*20);
  Canvas->Brush->Color=clBackground;
  Canvas->FillRect(r);
  Canvas->Font->Color=clWhite;
  Canvas->Font->Size=14;
  Canvas->Font->Name="Tahoma";
  ::DrawTextA(Canvas->Handle,"Nie sú vybrané žiadne osoby.",-1,&r,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
  return;
  }
for(v_users::iterator i=users->begin();i!=users->end();i++)
  {
  RRect r8(i->r.Left,0,i->r.Right,FPDobaZaciatok*FBunkaHeight);
  RRect r16(i->r.Left,FPDobaZaciatok*FBunkaHeight,i->r.Right,FPDobaKoniec*FBunkaHeight);
  RRect r24(i->r.Left,FPDobaKoniec*FBunkaHeight,i->r.Right,24*FBunkaHeight*FBunkaMultiply);
// biele pozadie a vnorenie do plochy
  Canvas->Brush->Color=ColorNoc;
  Canvas->FillRect(r8);
  Canvas->FillRect(r24);
  r8.Left+=2;r24.Left+=2;
  if(i->id==users->Selected)
    Canvas->Font->Style = TFontStyles()<<fsBold;
  else
    Canvas->Font->Style = TFontStyles();
  ::DrawTextA(Canvas->Handle,i->meno.c_str(),-1,&r8,DT_LEFT|DT_TOP|DT_SINGLELINE);
  ::DrawTextA(Canvas->Handle,i->meno.c_str(),-1,&r8,DT_RIGHT|DT_BOTTOM|DT_SINGLELINE);
  ::DrawTextA(Canvas->Handle,i->meno.c_str(),-1,&r24,DT_LEFT|DT_TOP|DT_SINGLELINE);
  ::DrawTextA(Canvas->Handle,i->meno.c_str(),-1,&r24,DT_RIGHT|DT_BOTTOM|DT_SINGLELINE);
  Canvas->Brush->Color=ColorDen;
  Canvas->FillRect(r16);
  r16.Left+=2;
  ::DrawTextA(Canvas->Handle,i->meno.c_str(),-1,&r16,DT_LEFT|DT_TOP|DT_SINGLELINE);
  ::DrawTextA(Canvas->Handle,i->meno.c_str(),-1,&r16,DT_RIGHT|DT_BOTTOM|DT_SINGLELINE);
  int x=i->r.Left;
  int ww=i->r.Right-i->r.Left;
  for(int i=0;i<24;i++)
    {
    Canvas->Pen->Color=TColor(0xA2DBF6);
    Canvas->MoveTo(x,i*FBunkaHeight*FBunkaMultiply);Canvas->LineTo(x+ww,i*FBunkaHeight*FBunkaMultiply);
    Canvas->Pen->Color=TColor(clBtnFace);
    for(int j=1;j<FBunkaMultiply;j++)
      {Canvas->MoveTo(x,i*FBunkaHeight*FBunkaMultiply+FBunkaHeight*j);Canvas->LineTo(x+ww,i*FBunkaHeight*FBunkaMultiply+FBunkaHeight*j);}
    }

  Canvas->Pen->Color=clBlack;
  Canvas->MoveTo(x,0);Canvas->LineTo(x,24*FBunkaHeight*FBunkaMultiply);
  Canvas->MoveTo(x+ww,0);Canvas->LineTo(x+ww,24*FBunkaHeight*FBunkaMultiply);
  Canvas->MoveTo(x,0);Canvas->LineTo(x+ww,0);
  Canvas->MoveTo(x,24*FBunkaHeight*FBunkaMultiply);Canvas->LineTo(x+ww,24*FBunkaHeight*FBunkaMultiply);
//  DrawDebugRectangle(i->r);
  }
}

///
/// Vykreslenie popredia
///
void TRurPlanCalendar::PaintForeground()
{
switch(FTyp)
  {
  case pcplan1:PaintForeground1();break;
  case pcplan5:PaintForeground5();break;
  case pcplan7:PaintForeground7();break;
  case pcplan31:PaintForeground31();break;
  case pcplanU:PaintForegroundU();break;
  };
}

///
/// Vykreslenie popredia
///
void TRurPlanCalendar::PaintForeground1()
{

}

///
/// Vykreslenie popredia
///
void TRurPlanCalendar::PaintForeground5()
{

}

///
/// Vykreslenie popredia
///
void TRurPlanCalendar::PaintForeground7()
{
for(int j=0;j<7;j++)
  {
  for(v_items::iterator i=days[j].items.begin();i!=days[j].items.end();i++)
    {
    (*i).DrawItem(Canvas,this);
//    DrawDebugRectangle((*i).r);
    }
  }
}

///
/// Vykreslenie popredia
///
void TRurPlanCalendar::PaintForeground31()
{
for(int j=0;j<42;j++)
  {
  for(v_items::iterator i=days[j].items.begin();i!=days[j].items.end();i++)
    {
    (*i).DrawItem(Canvas,this);
//    DrawDebugRectangle((*i).r);
    }
  }
}

///
/// Vykreslenie popredia
///
void TRurPlanCalendar::PaintForegroundU()
{
for(v_users::iterator j=users->begin();j!=users->end();j++)
for(std::vector<RurCalendarItem*>::iterator i=j->time.begin();i!=j->time.end();i++)
  {
  RurCalendarItem *r1=(*i);
  DEBUG(r1->Debug());
  DrawItem(Canvas,r1);
  }
}

///
/// Vykreslenie pomocnych znaciek pre ladenie
///
void TRurPlanCalendar::DrawDebugRectangle(TRect r)
{
r.right--;
r.bottom--;
TColor cp=Canvas->Pen->Color;
TColor cf=Canvas->Font->Color;
Canvas->Pen->Color=clBlue;
Canvas->MoveTo(r.Left,r.Top);Canvas->LineTo(r.Left+5,r.Top);Canvas->LineTo(r.Left,r.Top+5);Canvas->LineTo(r.Left,r.Top);
Canvas->MoveTo(r.Right,r.Top);Canvas->LineTo(r.Right-5,r.Top);Canvas->LineTo(r.Right,r.Top+5);Canvas->LineTo(r.Right,r.Top);
Canvas->MoveTo(r.Left,r.Bottom);Canvas->LineTo(r.Left+5,r.Bottom);Canvas->LineTo(r.Left,r.Bottom-5);Canvas->LineTo(r.Left,r.Bottom);
Canvas->MoveTo(r.Right,r.Bottom);Canvas->LineTo(r.Right-5,r.Bottom);Canvas->LineTo(r.Right,r.Bottom-5);Canvas->LineTo(r.Right,r.Bottom);
AnsiString tmp;
tmp.printf("%d,%d,%d,%d",r.Left,r.Top,r.right,r.Bottom);
Canvas->Font->Color=clBlue;
//Canvas->TextOut(r.left+5,r.top+5,tmp);
Canvas->Pen->Style = psSolid;
Canvas->Pen->Color=cp;
Canvas->Font->Color=cf;
}

///
/// Vykreslenie zvisleho pruhu s cislami
///
void TRurPlanCalendar::DrawPruzok()
{
// vykreslenie pruzku s cislami
if(bpruzokh->HandleAllocated())
  {
  int pos=0;
  while(pos<VertScrollBar->Range)
    {
    Canvas->Draw(0,pos,bpruzokh);
    pos+=bpruzokh->Height;
    }
  }
::SetBkMode(Canvas->Handle, TRANSPARENT);
Canvas->Font->Color=clBlack;
Canvas->Font->Name="Tahoma";
AnsiString as;
for(int i=0;i<24;i++)
  {
  switch(FScale)
    {
    case pcps60:
      {
      Canvas->MoveTo(0,i*FBunkaHeight);Canvas->LineTo(38,i*FBunkaHeight);
      RRect r1(0,i*FBunkaHeight,38,i*FBunkaHeight+FBunkaHeight);
      as.printf("%d:00",i);
      Canvas->Font->Size=8;
      ::DrawTextA(Canvas->Handle,as.c_str(),-1,&r1,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
      }
      break;
    case pcps30:
    case pcps15:
    case pcps10:
    case pcps6:
    case pcps5:
      {
      Canvas->MoveTo(0,i*FBunkaHeight*FBunkaMultiply);Canvas->LineTo(38,i*FBunkaHeight*FBunkaMultiply);
      RRect r1(0,i*FBunkaHeight*FBunkaMultiply,25,(i+1)*FBunkaHeight*FBunkaMultiply);
      as.printf("%d",i);
      Canvas->Font->Size=14;
      ::DrawTextA(Canvas->Handle,as.c_str(),-1,&r1,DT_RIGHT|DT_TOP|DT_SINGLELINE);
      Canvas->Font->Size=8;
      for(int j=0;j<FBunkaMultiply;j++)
        {
        Canvas->MoveTo(26,i*FBunkaHeight*FBunkaMultiply+FBunkaHeight*j);Canvas->LineTo(38,i*FBunkaHeight*FBunkaMultiply+FBunkaHeight*j);
        as.printf("%02d",j*FBunkaMinuty);
        Canvas->TextOut(26,i*FBunkaHeight*FBunkaMultiply+FBunkaHeight*j+3,as);
        }
      }
      break;
    }
  }
}

///
/// Vykreslenie jedneho stlpceka
/// \param d Ktory den sa vykresluje v stlpceku
/// \param vect Z tohoto predpocitaneho slotu zober zoznam
///
void TRurPlanCalendar::DrawGrid(int x,int ww,TDateTime d, int vect)
  {
  std::vector<RurCalendarItem*> &v=v_time[vect];
  int d_int=d;
  TDateTime d_end=EndOfTheDay(d);
  RRect r8(x,0,x+ww,FPDobaZaciatok*FBunkaHeight);
  RRect r16(x,FPDobaZaciatok*FBunkaHeight,x+ww,FPDobaKoniec*FBunkaHeight);
  RRect r24(x,FPDobaKoniec*FBunkaHeight,x+ww,24*FBunkaHeight*FBunkaMultiply);
// biele pozadie a vnorenie do plochy
  Canvas->Brush->Color=ColorNoc;
  Canvas->FillRect(r8);
  Canvas->FillRect(r24);
  if(FDatum==d)
    Canvas->Font->Style=TFontStyles()<<fsBold;
  else
    Canvas->Font->Style=TFontStyles();
  if(!hidedate)
    Canvas->TextOut(r8.left+3,r8.top+2,d.DateString().c_str());
  if(!hidedate)
    Canvas->TextOut(r24.left+3,r24.top+2,d.DateString().c_str());
  Canvas->Brush->Color=ColorDen;
  Canvas->FillRect(r16);
  if(!hidedate)
    Canvas->TextOut(r16.left+3,r16.top+2,d.DateString().c_str());
  Canvas->Font->Style=TFontStyles();
  if(men.count && hidemeniny==false)
    Canvas->TextOut(r16.left+3,r16.top+2+FBunkaHeight,men.Find(d));

  for(int i=0;i<24;i++)
    {
    Canvas->Pen->Color=TColor(0xA2DBF6);
    Canvas->MoveTo(x,i*FBunkaHeight*FBunkaMultiply);Canvas->LineTo(x+ww,i*FBunkaHeight*FBunkaMultiply);
    Canvas->Pen->Color=TColor(clBtnFace);
    for(int j=1;j<FBunkaMultiply;j++)
      {Canvas->MoveTo(x,i*FBunkaHeight*FBunkaMultiply+FBunkaHeight*j);Canvas->LineTo(x+ww,i*FBunkaHeight*FBunkaMultiply+FBunkaHeight*j);}
    }

  Canvas->Pen->Color=clBlack;
  Canvas->MoveTo(x,0);Canvas->LineTo(x,24*FBunkaHeight*FBunkaMultiply);
  Canvas->MoveTo(x+ww,0);Canvas->LineTo(x+ww,24*FBunkaHeight*FBunkaMultiply);
  Canvas->MoveTo(x,0);Canvas->LineTo(x+ww,0);
  Canvas->MoveTo(x,24*FBunkaHeight*FBunkaMultiply);Canvas->LineTo(x+ww,24*FBunkaHeight*FBunkaMultiply);
  // prepocitanie pozicie
  if(needrecalc)
    {
    RPRecalcator rrc;
    int poc=rca.GetItemsInContainer();
    DEBUG("Need recalc grid");
    for(int i=0;i<poc;i++)
      {
      RurCalendarItem &r1=rca[i];
      int from_int=r1.from;
      if(r1.from<d_end && d<r1.to && r1.from<d) // zaciatok alebo koniec je v inom dni
        {
        v_date[vect].push_back(&r1);
        }
      else if(r1.from<d_end && d<r1.to && r1.to>d_end) // zaciatok alebo koniec je v inom dni
        {
        v_date[vect].push_back(&r1);
        }
      else if(from_int==d_int && r1.from==r1.to && r1.from==DateOf(r1.from)) // bez hodiny v dni
        {
        v_date[vect].push_back(&r1);
        }
      else if(from_int==d_int) // prepocitam iba v mojom stlpci
        {
        int _f=TimeToCol(TimeOf(r1.from));
        int _t=TimeToCol(TimeOf(r1.to));
        if(_t<=_f)
          _t=_f+1;
        r1.r.Top=_f*20;
        r1.r.Bottom=_t*20+1;
        int kde;
        if(rrc.FindFreeSpace(&r1,_f,_t,kde))
          { // este poposuvam sirky
          rrc.SetSirky(_f,_t,kde,x+5,ww-10);
          }
        v.push_back(&r1);
        DEBUG(r1.Debug());
        }
      }
    }
  // vykreslenie jednotlivych terminov
  DEBUG("Draw");
  for(std::vector<RurCalendarItem*>::iterator i=v.begin();i!=v.end();i++)
    {
    RurCalendarItem *r1=(*i);
    DEBUG(r1->Debug());
    DrawItem(Canvas,r1);
    }
  Canvas->Font->Style=TFontStyles();
  }

///
/// Vykreslenie polozky
///
void TRurPlanCalendar::DrawItem(TCanvas *c,RurCalendarItem *r1)
{
RRect rr(r1->r);
RRect rrcolor=rr;rrcolor.right=rrcolor.left+4;
RRect rrwhite=rr;rrwhite.Left+=4;
c->Brush->Color=clWhite;
c->FillRect(rrwhite);
c->Brush->Color=r1->color;
c->FillRect(rrcolor);
InflateRect(&rrwhite,-2,-2);
c->Brush->Color=clWhite;
if(r1->splnene)
  c->Font->Style=TFontStyles()<<fsStrikeOut;
else
  c->Font->Style=TFontStyles();
::DrawTextA(c->Handle,r1->text.c_str(),-1,&rrwhite,DT_LEFT|DT_WORDBREAK);
if(rca.si==r1)
  {
  Canvas->Brush->Color = clRed;
  Canvas->FrameRect(rr);
  }
else
  {
  c->Brush->Color = clBlack;
  c->FrameRect(rr);
  }
}

///
/// True, ak potrebuje nieco kreslit do headera
///
bool TRurPlanCalendar::NeedPBH()
{
if(FTyp==pcplan1)
  {
  int poc=v_date[0].size();
  if(poc && pbh)
    pbh->Prepocitaj();
  return poc>0;
  }
if(FTyp==pcplan5)
  {
  unsigned int poc=0;
  if(v_date[0].size()>poc) poc=v_date[0].size();
  if(v_date[1].size()>poc) poc=v_date[1].size();
  if(v_date[2].size()>poc) poc=v_date[2].size();
  if(v_date[3].size()>poc) poc=v_date[3].size();
  if(v_date[4].size()>poc) poc=v_date[4].size();
  if(poc && pbh)
    pbh->Prepocitaj();
  return poc>0;
  }
if(FTyp==pcplanU)
  {
  pbh->Prepocitaj();
  return true;
  }
return false;
}

void Paint3dRamik(TCanvas *CC,TRect r1,AnsiString as)
  {
  CC->Brush->Color=TColor(clBtnFace);
  CC->FillRect(r1);
  Frame3D(CC,r1,clBtnHighlight,clBtnShadow,1);
  CC->Font->Color=TColor(clBlack);
  ::DrawTextA(CC->Handle,as.c_str(),-1,&r1,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
  }

///
/// Reakcia na spravu Windows na vykreslenie
///
MESSAGE void __fastcall TRurPlanCalendar::WMPaint(Messages::TWMPaint &Message)
  {
  HDC dc;
  TPaintStruct ps;
  dc=Message.DC;
  if(dc==0)
    {
    dc=::BeginPaint(Handle,&ps);
    Canvas->Handle=dc;

//    HRGN MyRgn;
//    MyRgn = ::CreateRectRgn(0,BarHeight,Width,Height);
//    ::SelectClipRgn(Canvas->Handle,MyRgn);

    ::SetWindowOrgEx(dc,0,VertScrollBar->Position-BarHeight,0);
    Paint();
//    ::SelectClipRgn(Canvas->Handle,NULL);
//    ::DeleteObject(MyRgn);
    ::SetWindowOrgEx(dc,0,0,0);
    PaintHeader();

    Canvas->Handle=0;
    ::EndPaint(Handle,&ps);
    }
  }

void __fastcall TRurPlanCalendar::SetColorNoc(TColor value)
{
if(FColorNoc != value) {
  FColorNoc = value;
  Invalidate();
  }
}

TColor __fastcall TRurPlanCalendar::GetColorNoc()
{
return FColorNoc;
}

void __fastcall TRurPlanCalendar::SetColorDen(TColor value)
{
if(FColorDen != value) {
  FColorDen = value;
  Invalidate();
  }
}

TColor __fastcall TRurPlanCalendar::GetColorDen()
{
return FColorDen;
}

void TRurPlanCalendar::CalcOdDo()
{
int dd=FDatum.DayOfWeek();
int dow=((FDatum.DayOfWeek() - 1 + 7) % 7)-1;
if(dd==1) dow=6;
switch(FTyp)
  {
  case pcplan1:FDatum_od=FDatum;FDatum_do=FDatum;break;
  case pcplan5:FDatum_od=FDatum-dow;FDatum_do=FDatum_od+4;break;
  case pcplan7:FDatum_od=FDatum-dow;FDatum_do=FDatum_od+6;break;
  case pcplan31:
    FDatum_od=StartOfTheMonth(FDatum);
    FDatum_do=DateOf(EndOfTheMonth(FDatum));
    break;
  }
}

void TRurPlanCalendar::CalcTerminPos()
  {
  switch(FTyp)
    {
    case pcplan1:break;
    case pcplan5:break;
    case pcplan7:
      {
      int max[7];for(int i=0;i<7;i++) max[i]=1;
      int cx=ClientWidth;
      int x=ClientWidth/2;
      int y=Height/3;
      TRect r;
      int SZ=18;
      for(int i=0;i<rca.GetItemsInContainer();i++)
        {
        RurCalendarItem &r1=rca[i];
        TDateTime ts=DateOf(r1.from);
        if(ts==FDatum_od)
          {r1.r=Rect(0,1+16*max[0],x+1,18+16*max[0]);max[0]++;}
        else if(ts==TDateTime(FDatum_od+1))
          {r1.r=Rect(0,y+1+16*max[1],x+1,y+SZ+16*max[1]);max[1]++;}
        else if(ts==TDateTime(FDatum_od+2))
          {r1.r=Rect(0,y*2+1+16*max[2],x+1,y*2+SZ+16*max[2]);max[2]++;}

        else if(ts==TDateTime(FDatum_od+3))
          {r1.r=Rect(x,1+16*max[3],cx,SZ+16*max[3]);max[3]++;}
        else if(ts==TDateTime(FDatum_od+4))
          {r1.r=Rect(x,y+1+16*max[4],cx,y+SZ+16*max[4]);max[4]++;}
        else if(ts==TDateTime(FDatum_od+5))
          {r1.r=Rect(x,y*2+1+16*max[5],cx,y*2+SZ+16*max[5]);max[5]++;}
        else if(ts==TDateTime(FDatum_od+6))
          {r1.r=Rect(x,y*2+y/2+1+16*max[6],cx,y*2+y/2+SZ+16*max[6]);max[6]++;}
        else
          r1.r=Rect(0,0,0,0);
        }
      }
      break;
    case pcplan31:
      break;
  }
  }

///
/// Nastavenie typu kalendara
///
void __fastcall TRurPlanCalendar::SetTyp(TPlanCalendarTyp value)
{
if(FTyp!=value)
  {
  FTyp=value;
  UpdateRange();
  CalcOdDo();
  NeedRecalc();
  Invalidate();
  }
}

///
/// Nastavenie mierky
///
void __fastcall TRurPlanCalendar::SetScale(TPlanCalendarScale Value)
{
if(FScale!=Value)
  {
  FScale=Value;
  switch(FScale)
    {
    case pcps60:
      FBunkaCount=60/60*24;
      FBunkaMultiply=1;
      break;
    case pcps30:
      FBunkaCount=60/30*24;
      FBunkaMultiply=2;
      break;
    case pcps15:
      FBunkaCount=60/15*24;
      FBunkaMultiply=4;
      break;
    case pcps10:
      FBunkaCount=60/10*24;
      FBunkaMultiply=6;
      break;
    case pcps6:
      FBunkaCount=60/6*24;
      FBunkaMultiply=10;
      break;
    case pcps5:
      FBunkaCount=60/5*24;
      FBunkaMultiply=12;
      break;
    }
  FBunkaMinuty=60/FBunkaMultiply;
  UpdateRange();
  NeedRecalc();
  Invalidate();
  }
}

///
/// Nastavi spravnu velkost scrolovacej listy
///
void TRurPlanCalendar::UpdateRange()
{
switch(FTyp)
  {
  case pcplan1:
    FTypDays=1;
    VertScrollBar->Range=FBunkaHeight*FBunkaCount+1;
    break;
  case pcplan5:
    FTypDays=5;
    VertScrollBar->Range=FBunkaHeight*FBunkaCount+1;
    break;
  case pcplan7:
    FTypDays=7;
    VertScrollBar->Range=0;
    break;
  case pcplan31:
    FTypDays=42; // par dni pred aj za, toto je cely stvorec
    VertScrollBar->Range=0;
    break;
  case pcplanU:
    FTypDays=1;
    VertScrollBar->Range=FBunkaHeight*FBunkaCount+1;
    break;
  };
FPDobaZaciatok=TimeToCol(FPDobaTZaciatok);
FPDobaKoniec=TimeToCol(FPDobaTKoniec);
}

///
/// Prevod datum na stlpec
///
int TRurPlanCalendar::TimeToCol(TDateTime t)
{
unsigned short hour, min, ss, sss;
t.DecodeTime(&hour, &min, &ss, &sss);
int _f = 0;
switch(FScale)
  {
  case pcps60:
    _f = hour;
    break;
  case pcps30:
  case pcps15:
  case pcps10:
  case pcps6:
  case pcps5:
    _f = hour * FBunkaMultiply + min / FBunkaMinuty;
    break;
  }
return _f;
}

///
/// Prevod stlpca na cas
///
TDateTime TRurPlanCalendar::ColToTime(int c)
{
short hour, mn;
switch(FScale)
  {
  case pcps60:
    hour = c;
    mn = 0;
    break;
  case pcps30:
    hour = c / FBunkaMultiply;
    mn = (c % FBunkaMultiply) * FBunkaMinuty;
    break;
  case pcps15:
    hour = c / FBunkaMultiply;
    mn = (c % FBunkaMultiply) * FBunkaMinuty;
    break;
  case pcps10:
    hour = c / FBunkaMultiply;
    mn = (c % FBunkaMultiply) * FBunkaMinuty;
    break;
  case pcps6:
    hour = c / FBunkaMultiply;
    mn = (c % FBunkaMultiply) * FBunkaMinuty;
    break;
  case pcps5:
    hour = c / FBunkaMultiply;
    mn = (c % FBunkaMultiply) * FBunkaMinuty;
    break;
  }
return TDateTime(hour, mn, 0, 0);
}

TPopupMenu* __fastcall TRurPlanCalendar::GetPopupMenu(void)
{
TPopupMenu *tmp=TScrollingWinControl::GetPopupMenu();
TMenuItem *mi,*mm;
if(!tmp)
  {
  tmp=new TPopupMenu(this);
  FVlastneMenu=tmp;
  PopupMenu=tmp;
  }
else
  {
  if(tmp->FindItem(FCommand,fkCommand))
    return tmp;
//  mi=NewLine();
//  tmp->Items->Add(mi);
  }
mm=NewSubMenu("Mierka", 0, "RGridItem1",NULL,-1);
mm->Hint="Mierka zobrazenia";
mm->Tag=-1;
tmp->Items->Add(mm);
FCommand=mm->Command;
// podriadene polozky
mi=NewItem("60 minút", 0, false, true, MMenuExecute, 0, "RGridItem2");
mi->Hint="Jedna bunka má 60 minút";
mi->Tag=-20;
mm->Add(mi);
mi=NewItem("30 minút", 0, false, true, MMenuExecute, 0, "RGridItem2");
mi->Hint="Jedna bunka má 30 minút";
mi->Tag=-21;
mm->Add(mi);
mi=NewItem("15 minút", 0, false, true, MMenuExecute, 0, "RGridItem2");
mi->Hint="Jedna bunka má 15 minút";
mi->Tag=-22;
mm->Add(mi);
mi=NewItem("10 minút", 0, false, true, MMenuExecute, 0, "RGridItem2");
mi->Hint="Jedna bunka má 10 minút";
mi->Tag=-23;
mm->Add(mi);
mi=NewItem("6 minút", 0, false, true, MMenuExecute, 0, "RGridItem2");
mi->Hint="Jedna bunka má 6 minút";
mi->Tag=-24;
mm->Add(mi);
mi=NewItem("5 minút", 0, false, true, MMenuExecute, 0, "RGridItem2");
mi->Hint="Jedna bunka má 5 minút";
mi->Tag=-25;
mm->Add(mi);
return tmp;
}

void __fastcall TRurPlanCalendar::MMenuExecute(TObject *Sender)
{
switch(((TMenuItem*)(Sender))->Tag)
  {
  case -20:Scale=pcps60;break;
  case -21:Scale=pcps30;break;
  case -22:Scale=pcps15;break;
  case -23:Scale=pcps10;break;
  case -24:Scale=pcps6;break;
  case -25:Scale=pcps5;break;
  }
VertScrollBar->Position=PDobaZaciatok*FBunkaHeight;  
}

void __fastcall TRurPlanCalendar::SetDatum(TDate value)
{
if(FDatum != value) {
  FDatum = value;
  CalcOdDo();
  Invalidate();
  }
}

TDate __fastcall TRurPlanCalendar::GetDatum()
{
return FDatum;
}

void TRurPlanCalendar::AddItem(RurCalendarItem &ri)
{
rca.Add(ri);
DEBUG("ADD: "+ri.Debug());
needrecalc=true;
}

void TRurPlanCalendar::DeleteItem(long hash)
{
// empty
needrecalc=true;
}

void TRurPlanCalendar::DeleteItem(RurCalendarItem *ri)
{
for(int i=0;i<rca.GetItemsInContainer();i++)
  if(&rca[i]==ri)
    {
    rca.Destroy(i);
    rca.select=-1;
    needrecalc=true;
    return;
    }
}

void TRurPlanCalendar::ClearItems()
{
rca.Flush();
needrecalc=true;
}

///
/// Pustenie mysky
///
void __fastcall TRurPlanCalendar::MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
{
DEBUG(__FUNC__)
try {
try {
  StartDC(X, Y);
  switch(Button)
    {
    case mbLeft: LMouseUp(X, Y);break;
    case mbRight: RMouseUp(X, Y);break;
    }
  EndDC();
  TScrollingWinControl::MouseUp(Button, Shift, X, Y);
} catch (...) {
  }
} __finally {
  drag = false;
  }
}

///
/// Stlacenie mysky
///
void __fastcall TRurPlanCalendar::MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
{
DEBUG(__FUNC__)
SetFocus();
StartDC(X,Y);
switch(Button)
  {
  case mbLeft:
    LMouseDown(X,Y);
    drag=true;
    dragy=Y;
    if(dragmode>0)
      {
      if(SelBunka!=-1) // predtym mal selektnute v poli a ideme na polozku
        {
        DrawSelection();
        SelBunka=-1;
        }
      if(rca.select!=-1)
        {
        RurCalendarItem *r1=rca.si;
        rca.select=-1;
        rca.si=NULL;
        DrawItem(Canvas,r1);
        }
      rca.select=draggeditem;
      rca.si=&(rca[draggeditem]);
      DrawItem(Canvas,rca.si);
      dragrect=rca[draggeditem].r;
      DrawDraggedItem();
      }
    break;
  }
EndDC();
TScrollingWinControl::MouseDown(Button,Shift,X,Y);
}

///
/// Pohyb mysky
///
void __fastcall TRurPlanCalendar::MouseMove(Classes::TShiftState Shift, int X, int Y)
  {
  if(!drag)
    {
    StartDC(X,Y);
    switch(FTyp)
      {
      case pcplan1:
      case pcplan5:
      case pcplanU:
        int sel = rca.FindXY2(X, Y);
        if(sel!=-1) { // sme nad nejakym terminom
          draggeditem = sel;
          int okraj = rca.FindXYPos(sel, X, Y);
          if(okraj==0) {
            Cursor = crDrag;
            dragmode = 1; // budeme tahat termin
            }
          else {
            Cursor = crSizeNS;
            if(okraj==-1) dragmode = 2; // horny okraj
            else          dragmode = 3; // dolny okraj
            }
          }
        else {
          Cursor = crDefault;
          dragmode = 0; // zase selekcie
          }
        break;
      }
    EndDC();
    return;
    }
  if(!Shift.Contains(ssLeft)) // draguje bez lavej mysky
    {                         // dojazd po dvojkliku
    drag=false;
    return;
    }

  // tahame alebo selektujeme
  if(dragmode==1 /*&& Top<Y */&& Y<10)
    {
    drag = false;
    BeginDrag(true);
    return;
    }
  StartDC(X, Y);
  switch(FTyp)
    {
    case pcplan1:
      if(dragmode>0) { // dragujeme termin
        MouseMoveDragItem(X, Y);
        EndDC();
        return;
        }

      if(SelBunka!=-1) { // selektujeme bunky
        int tmp = Y/20+1;
        RRect r8(40, SelBunka2*20, 40+SelSirka, tmp*20);
        ::InvertRect(Canvas->Handle, &r8);
        SelBunka2 = tmp;
        }
      break;

    case pcplan5:
      if(dragmode>0) { // dragujeme termin
        MouseMoveDragItem(X, Y);
        EndDC();
        return;
      }

      if(SelBunka!=-1) { // selektujeme bunky
        int tmp = Y/20+1;
        RRect r8(days_selected->r.Left, SelBunka2*20, days_selected->r.Left+SelSirka, tmp*20);
        ::InvertRect(Canvas->Handle, &r8);
        SelBunka2 = tmp;
        }
      break;

    case pcplanU:
      if(dragmode>0) { // dragujeme termin
        MouseMoveDragItem(X, Y);
        EndDC();
        return;
      }

      if(SelBunka!=-1 && users->Selected) { // selektujeme bunky
        int tmp = Y/20+1;
        TRurPlanCalendarUser *u = users->SelectedUser;
        RRect r8(u->r.Left, SelBunka2*20, u->r.Left+SelSirka, tmp*20);
        ::InvertRect(Canvas->Handle, &r8);
        SelBunka2 = tmp;
        }
      break;

    }
  EndDC();
  }

void TRurPlanCalendar::MouseMoveDragItem(int X, int Y)
{
DrawDraggedItem();
int dify = Y - dragy;
switch(dragmode) {
  case 1:
    dragrect.Top = rca[draggeditem].r.Top + dify;
    dragrect.Bottom = rca[draggeditem].r.Bottom + dify;
    break;
  case 2:
    dragrect.Top = rca[draggeditem].r.Top + dify;
    break;
  case 3:
    dragrect.Bottom = rca[draggeditem].r.Bottom + dify;
    break;
  }
// zaokruhlenie na bunky
dragrect.Top = dragrect.Top/20*20;
dragrect.Bottom = dragrect.Bottom/20*20;
DrawDraggedItem();
}

void TRurPlanCalendar::StartDC(int &x, int &y)
  {
  //dc_count++;
  //if(dc_count>1)
  //  return;
  //DEBUG(__FUNC__)
  Canvas->Handle = ::GetDC(Handle);
  ::SetWindowOrgEx(Canvas->Handle, 0, VertScrollBar->Position, 0);
  y += VertScrollBar->Position;
  }

void TRurPlanCalendar::EndDC()
  {
  //if(dc_count>0)
  //  dc_count--;
  //if(dc_count)
  //  return;
  //DEBUG(__FUNC__)
  ::ReleaseDC(Handle, Canvas->Handle);
  Canvas->Handle = 0;
  }

///
/// Kreslenie selekcie
///
void TRurPlanCalendar::DrawSelection()
  {
  RRect r8;
  switch(FTyp) {
    case pcplan1:
      r8 = RRect(40, SelBunka*20, 40+SelSirka, SelBunka2*20);
      ::InvertRect(Canvas->Handle, &r8);
      break;
    case pcplan5:
      if(days_selected) {
        r8 = RRect(days_selected->r.Left, SelBunka*20, days_selected->r.Left+SelSirka, SelBunka2*20);
        ::InvertRect(Canvas->Handle, &r8);
      }
      break;
    case pcplanU:
      if(users->Selected) {
        TRurPlanCalendarUser *u = users->SelectedUser;
        r8 = RRect(u->r.Left, SelBunka*20, u->r.Left+SelSirka, SelBunka2*20);
        ::InvertRect(Canvas->Handle, &r8);
      }
      break;
    }
  }

// invertnem selektovany termin
void TRurPlanCalendar::DrawSelTermin()
  {
  if(rca.select==-1) return;
  RurCalendarItem &ri=rca[rca.select];
  RRect r8(ri.r);
  ::InvertRect(Canvas->Handle,&r8);
  }

// nakresli stvorec okolo dragovaneho terminu
void TRurPlanCalendar::DrawDraggedItem()
  {
  RRect r8(dragrect);
  Canvas->Pen->Mode=pmXor;
  Canvas->Pen->Style = psDot;
  Canvas->MoveTo(r8.Left,r8.Top);
  Canvas->LineTo(r8.Right-1,r8.Top);
  Canvas->LineTo(r8.Right-1,r8.Bottom-1);
  Canvas->LineTo(r8.Left,r8.Bottom-1);
  Canvas->LineTo(r8.Left,r8.Top);
  Canvas->Pen->Mode=pmCopy;
  Canvas->Pen->Style = psSolid;
  }

///
/// Stlacenie lavej mysky
/// Rozlisujeme medzi kliknutim na nejaku polozku a kliknutim na nejaky den.
///
void TRurPlanCalendar::LMouseDown(int X, int Y)
{
SelOriginX = X;
SelOriginY = Y;
switch(FTyp)
  {
  case pcplan1:
    if(dragmode>0)
      return;
    if(SelBunka!=-1)
      DrawSelection();
    if(rca.select!=-1) {
      RurCalendarItem *r1 = rca.si;
      rca.select = -1;
      rca.si = NULL;
      DrawItem(Canvas, r1);
      }
    rca.select = -1;
    rca.si = NULL;
    SelBunka = Y/20; SelBunka2 = SelBunka+1;
    DrawSelection();
    break;
  case pcplan5:
    if(rca.select!=-1) {
      RurCalendarItem *r1 = rca.si;
      rca.select = -1;
      rca.si = NULL;
      DrawItem(Canvas, r1);
      }
    if(FindDayXY(X, Y)) { // klikol niekde do stlpca, ktory predstavuje den
      if(FDatum != days_selected->d) {
        FDatum = days_selected->d;
        }
      Paint();
      SelBunka = Y/20; SelBunka2 = SelBunka+1;
      DrawSelection();
      }
    break;
  case pcplan7:
  case pcplan31:
    if(FindDayXY(X, Y)) // klikol niekde do stlpca, ktory predstavuje den
      {
      FDatum = days_selected->d;
      if(FindDayItemXY(X, Y))
        {
        rca.si = days_selecteditem->ri;
        }
      Paint();
      }
    break;
  case pcplanU: {
    if(rca.select!=-1)
      {
      RurCalendarItem *r1 = rca.si;
      rca.select = -1;
      rca.si = NULL;
      DrawItem(Canvas, r1);
      }
    int uid = FindUserXY(X, Y);
    users->Selected = uid;
    Paint();
    if(uid && FOnUserSelect)
      FOnUserSelect(this, uid);
    if(uid) {
      SelBunka = Y/20; SelBunka2 = SelBunka+1;
      DrawSelection();
      }
    }
    break;
  }
if(rca.si && FOnSelTerm2)
  FOnSelTerm2(this, rca.si);
}

///
/// Vyhlada den na danej pozicii.
///
bool TRurPlanCalendar::FindDayXY(int X,int Y)
{
for(int i=0; i<FTypDays; i++)
  {
  if(PtInRect(&(days[i].r), TPoint(X,Y)))
    {
    days_position = i;
    days_selected = days + i;
    return true;
    }
  }
return false;
}

///
/// Vyhlada polozku v dany den na danej pozicii.
///
bool TRurPlanCalendar::FindDayItemXY(int X,int Y)
{
if(days_selected==NULL) return false;
for(v_items::iterator i=days_selected->items.begin();i!=days_selected->items.end();i++)
  {
  if(PtInRect(&((*i).r),TPoint(X,Y)))
    {
    days_selecteditem=&(*i);
    return true;
    }
  }
days_selecteditem=NULL;
return false;
}

///
/// Najde uzivatela po kliku do kalendara
///
int TRurPlanCalendar::FindUserXY(int X, int Y)
{
for(v_users::iterator i=users->begin(); i!=users->end(); i++) {
  if(PtInRect(i->r, TPoint(X, Y)))
    return i->id;
  }
return 0;
}

///
/// Pustenie lavej mysky mysky
///
void TRurPlanCalendar::LMouseUp(int X, int Y)
{
if(!drag) return;
int sel;
switch(FTyp)
  {
  case pcplan1:
    if(dragmode>0) {
      LMouseUpEvents(FDatum_od, FDatum_do);
      return;
      }
    SelBunka2 = Y/20 + 1;
    if(SelBunka>SelBunka2) // normalizujem selekciu
      std::swap(SelBunka, SelBunka2);
    if(SelBunka+1==SelBunka2) // sanca ze selektnem termin
      {
      int sel = rca.FindXY(X,Y);
      if(sel!=-1)
        {
        ShowHint = true;
        Hint = rca[rca.select].text;
        if(SelBunka!=-1) // odmazem staru selekciu
          {DrawSelection();SelBunka=-1;}
        DrawItem(Canvas, rca.si);
        if(FOnSelTerm) // poviem, ze selektol termin
          FOnSelTerm(this);
        if(rca.si && FOnSelTerm2)
          FOnSelTerm2(this, rca.si);
        }
      }
    break;
  case pcplan5:
    if(dragmode>0) {
      LMouseUpEvents(days_selected->d, days_selected->d);
      return;
      }
  case pcplanU:
    if(dragmode>0) {
      LMouseUpEvents(FDatum, FDatum);
      return;
      }
    sel=rca.FindXY(X,Y);
    if(sel!=-1)
      {
      ShowHint=true;
      Hint=rca[rca.select].text;
      if(SelBunka!=-1) // odmazem staru selekciu
        {DrawSelection();SelBunka=-1;}
      DrawItem(Canvas,rca.si);
      if(rca.si && FOnSelTerm2)
        FOnSelTerm2(this,rca.si);
      if(FOnSelTerm) // poviem, ze selektol termin
        FOnSelTerm(this);
      }
    break;
  case pcplan7:
  case pcplan31:
    break;
  }
}

///
/// Vyvola eventy pri pusteni lavej mysky
///
void TRurPlanCalendar::LMouseUpEvents(TDateTime dtb, TDateTime dte)
{
DrawDraggedItem();
rca.select = draggeditem;
int a = dragrect.Top / 20;
int b = dragrect.Bottom / 20;
if(a<0) {b += -a; a = 0;}
if(b>FBunkaCount) {a -= b-FBunkaCount; b = FBunkaCount;}
TDateTime t1 = dtb + ColToTime(a);
TDateTime t2 = dte + ColToTime(b);
if(rca.si && FOnSelTerm2)
  FOnSelTerm2(this, rca.si);
if(FOnMoveTerm) { // poviem, ze selektol termin
  RurCalendarItem *ri = &rca[draggeditem];
  if(!(ri->from==t1 && ri->to==t2))
    FOnMoveTerm(this, ri, t1, t2);
  }
}

// vrati id selektovaneho prvku
// pozor na prvky miesane z viac tabuliek, moze sa zhodovat
long TRurPlanCalendar::GetSelHash()
  {
  if(rca.select!=-1)
    return rca[rca.select].hash;
  return -1;
  }

///
/// Vrati pointer na selektovany prvok
///
RurCalendarItem* TRurPlanCalendar::GetSelItem()
{
switch(FTyp)
  {
  case pcplan1:
  case pcplan5:
  case pcplanU:
    if(rca.select!=-1)
      return rca.si;
    break;
  case pcplan7:
  case pcplan31:
    if(days_selecteditem)
      return days_selecteditem->ri;
    break;
  }  
return NULL;
}

void TRurPlanCalendar::RMouseUp(int X,int Y)
  {
  if(FOnRightMouse)
    FOnRightMouse(this);
  }

///
/// Vrati rozsah selekcie v datumoch
///
bool TRurPlanCalendar::GetSel(TDateTime &from, TDateTime &to)
{
switch(FTyp)
  {
  case pcplan1:
    if(SelBunka==-1) {
      from=FDatum_od+TDateTime(8,0,0,0);
      to=FDatum_do+TDateTime(8,30,0,0);
      return false;
      }
    else {
      from=FDatum_od+TDateTime(SelBunka/FBunkaMultiply,SelBunka%FBunkaMultiply*FBunkaMinuty,0,0);
      to=FDatum_do+TDateTime(SelBunka2/FBunkaMultiply,SelBunka2%FBunkaMultiply*FBunkaMinuty,0,0);
      return true;
      }
  case pcplan5:
    if(SelBunka==-1) {
      from = FDatum + TDateTime(8,0,0,0);
      to = FDatum + TDateTime(8,30,0,0);
      return false;
      }
    else {
      from = FDatum + ColToTime(SelBunka);
      to = FDatum + ColToTime(SelBunka2);
      return true;
      }
  case pcplan7:
    from=FDatum+TDateTime(8,0,0,0);
    to=FDatum+TDateTime(8,30,0,0);
    return false;
  case pcplan31:
    from=FDatum+TDateTime(8,0,0,0);
    to=FDatum+TDateTime(8,30,0,0);
    return false;
  }
return false;  
}

///
/// Vrati pracovny cas
///
void TRurPlanCalendar::GetWorkTime(TDateTime &from, TDateTime &to)
{
from=FDatum_od+FPDobaTZaciatok;
to=FDatum_do+FPDobaTKoniec;
}

///
/// Nastavi pracovny cas
///
void TRurPlanCalendar::SetWorkTime(TDateTime from, TDateTime to)
{
FPDobaTZaciatok=TimeOf(from);
FPDobaTKoniec=TimeOf(to);
UpdateRange();
}

///
/// Zmena velkosti
///
void __fastcall TRurPlanCalendar::Resize(void)
{
needrecalc=true;
Invalidate();
}

void __fastcall TRurPlanCalendar::DblClick(void)
{
DEBUG(__FUNC__)
drag=false;
if(rca.si && FOnEditTerm) // poviem, ze selektol termin
  FOnEditTerm(this,rca.si);
else if(FOnDblClick)
  {
  FOnDblClick(this);
  }
}


void __fastcall TRurPlanCalendar::SetPDobaZaciatok(int value)
{
if(FPDobaZaciatok != value) {
  FPDobaZaciatok = value;
  Invalidate();
  }
}

int __fastcall TRurPlanCalendar::GetPDobaZaciatok()
{
return FPDobaZaciatok;
}

void __fastcall TRurPlanCalendar::SetPDobaKoniec(int value)
{
if(FPDobaKoniec != value) {
  FPDobaKoniec = value;
  Invalidate();
  }
}

int __fastcall TRurPlanCalendar::GetPDobaKoniec()
{
return FPDobaKoniec;
}

// meniny su v extra subore
void __fastcall TRurPlanCalendar::SetMeniny(AnsiString value)
{
men.count=0;
FMeniny = value;
TStringList *sl=new TStringList;
TStringList *sl2=new TStringList;
try {
  sl->LoadFromFile(value);
  for(int i=0;i<sl->Count;i++)
    {
    AnsiString tmp=sl->operator [](i);
    int d,m;
    char meno[255];
    sscanf(tmp.c_str(),"%d.%d.",&d,&m);
    sl2->Delimiter=',';
    sl2->DelimitedText=tmp.SubString(tmp.Pos(";")+1,255);
    for(int j=0;j<sl2->Count;j++)
      {
      men.meniny[men.count].den=d;
      men.meniny[men.count].mesiac=m;
      men.meniny[men.count].meno=sl2->Strings[j];
      men.count++;
      }
    }
} catch(...) {}
delete sl;
delete sl2;
}

void TRurPlanCalendar::HideDate()
{
hidedate=true;
}

void TRurPlanCalendar::HideMeniny()
{
hidemeniny=true;
}

AnsiString TRurPlanCalendar::GetDateMeniny()
{
return FDatum.DateString()+" "+men.Find(FDatum);
}

AnsiString TRurPlanCalendar::GetMenoMeniny(AnsiString _meno)
{
return men.FindMeno(_meno);
}

///
/// Pripojenie hlavicky
///
void TRurPlanCalendar::SetHeader(TRurPlanCalendarHeader *pb)
{
pbh=pb;
pbh->master=this;
pbh->Invalidate();
pbh->Visible=false;
}

///
/// Reakcia na ziadost o hint
///
MESSAGE void __fastcall TRurPlanCalendar::CMHintShow(Messages::TMessage &Message)
{
PHintInfo ph=(PHintInfo)Message.LParam;
RurCalendarItem *i=GetSelItem();
if(i)
  ph->HintStr=i->from.DateTimeString()+" - "+i->to.DateTimeString()+"\r\n"+i->text;
else
  ph->HintStr="";
ph->ReshowTimeout=2000;
Message.Result=0;
}

bool __fastcall TRurPlanCalendar::DoMouseWheelDown(Classes::TShiftState Shift, const TPoint &MousePos)
{
VertScrollBar->Position+=20;
return true;
}

bool __fastcall TRurPlanCalendar::DoMouseWheelUp(Classes::TShiftState Shift, const TPoint &MousePos)
{
VertScrollBar->Position-=20;
return true;
}

/////////////////////////////////////////////////////
// Hlavicka ku kalendaru
/////////////////////////////////////////////////////

///
/// Konstruktor
///
__fastcall TRurPlanCalendarHeader::TRurPlanCalendarHeader(TComponent* Owner)
        : TScrollingWinControl(Owner)
{
sel=NULL;
master=NULL;
FOnDblClick=NULL;
FOnEditTerm=NULL;
FOnSelTerm=NULL;
FCanvas=new TCanvas();
bpruzokh=new Graphics::TBitmap();
AnsiString tmp=ExtractFilePath(ParamStr(0))+"img\\pruzokh.bmp";
if(FileExists(tmp))
  bpruzokh->LoadFromFile(tmp);
VertScrollBar->Range=20*20;
}

///
/// Destruktor
///
__fastcall TRurPlanCalendarHeader::~TRurPlanCalendarHeader()
{
delete bpruzokh;
delete FCanvas;
}

///
/// Nastavenie XP okrajov
///
void __fastcall TRurPlanCalendarHeader::CreateParams(TCreateParams &Params)
{
TScrollingWinControl::CreateParams(Params);
//Params.Style   |= WS_BORDER;
Params.ExStyle |= WS_EX_CLIENTEDGE;
}

///
/// Reakcia na spravu Windows na vykreslenie
///
MESSAGE void __fastcall TRurPlanCalendarHeader::WMPaint(Messages::TWMPaint &Message)
  {
  HDC dc;
  TPaintStruct ps;
  dc=Message.DC;
  if(dc==0)
    {
    dc=::BeginPaint(Handle,&ps);
    Canvas->Handle=dc;

    ::SetWindowOrgEx(dc,0,VertScrollBar->Position,0);
    Paint();

    Canvas->Handle=0;
    ::EndPaint(Handle,&ps);
    }
  }

///
/// Vykreslenie objektu
///
void __fastcall TRurPlanCalendarHeader::Paint()
{
if(master==NULL) return;
int WW;
switch(master->FTyp)
  {
  case pcplan1:
    WW=Width-40-16-1;//SelSirka=WW;
//    WW=ClientWidth-40;
    DrawGrid(40,WW,master->Datum);
    DrawPruzok();
    break;
  case pcplan5:
    WW=(Width-8-40-16)/5-1;//SelSirka=WW;
    DrawGrid(40,WW,master->FDatum_od);
    DrawGrid(40+WW+2,WW,master->FDatum_od+1);
    DrawGrid(40+WW*2+4,WW,master->FDatum_od+2);
    DrawGrid(40+WW*3+6,WW,master->FDatum_od+3);
    DrawGrid(40+WW*4+8,WW,master->FDatum_od+4);
    DrawPruzok();
    break;
  case pcplan7:
    break;
  case pcplan31:
    break;
  case pcplanU:
    DrawPruzok();
    for(v_users::iterator i=master->users->begin();i!=master->users->end();i++)
      {
      DrawGrid(i->r.Left,i->r.Right-i->r.Left,master->FDatum_od);
      }
    break;
  }
// vykreslenie prvkov v jednom cykle
for(v_items::iterator i=items.begin();i!=items.end();i++)
  {
  DrawItem(Canvas,&(*i));
  }
}

///
/// Vykreslenie zvisleho pruhu s cislami
///
void TRurPlanCalendarHeader::DrawPruzok()
{
// vykreslenie pruzku s cislami
if(bpruzokh->HandleAllocated())
  {
  int pos=0;
  while(pos<VertScrollBar->Range)
    {
    Canvas->Draw(0,pos,bpruzokh);
    pos+=bpruzokh->Height;
    }
  }
}

///
/// Vykreslenie podmazu
///
void TRurPlanCalendarHeader::DrawGrid(int x,int ww,TDateTime d)
{
RRect r8(x,0,x+ww,VertScrollBar->Range);
Canvas->Brush->Color=master->ColorDen;
Canvas->FillRect(r8);
Canvas->Pen->Color=TColor(0xA2DBF6);
int lines=VertScrollBar->Range/20+2;
for(int i=0;i<lines;i+=2)
  {
  Canvas->MoveTo(x,i*20);Canvas->LineTo(x+ww,i*20);
  }
Canvas->Pen->Color=TColor(clBtnFace);
for(int i=1;i<lines;i+=2)
  {
  Canvas->MoveTo(x,i*20);Canvas->LineTo(x+ww,i*20);
  }
}

///
/// Vykreslenie konkretneho prvku
///
void TRurPlanCalendarHeader::DrawItem(TCanvas *c,TRurPlanCalendarHeaderItem *i)
{
RurCalendarItem *ri=i->ri;
RRect rr(i->r);
RRect rrcolor=rr;rrcolor.right=rrcolor.left+4;
RRect rrwhite=rr;rrwhite.Left+=4;
c->Brush->Color=clWhite;
c->FillRect(rrwhite);
c->Brush->Color=ri->color;
c->FillRect(rrcolor);
InflateRect(&rrwhite,-2,-2);
c->Brush->Color=clWhite;
if(ri->splnene)
  c->Font->Style=TFontStyles()<<fsStrikeOut;
else
  c->Font->Style=TFontStyles();
::DrawTextA(c->Handle,ri->text.c_str(),-1,&rrwhite,DT_LEFT|DT_WORDBREAK);
if(i==sel)
  c->Brush->Color = clRed;
else
  c->Brush->Color = clBlack;
c->FrameRect(rr);
}

///
/// Funkcia vypocita svoju vysku a rozmiestni si prvky podla udajov z mastra.
///
void TRurPlanCalendarHeader::Prepocitaj()
{
int poc=0;
if(master->FTyp==pcplan1)
  {
  int WW=Width-40-16-1;
  plochy[0]=RRect(40,0,40+WW,2000);
  poc=master->v_date[0].size();
  items.clear();
  std::vector<RurCalendarItem*> &v=master->v_date[0];
  int ii=0;
  for(std::vector<RurCalendarItem*>::iterator i=v.begin();i!=v.end();i++)
    {
    TRurPlanCalendarHeaderItem phi;
    RurCalendarItem *r1=(*i);
    phi.ri=r1;
    phi.r=RRect(45,ii*20,44+WW-8,ii*20+21);
    items.push_back(phi);
    ii++;
    }
  }
if(master->FTyp==pcplan5)
  {
  int WW=(Width-8-40-16)/5-1;
  items.clear();
  for(int k=0;k<5;k++)
    {
    plochy[k]=RRect(40+WW*k+2*k,0,40+WW*k+2*k+WW,2000);
    std::vector<RurCalendarItem*> &v=master->v_date[k];
    if(v.size()>poc) poc=v.size();
    int ii=0;
    for(std::vector<RurCalendarItem*>::iterator i=v.begin();i!=v.end();i++)
      {
      TRurPlanCalendarHeaderItem phi;
      RurCalendarItem *r1=(*i);
      phi.ri=r1;
      phi.r=RRect(plochy[k].left+5,ii*20,plochy[k].right-2,ii*20+21);
      items.push_back(phi);
      ii++;
      }
    }
  }
if(master->FTyp==pcplanU)
  {
  items.clear();
  for(v_users::iterator j=master->users->begin();j!=master->users->end();j++)
    {
    std::vector<RurCalendarItem*> &v=j->days;
    if(v.size()>poc) poc=v.size();
    int ii=0;
    for(std::vector<RurCalendarItem*>::iterator i=v.begin();i!=v.end();i++)
      {
      TRurPlanCalendarHeaderItem phi;
      RurCalendarItem *r1=(*i);
      phi.ri=r1;
      phi.r=RRect(j->r.left+5,ii*20,j->r.right-2,ii*20+21);
      items.push_back(phi);
      ii++;
      }
    }
  }
VertScrollBar->Range=20*poc+1;
Height=20*poc+5;
Invalidate();
}

void __fastcall TRurPlanCalendarHeader::Resize(void)
{
Invalidate();
}

void __fastcall TRurPlanCalendarHeader::MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
{
StartDC(X,Y);
// pri up skontrolujem, ci sa da nejaka bunka selektnut
if(Button==mbLeft)
for(v_items::iterator i=items.begin();i!=items.end();i++)
  {
  if(PtInRect(&i->r,TPoint(X,Y))) // klik zasiahol
    {
    sel=&(*i);
    DrawItem(Canvas,sel);
    if(sel && FOnSelTerm)
      FOnSelTerm(this,sel->ri);
    }
  }
koniec:
EndDC();
TScrollingWinControl::MouseUp(Button,Shift,X,Y);
}

void __fastcall TRurPlanCalendarHeader::MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
{
StartDC(X,Y);
if(Button==mbLeft && sel) // selekciu musim vymazat
  {
  TRurPlanCalendarHeaderItem *s=sel;
  sel=NULL;
  DrawItem(Canvas,s);
  }
EndDC();
TScrollingWinControl::MouseDown(Button,Shift,X,Y);
}

void __fastcall TRurPlanCalendarHeader::MouseMove(Classes::TShiftState Shift, int X, int Y)
{
StartDC(X,Y);
EndDC();
//TScrollingWinControl::MouseMove(Button,Shift,X,Y);
}

void __fastcall TRurPlanCalendarHeader::DblClick(void)
{
if(sel && FOnEditTerm)
  FOnEditTerm(this,sel->ri);
else if(FOnDblClick)
  FOnDblClick(this);
}

void TRurPlanCalendarHeader::StartDC(int &x,int &y)
{
Canvas->Handle=::GetDC(Handle);
::SetWindowOrgEx(Canvas->Handle,0,VertScrollBar->Position,0);
y+=VertScrollBar->Position;
}

void TRurPlanCalendarHeader::EndDC()
{
::ReleaseDC(Handle,Canvas->Handle);
Canvas->Handle=0;
}

///
/// Reakcia na ziadost o hint
///
MESSAGE void __fastcall TRurPlanCalendarHeader::CMHintShow(Messages::TMessage &Message)
{
PHintInfo ph=(PHintInfo)Message.LParam;
if(sel)
  {
  RurCalendarItem *i=sel->ri;
  ph->HintStr=i->from.DateTimeString()+" - "+i->to.DateTimeString()+"\r\n"+i->text;
  }
else
  ph->HintStr="";
ph->ReshowTimeout=2000;
Message.Result=0;
}

RurCalendarItem* TRurPlanCalendarHeader::GetSelItem()
{
if(sel) return sel->ri;
return NULL;
}

