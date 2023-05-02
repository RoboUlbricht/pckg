/*
 * Copyright (c) 2004-2006 Ing. Robert Ulbricht
 *
 * Projekt: Optivus
 * Obsah: TRurCalendar
 * Kalendar s mesiacmi
 */

#include <vcl.h>
#pragma hdrstop

#include "RurCalendar.h"
#pragma package(smart_init)
#include <dateutils.hpp>

#define SX 135
#define SY 121
#define COLOR_CAPTION TColor(RGB(230, 230, 230))
#define COLOR_ODDO TColor(RGB(169, 186, 243))
//#define COLOR_INACTIVE TColor(0x00BCF4FF)
#define COLOR_INACTIVE TColor(RGB(230, 235, 255))
//#define COLOR_INACTIVE clYellow

//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TRurCalendar *)
{
        new TRurCalendar(NULL);
}

int RurCalendarArray::Find(int r,int m,int d)
  {
  if(d==-1) return -1;
  try
    {
    TDateTime tt(r,m,d);
    for(int i=0;i<GetItemsInContainer();i++)
      {
      if((*this)[i].from<=tt && tt<=(*this)[i].to)
        return i;
      }
    }
  catch(...)
    {}
  return -1;
  }

// hodina je *2 aby preniesla aj polhodiny
int RurCalendarArray::Find(int r,int m,int d,int hodina)
  {
  if(d==-1) return -1;
  try
    {
    TDateTime tt(r,m,d);
    TDateTime _time(hodina/2,hodina%2*30,0,0);
    for(int i=0;i<GetItemsInContainer();i++)
      {
      if((*this)[i].from<=tt && tt<=(*this)[i].to)
        {
        if(TimeOf((*this)[i].from)<=_time && _time<TimeOf((*this)[i].to))
          {
          select=i;
          return i;
          }
        }
      }
    }
  catch(...)
    {}
  return -1;
  }

int RurCalendarArray::FindXY(int x,int y)
{
select=-1;
si=NULL;
for(int i=0;i<GetItemsInContainer();i++)
  {
  RurCalendarItem &ri=(*this)[i];
  if(ri.r.left<=x && x<=ri.r.right &&
     ri.r.top<=y && y<=ri.r.bottom)
    {
    si=&ri;
    select=i;return i;
    }
  }
return select;
}

int RurCalendarArray::FindXY2(int x,int y)
{
for(int i=0;i<GetItemsInContainer();i++)
  {
  RurCalendarItem &ri=(*this)[i];
  if(ri.r.left<=x && x<=ri.r.right &&
     ri.r.top<=y && y<=ri.r.bottom)
    {
    return i;
    }
  }
return -1;
}


// vracia zasiahnutie okrajov
// -1 horny
// 1 dolny
// 0 niekde inde
int RurCalendarArray::FindXYPos(int id,int x,int y)
{
RurCalendarItem &ri=(*this)[id];
if(ri.r.top<=y && y<=ri.r.top+5)
  return -1;
if(ri.r.bottom-5<=y && y<=ri.r.bottom)
  return 1;
return 0;
}


///
/// Konstruktor
///
__fastcall TRurCalendar::TRurCalendar(TComponent* Owner)
        : TGraphicControl(Owner)
{
month=NULL;
is_init=false;
Width = 145;
Height = 127;
CalcRozmery();
FRok=2000;
FMesiac=1;
FCiary=false;
FVLinka=false;
FMove=true;
FColorInactive=false;
Datum=Date();
CalcMinMax();
bmp=new Graphics::TBitmap();
is_init=true;
}

///
/// Destruktor
///
__fastcall TRurCalendar::~TRurCalendar()
{
delete bmp;
if(month) delete[] month;
}

struct RRect : public TRect
  {
  RRect(int a,int b,int c,int d) {left=a;top=b;right=c;bottom=d;}
  void Okraj(int a,int b) {left-=a;right+=a;top-=b;bottom+=b;}
  };

///
/// Rozmery kalendara v mesiacoch
///
void TRurCalendar::CalcRozmery()
{
FStlpcov=Width/SX;  // 147
FRiadkov=Height/SY; // 127
if(FStlpcov==0) FStlpcov=1;
if(FRiadkov==0) FRiadkov=1;
if(month) delete[] month;
month=new TRurCalendarMonth[FStlpcov*FRiadkov];
}

///
/// Spocita selekciu v kalendari od do
///
void TRurCalendar::CalcOdDo()
  {
  int dd=FDatum.DayOfWeek();
  int dow=((FDatum.DayOfWeek() - 1 + 7) % 7)-1;
  if(dd==1) dow=6;
  switch(FTyp)
    {
    case cplanAll:
    case cplan1:FDatum_od=FDatum;FDatum_do=FDatum;break;
    case cplan5:FDatum_od=FDatum-dow;FDatum_do=FDatum_od+4;break;
    case cplan7:
      FDatum_od=FDatum-dow;FDatum_do=FDatum_od+6;
      break;
    case cplan31:
      unsigned short rr,mm,dd;
      FDatum.DecodeDate(&rr,&mm,&dd);
      FDatum_od=TDateTime(rr,mm,1);
      FDatum_do=TDateTime(rr,mm,DaysPerMonth(rr, mm));
      break;
    }
  }

///
/// Spocita minimalny a maximalny zobrazeny mesiac v kalendari
///
void TRurCalendar::CalcMinMax()
  {
  FDatum_min=TDateTime(FRok,FMesiac,1);
  int rr=FRok;
  int mm=FMesiac+FStlpcov*FRiadkov-1;
  while(mm>12)
    {rr++;mm-=12;}
  FDatum_max=TDateTime(rr,mm,DaysPerMonth(rr,mm));
  }

///
/// Kreslenie ciary
///
void RCLine(TCanvas *CC, int x1, int y1, int x2, int y2)
{
CC->MoveTo(x1,y1);
CC->LineTo(x2,y2);
}

///
/// Kreslenie obdlznika
///
void RCRectangle(TCanvas *CC, TRect r2)
{
CC->MoveTo(r2.left,r2.top);
CC->LineTo(r2.right,r2.top);
CC->LineTo(r2.right,r2.bottom);
CC->LineTo(r2.left,r2.bottom);
CC->LineTo(r2.left,r2.top);
}

///
/// Predhryzenie udajov mesiaca
///
void TRurCalendar::PrepareMonth(TRurCalendarMonth &mesiac, int R, int M)
{
if(mesiac.prepared) return;

int n;
mesiac.dat_s=TDateTime(R,M+1,1);
mesiac.dat_k=EndOfTheMonth(mesiac.dat_s);
bool inactive = true;
if(mesiac.dat_s<=FDatum && FDatum<=mesiac.dat_k)
  inactive = false;

for(int i=0;i<49;i++)
  {
  mesiac.day[i].Clear(); // cierne pismo, biely podklad
  if(FColorInactive && inactive)
    mesiac.day[i].cbackground = COLOR_INACTIVE;
  }
// vikend je sedy
for(int i=0;i<7;i++)
  {
  mesiac.day[i*7+5].cpismo=clGray;
  mesiac.day[i*7+6].cpismo=clGray;
  }

for(int k=0;k<7;k++) // os x
  for(int l=0;l<7;l++) // os y
    {
    TRurCalendarDay &cd=mesiac.day[k+l*7];
    cd.nazov=GetCellText(k,l,n);
    if(n!=-1) // je to den z kalendara
      {
      cd.style|=TRCD_DATE;
      TDateTime dd(R,M+1,n);
      if(dd==Date())
        cd.style|=TRCD_REDFRAME;
      if(dd==FDatum)
        cd.style|=TRCD_BLACKFRAME;
      if(FDatum_od<=dd && dd<=FDatum_do)
        cd.cbackground = COLOR_ODDO;
      int nn=rca.Find(R,M+1,n);
      if(nn!=-1)
        {
        cd.cstyle=TFontStyles()<<fsBold;
        RurCalendarItem &ri=rca[nn];
        cd.cbackground=ri.color;
        if(cd.popis.Length()) cd.popis+="\r\n";
        cd.popis+=ri.text;
        }
      nn=sviatky.Find(R,M+1,n);
      if(nn!=-1)
        {
        RurCalendarItem &ri=sviatky[nn];
        cd.cpismo=clRed;
        if(cd.popis.Length()) cd.popis+="\r\n";
        cd.popis+=ri.text;
        }
      }
    }
mesiac.prepared=true;
}

///
/// Nastavi repaint predpocitaneho
///
void TRurCalendar::Dirty()
{
for(int i=0;i<FStlpcov*FRiadkov;i++)
  month[i].prepared=false;
Invalidate();
}

///
/// Vykreslenie kalendara
///
void __fastcall TRurCalendar::Paint()
{
bmp->Width=Width;
bmp->Height=Height;
TCanvas *CC=bmp->Canvas;
RRect rr(0,0,Width,Height);
// biele pozadie a vnorenie do plochy
CC->Brush->Color=clBtnFace;
CC->FillRect(rr);
int sx=145;
int sy=130;
int bx=(sx-8)/7;
int by=(sy-9)/8;
rr.right=(1+bx*7)*FStlpcov+2;
rr.bottom=(1+by*8)*FRiadkov+2;
int ofx=(Width-rr.right)/2;
int ofy=(Height-rr.bottom)/2;
OffsetRect(&rr,ofx,ofy);
rect_all=rr;
if(FColorInactive)
  CC->Brush->Color = COLOR_INACTIVE;
else
  CC->Brush->Color=TColor(0x00ffffff);
CC->FillRect(rr);
Frame3D(CC,rr,clBtnShadow,clBtnHighlight,1);
int M=FMesiac-1;_M=FMesiac;
int R=_R=FRok;

for(int i=0;i<FRiadkov;i++)
  {
  int Y=rr.top+i*(1+by*8);
  for(int j=0;j<FStlpcov;j++)
    {
    TRurCalendarMonth &rcm=month[i*FStlpcov+j];
    int FirstDate = EncodeDate(R, M+1, 1);

    FMonthOffset = 2 - ((DayOfWeek(FirstDate) - 1 + 7) % 7);
    if (FMonthOffset == 2)
      FMonthOffset = -5;
    PrepareMonth(rcm,R,M);

    int X=rr.left+j*(1+bx*7);

    // horna plocha pre nazov mesiaca
    bool inactive = true;
    if(rcm.dat_s<=FDatum && FDatum<=rcm.dat_k)
      inactive = false;
    CC->Brush->Color = COLOR_CAPTION;
    RRect r1(X,Y,X+bx*7+1,Y+by);
    CC->FillRect(r1);
    //Frame3D(CC, r1, clBtnHighlight, clBtnShadow, 1);
    String nazov_mesiaca;
    nazov_mesiaca.printf(L"%s %d",System::Sysutils::FormatSettings.LongMonthNames[M],R);
    CC->Font->Color = TColor(clBlack);
    CC->Font->Style = inactive ? TFontStyles() : TFontStyles()<<fsBold;
    ::DrawText(CC->Handle,nazov_mesiaca.c_str(),-1,&r1,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    if(inactive==false && FColorInactive)
      {
      CC->Brush->Color=TColor(0x00ffffff);
      RRect r1(X,Y+by,X+bx*7+1,Y+by*8);
      CC->FillRect(r1);
      }

    // ciary medzi dnami
    CC->Pen->Color=TColor(clBtnFace);
    if(FCiary)
      {
      // zvisle ciary
      for(int k=0;k<8;k++)
        RCLine(CC,X+k*bx,Y+by,X+k*bx,Y+by*8);
      // vodorovne
      for(int k=1;k<9;k++)
        RCLine(CC,X,Y+k*by,X+bx*7+1,Y+k*by);
      }
    if(FVLinka)
      RCLine(CC,X+1,Y+2*by,X+bx*7+1,Y+2*by);

    // nazvy dni
    ::SetBkMode(CC->Handle, TRANSPARENT);
    for(int k=0;k<7;k++) // os x
      {
      CC->Font->Style=TFontStyles();
      for(int l=0;l<7;l++) // os y
        {
        TRurCalendarDay &cd=rcm.day[k+l*7];
        CC->Font->Color=cd.cpismo;
        CC->Brush->Color=cd.cbackground;

        int n;
        RRect r2(X+k*bx,Y+(l+1)*by,X+k*bx+bx,Y+(l+2)*by);
        if(cd.style&TRCD_DATE)
          {
          RRect r3=r2;r3.left++;r3.top++;
          CC->FillRect(r3);
          CC->Font->Style=cd.cstyle;
          }

        ::DrawTextA(CC->Handle,cd.nazov.c_str(),-1,&r2,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        if(cd.style&TRCD_BLACKFRAME)
          {
          CC->Pen->Color=TColor(clBlack);
          RCRectangle(CC,r2);
          }
        if(cd.style&TRCD_REDFRAME)
          {
          CC->Pen->Color=TColor(clRed);
          RCRectangle(CC,r2);
          }
        }
      }
    M++;
    if(M>11) {M=0;R++;}
    _R=R;_M=M+1;
    }
  }

if(FMove)
  {
  rect_l=Rect(3,3,16,14);
  rect_r=Rect(120,3,133,14);
  OffsetRect(&rect_l,ofx,ofy);
  OffsetRect(&rect_r,ofx,ofy);
  ::DrawFrameControl(CC->Handle,&rect_l,DFC_SCROLL,DFCS_SCROLLLEFT|DFCS_FLAT);
  ::DrawFrameControl(CC->Handle,&rect_r,DFC_SCROLL,DFCS_SCROLLRIGHT|DFCS_FLAT);
  }
::BitBlt(Canvas->Handle, 0, 0, Width, Height, CC->Handle, 0, 0, SRCCOPY);
}
//---------------------------------------------------------------------------
namespace Rurcalendar
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TRurCalendar)};
                 RegisterComponents("Rur", classes, 0);
        }
}
//---------------------------------------------------------------------------


void __fastcall TRurCalendar::SetRiadkov(int value)
{
if(FRiadkov != value) {
  FRiadkov = value;
  if(month) delete[] month;
  month=new TRurCalendarMonth[FStlpcov*FRiadkov];
  Invalidate();
  }
}
int __fastcall TRurCalendar::GetRiadkov()
{
return FRiadkov;
}

void __fastcall TRurCalendar::SetStlpcov(int value)
{
if(FStlpcov != value) {
  FStlpcov = value;
  if(month) delete[] month;
  month=new TRurCalendarMonth[FStlpcov*FRiadkov];
  Invalidate();
  }
}

int __fastcall TRurCalendar::GetStlpcov()
{
return FStlpcov;
}

void __fastcall TRurCalendar::SetDatum(TDate value)
{
if(FDatum != value) {
  FDatum = value;
  CalcOdDo();
  if(!(GetMin()<=value && value<=GetMax()))
    {
    unsigned short rr,mm,dd;
    FDatum.DecodeDate(&rr,&mm,&dd);
    FRok=rr;
    FMesiac=mm;
    CalcMinMax();
    }
  Dirty();
  }
}

TDate __fastcall TRurCalendar::GetDatum()
{
return FDatum;
}

void __fastcall TRurCalendar::SetRok(int value)
{
if(FRok != value) {
  FRok = value;
  CalcMinMax();
  Dirty();
  }
}

int __fastcall TRurCalendar::GetRok()
{
return FRok;
}

void __fastcall TRurCalendar::SetMesiac(int value)
{
if(FMesiac != value) {
  FMesiac = value;
  CalcMinMax();
  Dirty();
  }
}

int __fastcall TRurCalendar::GetMesiac()
{
return FMesiac;
}

bool TRurCalendar::IsLeapYear(int AYear)
{
  return  ((AYear %   4) == 0) &&
         (((AYear % 100) != 0) ||
          ((AYear % 400) == 0));
};


int TRurCalendar::DaysPerMonth(int AYear, int AMonth)
{
  int result;
  const int DaysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  result = DaysInMonth[AMonth-1];
  if ((AMonth == 2) && IsLeapYear(AYear))
    ++result; // leap-year Feb is special

  return result;
};

int TRurCalendar::DaysThisMonth()
{
  return DaysPerMonth(_R, _M);
};

AnsiString TRurCalendar::GetCellText(int ACol, int ARow, int &n)
{
  int DayNum;
  AnsiString  result;
  n=-1;
  if (!ARow)
    // day names at tops of columns
    //
    result = System::Sysutils::FormatSettings.ShortDayNames[(1 + ACol) % 7];
  else
  {
    DayNum = FMonthOffset + ACol + (ARow - 1) * 7;
    if ((DayNum < 1) || (DayNum > DaysThisMonth()))
      result = "";
    else
      {result = IntToStr(DayNum);n=DayNum;}
  }

  return result;
};

void TRurCalendar::AddItem(RurCalendarItem &ri)
  {
  rca.Add(ri);
  }

void TRurCalendar::DeleteItem(long hash)
  {
  // empty
  }

void TRurCalendar::ClearItems()
  {
  rca.Flush();
  }

///
/// Pridanie sviatku
///
void TRurCalendar::AddSviatok(TDateTime d, AnsiString nazov)
{
RurCalendarItem ri;
ri.from=d;
ri.to=d;
ri.text=nazov;
sviatky.Add(ri);
}

///
/// Vymazanie sviatkov
///
void TRurCalendar::ClearSviatky()
{
sviatky.Flush();
}


void TRurCalendar::Today()
  {
  TDateTime tt=Date();
  unsigned short r,m,d;
  tt.DecodeDate(&r,&m,&d);
  Rok=r;
  Mesiac=m;
  Datum=tt;
  CalcRozmery();
  CalcMinMax();
  CalcOdDo();
  }

void __fastcall TRurCalendar::SetCiary(bool value)
{
        if(FCiary != value) {
                FCiary = value;
                Invalidate();
        }
}
bool __fastcall TRurCalendar::GetCiary()
{
        return FCiary;
}

void __fastcall TRurCalendar::SetVLinka(bool value)
{
        if(FVLinka != value) {
                FVLinka = value;
                Invalidate();
        }
}

bool __fastcall TRurCalendar::GetVLinka()
{
return FVLinka;
}

void __fastcall TRurCalendar::SetEnableMove(bool value)
{
if(FMove != value) {
  FMove = value;
  Invalidate();
  }
}

void __fastcall TRurCalendar::SetColorInactive(bool value)
{
if(FColorInactive != value) {
  FColorInactive = value;
  Invalidate();
  }
}

void __fastcall TRurCalendar::SetTyp(TCalendarTyp value)
{
if(FTyp != value) {
  FTyp=value;
  CalcOdDo();
  Dirty();
  }
}

// vrati datum, ktory zodpoveda pozicii mysky
TDateTime TRurCalendar::GetXYDate(int X, int Y)
{
    X-=rect_all.left;
    Y-=rect_all.top;
int xx=X/SX; // do ktoreho stlpca
int yy=Y/SY; // do ktoreho riadku
int difx=X-xx*SX; // rozdiel v jednej bunke
int dify=Y-yy*SY;
int dx=difx/19;
int dy=dify/15;
int n;
_M=FMesiac+yy*FStlpcov+xx;
_R=FRok;
while(_M>12)
  {_R++;_M-=12;}
int M=_M-1;
int R=_R;
int FirstDate = EncodeDate(R, M+1, 1);

FMonthOffset = 2 - ((DayOfWeek(FirstDate) - 1 + 7) % 7);
if (FMonthOffset == 2)
  FMonthOffset = -5;
AnsiString as=GetCellText(dx,dy-1,n);
try {
if(n!=-1)
  return TDateTime(_R,_M,n);
} catch(...) {}
return FDatum;
}

///
/// Vrati popis daneho dna
///
AnsiString TRurCalendar::GetXYDateDescription(int X, int Y)
{
    X-=rect_all.left;
    Y-=rect_all.top;
int xx=X/SX; // do ktoreho stlpca
int yy=Y/SY; // do ktoreho riadku
if(yy>=FRiadkov || xx>=FStlpcov) return "";
int difx=X-xx*SX; // rozdiel v jednej bunke
int dify=Y-yy*SY;
int dx=difx/19;
int dy=dify/15-1;
if(dx<0 || dy<0) return "";
TRurCalendarMonth &rcm=month[yy*FStlpcov+xx];
TRurCalendarDay   &day=rcm.day[dx+dy*7];
return day.popis;
}

void TRurCalendar::MoveUp()
{
FMesiac--;
if(FMesiac<1) {FMesiac=12;FRok--;}
CalcMinMax();CalcOdDo();
if(OnMoveClick) OnMoveClick(this);
Dirty();
}

void TRurCalendar::MoveDown()
{
FMesiac++;
if(FMesiac>12) {FMesiac=1;FRok++;}
CalcMinMax();CalcOdDo();
if(OnMoveClick) OnMoveClick(this);
Dirty();
}

///
/// Kliknutie myskou
///
void __fastcall TRurCalendar::MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
  {
  if(Button==0)
    {
    if(!PtInRect(&rect_all,TPoint(X,Y))) // klik uplne mimo
      return;
    // klik dolava
    if(FMove && PtInRect(&rect_l,TPoint(X,Y)))
      {
      MoveUp();
      return;
      }
    // klik doprava
    if(FMove && PtInRect(&rect_r,TPoint(X,Y)))
      {
      MoveDown();
      return;
      }

    X-=rect_all.left;
    Y-=rect_all.top;
    // niekde inde
    int xx=X/SX; // do ktoreho stlpca
    int yy=Y/SY; // do ktoreho riadku
    int difx=X-xx*SX; // rozdiel v jednej bunke
    int dify=Y-yy*SY;
    int dx=difx/19;
    int dy=dify/15;
    int n;
    _M=FMesiac+yy*FStlpcov+xx;
    _R=FRok;
    while(_M>12)
      {_R++;_M-=12;}
    int M=_M-1;
    int R=_R;
    int FirstDate = EncodeDate(R, M+1, 1);

    FMonthOffset = 2 - ((DayOfWeek(FirstDate) - 1 + 7) % 7);
    if (FMonthOffset == 2)
      FMonthOffset = -5;
    AnsiString as=GetCellText(dx,dy-1,n);
    if(n!=-1)
      {
      FDatum=TDateTime(_R,_M,n);
      CalcOdDo();
      Dirty();
      if(OnCellClick)
        OnCellClick(this);
      }
    }
  }

void __fastcall TRurCalendar::DblClick(void)
  {
  if(OnDblClick)
    OnDblClick(this);
  }

void __fastcall TRurCalendar::Resize(void)
  {
  TGraphicControl::Resize();
  if(!is_init) return;
  CalcRozmery();
  CalcMinMax();
  }

void __fastcall TRurCalendar::SetOnMoveClick(TNotifyEvent value)
{
if(FOnMoveClick != value) {
  FOnMoveClick = value;
  }
}
TNotifyEvent __fastcall TRurCalendar::GetOnMoveClick()
{
  return FOnMoveClick;
}
void __fastcall TRurCalendar::SetOnCellClick(TNotifyEvent value)
{
if(FOnCellClick != value) {
  FOnCellClick = value;
  }
}
TNotifyEvent __fastcall TRurCalendar::GetOnCellClick()
{
  return FOnCellClick;
}

///
/// Reakcia na ziadost o hint
///
MESSAGE void __fastcall TRurCalendar::CMHintShow(Messages::TMessage &Message)
{
PHintInfo ph=(PHintInfo)Message.LParam;
//Hint=GetXYDate(ph->CursorPos.x,ph->CursorPos.y).DateString();
ph->HintStr=GetXYDateDescription(ph->CursorPos.x,ph->CursorPos.y);
ph->ReshowTimeout=2000;
Message.Result=0;
}
