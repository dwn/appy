////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
//Primary user definitions.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#define APP_PRINTVERSION
#define API //Code placed before API method prototype
#define MAX_STR 1024
// #define APP_CHECKLICENSE
// #define APP_FULLSCREEN
// #define APP_SUPPORTCONTACTSTRING "danwnielsen@gmail.com"
// #define APP_CONTROLLERIPADDRESS "192.168.0.254"
// #define APP_NUMNODES 1
// #define APP_IMAGEDIRECTORY "../res/img/"
// #define APP_MODELDIRECTORY "../res/mdl/"
// #define APP_FONTDIRECTORY "../res/fnt/"
// #define APP_AUDIODIRECTORY "../res/aud/"
// #define APP_TEXTDIRECTORY "../res/txt/"
// #define APP_OUTPUTDIRECTORY "../res/out/"
#define APP_FRAMEWIDTH 640
#define APP_FRAMEHEIGHT 480
#define APP_NUMPIXELS (APP_FRAMEWIDTH*APP_FRAMEHEIGHT)
#define APP_HALFFRAMEWIDTH (APP_FRAMEWIDTH>>1)
#define APP_HALFFRAMEHEIGHT (APP_FRAMEHEIGHT>>1)
#define APP_NUMSOUNDBUFFERS 2
#define APP_SOUNDRATE 44100
//Memory = 2 MB
//FrameBuff    614400 B = 640 width * 480 height * ushort
//OffFrameBuff 614400 B = 640 width * 480 height * ushort
//ZInvBuff     614400 B = 640 width * 480 height * ushort
//SoundBuffTmp  65536 B = 8 kB * 2 upsample factor * 2 channels * short
//ScratchBuff  131072 B = 64 kB * short
//PlayBuffs     57344 B = 8 buffers * 7168 B
#define APP_SOUNDBUFFERSIZE 32768 //Each of the 2 buffers; (power of 2)>=2048
                                  //Possible sound delay during stream play =
                                  //APP_SOUNDBUFFERSIZE / APP_SOUNDRATE seconds
#define APP_SCRATCHBUFFERSIZE 65536 //Any overrun goes into soundBuffTmp
#define APP_PLAYBUFFERSIZE 7168 //Text capacity in bytes for one channel of 8
#define APP_SOUNDEFFECTBUFFERSIZE APP_SOUNDBUFFERSIZE //If you do not mind overwriting scratch buffer: (APP_SOUNDBUFFERSIZE+APP_SCRATCHBUFFERSIZE)
#define APP_SOUNDEFFECTBEEP 0
#define APP_SOUNDEFFECTPOP 1
#define APP_SOUNDEFFECTTHUNDER 2
#define APP_HEADLINERED(x,y) 255
#define APP_HEADLINEGREEN(x,y) (128+((y)<<3))
#define APP_HEADLINEBLUE(x,y) ((y)<<4)
#define APP_TEXTRED(x,y) 200
#define APP_TEXTGREEN(x,y) 128
#define APP_TEXTBLUE(x,y) 128
#define APP_UNDERLINERED(x) 0
#define APP_UNDERLINEGREEN(x) 0
#define APP_UNDERLINEBLUE(x) 0
#define APP_BOLDRED(x,y) 216
#define APP_BOLDGREEN(x,y) 216
#define APP_BOLDBLUE(x,y) 216
#define APP_INPUTLEFT           0x1
#define APP_INPUTRIGHT          0x2
#define APP_INPUTLEFTAFTERRIGHT 0x4
#define APP_INPUTUP             0x8
#define APP_INPUTDOWN          0x10
#define APP_INPUTUPAFTERDOWN   0x20
#define APP_INPUTSPACE         0x40
#define APP_INPUTQ             0x80
#define APP_INPUTA            0x100
#define APP_INPUTD            0x200
#define APP_INPUTAAFTERD      0x400
#define APP_INPUTW            0x800
#define APP_INPUTS           0x1000
#define APP_INPUTWAFTERS     0x2000
#define APP_INPUTZ           0x4000
#define APP_INPUTESC     0x80000000
#define APP_INPUTDEFAULT (APP_INPUTD|APP_INPUTS)