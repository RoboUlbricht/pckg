//---------------------------------------------------------------------------

#ifndef RurTableH
#define RurTableH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <DB.hpp>
#if defined(RUR_DB_USE_ADO)

#else
#include <FireDAC.Comp.Client.hpp>
#endif

#include "rurarray.h"

#define RUR_TP_TEXT  ftString
#define RUR_TP_BOOL  ftBoolean
#define RUR_TP_MEMO  ftMemo
#define RUR_TP_DATE  ftDate
#define RUR_TP_TIME  ftTime
#define RUR_TP_LONG  ftInteger
#define RUR_TP_INCR  ftAutoInc
#define RUR_TP_FLOAT ftFloat
struct RurTablePolozka
  {
  char meno[20]; // nazov v dbf
  int typ;
  int dlzka;
  bool index;
  bool visible;   // ci ju ukaze cloveku
  char pokec[50]; // nazov pre ludi
  char mpokec[20];// maly nazov v gride
  int sirka;      // sirka
  RurTablePolozka() {;}
  RurTablePolozka(const char *s1,const char *s2,const char *s3);
  };

class TRurTableSource : public RArray<RurTablePolozka>
  {
public:
  TRurTableSource() : RArray<RurTablePolozka>(16,16) {}
  };

class TRurTable;  
class TRurTableSourceEnum
  {
  int i;
  TRurTableSource *src;
  TRurTable *tab;
public:
  RurTablePolozka *rp;
  TRurTableSourceEnum(TRurTable *t);
  bool Next();
  int ID();
  };

struct RurTableEnable
  {
  int tag; // takto ho najdem v dialogu
  int fl;  // 1-enable 2-verify 3-vsetko
  bool enable; // ci moze polozku zobrazit
  bool verify; // ci ma polozku kontrolovat
  char pokec[50]; // je tu nazov dbf polozky alebo priamo pokec
  };

class TRurTableEnable : public RArray<RurTableEnable>
  {
public:
  TRurTableEnable() : RArray<RurTableEnable>(16,16) {}
  bool Enable(int n);
  bool Verify(int n);
  };

struct RurTableFilterPolozka
  {
  char meno[50];
  RurTableFilterPolozka() {;}
  RurTableFilterPolozka(const char *s);
  };

class TRurTableFilter : public RArray<RurTableFilterPolozka>
  {
public:
  TRurTableFilter() : RArray<RurTableFilterPolozka>(16,16) {}
  void Kill(const char *s);
  };

struct RurTableFilterPolozkaEx
  {
  char meno[50];
  char sql[255];
  int typ;
  RurTableFilterPolozkaEx() {typ=0;}
  };

class TRurTableFilterEx : public RArray<RurTableFilterPolozkaEx>
  {
public:
  TRurTableFilterEx() : RArray<RurTableFilterPolozkaEx>(0,16) {}
//  void Kill(const char *s);
  };

//---------------------------------------------------------------------------
class PACKAGE TRurTable : public TFDTable
{
int rur_lock_count;        // kontroluje pocet locknuti
TRurTable *table_add;      // moze mat tohoto priradeneho
TRurTableSource *t_source; // to je zdrojak na vytvorenie table
TRurTableFilter *t_filter; // tu su mena filtrov
TRurTableFilterEx *t_filter_ex; // tu su mena filtrov rozsirenych
TRurTableEnable *t_enable; // tu su povolenia zobrazenia a kontroly
TRurTable *history;
bool enable_history;
// toto sluzi na triedenie na gride
int disp_cols[10]; // tieto polozky ukaz v query
int sort_cols;     // podla tohoto sortuj query
bool sort_zostupne;// poraduie triedenia
// toto sluzi na filtrovanie
int filt_cols1[10];        // vybrata polozka
int filt_cols2[10];        // akcia
AnsiString filt_cols3[10]; // text

AnsiString ini_file; // tu si odklada nastavenia
AnsiString filter; // tu je ulozene filtrovanie
AnsiString filter2; // tu je ulozene filtrovanie

bool nocheck_fields; // nekontroluje spravnost poloziek
private:
  bool MaSpravnePolozky();
  void SaveIni();
  void LoadIni();
  void LoadFilter(const char *s);
  void SaveFilter(const char *s);
  void LoadFilters();
  void SaveFilters();
protected:
public:
        __fastcall TRurTable(TComponent* Owner);
        __fastcall virtual ~TRurTable(void);
  virtual void __fastcall Post(void);
  void AddMyFields(TRurTable *t,bool his,bool idx=true);
//  bool CreateFrom(const char *source,const char *target);
  bool CreateHistory(const char *target);
  void EnableHistory(bool hi) {enable_history=false;}
  void SetMaSpravnePolozky() {nocheck_fields=true;}
  void SetAddTable(TRurTable *ttt) {table_add=ttt;}
  TRurTable* GetAddTable() {return table_add;}
  TRurTableSource* GetSource() {return t_source;}
  TRurTableFilter* GetTFilter() {return t_filter;}
  TRurTableFilterEx* GetTFilterEx() {return t_filter_ex;}
  TRurTableEnable* GetEnable() {return t_enable;}
  int GetDispCol(int i) {return disp_cols[i];}
  void SetDispCol(int i,int x) {disp_cols[i]=x;}
  int GetSortCol() {return sort_cols;}
  void SetSortCol(int x,bool zo) {sort_cols=x;sort_zostupne=zo;}
  void SetFiltCol(int x,int a,int b,AnsiString c);
  void GetFiltCol(int x,int &a,int &b,AnsiString &c);
  void SetFilter(AnsiString as) {filter=as;}
  void SetFilter2(AnsiString as) {filter2=as;}
  bool GetZostupne() {return sort_zostupne;}
  AnsiString GetSelect();
  AnsiString GetSort(bool asc_desc=true);
  AnsiString GetFilter();
  void LoadSaveFilter(int what,const char *s);
  AnsiString GetIniString() {return ini_file;}
  AnsiString GetColName(const char *s);
  bool CanPost(AnsiString &as);
  int LockOpen();
  int LockClose();
  TRurTableSourceEnum *CreateSourceEnum();
__published:
};
//---------------------------------------------------------------------------
#endif
