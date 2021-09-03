//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "RurSpeedButton.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TRurSpeedButton *)
{
        new TRurSpeedButton(NULL);
}
//---------------------------------------------------------------------------
__fastcall TRurSpeedButton::TRurSpeedButton(TComponent* Owner)
        : TSpeedButton(Owner)
{
begindrag=false;
FSQLConnection=NULL;
FSQLMaximum=10;
FAppend=false;
lb=NULL;
texty=NULL;
Glyph->Handle=::LoadBitmap(NULL,MAKEINTRESOURCE(32752));
}

__fastcall TRurSpeedButton::~TRurSpeedButton(void)
{
if(texty)
  delete[] texty;
}

class TRListBox : public TCustomListBox
  {
public:
  __fastcall virtual TRListBox(Classes::TComponent* c) : TCustomListBox(c) {}
  virtual void __fastcall CreateParams(Controls::TCreateParams &Params);
  virtual void __fastcall CreateWnd(void);
  __property OnMouseDown;
  __property OnMouseUp;
  __property OnExit;
  __property MouseCapture;
  };

void __fastcall TRListBox::CreateParams(Controls::TCreateParams &Params)
{
TCustomListBox::CreateParams(Params);
Params.Style|=WS_BORDER;
Params.ExStyle|=WS_EX_TOOLWINDOW|WS_EX_TOPMOST;
Params.WindowClass.style=CS_SAVEBITS;
}

void __fastcall TRListBox::CreateWnd(void)
{
TCustomListBox::CreateWnd();
//Windows->
}

void __fastcall TRurSpeedButton::MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
{
if(Button==mbLeft && Enabled && FDBMemo)
  {
  if(lb) return;
  TPoint pp=Point(FDBMemo->Left,FDBMemo->Top+FDBMemo->Height);
  TWinControl *par=FDBMemo->Parent;
  pp.x+=par->Left;
  pp.y+=par->Top;
  while(par && par->Parent && dynamic_cast<TForm*>(par->Parent)==NULL)
    {
    par=par->Parent;
    pp.x+=par->Left;
    pp.y+=par->Top;
    }
  if(par && par->Parent)
    par=par->Parent;
  lb=new TRListBox((TComponent*)NULL);
  lb->Left=pp.x;
  lb->Top=pp.y;
  lb->Width=FDBMemo->Width;
  ((TRListBox*)lb)->OnMouseDown=ListMouseDown;
  ((TRListBox*)lb)->OnMouseUp=ListMouseUp;
  ((TRListBox*)lb)->OnExit=ExitMouse;
  lb->Parent=par;
  ::SetWindowPos(lb->Handle,HWND_TOPMOST,0,0,lb->Width,300,0/*SWP_NOMOVE*/);
  TFDQuery *qtmp=new TFDQuery(0);
  qtmp->Connection=FSQLConnection;
  qtmp->SQL->Text=FSQL;
  qtmp->OpenOrExecute();
  qtmp->Last();
  texty=new AnsiString[FSQLMaximum];
  int m=FSQLMaximum;
  while(m && !(qtmp->Bof))
    {
    AnsiString stmp=qtmp->Fields->operator [](0)->AsString;
    if(stmp.Length() && lb->Items->IndexOf(stmp)==-1)
      {
      int dl=stmp.Pos("\r\n");
      if(dl==0) dl=stmp.Length();
      AnsiString stmp1=stmp.SubString(1,dl-1);
      int x=lb->Items->Add(stmp1);
      texty[x]=stmp;
      m--;
      }
    qtmp->Prior();
    }
  delete qtmp;
  }
  ((TRListBox*)lb)->MouseCapture=true;
  lb->SetFocus();
}

void __fastcall TRurSpeedButton::ListMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
if(!PtInRect(lb->ClientRect,Point(X,Y)))
  {
  ExitMouse(Sender);
  }
else
  begindrag=true;
}


void __fastcall TRurSpeedButton::ListMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
if(!begindrag) return;
int sel=lb->ItemIndex;
if(sel>=0)
  {
  if(FAppend)
    {
    if(FDBMemo->Field->AsString.Length())
    FDBMemo->Field->AsString=FDBMemo->Field->AsString+"\r\n"+texty[sel];
    else
    FDBMemo->Field->AsString=FDBMemo->Field->AsString+texty[sel];
    }
  else
    FDBMemo->Field->AsString=texty[sel];
  }
if(lb)
  delete lb;
lb=NULL;
FDBMemo->SetFocus();
begindrag=false;
}

void __fastcall TRurSpeedButton::ExitMouse(TObject *Sender)
{
if(lb)
  delete lb;
lb=NULL;
FDBMemo->SetFocus();
begindrag=false;
}

//---------------------------------------------------------------------------
namespace Rurspeedbutton
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TRurSpeedButton)};
                 RegisterComponents("Rur", classes, 0);
        }
}
//---------------------------------------------------------------------------
