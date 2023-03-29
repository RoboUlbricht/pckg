#ifndef __rurcalendarbase_h
#define __rurcalendarbase_h

#include "rurarray.h"

// toto je zakladna polozka, ktora sa vyznacuje v kalendari
struct RurCalendarItem
  {
  long hash; // id polozky
  long typ;  // rozlisenie o aku kalendarovy typ sa jedna
  long uid;  // ID uzivatela
  TRect r;
  TColor color;
  TDateTime from;
  TDateTime to;
//  TDateTime from_time;
//  TDateTime to_time;
  bool splnene; // splneny termin
  AnsiString text;

  RurCalendarItem() {
    hash=typ=0;
    uid = 0;
    r.left=0; r.top=0; r.right=0; r.bottom=0;
    splnene = false;
    }
  AnsiString Debug() {return text+" {"+r.left+","+r.top+","+r.right+","+r.bottom+"} <"+from.DateTimeString()+" "+to.DateTimeString()+">";}
  };

// pole urychluje niektore cinnosti s polozkami  
class RurCalendarArray : public RArray<RurCalendarItem>
  {
public:
  int select; ///< Selektnute poradie
  RurCalendarItem *si;

  RurCalendarArray() : RArray<RurCalendarItem>(64,64) {select=-1;si=NULL;}
  int Find(int r,int m,int d);
  int Find(int r,int m,int d,int hodina);
  int FindXY(int x,int y);
  int FindXY2(int x,int y);
  int FindXYPos(int id,int x,int y);
  };

#endif