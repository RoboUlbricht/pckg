//---------------------------------------------------------------------------

#include <basepch.h>

#pragma hdrstop

#include "RurTable.h"
#include <io.h>
#include <dir.h>
#include <inifiles.hpp>
//#include "rurfile.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TRurTable *)
{
        new TRurTable(NULL);
}

RurTablePolozka::RurTablePolozka(const char *s1,const char *s2,const char *s3)
  {
  index=false;
  visible=false;
  strcpy(meno,s1);
  strcpy(pokec,s1); // pokial mu ty nenastavis meno
  *mpokec=0;
  switch(*s2)
    {
    case 'T':typ=RUR_TP_TEXT;break;
    case 'B':typ=RUR_TP_BOOL;break;
    case 'M':typ=RUR_TP_MEMO;break;
    case 'D':typ=RUR_TP_DATE;break;
    case 'I':typ=RUR_TP_TIME;break;
    case 'L':typ=RUR_TP_LONG;break;
    case '+':typ=RUR_TP_INCR;break;
    case 'F':typ=RUR_TP_FLOAT;break;
    }
  if(strlen(s2)==2 && s2[1]=='*') // L* - v tomto pripade vytvor primarny index
    index=true;
  if(s3)
    dlzka=atoi(s3);
  }

TRurTableSourceEnum::TRurTableSourceEnum(TRurTable *t)
  {
  tab=t;
  src=tab->GetSource();
  i=0;
  rp=&((*src)[i]);
  }

bool TRurTableSourceEnum::Next()
  {
  if(i<src->GetItemsInContainer())
    {i++;rp=&((*src)[i]);return true;}
  else
    {
    if(tab->GetAddTable() && src==tab->GetSource())
      {
      src=tab->GetAddTable()->GetSource();
      i=0;
      rp=&((*src)[i]);
      return true;
      }
    }
  return false;
  }

int TRurTableSourceEnum::ID()
  {
  return tab->GetSource()==src ? i : 1000+i;
  }

// zisti ci je polozka povolena
bool TRurTableEnable::Enable(int n)
  {
  for(int i=0;i<GetItemsInContainer();i++)
    {
    RurTableEnable &re=(*this)[i];
    if(re.fl&1 && re.tag==n)
      return re.enable;
    }
  return true;
  }

// zisti ci je polozka kontrolovana
bool TRurTableEnable::Verify(int n)
  {
  for(int i=0;i<GetItemsInContainer();i++)
    {
    RurTableEnable &re=(*this)[i];
    if(re.fl&2 && re.tag==n)
      return re.verify;
    }
  return false;
  }

RurTableFilterPolozka::RurTableFilterPolozka(const char *s)
  {
  strcpy(meno,s);
  }

void TRurTableFilter::Kill(const char *s)
  {
  for(int i=0;i<GetItemsInContainer();i++)
    if(strcmp(s,(*this)[i].meno)==0)
      {Destroy(i);return;}
  }

//---------------------------------------------------------------------------
__fastcall TRurTable::TRurTable(TComponent* Owner)
        : TFDTable(Owner)
{
nocheck_fields=false; // implicitne kontroluj
rur_lock_count=0;
table_add=0; // nema ziadneho podriadeneho
t_source=new TRurTableSource();
t_filter=new TRurTableFilter();
t_filter_ex=new TRurTableFilterEx();
t_enable=new TRurTableEnable();
history=0;
enable_history=true;
sort_cols=-1;
sort_zostupne=false;
for(int i=0;i<10;i++) disp_cols[i]=-1;

}
__fastcall TRurTable::~TRurTable(void)
{
if(ini_file.Length())
  SaveIni();
delete t_source;
delete t_filter;
delete t_filter_ex;
delete t_enable;
if(history)
  {
  history->Close();
  delete history;
  }
}

//// podla skriptu vytvori tabulku
//bool TRurTable::CreateFrom(const char *source,const char *target)
//{
//// nacitanie skriptu
//RurFile rf(source,"r",'/');
//if(!rf.IsValid())
//  return false;
//char tmp[256];
//bool is_enable=false;
//bool is_extended=false;
//while(rf.GetLine(tmp,250))
//  {
//  char *s1,*s2,*s3,*s4,*s5,*s6;
//  if(strcmp(tmp,"[kontrola]")==0) // nasleduju kontrolne polozky
//    {is_enable=true;is_extended=false;}
//  if(strcmp(tmp,"[extended]")==0) // nasleduju kontrolne polozky
//    {is_enable=false;is_extended=true;}
//  s1=strtok(tmp,";"); // nazov
//  s2=strtok(0,";");   // typ
//  s3=strtok(0,";");   // dlzka
//  s4=strtok(0,";");   // visible
//  s5=strtok(0,";");   // pokec pre cloveka
//  s6=strtok(0,";");   // maly pokec pre grid
//  if(is_enable && s1 && s2 && s3 && s4 && s5)
//    {
//    RurTableEnable re;
//    re.tag=atoi(s1);
//    re.fl=atoi(s2);
//    re.enable=atoi(s3);
//    re.verify=atoi(s4);
//    strcpy(re.pokec,s5);
//    t_enable->Add(re);
//    }
//  else if(is_extended && s1 && s2)
//    {
//    RurTableFilterPolozkaEx re;
//    strcpy(re.meno,s1);
//    strcpy(re.sql,s2);
//    if(s3) re.typ=atoi(s3); // volitelny parameter specialneho typu
//    t_filter_ex->Add(re);
//    }
//  else if(s1 && s2)
//    {
//    RurTablePolozka rp(s1,s2,s3);
//    if(s4 && atoi(s4)==1)
//      rp.visible=true;
//    if(s5 && strlen(s5)>50) s5[49]=0;
//    if(s5)
//      strcpy(rp.pokec,s5);
//    if(s6)
//      strcpy(rp.mpokec,s6);
//    t_source->Add(rp);
//    }
//  }
//// ini file
//ini_file=ChangeFileExt(target,".ini");
//LoadIni();
//// ci uz existuje
//TableName = target;
//TableType = ttParadox;
//if(access(target,0)==-1) // Exist nepouzivaj
//  {
//  AddMyFields(this,false);
//  CreateTable();
//  }
//else
//  {
//  if(nocheck_fields) // nie je nutne kontrolovat spravnost poli
//    return true;
//  Open();
//  bool vys=MaSpravnePolozky();
//  if(!vys)
//    {
//    TRurTable *tmp=new TRurTable(0);
//    tmp->TableName = ExtractFilePath(TableName)+"new_";
//    tmp->TableName+=ExtractFileName(TableName);
//    tmp->TableType = ttParadox;
//    AddMyFields(tmp,false); // nie history
//    tmp->CreateTable();
//    TBatchMove *bm=new TBatchMove(0);
//    bm->Source=this;
//    bm->Destination=tmp;
//    // ak je polozka z noveho aj v starom, kopiruj jej obsah
//    for(int i=0;i<t_source->GetItemsInContainer();i++)
//      {
//      if(FindField((*t_source)[i].meno)) // najdi polozku s tymto menom
//        bm->Mappings->Add((*t_source)[i].meno);
//      }
//    bm->Execute(); // to to kopiruj
//    delete bm;
//    Close();
//    AnsiString zlh=ExtractFilePath(TableName)+"bak_";
//    zlh+=ExtractFileName(TableName);
//    AnsiString zli=ChangeFileExt(zlh,".*");
//    ffblk ff;
//    int a=findfirst(zli.c_str(),&ff,0);
//    while(!a)
//      {
//      AnsiString tmp=ExtractFilePath(TableName)+ff.ff_name;
//      unlink(tmp.c_str());
//      a=findnext(&ff);
//      }
//    RenameTable(zlh.c_str());
//    TableName=target; // lebo on si to zmenil v rename
//    tmp->RenameTable(target);
//    delete tmp;
//    }
//  Close();
//  }
//return true;
//}

bool TRurTable::CreateHistory(const char *target)
  {
  history=new TRurTable(0);
  history->TableName = target;

  // TODO
//  history->TableType = ttParadox;

  if(access(target,0)==-1)
    {
    AddMyFields(history,true);
    history->CreateDataSet();
    }
  else
    {

    }
  return true;
  }

// pridava polozky
// ak je history prida tri nove polozka
// a indexuje podla datumu
void TRurTable::AddMyFields(TRurTable *t,bool his,bool idx)
  {
  t->FieldDefs->Clear();
  if(his)
    {
    TFieldDef *nf = t->FieldDefs->AddFieldDef();
    nf->DataType=ftDate;
    nf->Name="h_datum";

    nf = t->FieldDefs->AddFieldDef();
    nf->DataType=ftString;
    nf->Name="h_meno";
    nf->Size=10;

    nf = t->FieldDefs->AddFieldDef();
    nf->DataType=ftString;
    nf->Name="h_what";
    nf->Size=1;

// history nepotrebuje indexovat, lebo vzdy dava len append
    t->IndexDefs->Clear();
    }
  for(int i=0;i<t_source->GetItemsInContainer();i++)
    {
    TFieldDef *nf = t->FieldDefs->AddFieldDef();
    nf->DataType=(TFieldType)(*t_source)[i].typ;
    nf->Name=(*t_source)[i].meno;
    switch((*t_source)[i].typ)
      {
      case RUR_TP_TEXT:
      case RUR_TP_MEMO:
        nf->Size=(*t_source)[i].dlzka;
        break;
      }
    }
  if(idx && his==false)
    {
    AnsiString ss;
    for(int i=0;i<t_source->GetItemsInContainer();i++)
      if((*t_source)[i].index)
        {
        if(ss.Length())
          ss=ss+";"+(*t_source)[i].meno;
        else
          ss=(*t_source)[i].meno;
        }
    t->IndexDefs->Clear();
    if(ss.Length())
      {
      TIndexDef *ni = t->IndexDefs->AddIndexDef(); // add an index
      ni->Name = "PIndex";
      ni->Fields = ss;
      ni->Options << ixPrimary << ixUnique;
      }
    }
  }

bool TRurTable::MaSpravnePolozky()
  {
  if(nocheck_fields) // bol uz predtym skontrolovany
    return true;
  if(t_source->GetItemsInContainer()!=FieldDefs->Count)
    return false;
  for(int i=0;i<t_source->GetItemsInContainer();i++)
    {
    TFieldDef *nf = FieldDefs->Items[i];
    if(nf->DataType!=(*t_source)[i].typ)
      return false;
    if(nf->Name!=AnsiString((*t_source)[i].meno))
      return false;
    if(nf->DataType==RUR_TP_TEXT &&
       nf->Size!=(*t_source)[i].dlzka)
      return false;
    }
  return true;
  }

// uklada si nastavenie
void TRurTable::SaveIni()
  {
  TIniFile *ini=new TIniFile(ini_file);
  ini->WriteInteger("Sort","Sort1",sort_cols);
  ini->WriteInteger("Sort","Zostupne",sort_zostupne);
  for(int i=0;i<10;i++)
    {
    AnsiString tmp;
    tmp.printf("Col%d",i);
    ini->WriteInteger("Cols",tmp,disp_cols[i]);
    }
  for(int i=0;i<t_enable->GetItemsInContainer();i++)
    {
    RurTableEnable &re=(*t_enable)[i];
    AnsiString tmp(re.pokec);
    AnsiString out;
    out+=re.enable ? "1" : "0";
    out+=re.verify ? "1" : "0";
    ini->WriteString("Verify",tmp,out);
    }
  delete ini;
  SaveFilter("Filter");
  }

// obnovi si nastavenia
void TRurTable::LoadIni()
  {
  TIniFile *ini=new TIniFile(ini_file);
  sort_cols=ini->ReadInteger("Sort","Sort1",-1);
  sort_zostupne=ini->ReadInteger("Sort","Zostupne",0);
  for(int i=0;i<10;i++)
    {
    AnsiString tmp;
    tmp.printf("Col%d",i);
    disp_cols[i]=ini->ReadInteger("Cols",tmp,-1);
    }
  for(int i=0;i<t_enable->GetItemsInContainer();i++)
    {
    RurTableEnable &re=(*t_enable)[i];
    AnsiString tmp(re.pokec);
    AnsiString out;
    out=ini->ReadString("Verify",tmp,"");
    if(out.Length()==2)
      {
      re.enable=out[1]=='1' ? true : false;
      re.verify=out[2]=='1' ? true : false;
      }
    }
  delete ini;
  LoadFilter("Filter");
  }

void TRurTable::LoadFilter(const char *s)
  {
  TIniFile *ini=new TIniFile(ini_file);
  for(int i=0;i<10;i++)
    {
    AnsiString tmp;
    tmp.printf("Col1_%d",i);
    filt_cols1[i]=ini->ReadInteger(s,tmp,-1);
    tmp.printf("Col2_%d",i);
    filt_cols2[i]=ini->ReadInteger(s,tmp,-1);
    tmp.printf("Col3_%d",i);
    filt_cols3[i]=ini->ReadString(s,tmp,"");
    }
  delete ini;

  }

void TRurTable::SaveFilter(const char *s)
  {
  TIniFile *ini=new TIniFile(ini_file);
  for(int i=0;i<10;i++)
    {
    AnsiString tmp;
    tmp.printf("Col1_%d",i);
    ini->WriteInteger(s,tmp,filt_cols1[i]);
    tmp.printf("Col2_%d",i);
    ini->WriteInteger(s,tmp,filt_cols2[i]);
    tmp.printf("Col3_%d",i);
    ini->WriteString(s,tmp,filt_cols3[i]);
    }
  delete ini;
  }

  void TRurTable::LoadFilters() {

    TIniFile *ini = new TIniFile(ini_file);
    t_filter->Flush();

    for (int i = 0; i < 100; i++) {

      AnsiString tmp;
      tmp.printf("Filter_%d", i);
      tmp = ini->ReadString("Filters", tmp, "");

      if (tmp.Length() == 0) {

        i = 100;
      }
      else {

        RurTableFilterPolozka filter_polozka(tmp.c_str());
        t_filter->Add(filter_polozka);
      }
    }

    delete ini;
  }

void TRurTable::SaveFilters()
  {
  TIniFile *ini=new TIniFile(ini_file);
  for(int i=0;i<t_filter->GetItemsInContainer();i++)
    {
    AnsiString tmp;
    tmp.printf("Filter_%d",i);
    ini->WriteString("Filters",tmp,(*t_filter)[i].meno);
    }
  for(int i=t_filter->GetItemsInContainer();i<100;i++)
    {
    AnsiString tmp;
    tmp.printf("Filter_%d",i);
    ini->DeleteKey("Filters",tmp);
    }
  delete ini;
  }

// cez tuto funkciu urobis vsetko
// what 1-load 2-save 3-delete 4-init
void TRurTable::LoadSaveFilter(int what,const char *s)
  {
  switch(what)
    {
    case 1:
      LoadFilter(s);
      break;
    case 2: {
      SaveFilter(s);
      RurTableFilterPolozka filter_polozka(s);
      t_filter->Add(filter_polozka);
      SaveFilters();
      break;
    }
    case 3:
      {
      TIniFile *ini=new TIniFile(ini_file);
      t_filter->Kill(s);
      ini->EraseSection(s);
      delete ini;
      }
      SaveFilters();
      break;
    case 4:
      LoadFilters();
      break;
    }
  }

// jedina podmienka: index musi byt v select
AnsiString TRurTable::GetSelect()
  {
  AnsiString tmp;
  int pos;
  bool first=false;
  bool idx=false;
  bool robil_nieco=false;
  for(int i=0;i<6;i++)
    {
    if(GetDispCol(i)!=-1)
      {
      robil_nieco=true;
      if(GetDispCol(i)==sort_cols) idx=true; // index uz je zahrnuty v select
      if(first) tmp+=", ";
      int pp=GetDispCol(i);
      if(pp>1000) // zober z podriadenej
        {
        tmp+="v.\"";
        tmp+=(*table_add->GetSource())[pp-1000].meno;
        }
      else
        {
        tmp+="u.\"";
        tmp+=(*t_source)[pp].meno;
        }
      tmp+="\"";
      first=true;
      }
    }
  if(!robil_nieco) // podhodim prvych 6
    {
    TRurTableSource *ts=GetSource();
    int poc=0;
    for(int i=0;i<ts->GetItemsInContainer() ;i++)
      {
      RurTablePolozka &rp=(*ts)[i];
      if(rp.visible)
        {
        if(first) tmp+=", ";
        tmp+="u.\"";
        tmp+=(*t_source)[i].meno;
        tmp+="\"";
        first=true;
        poc++;
        if(poc==6)
          goto stacilo;
        }
      }
    }
stacilo:
  if(!idx) // index musi byt zahrnuty v select
    {  // uz tam cosi je a ja mam sortovane
    if(first /*&& sort_cols!=-1*/) // ciarku dan vzdy, lebo GetSort vzdy nieco vrati
      tmp+=", ";
    tmp+=GetSort(false);  // nesmie dat desc
    }
  tmp+=" ";
  return tmp;
  }

AnsiString
TRurTable::GetFilter()
  {
  AnsiString tmp=filter;
  if(filter2.Length())
    {
    if(tmp.Length()) tmp+=" AND ";
    tmp+=filter2;
    }
  if(tmp.Length()) tmp=AnsiString("(")+tmp+")";
  return tmp;
  }


// asc_desc daj false, ked nerobis sort v sql
AnsiString TRurTable::GetSort(bool asc_desc)
  {
  AnsiString tmp;
  if(sort_cols>1000)
    {
    tmp="v.\"";
    tmp+=(*table_add->GetSource())[sort_cols-1000].meno;
    }
  else
    {
    tmp="u.\"";
    tmp+=sort_cols!=-1 ? (*t_source)[sort_cols].meno :
                         (*t_source)[0].meno;
    }
  tmp+="\"";
  if(asc_desc && sort_zostupne) // musi to naozaj chciet
    tmp+=" desc";
  return tmp;
  }

// uklada si nastavenie filtracie
void TRurTable::SetFiltCol(int x,int a,int b,AnsiString c)
  {
  filt_cols1[x]=a;
  filt_cols2[x]=b;
  filt_cols3[x]=c;
  }

void TRurTable::GetFiltCol(int x,int &a,int &b,AnsiString &c)
  {
  a=filt_cols1[x];
  b=filt_cols2[x];
  c=filt_cols3[x];
  }

// podla nazvu v dbf vrati pokec
AnsiString TRurTable::GetColName(const char *s)
  {
  for(int i=0;i<t_source->GetItemsInContainer();i++)
    if(strcmp((*t_source)[i].meno,s)==0)
      return AnsiString((*t_source)[i].pokec);
  return AnsiString(s);
  }

AnsiString SafeString(Variant v)
  {
  if(v.IsNull())
    return AnsiString("");
  else return VarToStr(v.AsType(0x000C));
  }


// na zaklade kontrolovanych poloziek povie, ci je vsetko vyplnene
bool TRurTable::CanPost(AnsiString &as)
  {
  as="Dialóg nemožno ukoncit, pretože neboli vyplnené nasledujúce položky:\n";
  bool vys=true;
  for(int i=0;i<t_enable->GetItemsInContainer();i++)
    {
    RurTableEnable &re=(*t_enable)[i];
    if(re.fl&2 && re.verify)
      {
      AnsiString tmp=SafeString(FieldValues[re.pokec]);
      if(tmp==AnsiString(""))
        {
        as+=AnsiString("\n")+GetColName(re.pokec);
        vys=false;
        }
      }
    }
  return vys;
  }

// zazalohuje si sama udaje
// ak ma otvorene history
void __fastcall TRurTable::Post(void)
  {
  TFDTable::Post();
  if(history && enable_history)
    { // enable_history sa obcas hodi ked nepotrebujem zachovat postupnosti
    history->OpenOrExecute();
    history->Append();
    history->FieldValues["h_meno"]="pokus";
    history->FieldValues["h_datum"]=TDateTime().CurrentDate();
    for(int i=0;i<Fields->Count;i++)
      history->Fields->Fields[i+3]=Fields->Fields[i];
    history->Post();
    }
  }

int TRurTable::LockOpen()
  {
  rur_lock_count++;
  if(rur_lock_count==1)
    OpenOrExecute();
  return rur_lock_count;
  }

int TRurTable::LockClose()
  {
  if(rur_lock_count<=0)
    return -1;
  rur_lock_count--;
  if(rur_lock_count==0)
    Close();
  return rur_lock_count;
  }

TRurTableSourceEnum* TRurTable::CreateSourceEnum()
  {
  return new TRurTableSourceEnum(this);
  }


//---------------------------------------------------------------------------
namespace Rurtable
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TRurTable)};
                 RegisterComponents("Rur", classes, 0);
        }
}
//---------------------------------------------------------------------------

