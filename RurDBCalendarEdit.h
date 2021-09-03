//---------------------------------------------------------------------------

#ifndef RurDBCalendarEditH
#define RurDBCalendarEditH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <buttons.hpp>
#include <Data.DB.hpp>
#include <dbctrls.hpp>
//---------------------------------------------------------------------------
class PACKAGE TRurDBCalendarEdit : public TCustomEdit
{
private:
  TSpeedButton *FSipka;
  TFieldDataLink *FDataLink;
  TField* __fastcall GetField();
  AnsiString __fastcall GetDataField();
  TDataSource* __fastcall GetDataSource();
  void __fastcall SetDataField(AnsiString Value);
  void __fastcall SetDataSource(TDataSource *Value);
  void __fastcall DataChange(TObject *Sender);
  void __fastcall UpdateData(TObject *Sender);
  void __fastcall SetEditRect(void);
  DYNAMIC void __fastcall Change(void);
  MESSAGE void __fastcall WMSize(TWMSize &Message);
  void __fastcall CMExit(TWMNoParams Message);
protected:
  virtual void __fastcall CreateParams(TCreateParams &Params);
  virtual void __fastcall CreateWnd(void);
public:
  __fastcall TRurDBCalendarEdit(TComponent* Owner);
  __fastcall ~TRurDBCalendarEdit();
  void __fastcall SipkaClick(TObject *Sender);
  __property TField* Field = {read=GetField};
__published:
  __property AnsiString DataField = {read=GetDataField, write=SetDataField, nodefault};
  __property TDataSource* DataSource = {read=GetDataSource, write=SetDataSource, nodefault};
  __property OnChange;
BEGIN_MESSAGE_MAP
  VCL_MESSAGE_HANDLER(WM_SIZE, TWMSize, WMSize)
  VCL_MESSAGE_HANDLER(CM_EXIT, TWMNoParams, CMExit)
END_MESSAGE_MAP(TCustomEdit);
};
//---------------------------------------------------------------------------
#endif
