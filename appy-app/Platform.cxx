////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
////////////////////////////////////////////////////////////////////////////////
#include "Platform.h"

Platform::Platform() {
/*
  buff=app->getFrameBuffer();
  offBuff=app->getOffFrameBuffer();
  map=app->getZInvBuffer();
  app->setZInvBufferClear();
  //Draw scene
  int y;
  for(int i=0;i<70;i++) {
    for(int j=0;j<300;j++) {
      agPoint(buff+(480-j)*AG_WIDTH+i,255,255,255);
      *(map+(480-j)*AG_WIDTH+i)=255;
    }
  }
  for(int i=10;i<500;i++) {
    for(int j=300;j<450;j++) {
      agPoint(buff+(480-j)*AG_WIDTH+i,255,255,255);
      *(map+(480-j)*AG_WIDTH+i)=255;
    }
  }
  y=20;
  for(int i=10;i<300;i++) {
    y+=rand()%3-1;
    for(int j=299;j>300-y;j--) {
      agPoint(buff+(480-j)*AG_WIDTH+i,255,255,255);
      *(map+(480-j)*AG_WIDTH+i)=255;
    }
  }
  for(int j=10;j<200;j++) {
    for(int i=0;i<j;i++) {
      agPoint(buff+(480-(j+100))*AG_WIDTH+i,255,255,255);
      *(map+(480-(j+100))*AG_WIDTH+i)=255;
    }
  }
  for(int i=400;i<470;i++) {
    for(int j=200;j<260;j++) {
      agPoint(buff+(480-j)*AG_WIDTH+i,255,255,255);
      *(map+(480-j)*AG_WIDTH+i)=255;
    }
  }/*
  //Initial values
  ch.x=150;
  ch.y=480-50;
  ch.vx=0;
  ch.vy=0;
  ch.ay=3;
  ch.vxAbsMax=14;
  ch.vyTerminal=30;
  ch.vyJumpInitial=-25;
  ch.jumping=false;
  ch.i=0;
  ch.state=5;
  accGrav=2;
  for(int i=0;i<10;i++) {
    ch.u[i]=0-24; ch.l[i]=11-24; ch.r[i]=35-24; ch.d[i]=47-24;
  }
  ch.u[8]=14-24; ch.u[9]=14-24;
  //Read images
  char fileName[32];
  ch.spr[0]=offBuff;
  for(int i=0;i<10;i++) {
    sprintf(fileName,"joe/%d.bmp",i);
    agRead(ch.spr[i],fileName,ch.w+i,ch.h+i,0);
    if (i<9) ch.spr[i+1]=ch.spr[i]+ch.w[i]*ch.h[i];
  }
*/}

Platform::~Platform() {
}

void Platform::update() {
/*
#define platformCollisionScan0(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17) \
  do { \
    if (j p1 0) { \
       p2##p3; \
       j p4 ch.v##p5; \
    } \
    i=0; \
    while(i p6 p7 p8 4 && \
          (coll=(map[(ch.y+ch.vy+ p10 +ch.u[h])*AG_WIDTH+ch.x+ch.vx+ p9 +ch.l[h]]|| \
                 map[(ch.y+ch.vy+ p10 +ch.u[h])*AG_WIDTH+ch.x+ch.vx+ p9 +ch.r[h]]|| \
                 map[(ch.y+ch.vy+ p10 +ch.d[h])*AG_WIDTH+ch.x+ch.vx+ p9 +ch.l[h]]|| \
                 map[(ch.y+ch.vy+ p10 +ch.d[h])*AG_WIDTH+ch.x+ch.vx+ p9 +ch.r[h]]  ))) { \
      i##p11; \
    } \
    p5##p12; \
    j p13 ch.v##p14; \
  } while(p5 p15 -ch.v##p16 && coll); \
  p5##p17; \
  ch.x+=ch.vx+p9; \
  ch.y+=ch.vy+p10;
#define platformCollisionScan1(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19) \
  coll=true; \
  while(p5 p15 -ch.v##p16 && coll) { \
    if (j p1 0) { \
      i=0; \
      while(i p6 p7 p8 4 && \
            (coll=(map[(ch.y+ch.vy+ p10 +ch.u[h])*AG_WIDTH+ch.x+ch.vx+ p9 +ch.l[h]]|| \
                   map[(ch.y+ch.vy+ p10 +ch.u[h])*AG_WIDTH+ch.x+ch.vx+ p9 +ch.r[h]]|| \
                   map[(ch.y+ch.vy+ p10 +ch.d[h])*AG_WIDTH+ch.x+ch.vx+ p9 +ch.l[h]]|| \
                   map[(ch.y+ch.vy+ p10 +ch.d[h])*AG_WIDTH+ch.x+ch.vx+ p9 +ch.r[h]]  ))) { \
        i##p11; \
      } \
      p2##p3; \
      j p4 ch.v##p18; \
    } \
    p18##p12; \
    j p13 ch.v##p14; \
  } \
  p2##p19; \
  p18##p17; \
  ch.x+=ch.vx+p9; \
  ch.y+=ch.vy+p10;

  int h=ch.i+ch.state;
  int x=0;
  int y=0;
  int i=0; //Init 0 needed?
  int j=0;
  int coll; //Should be bool
  if (ch.vx>0) {
    if (ch.vy>=0) {
      if (ch.vx>ch.vy) {
        platformCollisionScan0(<,y,--,+=,x,>,y,-,x,i,--,--,-=,y,>=,x,++);
////////////////////////
//  do {
//    if (j<0) {
//       y--;
//       j+=ch.vx;
//    }
//    i=0;
//    while(i>y-4&&
//          (coll=(map[(ch.y+ch.vy+ i +ch.u[h])*AG_WIDTH+ch.x+ch.vx+ x +ch.l[h]]||
//                 map[(ch.y+ch.vy+ i +ch.u[h])*AG_WIDTH+ch.x+ch.vx+ x +ch.r[h]]||
//                 map[(ch.y+ch.vy+ i +ch.d[h])*AG_WIDTH+ch.x+ch.vx+ x +ch.l[h]]||
//                 map[(ch.y+ch.vy+ i +ch.d[h])*AG_WIDTH+ch.x+ch.vx+ x +ch.r[h]]  ))) {
//      i--;
//    }
//    x--;
//    j-=ch.vy;
//  } while(x>=-ch.vx&&coll);
//  x++;
//  ch.x+=ch.vx+x;
//  ch.y+=ch.vy+i;
///////////////////////
      }
      else {
        platformCollisionScan1(<,x,--,+=,x,>,y,-,x,i,--,--,-=,x,>=,x,++,y,++);
      }
    }
    else {
      if (ch.vx>-ch.vy) {
        platformCollisionScan0(>,y,++,-=,x,<,y,+,x,i,++,--,-=,y,>=,x,++);
      }
      else {
        platformCollisionScan1(>,x,--,+=,x,<,y,+,x,i,++,++,+=,x,>=,x,--,y,++);
      }
    }
  }
  else {
    if (ch.vx<0) {
      if (ch.vy>=0) {
        if (-ch.vx>ch.vy) {
           platformCollisionScan0(>,y,--,+=,x,>,y,-,x,i,--,++,+=,y,<=,x,--);
        }
        else {
          platformCollisionScan1(>,x,++,-=,x,>,y,-,x,i,--,--,-=,x,<=,x,++,y,--);
        }
      }
      else {
        if (ch.vx<ch.vy) {
          platformCollisionScan0(<,y,++,-=,x,<,y,+,x,i,++,++,+=,y,<=,x,--);
        }
        else {
          platformCollisionScan1(<,x,++,-=,x,<,y,+,x,i,++,++,+=,x,<=,x,--,y,--);
        }
      }
    }
    else { //Case ch.vx==0
      if (ch.vy>=0) {
        i=0;
        while(i>-ch.vy-4&&
              (coll=(map[(ch.y+ch.vy+i+ch.u[h])*AG_WIDTH+ch.x+ch.l[h]]||
                     map[(ch.y+ch.vy+i+ch.u[h])*AG_WIDTH+ch.x+ch.r[h]]||
                     map[(ch.y+ch.vy+i+ch.d[h])*AG_WIDTH+ch.x+ch.l[h]]||
                     map[(ch.y+ch.vy+i+ch.d[h])*AG_WIDTH+ch.x+ch.r[h]]  )))
          i--;
        ch.y+=ch.vy+i;
      }
      else {
        i=0;
        while(i<-ch.vy+4&&
              (coll=(map[(ch.y+ch.vy+i+ch.u[h])*AG_WIDTH+ch.x+ch.l[h]]||
                     map[(ch.y+ch.vy+i+ch.u[h])*AG_WIDTH+ch.x+ch.r[h]]||
                     map[(ch.y+ch.vy+i+ch.d[h])*AG_WIDTH+ch.x+ch.l[h]]||
                     map[(ch.y+ch.vy+i+ch.d[h])*AG_WIDTH+ch.x+ch.r[h]]  )))
          i++;
        ch.y+=ch.vy+i;
      }
    }
  }
//  if ((ch.vx<0&&
//       (wld[ch.x+ch.l[h]-1][ch.y+ch.u[h]] ||
//        wld[ch.x+ch.l[h]-1][ch.y+ch.d[h]-14])) ||
//      (ch.vx>0&&
//       (wld[ch.x+ch.r[h]+1][ch.y+ch.u[h]] ||
//        wld[ch.x+ch.r[h]+1][ch.y+ch.d[h]-14]))   ) {
//     ch.vx=0;
//     textprintf(bkg,font,200,200,makecol32(255,rand()%255,rand()%255),"OOOOO");
//  }
  agGreenScreenBlit(buff,offBuff,ch.w[h],ch.h[h],ch.x,ch.y);
//  rect(offscr, ch.x+ch.vx+ch.l[h],
//               ch.y+ch.vy+ch.u[h],
//               ch.x+ch.vx+ch.r[h],
//               ch.y+ch.vy+ch.d[h], makecol32(255,0,0));
//  rect(offscr, ch.x+ch.l[h],
//               ch.y+ch.u[h],
//               ch.x+ch.r[h],
//               ch.y+ch.d[h], makecol32(0,0,255));
/*
  if (map[(ch.y+ch.d[h]+1)*AG_WIDTH+ch.x+ch.l[h]]||
      map[(ch.y+ch.d[h]+1)*AG_WIDTH*ch.x+ch.r[h]]  ) { //On the ground
    ch.vx/=ch.accGrav;
    ch.vx*=ch.accGrav;
    ch.vy=0;
//    textprintf(bkg,font,100,100,makecol32(255,rand()%255,255),"OOOOO");
//    if (k>0) { k--;for(i=0;i<32000;i++) for(j=0;j<3000;j++); } //**********
    if (keypressed()) {
      if (ch.state > 3) ch.state=(ch.state&1)*3;
      if (key[KEY_SPACE]) k=5;//**********
      if (key[KEY_UP]) {
        ch.vy=ch.vyJumpInitial;
        ch.jumping=true;
      }
      if (key[KEY_DOWN]) {
        if (ch.state==0) ch.state=8;
        else ch.state=9;
        ch.i=0;
      }
      else {
        if (key[KEY_LEFT]) {
          ch.vx-=ch.accGrav<<1;
          ch.state=0;
          ch.i++;
          ch.i%=3;
        }
        if (key[KEY_RIGHT]) {
          ch.vx+=ch.accGrav<<1;
          ch.state=3;
          ch.i++;
          ch.i%=3;
	      }
      }
    }
    if (ch.vx>0) ch.vx-=ch.accGrav;
    else if (ch.vx<0) ch.vx+=ch.accGrav;
    if (ch.vx>6*ch.accGrav) ch.vx=6*ch.accGrav;
    else if (ch.vx<6*-ch.accGrav) ch.vx=-6*ch.accGrav;
  }
  else { //In mid-air
    if (keypressed()) {
      if (ch.state<4||ch.state>7) ch.state=6+(ch.state&1);
      ch.i=0;
      if (ch.jumping&&ch.vy<0&&!key[KEY_UP]) {
        ch.jumping=false;
        ch.vy>>=1;
      }
      if (key[KEY_LEFT]) {
         ch.state=6;
         ch.vx-=ch.ax;
      }
      if (key[KEY_RIGHT]) {
         ch.state=7;
         ch.vx+=ch.ax;
      }
      if (key[KEY_DOWN]) {
         if (ch.state==6) ch.state=8;
         else ch.state=9;
         ch.i=0;
      }
      if (key[KEY_ESC]) cont=FALSE;
    }
    if (ch.vx>ch.vxAbsMax) ch.vx=ch.vxAbsMax;
    else if (ch.vx<-ch.vxAbsMax) ch.vx=-ch.vxAbsMax;
  }      
  ch.vy+=ch.accGrav;
  if (ch.vy > ch.vyTerminal) ch.vy=ch.vyTerminal;
  else if (ch.vy < -ch.vyTerminal) ch.vy=-ch.vyTerminal;
#undef platformCollisionScan0
#undef platformCollisionScan1
*/
}
