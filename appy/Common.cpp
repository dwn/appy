////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
////////////////////////////////////////////////////////////////////////////////
#include "Common.h"
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>

static unsigned int _commonRandomCounter=0;
static unsigned char _commonRandomOld=0;
static int _commonDate[6];

unsigned char commonGetRed(unsigned short* pixel) {
  return (*pixel&0xf800)>>8;
}

unsigned char commonGetGreen(unsigned short* pixel) {
  return (*pixel&0x7e0)>>3;
}

unsigned char commonGetBlue(unsigned short* pixel) {
  return (*pixel&0x1f)<<3;
}

void commonGetRGB(unsigned char* rgb,unsigned short* pixel) {
  rgb[0]=(*pixel&0xf800)>>8;
  rgb[1]=(*pixel&0x7e0)>>3;
  rgb[2]=(*pixel&0x1f)<<3;
}

void commonSetPixel(unsigned short* pixel,unsigned char r,unsigned char g,
                    unsigned char b) {
  *pixel=((r>>3)<<11)|((g>>2)<<5)|(b>>3);
}

void commonSetScanline(unsigned short* pixel,const unsigned int len,
                       const unsigned short colorPacked) {
  for(unsigned int i=0;i<len;i++) pixel[i]=colorPacked;
}

void commonSetZPixel(unsigned short* pixel,unsigned short* zInvBuff,
                     unsigned short zInv,unsigned char r,unsigned char g,
                     unsigned char b) {
  if (zInv>=*zInvBuff) *pixel=((r>>3)<<11)|((g>>2)<<5)|(b>>3);
}

unsigned short commonPackRGB(unsigned char r,unsigned char g,unsigned char b) {
  return ((r>>3)<<11)|((g>>2)<<5)|(b>>3);
}

void commonRandomCounterClear() {
  _commonRandomCounter=_commonRandomOld=0;
}

void commonRandomCounterSeed(unsigned int seed) {
  _commonRandomCounter=seed;
}

bool commonRandomBool() {
  _commonRandomCounter++;
  _commonRandomOld=commonPermute256[(unsigned char)
    ((_commonRandomCounter>>8)^_commonRandomOld)];
  return _commonRandomOld&1;
}

unsigned char commonRandomUchar() {
  _commonRandomCounter++;
  return _commonRandomOld=commonPermute256[(unsigned char)
           ((_commonRandomCounter>>8)^_commonRandomOld)];
}

unsigned short commonRandomUshort() {
  return (commonRandomUchar()<<8)|commonRandomUchar();
}

unsigned char commonNumDaysInMonth(int year,int month) {
  bool oddMonth=month&1;
  bool pastJuly=month>7;
  int numDaysInMonth=31-(pastJuly? oddMonth : !oddMonth);
  if (month==2) { //Leap year determination
    numDaysInMonth--;
    if ((year&3)||!((year%100)||!(year%400))) numDaysInMonth--;
  }
  return numDaysInMonth;
}

//Must be in minimal ranges; e.g. hr=[0..59]
void commonSetDate(int yr,int mo,int day,int hr,int min,int sec) {
  _commonDate[0]=yr;
  _commonDate[1]=mo;
  _commonDate[2]=day;
  _commonDate[3]=hr;
  _commonDate[4]=min;
  _commonDate[5]=sec;
}

//This function is not currently supported under Windows Mobile!
void commonSetDateToPresent() {
#if defined(APP_WINDOWS)&&!defined(APP_WINDOWSDIRECTDRAWMOBILE)
  char b[32];
  _strdate(b); _strtime(b+9);
  b[2]=b[5]=b[8]=b[11]=b[14]=b[17]='\0';
  _commonDate[0]=atoi(b+6)+2000;
  _commonDate[1]=atoi(b);
  _commonDate[2]=atoi(b+3);
  _commonDate[3]=atoi(b+9);
  _commonDate[4]=atoi(b+12);
  _commonDate[5]=atoi(b+15);
#endif
}

//Must be in minimal ranges; e.g. hr=[0..59]
void commonLesserDate(int *result,int *date0,int *date1) {
  bool writeDate0=false;
  for(int i=0;i<5;i++) {
    if (date0[i]<date1[i]) { writeDate0=true; break; }
    else if (date0[i]>date1[i]) { writeDate0=false; break; }
  }
  if (writeDate0) {
    result[0]=date0[0]; result[1]=date0[1]; result[2]=date0[2];
    result[3]=date0[3]; result[4]=date0[4]; result[5]=date0[5];
  }
  else {
    result[0]=date1[0]; result[1]=date1[1]; result[2]=date1[2];
    result[3]=date1[3]; result[4]=date1[4]; result[5]=date1[5];
  }
}

//Must be in minimal ranges; e.g. hr=[0..59]
void commonGreaterDate(int *result,int *date0,int *date1) {
  bool writeDate0=false;
  for(int i=0;i<5;i++) {
    if (date0[i]>date1[i]) { writeDate0=true; break; }
    else if (date0[i]<date1[i]) { writeDate0=false; break; }
  }
  if (writeDate0) {
    result[0]=date0[0]; result[1]=date0[1]; result[2]=date0[2];
    result[3]=date0[3]; result[4]=date0[4]; result[5]=date0[5];
  }
  else {
    result[0]=date1[0]; result[1]=date1[1]; result[2]=date1[2];
    result[3]=date1[3]; result[4]=date1[4]; result[5]=date1[5];
  }
}

void commonSetArgToDate(int *argDate) {
  argDate[0]=_commonDate[0];
  argDate[1]=_commonDate[1];
  argDate[2]=_commonDate[2];
  argDate[3]=_commonDate[3];
  argDate[4]=_commonDate[4];
  argDate[5]=_commonDate[5];
}

//The array addend contains [years,months,days,hours,minutes,seconds]
//Negative values are acceptable for addend
//Minimal ranges are not required; e.g. 10000 days is acceptable input
//Input addend is modified to minimal ranges! E.g. [0 min,60 sec]->[1 min,0 sec]
void commonOffsetDate(int *result,int *addend) {
  result[3]=_commonDate[3]+addend[3];
  result[4]=_commonDate[4]+addend[4];
  result[5]=_commonDate[5]+addend[5];
  result[4]+=result[5]/60; //Check range of seconds
  result[5]%=60;
  if (result[5]<0) {
    result[5]+=60;
    result[4]--;
  }
  result[3]+=result[4]/60; //Check range of minutes
  result[4]%=60;
  if (result[4]<0) {
    result[4]+=60;
    result[3]--;
  }
  result[2]+=result[3]/24; //Check range of hours
  result[3]%=24;
  if (result[3]<0) {
    result[3]+=24;
    result[2]--;
  }
  result[0]=_commonDate[0]+addend[0]; //Year
  result[1]=_commonDate[1]+addend[1]; //Month
  while(result[1]<1) { //Month subtraction check
    result[1]+=12;
    result[0]--;
  }
  while(result[1]>12) { //Month addition check
    result[1]-=12;
    result[0]++;
  }
  int numDaysInMonth,numDays=addend[2]; //Day accumulation to month boundary
  result[2]=_commonDate[2]+addend[2];
  if (result[2]<1) { //Day subtraction till end of previous month
    numDays+=_commonDate[2];
    result[1]--;
    if (result[1]<1) { //Month subtraction check for days
      result[1]+=12;
      result[0]--;
    }
    while(-numDays>=(numDaysInMonth=commonNumDaysInMonth(result[0],result[1]))) {
      result[1]--;
      if (result[1]<1) { //Month subtraction check
        result[1]+=12;
        result[0]--;
      }
      numDays+=numDaysInMonth;
    }
    result[2]=numDaysInMonth+numDays;
  }
  numDaysInMonth=commonNumDaysInMonth(result[0],result[1]);
  if (result[2]>numDaysInMonth) { //Day addition till beginning of next month
    numDays-=commonNumDaysInMonth(result[0],result[1])-_commonDate[2]+1;
    result[1]++;
    if (result[1]>12) { //Month addition check
      result[1]-=12;
      result[0]++;
    }
    while(numDays>=(numDaysInMonth=commonNumDaysInMonth(result[0],result[1]))) {
      result[1]++;
      if (result[1]>12) { //Month addition check
        result[1]-=12;
        result[0]++;
      }
      numDays-=numDaysInMonth;
    }
    result[2]=1+numDays;
  }
  //No year=0, only year in (..,-2,-1,1,2,..)
  if (_commonDate[0]<0&&result[0]>=0) result[0]++;
  if (_commonDate[0]>0&&result[0]<=0) result[0]--;
}

void commonIncrementDay() {
  _commonDate[2]++;
  if (_commonDate[2]>commonNumDaysInMonth(_commonDate[0],_commonDate[1])) {
    _commonDate[1]++;
    _commonDate[2]=1;
  }
  if (_commonDate[1]>12) {
    _commonDate[0]++;
    _commonDate[1]=1;
  }
  //No year=0, only years=[..,-2,-1,1,2,..]
  if (!_commonDate[0]&&_commonDate[1]==1&&_commonDate[2]==1) _commonDate[0]++;
}
