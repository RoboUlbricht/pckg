//---------------------------------------------------------------------------

#ifndef RurSpeedButtonH
#define RurSpeedButtonH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Buttons.hpp>
#include <Controls.hpp>
#include <DBCtrls.hpp>
#include <FireDAC.Comp.Client.hpp>
//---------------------------------------------------------------------------
class PACKAGE TRurSpeedButton : public TSpeedButton
{
private:
        TCustomListBox* lb;
        TDBMemo* FDBMemo;
        AnsiString FSQL;
        TFDConnection* FSQLConnection;
        int FSQLMaximum;
        bool FAppend;
        AnsiString* texty;
        bool begindrag;
protected:
  void __fastcall ListMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall ListMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall ExitMouse(TObject *Sender);
public:
        __fastcall TRurSpeedButton(TComponent* Owner);
        __fastcall virtual ~TRurSpeedButton(void);
        DYNAMIC void __fastcall MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
__published:
        __property TDBMemo* DBMemo  = { read=FDBMemo, write=FDBMemo };
        __property AnsiString SQL  = { read=FSQL, write=FSQL };
        __property bool AppendMode = { read=FAppend, write=FAppend };
        __property TFDConnection* SQLConnection  = { read=FSQLConnection, write=FSQLConnection };
        __property int SQLMaximum  = { read=FSQLMaximum, write=FSQLMaximum };
};
//---------------------------------------------------------------------------
#endif
