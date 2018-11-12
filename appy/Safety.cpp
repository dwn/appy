////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
////////////////////////////////////////////////////////////////////////////////
#include "Safety.h"
#include "MoreMath.h"

////////////////////////////////////////////////////////////////////////////////
//NIOSH
////////////////////////////////////////////////////////////////////////////////

int nioshActionLimit(int horiz,int vert,int vertDisp,unsigned char freq,
                     unsigned char workDurIndex) {
  vertDisp=mmAbs(vertDisp);
  if (horiz<(152<<16)) horiz=(152<<16);
  if (vertDisp<(76<<16)) vertDisp=(76<<16);

  #define NIOSH_LC 400
  #define NIOSH_HM ((152<<16)/(horiz>>8))
  #define NIOSH_VM (0x10000-((26*mmAbs(vert-(762<<16)))>>16))
  #define NIOSH_DM (179+((76<<16)/(vertDisp>>8)))
  #define NIOSH_FM (0x100- \
                     ((freq<<8)/ \
                      nioshFrequencyMax[((workDurIndex>0)<<1)+ \
                                         (vert>(762<<16))]))
  #define NIOSH_SM ((NIOSH_VM*((NIOSH_HM*NIOSH_DM*NIOSH_FM)>>10))>>14)

  return ((NIOSH_LC*NIOSH_SM+0x8000)>>16);

  #undef NIOSH_LC
  #undef NIOSH_HM
  #undef NIOSH_VM
  #undef NIOSH_DM
  #undef NIOSH_FM
  #undef NIOSH_SM
}

int nioshRecommendedWeightLimit(int horiz,int vert,int vertDisp,int asymAngle,
                                unsigned char freqIndex,
                                unsigned char workDurIndex,
                                unsigned char coupling) {
  vertDisp=mmAbs(vertDisp);
  while(asymAngle>=256) asymAngle-=1024;
  while(asymAngle<=-256) asymAngle+=1024;
  if (horiz<(254<<16)) horiz=(254<<16);
  if (vertDisp<(46<<16)) vertDisp=(46<<16);

  #define NIOSH_LC 227
  #define NIOSH_HM ((254<<16)/(horiz>>8))
  #define NIOSH_VM (0x10000-(19*mmAbs((vert>>16)-762)))
  #define NIOSH_DM (210+((46<<16)/(vertDisp>>8)))
  #define NIOSH_AM (0x10000-74*mmAbs(asymAngle))
  #define NIOSH_FM nioshFrequencyMultiplier[((3*freqIndex+workDurIndex)<<1)+ \
                                              (vert>=(762<<16))]
  #define NIOSH_CM nioshCouplingMultiplier[(coupling<<1)+(vert>(762<<16))]
  #define NIOSH_PM ((((NIOSH_HM*NIOSH_VM)>>9)* \
                      ((NIOSH_DM*NIOSH_AM)>>9))>>14)
  #define NIOSH_SM ((NIOSH_PM*(NIOSH_FM*NIOSH_CM))>>14)

//printf("%f %f %f %f %f %f \n",NIOSH_HM/256.,NIOSH_VM/65536.,NIOSH_DM/256.,NIOSH_AM/65536.,NIOSH_FM/128.,NIOSH_CM/128.);
  return ((NIOSH_LC*NIOSH_SM+0x8000)>>16);

  #undef NIOSH_LC
  #undef NIOSH_HM
  #undef NIOSH_VM
  #undef NIOSH_DM
  #undef NIOSH_AM
  #undef NIOSH_FM
  #undef NIOSH_CM
  #undef NIOSH_PM
}

float nioshCarryingCapacity(float baseWeight,float vert,float travelDist,
                            unsigned char freqIndex,float age,
                            float bodyWeight,float taskDur) {
  return 0;
}

float nioshPushingCapacity(float baseForce,float vert,float travelDist,
                           unsigned char freqIndex,float age,float bodyWeight,
                           float taskDur) {
  return 0;
}

float nioshPullingCapacity(float baseForce,float vert,float travelDist,
                           unsigned char freqIndex,float age,float bodyWeight,
                           float taskDur) {
  return 0;
}

float nioshLiftingCapacity(float baseWeight,float horiz,float vert,float dist,
                           unsigned char freqIndex,float taskDur,float twist,
                           float coupling,float heatStress,float age,
                           float bodyWeight) {
  return 0;
}

float nioshLoweringCapacity(float baseWeight,float horiz,float vert,
                            unsigned char freqIndex,float age,
                            float bodyWeight,float taskDur) {
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//RULA
////////////////////////////////////////////////////////////////////////////////

unsigned char rulaArmWrist(unsigned char upperArmPos,bool shoulderRaised,
                           bool upperArmAbduction,
                           bool armSupportedOrPersonLeaning,
                           unsigned char lowerArmPos,
                           bool lowerArmAcrossMidline,
                           bool lowerArmOutToSideOfBody,
                           unsigned char wristPos,
                           bool wristBentFromMidline,
                           bool wristTwist) {
  return rulaArmWristTable[
    ((((3*(upperArmPos+shoulderRaised+upperArmAbduction-
           armSupportedOrPersonLeaning-1)+
           (lowerArmPos+lowerArmAcrossMidline+lowerArmOutToSideOfBody-1))<<2)+
           (wristPos+wristBentFromMidline-1))<<1)+(wristTwist-1)];
}

unsigned char rulaNeckTrunkLeg(unsigned char neckPos,bool neckTwist,
                               bool neckSideBend,unsigned char trunkPos,
                               bool trunkTwist,bool trunkSideBend,
                               bool legUnsupported) {
  return rulaNeckTrunkLegTable[((6*(neckPos+neckTwist+neckSideBend-1)+
                                (trunkPos+trunkTwist+trunkSideBend-1))<<1)+
                                legUnsupported];
}

unsigned char rulaCombined(unsigned char armWristPosture,
                           bool armWristMuscleUse,unsigned char armWristLoad,
                           unsigned char neckTrunkLegPosture,
                           bool neckTrunkLegMuscleUse,
                           unsigned char neckTrunkLegLoad) {
  unsigned char a=armWristPosture+armWristMuscleUse+armWristLoad-1;
  unsigned char b=neckTrunkLegPosture+neckTrunkLegMuscleUse+neckTrunkLegLoad-1;
  if (a>6) a=7;
  if (b>6) b=6;
  return rulaCombinedTable[7*a+b];
}

////////////////////////////////////////////////////////////////////////////////
//Snook-Ciriello
////////////////////////////////////////////////////////////////////////////////

unsigned char snookEval(unsigned char taskIndex,bool gender,int load,
                        int posWorkBegin,int posWorkEnd,int handDist,
                        int period,int travelDist) {
  taskIndex=((!taskIndex?
                (gender?
                   (posWorkEnd>=787)+(posWorkEnd>=1448) :
                   (posWorkEnd>=711)+(posWorkEnd>=1346)) :
                (taskIndex==1?
                   (3+(gender?
                         (posWorkBegin>=787)+(posWorkBegin>=1448) :
                         (posWorkBegin>=711)+(posWorkBegin>=1346))) :
                   taskIndex+4))<<1)+gender;
  int dPosWork=(taskIndex<12? mmAbs(posWorkEnd-posWorkBegin) : posWorkBegin);
  int loadCategory[36],dPosWorkCategory[3],distCategory[3],periodCategory[5];
  int *loadCategoryTmp=loadCategory,*dPosWorkCategoryTmp=dPosWorkCategory,
      *distCategoryTmp=distCategory,*periodCategoryTmp=periodCategory;

  #define SNOOK_LOADCATEGORY(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r) \
    *loadCategoryTmp++=(r); *loadCategoryTmp++=(q); *loadCategoryTmp++=(p); \
    *loadCategoryTmp++=(o); *loadCategoryTmp++=(n); *loadCategoryTmp++=(m); \
    *loadCategoryTmp++=(l); *loadCategoryTmp++=(k); *loadCategoryTmp++=(j); \
    *loadCategoryTmp++=(i); *loadCategoryTmp++=(h); *loadCategoryTmp++=(g); \
    *loadCategoryTmp++=(f); *loadCategoryTmp++=(e); *loadCategoryTmp++=(d); \
    *loadCategoryTmp++=(c); *loadCategoryTmp++=(b); *loadCategoryTmp++=(a);

  #define SNOOK_DPOSWORKCATEGORY(a,b,c) \
    *dPosWorkCategoryTmp++=(c); *dPosWorkCategoryTmp++=(b); \
    *dPosWorkCategoryTmp++=(a);

  #define SNOOK_DISTCATEGORY(a,b,c) \
    *distCategoryTmp++=(c); *distCategoryTmp++=(b); *distCategoryTmp++=(a);

  switch(taskIndex) {
    case 0:
      SNOOK_LOADCATEGORY(289,276,262,249,236,222,209,196,182,169,156,142,129,
                          116,102,89,76,62);
    break; case 1: case 3:
      SNOOK_LOADCATEGORY(427,409,391,374,356,338,320,302,285,267,249,231,214,
                          196,178,160,142,125);
    break; case 2:
      SNOOK_LOADCATEGORY(262,249,236,222,209,196,182,169,156,142,129,116,102,
                          89,76,62,49,36);
    break; case 4:
      SNOOK_LOADCATEGORY(178,169,160,151,142,133,125,116,107,98,89,80,71,62,53,
                          44,36,27);
    break; case 5:
      SNOOK_LOADCATEGORY(343,329,316,302,289,276,262,249,236,222,209,196,182,
                          169,156,142,129,116);
    break; case 6: case 8: case 10:
      SNOOK_LOADCATEGORY(227,214,200,191,182,173,165,156,147,138,129,120,111,
                          102,93,80,67,53);
    break; case 7:
      SNOOK_LOADCATEGORY(387,374,360,347,334,320,307,294,280,267,254,240,227,
                          214,200,187,173,160);
    break; case 9:
      SNOOK_LOADCATEGORY(334,320,307,294,280,267,254,240,227,214,200,187,173,
                          160,147,133,120,107);
    break; case 11:
      SNOOK_LOADCATEGORY(325,311,298,285,271,258,245,231,218,205,191,178,165,
                          151,138,125,111,98);
    break; case 12:
      SNOOK_LOADCATEGORY(356,338,320,302,285,267,249,231,214,196,178,160,142,
                          125,107,89,71,53);
    break; case 13: case 15:
      SNOOK_LOADCATEGORY(467,445,423,400,378,356,334,311,289,267,245,222,200,
                          178,156,133,111,89);
    break; case 14:
      SNOOK_LOADCATEGORY(338,320,302,285,267,249,231,214,196,178,160,142,125,
                          107,89,71,53,36);
    break; case 16: case 17: case 18: case 19:
      SNOOK_LOADCATEGORY(338,325,311,298,285,271,258,245,231,218,205,191,178,
                          165,151,138,125,111);
      SNOOK_LOADCATEGORY(578,565,552,538,525,512,498,485,472,458,445,431,418,
                          405,391,378,365,351);
    break; case 20:
      SNOOK_LOADCATEGORY(0,325,311,298,285,271,258,245,231,218,205,191,178,165,
                          151,138,125,111);
    break; case 21:
      SNOOK_LOADCATEGORY(0,440,418,396,378,360,343,325,307,289,271,254,236,218,
                          200,182,160,138);
  }
  switch(taskIndex) {
    case 0:
      SNOOK_DPOSWORKCATEGORY(711,508,254);
    break; case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
           case 10:  case 11:
      SNOOK_DPOSWORKCATEGORY(762,508,254);
    break; case 12: case 14: case 16: case 18:
      SNOOK_DPOSWORKCATEGORY(1346,889,559);
    break; case 13: case 15: case 17: case 19:
      SNOOK_DPOSWORKCATEGORY(1448,940,635);
    break; case 20:
      SNOOK_DPOSWORKCATEGORY(0,1016,787);
    break; case 21:
      SNOOK_DPOSWORKCATEGORY(0,1092,838);
  }
  switch(taskIndex) {
    case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
    case 9: case 10:  case 11:
      SNOOK_DISTCATEGORY(381,254,178);
    break; case 12: case 13: case 14: case 15:
      SNOOK_DISTCATEGORY(15240,7620,2134);
    break; case 20: case 21:
      SNOOK_DISTCATEGORY(8534,4267,2134);
  }
  if (taskIndex<12||taskIndex>=20) {
    *periodCategoryTmp++=15;
    *periodCategoryTmp++=30;
    *periodCategoryTmp++=60;
    *periodCategoryTmp++=300;
  }
  else {
    *periodCategoryTmp++=30;
    *periodCategoryTmp++=60;
    *periodCategoryTmp++=300;
    *periodCategoryTmp++=1800;
  }
  *periodCategoryTmp++=28800;

  #undef SNOOK_LOADCATEGORY
  #undef SNOOK_DPOSWORKCATEGORY
  #undef SNOOK_DISTCATEGORY

  //Set loadIndex
  loadCategoryTmp-=18*(1+(taskIndex>=16&&taskIndex<20))-1;
  unsigned char loadIndex=0;
  for(int i=1;i<18*(1+(taskIndex>=16&&taskIndex<20))-(1+(taskIndex>=20));i++) {
    loadIndex+=(load>=*loadCategoryTmp++);
  }
  //Set dPosWorkIndex
  unsigned char dPosWorkIndex=
    (taskIndex<12?
       (dPosWork>=508) :
       (taskIndex<20? (gender? (dPosWork>=940) : (dPosWork>=889)) : 0));
  //Set distIndex
  int dist=(taskIndex<12? handDist : travelDist);
  unsigned char distIndex=(dist>=distCategory[1]);
  //Set periodIndex
  periodCategoryTmp-=4;
  unsigned char periodIndex=0;
  for(int i=1;i<4;i++) {
    periodIndex+=(period>=*periodCategoryTmp++);
  }
  //Set tableIndex
  int valCurr[16];
  for(int k=0;k<2;k++) {
    for(int j=0;j<2;j++) {
      for(int i=0;i<2;i++) {
        if (taskIndex<16||taskIndex>=20) {
          if (taskIndex<16) {
            for(int h=0;h<2;h++) {
              valCurr[h+((i+((j+(k<<1))<<1))<<1)]=snookTable[
                (3*5*18*3)*(taskIndex+1)+
                ((periodIndex+h)+5*((distIndex+i)-3*((dPosWorkIndex+j)+
                1+3*(loadIndex+k))))];
            }
          }
          else {
            for(int h=0;h<2;h++) {
              valCurr[h+((i+((j+(k<<1))<<1))<<1)]=snookTable[
                (3*5*18*3*16+5*36*3*4)+(3*5*17*2)*(taskIndex-20+1)+
                ((periodIndex+h)+5*((distIndex+i)-3*((dPosWorkIndex+j)+1+
                ((loadIndex+k)<<1))))];
            }
          }
        }
        else {
          valCurr[i+((j+(k<<1))<<1)]=snookTable[
            (3*5*18*3*16)+(5*36*3)*(taskIndex-16+1)+
            ((periodIndex+i)-5*((dPosWorkIndex+j)+1+3*(loadIndex+k)))];
        }
      }
    }
  }
  //Interpolation
  int tInterp=(((period-periodCategory[periodIndex])<<16)/
               (periodCategory[periodIndex+1]-periodCategory[periodIndex]));
  for(int i=0;i<(4<<(taskIndex<16||taskIndex>=20));i++) {
    valCurr[i]=(valCurr[i<<1]<<16)+
               (tInterp*(valCurr[(i<<1)+1]-valCurr[i<<1]));
  }
  if (taskIndex<16||taskIndex>=20) {
    tInterp=(((dist-distCategory[distIndex])<<8)/
             (distCategory[distIndex+1]-distCategory[distIndex]));
    for(int i=0;i<4;i++) {
      valCurr[i]=valCurr[i<<1]+
                 (tInterp*(((valCurr[(i<<1)+1]-valCurr[i<<1]))>>8));
    }
  }
  tInterp=(((dPosWork-dPosWorkCategory[dPosWorkIndex])<<8)/
           (dPosWorkCategory[dPosWorkIndex+1]-dPosWorkCategory[dPosWorkIndex]));
  for(int i=0;i<2;i++) {
    valCurr[i]=valCurr[i<<1]+
               (tInterp*(((valCurr[(i<<1)+1]-valCurr[i<<1]))>>8));
  }
  tInterp=(((load-loadCategory[loadIndex])<<8)/
           (loadCategory[loadIndex+1]-loadCategory[loadIndex]));
  return (valCurr[0]+(tInterp*((valCurr[1]-valCurr[0])>>8))+0x4000)>>15;
}
