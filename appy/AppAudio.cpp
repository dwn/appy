////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
////////////////////////////////////////////////////////////////////////////////
//...-------------> TotalCounter, since beginning of play
//   |------------> TotalCounter%AA_SIZE, since beginning of this buffer
//   |------------------------------| Current sound buffer, mono 16-bit samples
//          |-----> Counter, since beginning of this sound
//          |-------------| Length, this sound
//                 |-----> Remainder, portion left of this sound
//Sounds are smart and naturally stop filling at the buffer boundary:
//                        |-------->|------->
//                              Counter  Remainder
#include "AppAudio.h"
#include "Common.h"
#include "MoreMath.h"
#include "Noise.h"

static const unsigned short _aaNotePeriodTable12[]={ //12-edo scale
  800,755,713,673,635,599,566,534,504,476,449,424};
static const unsigned short _aaNotePeriodTable19[]={ //19-edo scale
  800,771,744,717,691,667,643,620,598,576,555,536,516,498,480,463,446,430,415};
static const unsigned short _aaNotePeriodTable31[]={ //31-edo scale
  800,782,765,748,732,715,700,684,669,654,840,626,612,598,585,572,
  559,547,535,523,512,500,489,478,468,457,447,437,428,418,409};
static unsigned short _aaDiatonicStep12[]={0,2,3,5,7,8,10};
static unsigned short _aaDiatonicStep19[]={0,3,5,8,11,13,16};
static unsigned short _aaDiatonicStep31[]={0,5,8,13,18,21,26};
//Phoneme parameters [freq0,freq1,bandwidth0,bandwidth1,amp0,amp1,asp,duration]
//Aspiration: how much static/will it jump or slide into next sound
//asp=0   no static; sliding
//asp=1   no static; not sliding
//asp=255 static; not sliding
//TODO: improve phonemes, especially 'r'
static const short aaSpeechTable[] = { //Phonemes (X-SAMPA parenthesized)
    300, 1000,  60,  90, 100,  80,   0,  4000,  //Phoneme ']' (N)
    640, 1190,  60,  90, 100, 100,   1,  6000,  //Phoneme '^' (@)
      0,    0,  60,  90, 100, 100,   0,  8000,  //Phoneme '_' (?)
    690, 1660,  60,  90, 100, 100,   1,  7000,  //Phoneme '`' ({)
    425, 1900,  60,  90, 100, 100,   1,  7000,  //Phoneme 'a' (e)
     90,  460,  60,  90, 100, 100, 255,  2000,  //Phoneme 'b' (b)
   2220, 2600, 100, 100,  25,  25, 255,  5000,  //Phoneme 'c' (S)
   1390, 2300,  60, 150,  50,  50, 255,   500,  //Phoneme 'd' (d)
    550, 1770,  60,  90, 100, 100,   0,  4500,  //Phoneme 'e' (E)
    280, 1420,  60,  90, 100, 100, 255,  1000,  //Phoneme 'f' (f)
    190,  580,  30,  45, 100, 100, 150,  1000,  //Phoneme 'g' (g)
   1000, 1200, 100,  70,  50,  40, 255,  9000,  //Phoneme 'h' (h)
    500, 1800,  60,  90, 100,  90,   1,  8000,  //Phoneme 'i' (I)
    280, 1420,  60,  90, 100, 100, 255,  1000,  //Phoneme 'j' (T)
    190, 1480,  30,  45, 100, 100, 255,  1000,  //Phoneme 'k' (k)
    460, 1080,  60,  90, 110, 110,   1,  8000,  //Phoneme 'l' (l)
    340, 1000,  60,  90, 100, 100,   1, 10000,  //Phoneme 'm' (m)
   1080, 1380,  60,  90, 100,  80,   1,  3150,  //Phoneme 'n' (n)
    570, 1080,  60,  90, 100, 100,   0,  9000,  //Phoneme 'o' (O)
    390,  760,  60,  90, 100, 100, 255,  1000,  //Phoneme 'p' (p)
    710, 1100,  60,  90, 100, 100,   1,  5000,  //Phoneme 'q' (A)
    590, 1280,  60,  90, 100, 100,  50,  5000,  //Phoneme 'r' (r\)
   3000, 3000,  60,  90,  50,  50,  80,  7000,  //Phoneme 's' (s)
    190, 2680,  60, 150, 100, 100, 255,   500,  //Phoneme 't' (t)
    450, 1030,  60,  90, 100, 100,   0,  5000,  //Phoneme 'u' (U)
    280, 1000,  60,  90, 110, 110,   0,  7000,  //Phoneme 'v' (v)
    400,  800,  60,  90, 100, 100,   0,  7000,  //Phoneme 'w' (u)
   2220, 3000,  60,  90,  70,  70,  60,  5000,  //Phoneme 'x' (Z)
    380, 2035,  60,  90, 100, 100,   1,  9000,  //Phoneme 'y' (i)
   1750, 3000,  60,  90,  70, 100,  60,  7000}; //Phoneme 'z' (z)
static unsigned short *_aaNotePeriodTable=(unsigned short*)_aaNotePeriodTable12;
static unsigned short *_aaDiatonicStep=(unsigned short*)_aaDiatonicStep12;
static int _aaTotalCounter[]={0,0,0,0,0,0,0,0};
static int _aaCounter[]={0,0,0,0,0,0,0,0};
static int _aaRemainder[]={0,0,0,0,0,0,0,0};
static unsigned char _aaIndex[]={0,0,0,0,0,0,0,0};
static int _aaPeriod[8]={0,0,0,0,0,0,0,0};
static unsigned char _aaNoteDuration[]={4,4,4,4,4,4,4,4};
static unsigned char _aaNoteOctave[]={4,4,4,4,4,4,4,4};
static char _aaNoteStyle[]={'.','.','.','.','.','.','.','.'};
static unsigned char _aaInstrument[]={0,0,0,0,0,0,0,0};
//Global speech variables [fm0,fm1,bw0,bw1,am0,am1,aspiration,saw]
static int _aaSpeechParam[][8]={
  {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}};
//Speech bandpass variables [b,c,y,z0,z1] for 2 filters
static int _aaSpeechBP[8][2][5];
static short *_aaV;
static int _aaBirdY0,_aaBirdY1,_aaBirdY2,
           _aaBirdMultiplier0=0x7fffffff,_aaBirdMultiplier1;
static Noise _aaNoise;

#define AA_LOOP \
  int sound; \
  if (!_aaCounter[track]) _aaRemainder[track]=length; \
  while(_aaRemainder[track]) {
#define AA_END \
    data[_aaTotalCounter[track]%AA_SIZE]=sound; \
    _aaTotalCounter[track]++; _aaCounter[track]++; _aaRemainder[track]--; \
    if (!_aaRemainder[track]) _aaCounter[track]=0; \
    if (!(_aaTotalCounter[track]%AA_SIZE)) break; \
  }
#define AA_RETURN return _aaRemainder[track];
#define aaBufferFull(totalCounterOld) \
  (!(_aaTotalCounter[track]%AA_SIZE)&&totalCounterOld!=_aaTotalCounter[track])

////////////////////////////////////////////////////////////////////////////////
//Common
////////////////////////////////////////////////////////////////////////////////

//Input track number, or -1 to reset all tracks
void aaReset(unsigned char track) {
  unsigned char trackFinal;
  if (track<0) { track=0; trackFinal=7; } else trackFinal=track;
  for(;track<=trackFinal;track++) {
    _aaIndex[track]=0;
  }
}

void aaResetSpeechParameters(unsigned char track) {
  unsigned char trackFinal;
  if (track<0) { track=0; trackFinal=7; } else trackFinal=track;
  for(;track<=trackFinal;track++) {
    for(int i=0;i<8;i++) _aaSpeechParam[track][i]=aaSpeechTable[16+i];
    for(int j=0;j<2;j++) for(int i=0;i<5;i++) _aaSpeechBP[track][j][i]=0;
  }
}

void aaUpsampleAndMakeStereo(short* dataSrc,const unsigned int lengthBefore,
                             short* dataDst) {
  const short* dataSrco=dataSrc;
  short *data=(dataDst? dataDst : dataSrc/*in-place*/)+((lengthBefore-1)<<2);
  const int valAtEdge=*(dataSrc+=lengthBefore-1);
  data[0]=data[2]=valAtEdge; //Stereo; no average at boundary
  data[1]=data[3]=valAtEdge;
  do { //Double and antialias datapoints, assuming lengthBefore>1
    data-=4;
    dataSrc--;
    const short val=*dataSrc,avg=(val+dataSrc[1])>>1;
    data[0]=val;
    data[1]=val;
    data[2]=avg;
    data[3]=avg;
  } while(dataSrc-dataSrco);
}

////////////////////////////////////////////////////////////////////////////////
//Pass filter
////////////////////////////////////////////////////////////////////////////////

void aaLowPass(short* data,int freqCut,int totalCounterBegin) {
  const unsigned int dt=(0x80000000/AA_SOUNDSAMPLERATE);
  const unsigned int alpha=dt/((0x80000000/((MM_PI>>7)*freqCut))+(dt>>8));
  for(int i=totalCounterBegin+1;i<AA_SIZE;i++) {
    data[i]=(data[i-1]+alpha*(data[i]-data[i-1]))>>8;
  }
}

void aaHighPass(short* data,int freqCut,int totalCounterBegin) {
  const unsigned int rc=0x80000000/((MM_PI>>7)*freqCut);
  const unsigned int alpha=(rc<<8)/((rc<<1)+(0x1000000/AA_SOUNDSAMPLERATE));
  int dataOld=data[totalCounterBegin],tmp;
  for(int i=totalCounterBegin+1;i<AA_SIZE;i++) {
    tmp=dataOld;
    dataOld=data[i];
    data[i]=(alpha*(data[i-1]+(data[i]-tmp)))>>7;
  }
}

void aaSetBandPass(int* b,int* c,int f,int bw) {
  const int r=mmExp2((-(MM_PI>>2)*(bw>>8)/AA_SPEECHSAMPLERATE)>>6);
  *c=-(((r>>1)*(r>>1))>>14);
  *b=(((r>>1)*(mmCos((f/AA_SPEECHSAMPLERATE)>>(16-10))>>1))>>13);
}

//Bandpass filter 12-dB 2-pole biquad for a single sample value
//Use aaSetBandPass() first!
void aaBandPassSample(int b,int c,int* y,int* z0,int* z1,int sample) {
  *y=(((((0x10000-b-c)>>6)*(sample>>6))>>4)+
      (((b>>2)*(*z0>>8))>>6)+
      (((c>>2)*(*z1>>8))>>6));
  *z1=*z0;
  *z0=*y;
}

////////////////////////////////////////////////////////////////////////////////
//Basic oscillator
////////////////////////////////////////////////////////////////////////////////

int aaSawtooth(unsigned char track,short* data,int length,
               int freq) {
  AA_LOOP;
  sound=(_aaCounter[track]*freq)&0x3ff;
  AA_END;
  AA_RETURN;
}

int aaSine(unsigned char track,short* data,int length,int freq) {
  AA_LOOP;
  sound=mmSin(_aaCounter[track]*freq)>>5;
  AA_END;
  AA_RETURN;
}

int aaSquare(unsigned char track,short* data,int length,
             int period) {
  AA_LOOP;
  sound=(_aaCounter[track]%period>(period>>1)? 1024 : 0);
  AA_END;
  AA_RETURN;
}

int aaTriangle(unsigned char track,short* data,int length,
               int period) {
  int moduloCounter,halfPeriod=period>>1;
  AA_LOOP;
  moduloCounter=_aaCounter[track]%period;
  sound=(moduloCounter>halfPeriod?
         4096-(((moduloCounter-halfPeriod)<<12)/halfPeriod) :
         (moduloCounter<<12)/halfPeriod);
  AA_END;
  AA_RETURN;
}

int aaSilence(unsigned char track,short* data,int length) {
  AA_LOOP;
  sound=0;
  AA_END;
  AA_RETURN;
}

////////////////////////////////////////////////////////////////////////////////
//Special effect
////////////////////////////////////////////////////////////////////////////////

int aaBird(unsigned char track,short* data,int length) {
#define AA_BIRDMULTIPLIEREND (2040-640)
  AA_LOOP;
  if (_aaBirdMultiplier0>=AA_BIRDMULTIPLIEREND&&
      (_aaCounter[track]&0x3fff)==(commonRandomUshort()&0x3fff)) {
    _aaBirdY1=0x10000;
    _aaBirdY0=_aaBirdY2=0;
    _aaBirdMultiplier0=(AA_BIRDMULTIPLIEREND-(128+64))+
                       (commonRandomUchar()&0x3f);
    _aaBirdMultiplier1=1023-commonRandomBool();
  }
  _aaBirdY0=((_aaBirdMultiplier0*_aaBirdY1)>>10)-
            ((_aaBirdMultiplier1*_aaBirdY2)>>10);
  _aaBirdY2=_aaBirdY1;
  _aaBirdY1=_aaBirdY0;
  sound=_aaBirdY0>>7;
  _aaBirdMultiplier0+=
    (!(_aaCounter[track]&0xf)&&_aaBirdMultiplier0<AA_BIRDMULTIPLIEREND? 1 : 0);
  AA_END;
  AA_RETURN;
#undef AA_BIRDMULTIPLIEREND
}

//Update noise between calls to this function
int aaRain(unsigned char track,short* data,int length,Noise* noise) {
  int t,tmp,ns;
  AA_LOOP;
  t=_aaTotalCounter[track];
  if (!(t&0x1ff)) noise->update();
  tmp=ns=noise->get1D(t<<4,0xff);
  tmp>>=5; tmp*=tmp;
  tmp>>=5; tmp*=tmp;
  sound=(mmSin((160+(ns>>3))*t)*tmp)>>13;
  AA_END;
  AA_RETURN;
}

int aaNoise(unsigned char track,short* data,int length,int freq,
            Noise* noise) {
  AA_LOOP;
  if (((_aaCounter[track]*freq)&0xffff)<freq) noise->update();
  sound=noise->get1D(_aaCounter[track]*freq,0xff)<<3;
  AA_END;
  AA_RETURN;
}

int aaPluck(unsigned char track,short* data,short* scratch,int length,
            int period) {
  if (!_aaCounter[track]) {
    for(int i=0;i<period;i++) {
      scratch[i]=(mmSin(i<<3)>>3)+((commonRandomUchar()-128)<<3);
    }
  }
  int k;
  AA_LOOP;
  k=_aaCounter[track]%period;
  sound=scratch[k]=(scratch[k]+(scratch[(k+period-1)%period])-1)>>1;
  AA_END;
  AA_RETURN;
}

int aaSpeechPhoneme(unsigned char track,short* data,int f0,int f1,int b0,
                    int b1,int a0,int a1,int asp,int length,int freq) {
  AA_LOOP;
  //sound=10-(asp>>3); sound=(sound<0? 0 : sound);
  sound=(asp? 0 : 9);
  _aaSpeechParam[track][0]+=(f0-_aaSpeechParam[track][0])>>sound;
  _aaSpeechParam[track][1]+=(f1-_aaSpeechParam[track][1])>>sound;
  _aaSpeechParam[track][2]+=(b0-_aaSpeechParam[track][2])>>sound;
  _aaSpeechParam[track][3]+=(b1-_aaSpeechParam[track][3])>>sound;
  _aaSpeechParam[track][4]+=(a0-_aaSpeechParam[track][4])>>sound;
  _aaSpeechParam[track][5]+=(a1-_aaSpeechParam[track][5])>>sound;
  _aaSpeechParam[track][6]+=(asp-_aaSpeechParam[track][6])>>sound;
  aaSetBandPass(_aaSpeechBP[track][0],_aaSpeechBP[track][0]+1,
                _aaSpeechParam[track][0],_aaSpeechParam[track][2]);
  aaSetBandPass(_aaSpeechBP[track][1],_aaSpeechBP[track][1]+1,
                _aaSpeechParam[track][1],_aaSpeechParam[track][3]);
  _aaSpeechParam[track][7]+=(freq/AA_SPEECHSAMPLERATE); //Sawtooth
  _aaSpeechParam[track][7]&=0xffff;
  sound=(((0x10000-(_aaSpeechParam[track][6]>>8))>>1)* //Non-aspirated
         ((_aaSpeechParam[track][7]-0x8000)>>1)+
         ((_aaSpeechParam[track][6]>>8)>>1)* //Aspirated
         ((commonRandomUshort()-0x8000)>>1))>>14;
  aaBandPassSample(_aaSpeechBP[track][0][0],_aaSpeechBP[track][0][1],
                   _aaSpeechBP[track][0]+2,_aaSpeechBP[track][0]+3,
                   _aaSpeechBP[track][0]+4,
                   ((sound>>3)*(_aaSpeechParam[track][4]>>(8+3)))>>2);
  aaBandPassSample(_aaSpeechBP[track][1][0],_aaSpeechBP[track][1][1],
                   _aaSpeechBP[track][1]+2,_aaSpeechBP[track][1]+3,
                   _aaSpeechBP[track][1]+4,
                   ((sound>>3)*(_aaSpeechParam[track][5]>>(8+3)))>>2);
  sound=(_aaSpeechBP[track][0][2]+_aaSpeechBP[track][1][2])>>(8+4);
  AA_END;
//  aaLowPass(data,2000,_aaTotalCounter[track]); //TODO: CHECK THIS
  AA_RETURN;
}

//Returns true when end of input string is reached
bool aaSpeech(unsigned char track,const char* text,short* data) {
  int tmp=_aaTotalCounter[track];
  //Play residual sound that may still need playing
  if (_aaRemainder[track]) {
    aaSpeechPhoneme(track,data,_aaV[0]<<16,_aaV[1]<<16,_aaV[2]<<16,_aaV[3]<<16,
                    _aaV[4]<<16,_aaV[5]<<16,_aaV[6]<<16,
                    _aaV[7]/(44100/AA_SPEECHSAMPLERATE),91<<16);
  }
  char chr=text[_aaIndex[track]];
  for(;;) {
    if (!aaBufferFull(tmp)) { //Read new phoneme
      if (chr=='\0'||chr=='S'||chr=='M') return true; //End of string; 'S' means speech string next; 'M' means music string next
      if (chr=='_') aaResetSpeechParameters(track);
      _aaV=(short*)(aaSpeechTable+((chr-0x5d)<<3));
      chr=text[++_aaIndex[track]];
      //Play phoneme
      aaSpeechPhoneme(track,data,_aaV[0]<<16,_aaV[1]<<16,_aaV[2]<<16,
                      _aaV[3]<<16,_aaV[4]<<16,_aaV[5]<<16,_aaV[6]<<16,
                      _aaV[7]/(44100/AA_SPEECHSAMPLERATE),91<<16);
    }
    if (aaBufferFull(tmp)) return false;
  }
}

////////////////////////////////////////////////////////////////////////////////
//Music
////////////////////////////////////////////////////////////////////////////////

void aaMusicNote(unsigned char track,short* data,short* scratch) {
  int duration=AA_MUSICDURATIONMULTIPLIER*_aaNoteDuration[track];
  if (_aaPeriod[track]==-1) {
    aaSilence(track,data,duration);
  }
  else {
    switch(_aaInstrument[track]) {
      case 0:
        aaPluck(track,data,scratch,duration,
                _aaPeriod[track]>>_aaNoteOctave[track]);
      break; case 1:
        aaSine(track,data,duration,
               0x400/(_aaPeriod[track]>>_aaNoteOctave[track]));
      break; case 2:
        aaTriangle(track,data,duration,
                   (_aaPeriod[track]>>_aaNoteOctave[track]));
      break; case 3:
        aaSawtooth(track,data,duration,
                   0x41a/(_aaPeriod[track]>>_aaNoteOctave[track]));
      break; case 4:
        aaSquare(track,data,duration,
                 _aaPeriod[track]>>_aaNoteOctave[track]);
      break; case 5:
        aaSpeechPhoneme(track,data,710<<16,1100<<16,60<<16,90<<16,100<<16,
                        100<<16,1,duration,
                        0x55555555/(_aaPeriod[track]>>_aaNoteOctave[track]));
      break; case 6:
        aaBird(track,data,duration);
      break; case 7:
        aaRain(track,data,duration,&_aaNoise);
    }
  }
}

//Returns true when end of input string is reached
bool aaMusic(unsigned char track,const char* text,short* data,short* scratch) {
  int tmp=_aaTotalCounter[track];
  //Play residual sound that may still need playing
  if (_aaRemainder[track]) aaMusicNote(track,data,scratch);
  char chr=text[_aaIndex[track]];
  for(;;) {
    if (!aaBufferFull(tmp)) { //Read new note
      switch(chr) {
        case '*': //Rest
          _aaPeriod[track]=-1;
        break; case '\0': case 'S': case 'M': //End of string; 'S' means speech string next; 'M' means music string next
          _aaIndex[track]=0;
          return true;
        default: //Notes 'a' through 'g', and 'A' through 'G'
          _aaPeriod[track]=_aaDiatonicStep[chr-(chr>=0x61? 0x61 : 0x41)];
      }
      chr=text[++_aaIndex[track]];
      if (commonIsDigit(chr)) {
        _aaNoteOctave[track]=0;
        do {
          _aaNoteOctave[track]+=10*_aaNoteOctave[track]+(chr-0x30);
          chr=text[++_aaIndex[track]];
        } while(commonIsDigit(chr));
      }
      if (chr!='\0'&&chr!='S'&&chr!='M') {
        switch(chr) { //Accidentals: sharp #, flat -, double sharp x, double flat =
          case 'x': _aaPeriod[track]++; case '#': _aaPeriod[track]++;
          chr=text[++_aaIndex[track]];
          break;
          case '=': _aaPeriod[track]--; case '-': _aaPeriod[track]--;
          chr=text[++_aaIndex[track]];
        }
      }
      //Get pitch period
      _aaPeriod[track]=(_aaPeriod[track]>=0?
                        _aaNotePeriodTable[_aaPeriod[track]] : -1);
      //Higher octave for capital letter
      if (commonIsUppercaseLetter(text[_aaIndex[track]-1]))
        _aaPeriod[track]>>=1;
      //Style mark
      if (chr>=0x5b&&chr<=0x60) { //Style marks are [\]^_`
        _aaNoteStyle[track]=chr;
        chr=text[++_aaIndex[track]];
      }
      //Skip period character
      if (chr=='.') chr=text[++_aaIndex[track]];
      //Duration
      if (commonIsDigit(chr)) {
        _aaNoteDuration[track]=0;
        do {
          _aaNoteDuration[track]+=10*_aaNoteDuration[track]+(chr-0x30);
          chr=text[++_aaIndex[track]];
        } while(commonIsDigit(chr));
      }
      //Skip period character
      if (chr=='.') chr=text[++_aaIndex[track]];
      //Instrument
      if (commonIsDigit(chr)) {
        _aaInstrument[track]=0;
        do {
          _aaInstrument[track]+=10*_aaInstrument[track]+(chr-0x30);
          chr=text[++_aaIndex[track]];
        } while(commonIsDigit(chr));
      }
      //Play note
      aaMusicNote(track,data,scratch);
    }
    if (aaBufferFull(tmp)) return false;
  }
}

////////////////////////////////////////////////////////////////////////////////
//Multiplay - accepts a string containing both music and speech
////////////////////////////////////////////////////////////////////////////////

bool aaChompPlayString(unsigned char track,char* text) {
  if (*text=='\0') return true;
  _aaIndex[track]=0; //Reset audio text index
  char *p=text;
  do
    p++;
  while(*p!='M'&&*p!='S'&&*p!='\0');
  while(*p!='\0') { *text=*p; text++; p++; }
  *text='\0';
  return false;
}

bool aaPlay(unsigned char track,char* text,short* data,short* scratch) {
  if (*text=='M') {
    if (aaMusic(track,text+1,data,scratch))
      return aaChompPlayString(track,text);
  }
  else if (*text=='S') {
    if (aaSpeech(track,text+1,data))
      return aaChompPlayString(track,text);
  }
  return false;
}

#undef AA_LOOP
#undef AA_END
#undef AA_RETURN
