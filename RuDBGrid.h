//---------------------------------------------------------------------------

#ifndef RuDBGridH
#define RuDBGridH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <smDBGrid.hpp>
#include <Grids.hpp>
#include<vector>

class TRurTable;
class TDataSetCfg;
class TRurDBGrid;

typedef void __fastcall (__closure *TRurDBGridWWWEvent)(AnsiString path);
typedef bool __fastcall (__closure *TRurDBGridColEnabled)(TObject *Sender, AnsiString colname, AnsiString fieldname);
typedef void __fastcall (__closure *TRurDBGridGroupEvent)(TObject *Sender);
typedef void __fastcall (__closure *TRurDBGridTitleClickSort)(TColumn *col);
extern AnsiString (*TranslateGrid)(AnsiString s);
extern int GRID_RK_DELAY;

///
/// Definicia stlpca
///
struct TRurDBGridColumn
{
String caption;
String fieldname;
int width;
};

typedef std::vector<TRurDBGridColumn> vColumns;

///
/// Definicia pohladu
///
struct TRurDBGridView
{
String name;      ///< Nazov pohladu
String shortcut;  ///< Klavesova skratka
vColumns columns; ///< Zosnam stlpcov
bool is_system;   ///< Je to systemovy pohlad
};

typedef std::vector<TRurDBGridView> vViews;

///
/// Hladanie pomenovanych stlpcov
///
struct TRurDBGridColumnName
{
AnsiString name;
String sort;
TColumn *col;
};

///
/// Grid
///
class PACKAGE TRurDBGrid : public TSMDBGrid
{
private:
        TRurTable *trt; // pointer na moju table
        TDataSetCfg *cfg; // pointer na moju konfiguraciu
        AnsiString FRIniName;
        bool FTitleSort;
        bool FKeyLocateRecord;
        int FSort;
        Word FCommand;
        TPopupMenu *FVlastneMenu;
        TMenuItem *mih[4];
        TNotifyEvent FOnBeforeKeyLocate,FOnAfterKeyLocate;
        TNotifyEvent FOnDelayedScroll;
        TNotifyEvent FOnColumnsChanged;
        TRurDBGridTitleClickSort FOnTitleClickSort;
        TRurDBGridWWWEvent FOnEmail;
        TRurDBGridWWWEvent FOnWWW;
        TDBGridClickEvent  FOnCellCheckClick;
        TDBGridClickEvent  FOnCellClick2;
        TRurDBGridColEnabled FOnColEnable;
        TRurDBGridGroupEvent FOnGroupCreate;
        TButton *tmpb;
        TDateTime  ds_time; ///< cas, kedy to ma vypuknut
        TTimer    *ds_timer;///< timer ktory to sleduje
        int roll_koliesko;  ///< pouzite na znizovanie citlivosti kolieska
        static String view_path;   ///< Cesta do adresara, kde su ulozne pohlady
        vColumns def_columns; ///< Zoznam stlpcov z pohladu
        vViews def_views;     ///< Zoznam pohladov
        vViews user_views;     ///< Zoznam uzivatelskych pohladov (aj implicitne tam su)
        String last_view;     ///< Posledny pohlad
        AnsiString xmlname; ///< Meno na ukladanie kofiguracie
        bool configcolumns; ///< Ci moze konfigurovat stlpce
        bool loadedcolumns; ///< Ci boli stlpce nastavene vlastnymi prostriedkami
        TRurDBGridColumnName *names; ///< Zoznam pomenovanych stlpcov
        int namescount;     ///< Pocet pomenovanych stlpcov
        bool hide_indicator;///< Ma skryt dgIndicator po vypnuti multiselektu
        TNotifyEvent FOnExport;
        AnsiString FExtendedColumns;
        String dead_key;

        void __fastcall SetRIniName(AnsiString value);
        AnsiString __fastcall GetRIniName();
  DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
  DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
  void __fastcall MMenuExecute(TObject *Sender);
  void __fastcall DelayedTimer(TObject *Sender);
  AnsiString TranslateText(AnsiString s);
  void InvertSelectAllClick();
  void CreateGroupClick();
  void LoadView(int id);
  void SetupViews();

protected:
  DYNAMIC void __fastcall TitleClick(TColumn* Column);
  DYNAMIC void __fastcall KeyPress(TCHAR &Key);
  DYNAMIC void __fastcall KeyDown(Word &Key, Classes::TShiftState Shift);
  DYNAMIC void __fastcall KeyUp(Word &Key, Classes::TShiftState Shift);
  DYNAMIC TPopupMenu* __fastcall GetPopupMenu(void);
  DYNAMIC bool __fastcall DoMouseWheelDown(Classes::TShiftState Shift, const TPoint &MousePos);
  DYNAMIC bool __fastcall DoMouseWheelUp(Classes::TShiftState Shift, const TPoint &MousePos);
  void __fastcall WMVScroll(TWMVScroll& Message);
  virtual void __fastcall Scroll(int Distance);

  DYNAMIC void __fastcall ColumnMoved(int FromIndex, int ToIndex);
  DYNAMIC void __fastcall ColWidthsChanged(void);

//BEGIN_MESSAGE_MAP
//  MESSAGE_HANDLER(WM_VSCROLL, TWMVScroll, WMVScroll)
//END_MESSAGE_MAP(TRurDBGrid)

public:
        __fastcall TRurDBGrid(TComponent* Owner);
        __fastcall ~TRurDBGrid();
        void CreateRColumns(TRurTable *tb,bool all=false);
        void CreateRColumns(TDataSetCfg *_cfg,bool all=false);
        void SaveSirky();
        AnsiString GetAsText();
        AnsiString GetAsHtml();
        void Copy2Clipboard();
        void CopyCell2Clipboard();
        void InternalDelayedScroll();
        void SetupColumns();
        void PrisposobSirky(int zaciatok);
        static void ViewInitPath(String path);
        static void XMLInitPath(AnsiString path);
        static void XMLClosePath();
        void XMLInitName(AnsiString name);
        void XMLSave(bool force=false);
        TColumn* FindNamedColumn(AnsiString name);
        AnsiString FindColumnName(TColumn *col);
	void ReagujNaMenu(TMenuItem *menu);
  void DisableAutosave() {xmlname="";}
  String GetColOrder(TColumn *col, String def = EmptyStr);

  friend class TRurDBGridConfigurator;
  friend class TRurDBGridColumnsDlg;
  friend class TRurDBGridViewsDlg;

__published:
  __property AnsiString RIniName  = { read=GetRIniName, write=SetRIniName };
  __property bool TitleSort  = { read=FTitleSort, write=FTitleSort };
  __property bool KeyLocateRecord  = { read=FKeyLocateRecord, write=FKeyLocateRecord};
  __property TNotifyEvent OnBeforeKeyLocate  = { read=FOnBeforeKeyLocate, write=FOnBeforeKeyLocate };
  __property TNotifyEvent OnAfterKeyLocate  = { read=FOnAfterKeyLocate, write=FOnAfterKeyLocate };
  __property TNotifyEvent OnDelayedScroll  = { read=FOnDelayedScroll, write=FOnDelayedScroll };
  __property TNotifyEvent OnColumnsChanged  = { read=FOnColumnsChanged, write=FOnColumnsChanged };
  __property TRurDBGridTitleClickSort OnTitleClickSort  = { read=FOnTitleClickSort, write=FOnTitleClickSort };
  __property int Sort = { read = FSort, write = FSort };
  __property TRurDBGridWWWEvent OnEmail  = { read=FOnEmail, write=FOnEmail };
  __property TRurDBGridWWWEvent OnWWW  = { read=FOnWWW, write=FOnWWW };
  __property TDBGridClickEvent OnCellCheckClick = {read=FOnCellCheckClick, write=FOnCellCheckClick};
  __property TDBGridClickEvent OnCellClick2 = {read=FOnCellClick2, write=FOnCellClick2};
  __property TRurDBGridColEnabled OnColEnabled  = { read=FOnColEnable, write=FOnColEnable };
  __property TRurDBGridGroupEvent OnGroupCreate  = { read=FOnGroupCreate, write=FOnGroupCreate };
  __property TNotifyEvent OnExport  = { read=FOnExport, write=FOnExport };
  __property AnsiString ExtendedColumns = {read=FExtendedColumns, write=FExtendedColumns};
};
//---------------------------------------------------------------------------
#endif
