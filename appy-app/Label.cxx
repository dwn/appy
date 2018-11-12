////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
////////////////////////////////////////////////////////////////////////////////
#include "Common.h"
#include "App.h"
#include "MoreMath.h"
#include "AppGraph.h"
#include "Noise.h"
#include "dirent.h"

appCaput();

const bool WHITEBMPBKG=1;
const bool DEMOMODE=1;
const bool SLOWMODE=0;
const unsigned int WIND=             0x00000001;
const unsigned int DISTANCETRANSFORM=0x00000002;
const unsigned int NOISE=            0x00000004;
const unsigned int ACTIVEFORCE=      0x00000008;
const unsigned int IDEALLEADERLENGTH=50<<16; //In u16.16 pixels
const unsigned int COLLISIONAREATHRESHOLD=10; //In pixels
const unsigned int NUMLABELS=12;
const unsigned int NUMCURVEPOINTS=8;
const unsigned short LABELSCALE=253; //Normal scale is 256
const int LABELCURVE[][NUMCURVEPOINTS<<1] = { //In u16.16 pixels
  { 30<<16,0,30<<16,30<<16,0,30<<16,-30<<16,30<<16,-30<<16,0,-30<<16,-30<<16,0,-30<<16,30<<16,-30<<16 },
  { -10<<16,10<<16,-14<<16,2<<16,-14<<16,-8<<16,-2<<16,-14<<16,14<<16,-6<<16,14<<16,4<<16,4<<16,6<<16,-12<<16,20<<16 },
  { -36<<16,-20<<16,-36<<16,-24<<16,-16<<16,-40<<16,-12<<16,-40<<16,32<<16,20<<16,32<<16,24<<16,20<<16,36<<16,16<<16,36<<16 },
  { 0,40<<16,-20<<16,16<<16,-8<<16,16<<16,-8<<16,-36<<16,0,-40<<16,8<<16,-36<<16,8<<16,16<<16,20<<16,16<<16 },
  { 4<<16,0,4<<16,-1<<16,20<<16,-12<<16,4<<16,-22<<16,-16<<16,-14<<16,-16<<16,12<<16,4<<16,18<<16,20<<16,10<<16 }};
const unsigned int MARGIN=3; //In pixels
const unsigned int TEMPERATURETHRESHOLD=8192;
const unsigned int NOISEFREQ=1; //Actual frequency will be pow2(NOISEFREQ)
const unsigned char R=100;
const unsigned char G=0;
const unsigned char B=0;
unsigned int state[]={
  WIND,
  WIND|DISTANCETRANSFORM,
  WIND|DISTANCETRANSFORM|NOISE,
  ACTIVEFORCE,
  NOISE|ACTIVEFORCE
};
const unsigned int NUMSTATES=sizeof(state)/sizeof(unsigned int);
const unsigned int NUMLABELCURVES=sizeof(LABELCURVE)/sizeof(LABELCURVE[0]);

struct Label {
  int xo,yo,
      x,y,
      xOld,yOld,
      collision,
      aabb[4],
      p[NUMCURVEPOINTS<<1];
  unsigned int labelType,
               minIndex,minIndexOld,
               leaderLength;
  bool solved,solvedOld,stamped,
       blocked,blockedOld,
	     leaderRelaxed;
};

Label lbl[NUMLABELS];
unsigned short bmp[AG_WIDTH*AG_HEIGHT],bmp2[AG_WIDTH*AG_HEIGHT];
Noise noise;
int numCollisions,
	stateIndex=0;
char imageFileName[256][256]; //Allows for 256 files with names of 256 characters each
unsigned int numImageFiles,imageIndex=-1,
		         timeTot=0;
bool firstRun=true,veryFirstRun=true,veryVeryFirstRun=true,
     solvedAll;

int temperature(unsigned int t) {
  return mmExp(-(int)t);
}

void addWind(unsigned short* buff) {
  for(unsigned int i=0;i<AG_WIDTH*AG_HEIGHT;i++)
    buff[i]+=((i%AG_WIDTH)+(i/AG_WIDTH))<<4;
}

void setActiveForce(unsigned int t,int* dxActive,int* dyActive,bool swapAxes) {
  *dxActive=t*mmCos(t<<5); *dyActive=t*mmSin(t<<5);
  if (swapAxes) { int tmp=*dxActive; *dxActive=*dyActive; *dyActive=tmp; }
}

void resetPosition() {
  for(unsigned int i=0;i<NUMLABELS;i++) {
    if (lbl[i].solved) continue;
	  lbl[i].x=lbl[i].xo; lbl[i].y=lbl[i].yo;
  }
}

void resetBitmaps() {
  agRead(bmp,imageFileName[imageIndex],NULL,NULL,1);
  agFullScreenBlit(bmp2,bmp);
  agFullScreenMonochrome(bmp,WHITEBMPBKG,R,G,B);
  agFullScreenMonochrome(bmp2,WHITEBMPBKG,R,G,B);
}

void resetBitmap2() {
  agRead(bmp2,imageFileName[imageIndex],NULL,NULL,1);
  agFullScreenMonochrome(bmp2,WHITEBMPBKG,R,G,B);
}

void save() {
  agFullScreenSave(app->getFrameBuffer(),"buff.bmp");
  agFullScreenSave(app->getOffFrameBuffer(),"buffOff.bmp");
  agFullScreenSave(bmp,"bmp.bmp");
  agFullScreenSave(bmp2,"bmp2.bmp");
}

void init() {
  imageIndex=(imageIndex+1)%numImageFiles;
  stateIndex=0;
  for(unsigned int i=0;i<NUMLABELS;i++) {
    lbl[i].xo=((commonRandomUshort()%(AG_WIDTH-2))+1)<<16;
	  lbl[i].yo=((commonRandomUshort()%(AG_HEIGHT-2))+1)<<16;
	  lbl[i].labelType=commonRandomUchar()%NUMLABELCURVES;
  }
  for(unsigned int i=0;i<NUMLABELS;i++)
	for(unsigned int j=0;j<NUMCURVEPOINTS<<1;j++)
	  lbl[i].p[j]=(LABELCURVE[lbl[i].labelType][j]*LABELSCALE)>>8;
  for(unsigned int i=0;i<NUMLABELS;i++) {
    lbl[i].aabb[0]=lbl[i].aabb[1]=INT_MAX;
    lbl[i].aabb[2]=lbl[i].aabb[3]=INT_MIN;
	for(unsigned int j=0;j<NUMCURVEPOINTS;j++) {
    lbl[i].aabb[0]=mmMin(lbl[i].p[j<<1],lbl[i].aabb[0]);
    lbl[i].aabb[1]=mmMin(lbl[i].p[(j<<1)+1],lbl[i].aabb[1]);
    lbl[i].aabb[2]=mmMax(lbl[i].p[j<<1],lbl[i].aabb[2]);
    lbl[i].aabb[3]=mmMax(lbl[i].p[(j<<1)+1],lbl[i].aabb[3]);
  }
  lbl[i].collision=INT_MAX;
	lbl[i].stamped=lbl[i].solved=lbl[i].leaderRelaxed=false;
	lbl[i].blockedOld=lbl[i].blocked=false;
	lbl[i].minIndexOld=-1; lbl[i].minIndex=0;
  }
}

void collisionArea(unsigned short* buff,unsigned short* buff2,int x,int y,
                   unsigned char r,unsigned char g,unsigned char b) {
  unsigned short *ptr=buff+y*AG_WIDTH+x;
  if (*ptr==commonPackRGB(r,g,b)) return;
  agPoint(ptr,r,g,b);
  if (buff2[AG_WIDTH*y+x]) numCollisions++;
  if (x>0) collisionArea(buff,buff2,x-1,y,r,g,b);
  if (x<AG_WIDTH-1) collisionArea(buff,buff2,x+1,y,r,g,b);
  if (y>0) collisionArea(buff,buff2,x,y-1,r,g,b);
  if (y<AG_HEIGHT-1) collisionArea(buff,buff2,x,y+1,r,g,b);
}

void rein(unsigned int labelIndex) {
  lbl[labelIndex].leaderRelaxed=(!lbl[labelIndex].minIndex||lbl[labelIndex].blocked)&&(!lbl[labelIndex].minIndexOld||lbl[labelIndex].blockedOld);
  //Adjust leaderline lengths toward IDEALLEADERLENGTH
  int w,h;
  unsigned int distSqr,diff,minDiff=UINT_MAX;
  lbl[labelIndex].minIndexOld=lbl[labelIndex].minIndex;
  for(unsigned int i=0;i<5;i++) { //Test four nearby cells, and move if better
    if (i==lbl[labelIndex].minIndexOld) continue;
    w=((lbl[labelIndex].x+(((int)(i==1)-(int)(i==2))<<18))-lbl[labelIndex].xo)>>8;
    h=((lbl[labelIndex].y+(((int)(i==3)-(int)(i==4))<<18))-lbl[labelIndex].yo)>>8;
    distSqr=w*w+h*h;
    diff=mmAbs(distSqr-(IDEALLEADERLENGTH>>8)*(IDEALLEADERLENGTH>>8));
    if (diff<minDiff) { minDiff=diff; lbl[labelIndex].minIndex=i; }
  }
  if (commonRandomUchar()<16) //Random nudge to get around tiny bumps
    lbl[labelIndex].minIndex=(commonRandomUchar()&3)+1;
  lbl[labelIndex].x+=((int)(lbl[labelIndex].minIndex==1)-(int)(lbl[labelIndex].minIndex==2))<<18;
  lbl[labelIndex].y+=((int)(lbl[labelIndex].minIndex==3)-(int)(lbl[labelIndex].minIndex==4))<<18;
}

void bound(unsigned int labelIndex) {
  lbl[labelIndex].x=mmMax(lbl[labelIndex].x,(MARGIN<<16)-lbl[labelIndex].aabb[0]);
  lbl[labelIndex].y=mmMax(lbl[labelIndex].y,(MARGIN<<16)-lbl[labelIndex].aabb[1]);
  lbl[labelIndex].x=mmMin(lbl[labelIndex].x,((AG_WIDTH-1-MARGIN)<<16)-lbl[labelIndex].aabb[2]);
  lbl[labelIndex].y=mmMin(lbl[labelIndex].y,((AG_HEIGHT-1-MARGIN)<<16)-lbl[labelIndex].aabb[3]);
}

bool leaderCrossed(int ex,int ey,int fx, int fy,int px,int py,int qx, int qy) {
  //TODO: Handle coincident lines
  int efx=fx-ex,efy=fy-ey,
	    epx=px-ex,epy=py-ey,
      eqx=qx-ex,eqy=qy-ey,
      pqx=qx-px,pqy=qy-py,
	    pfx=fx-px,pfy=fy-py,
      res0=mmSgn((efx>>13)*(epy>>13)-(efy>>14)*(epx>>14)), //Sign of crossproduct z-components used to determine crossing
      res1=mmSgn((efx>>13)*(eqy>>13)-(efy>>14)*(eqx>>14)),
      res2=mmSgn((pqy>>13)*(epx>>13)-(pqx>>13)*(epy>>13)), //Intentionally exchanged terms here
      res3=mmSgn((pqx>>13)*(pfy>>13)-(pqy>>13)*(pfx>>13));
  return (res0&&res1&&res2&&res3&&res0!=res1&&res2!=res3);
}

int compareLabelLeaderLength(const void* a,const void* b) {
  return ((Label*)a)->leaderLength<((Label*)b)->leaderLength;
}

void stampSimple() {
  for(unsigned int i=0;i<NUMLABELS;i++) {
    if (!lbl[i].stamped) continue;
    agClosedCurve(bmp2,lbl[i].p,8,lbl[i].x,lbl[i].y,R,G,B+((i+1)<<3));
    agPaintFill(bmp2,lbl[i].x>>16,lbl[i].y>>16,R,G,B+((i+1)<<3));
  }
}

void stampSolved(unsigned short* buff) {
  //Determine current leaderline lengths
  for(unsigned int i=0;i<NUMLABELS;i++) {
    int w,h;
    w=(lbl[i].x-lbl[i].xo)>>8;
    h=(lbl[i].y-lbl[i].yo)>>8;
    lbl[i].leaderLength=mmSqrRt(w*w+h*h);
  }
  //Sort all labels by current leaderline lengths
  qsort(lbl,NUMLABELS,sizeof(Label),compareLabelLeaderLength);
  //If labels, including those already stamped, have crossed leaderlines, then swap them, unstamp them, mark them unsolved and unrelaxed
  for(unsigned int j=0;j<NUMLABELS-1;j++) {
    for(unsigned int i=j+1;i<NUMLABELS;i++) {
	    if (leaderCrossed(lbl[i].xo,lbl[i].yo,lbl[i].x,lbl[i].y,lbl[j].xo,lbl[j].yo,lbl[j].x,lbl[j].y)) {
	      int tmp;
	      tmp=lbl[i].x; lbl[i].x=lbl[j].x; lbl[j].x=tmp;
	      tmp=lbl[i].y; lbl[i].y=lbl[j].y; lbl[j].y=tmp;
	      lbl[i].stamped=lbl[i].solved=lbl[i].leaderRelaxed=false;
	      lbl[j].stamped=lbl[j].solved=lbl[j].leaderRelaxed=false;
        bound(i); bound(j); //After swap, label may run off frame, so readjust
		    //Redetermine current leaderline length
        int w,h;
        w=(lbl[i].x-lbl[i].xo)>>8;
        h=(lbl[i].y-lbl[i].yo)>>8;
        lbl[i].leaderLength=mmSqrRt(w*w+h*h);
        w=(lbl[j].x-lbl[j].xo)>>8;
        h=(lbl[j].y-lbl[j].yo)>>8;
        lbl[j].leaderLength=mmSqrRt(w*w+h*h);
        //Again sort all labels by current leaderline lengths
        qsort(lbl,NUMLABELS,sizeof(Label),compareLabelLeaderLength);
        j=i=0; //Restart the leaderline crossing check
	    }
	  }
  }
  //Draw previously stamped objects as immovable
  for(unsigned int i=0;i<NUMLABELS;i++) {
    if (!lbl[i].stamped) continue;
    agClosedCurve(bmp2,lbl[i].p,8,lbl[i].x,lbl[i].y,R,G,B+((i+1)<<3));
    agPaintFill(bmp2,lbl[i].x>>16,lbl[i].y>>16,R,G,B+((i+1)<<3));
  }
  //Draw the solved non-colliding unstamped labels as immovable objects on the original image (in order of leaderline length)
  for(unsigned int i=0;i<NUMLABELS;i++) {
    if (!lbl[i].solved||lbl[i].stamped) continue;
	  //Determine collision area
	  unsigned char r=0xff,g=0xff,b=0xff-(i<<3);
    agClosedCurve(buff,lbl[i].p,8,lbl[i].x,lbl[i].y,r,g,b);
    numCollisions=0;
    collisionArea(buff,bmp2,lbl[i].x>>16,lbl[i].y>>16,r,g,b);
	  lbl[i].collision=numCollisions;
	  //Stamp as immovable object if not in collision
	  if (lbl[i].collision<=COLLISIONAREATHRESHOLD) {
      agClosedCurve(bmp2,lbl[i].p,8,lbl[i].x,lbl[i].y,R,G,B+((i+1)<<3));
      agPaintFill(bmp2,lbl[i].x>>16,lbl[i].y>>16,R,G,B+((i+1)<<3));
      lbl[i].stamped=true;
	  }
	  else
	    lbl[i].solved=false;
  }
}

void stat(int T,unsigned int t) {
  app->setPrintLocation(0); app->setPrintString("T = ",0,NULL); app->setPrintInt(T);
  app->setPrintLocation(80); app->setPrintString("t = ",0,NULL); app->setPrintInt(t);
  app->setPrintLocation(160); app->setPrintString("stateIndex = ",0,NULL); app->setPrintInt(stateIndex);
  app->setPrintLocation(240); app->setPrintString("IDEALLEADERLENGTH = ",0,NULL); app->setPrintInt(IDEALLEADERLENGTH>>16);
  app->setPrintLocation(300); app->setPrintString("COLLISIONAREATHRESHOLD = ",0,NULL); app->setPrintInt(COLLISIONAREATHRESHOLD);
}

void bye(bool err,int T,unsigned int t) {
  unsigned short *buff=app->getFrameBuffer();
  app->setFrameBufferClear();
  for(unsigned int j=0;j<AG_WIDTH*AG_HEIGHT;j++)
	buff[j]=(bmp2[j]? 0xf800 : 0);
  for(unsigned int i=0;i<NUMLABELS;i++) {
	  unsigned char r=0xff,g=0xff,b=0xff-(i<<3);
    agClosedCurve(buff,lbl[i].p,8,lbl[i].x,lbl[i].y,r,g,b);
	  agPaintFill(buff,lbl[i].x>>16,lbl[i].y>>16,r,g,b);
    agQuad(buff,lbl[i].aabb[0]+lbl[i].x-(MARGIN<<16),lbl[i].aabb[1]+lbl[i].y-(MARGIN<<16),lbl[i].aabb[2]+lbl[i].x+(MARGIN<<16),lbl[i].aabb[1]+lbl[i].y-(MARGIN<<16),lbl[i].aabb[2]+lbl[i].x+(MARGIN<<16),lbl[i].aabb[3]+lbl[i].y+(MARGIN<<16),lbl[i].aabb[0]+lbl[i].x-(MARGIN<<16),lbl[i].aabb[3]+lbl[i].y+(MARGIN<<16),0xff,0xff,0);
	  agLine(buff,lbl[i].xo,lbl[i].yo,lbl[i].x,lbl[i].y,0xff,0,0xff);
  }
  stat(T,t);
  app->setPrintLocation(2020);
  if (err) app->setPrintString("!!!!!!!!!!!!!!! SOLUTION NOT FOUND !!!!!!!!!!!!!!! ",3,NULL);
  else app->setPrintString("--------------- SOLUTION FOUND --------------- ",1,NULL);
  app->update();
  save();
  app->setSleep(10000);
  firstRun=veryFirstRun=true;
  timeTot=0;
}

void mainUpdate() {
  unsigned short *buff=app->getFrameBuffer(),
	               *buffOff=app->getOffFrameBuffer();
  if (firstRun) {
  	if (veryFirstRun) {
      if (veryVeryFirstRun) {
        DIR *dir;
        dirent *ent;
        if (dir=opendir("..\\MEDIA\\")) {
          unsigned int ind=0;
          while(ent=readdir(dir)) {
            strcpy(imageFileName[ind],ent->d_name);
            if (imageFileName[ind][0]!='.') ind++;
          }
          closedir(dir);
          numImageFiles=ind;
        }
        veryVeryFirstRun=false;
      }
      init();
      veryFirstRun=false;
    }
    resetPosition();
    resetBitmaps();
	  stampSolved(buff);
	  if (state[stateIndex]&DISTANCETRANSFORM) {
      agFullScreenExtendedDistanceTransform(bmp,buff);
    }
	  if (state[stateIndex]&WIND) {
	    if (!(state[stateIndex]&DISTANCETRANSFORM))
        for(unsigned int i=0;i<AG_WIDTH*AG_HEIGHT;i++)
		      bmp[i]=0;
      addWind(bmp);
	    if (!(state[stateIndex]&DISTANCETRANSFORM))
        agFullScreenScale(bmp,3,0);
	  }
    if (state[stateIndex]&ACTIVEFORCE)
      for(unsigned int i=0;i<AG_WIDTH*AG_HEIGHT;i++)
        bmp[i]=0;
    agFullScreenCenterValue(bmp);
	  firstRun=false;
    app->setCounterClear();
  }
  unsigned int t=app->getCounter();
  int T=temperature(t<<11);
  //Set heightfield
  for(unsigned int i=0;i<AG_WIDTH*AG_HEIGHT;i++)
    buff[i]=bmp[i]+
            (state[stateIndex]&NOISE? (T>>6)*(noise.get2D((i%AG_WIDTH)<<NOISEFREQ,(i/AG_WIDTH)<<NOISEFREQ,0xff,0xff)>>7) : 0);
  //Calculate velocities and move position
  int dx[NUMLABELS],dy[NUMLABELS],dxActive=0,dyActive=0;
  if (state[stateIndex]&ACTIVEFORCE) setActiveForce(t,&dxActive,&dyActive,state[stateIndex]&NOISE);
  for(unsigned int i=0;i<NUMLABELS;i++) {
    if (lbl[i].solved) continue;
    unsigned short *ptr=buff+(lbl[i].y>>16)*AG_WIDTH+(lbl[i].x>>16);
	  dx[i]=((ptr[1]>>1)-(*ptr>>1))<<12;
    dy[i]=((ptr[AG_WIDTH]>>1)-(*ptr>>1))<<12;
	  lbl[i].x+=dx[i]+dxActive; lbl[i].y+=dy[i]+dyActive;
    bound(i);
  }
  //Calculate collision area with immovable objects
  //NOTE: This also draws the labels onto the heightfield
  for(unsigned int i=0;i<NUMLABELS;i++) {
    if (lbl[i].stamped) continue;
	  lbl[i].solvedOld=lbl[i].solved;
	  if (lbl[i].solved) {
	    lbl[i].xOld=lbl[i].x; lbl[i].yOld=lbl[i].y;
	    rein(i);
	  }
	  bound(i);
	  unsigned char r=0xff,g=0xff,b=0xff-(i<<3);
    agClosedCurve(buff,lbl[i].p,8,lbl[i].x,lbl[i].y,r,g,b);
    numCollisions=0;
    collisionArea(buff,bmp2,lbl[i].x>>16,lbl[i].y>>16,r,g,b);
	  lbl[i].collision=numCollisions;
  }
  solvedAll=true;
  for(unsigned int i=0;i<NUMLABELS;i++) {
    lbl[i].blockedOld=lbl[i].blocked;
    lbl[i].blocked=!(lbl[i].solved=(lbl[i].collision<=COLLISIONAREATHRESHOLD));
	  if (lbl[i].solvedOld&&!lbl[i].solved) {
	    lbl[i].solved=true;
	    lbl[i].x=lbl[i].xOld; lbl[i].y=lbl[i].yOld;
	  }
    if (!(lbl[i].leaderRelaxed||lbl[i].stamped)) solvedAll=false;
  }
  //Check leaderline crossings
  if (solvedAll)
    for(unsigned int j=0;j<NUMLABELS-1;j++) {
      for(unsigned int i=j+1;i<NUMLABELS;i++) {
	      if (leaderCrossed(lbl[i].xo,lbl[i].yo,lbl[i].x,lbl[i].y,lbl[j].xo,lbl[j].yo,lbl[j].x,lbl[j].y)) {
          lbl[i].stamped=lbl[i].solved=lbl[i].leaderRelaxed=false;
          lbl[j].stamped=lbl[j].solved=lbl[j].leaderRelaxed=false;
  		    solvedAll=false;
          resetBitmap2();
          stampSimple();
	      }
      }
    }
  //Display the information
  for(unsigned int j=0;j<AG_WIDTH*AG_HEIGHT;j++)
    buff[j]=(bmp2[j]? 0 : buff[j]);
  for(unsigned int i=0;i<NUMLABELS;i++) {
	  unsigned char r=0xff,g=0xff,b=0xff-(i<<3);
    agQuad(buff,lbl[i].aabb[0]+lbl[i].x-(MARGIN<<16),lbl[i].aabb[1]+lbl[i].y-(MARGIN<<16),lbl[i].aabb[2]+lbl[i].x+(MARGIN<<16),lbl[i].aabb[1]+lbl[i].y-(MARGIN<<16),lbl[i].aabb[2]+lbl[i].x+(MARGIN<<16),lbl[i].aabb[3]+lbl[i].y+(MARGIN<<16),lbl[i].aabb[0]+lbl[i].x-(MARGIN<<16),lbl[i].aabb[3]+lbl[i].y+(MARGIN<<16),0xff,0xff*lbl[i].solved,0);
	  agLine(buff,lbl[i].xo,lbl[i].yo,lbl[i].x,lbl[i].y,0xff,0xff*lbl[i].leaderRelaxed,0xff);
    stat(T,t);
  }
  //Standard update
  noise.update();
  if (DEMOMODE) app->update();
  if (SLOWMODE) app->setSleep(100);
  if (app->getInputCode()&0x80000000) delete app;
  //Determine state
  if (solvedAll)
    bye(0,T,t);
  if (T<=TEMPERATURETHRESHOLD) {
	  firstRun=true;
    stateIndex++;
	  if (stateIndex==NUMSTATES)
      bye(1,T,t);
  }
}

appCauda();
