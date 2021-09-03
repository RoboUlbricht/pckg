//----------------------------------------------------------------------------
#ifndef RuDBGridLocateH
#define RuDBGridLocateH
//----------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
//----------------------------------------------------------------------------
class TRurDBGrid;

class TLocateDlg : public TForm
{
__published:
	TButton *OKBtn;
        TLabel *Label1;
        TComboBox *c;
        TLabel *Label2;
        TEdit *e;
        TButton *Button1;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall eChange(TObject *Sender);
        void __fastcall cChange(TObject *Sender);
private:
public:
        TRurDBGrid *rd;
        int ii;
	virtual __fastcall TLocateDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TLocateDlg *LocateDlg;
//----------------------------------------------------------------------------
#endif    
