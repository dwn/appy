////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
////////////////////////////////////////////////////////////////////////////////
#include "Noise.h"
#include "Common.h"
#include "MoreMath.h"

Noise::Noise() {
  //Initialize gradient vectors
  //Gradient vectors for the 3D case:
  //[ 1  1  0] [ 1  1  0] [-1  1  0] [-1  1  0]
  //[ 1 -1  0] [-1 -1  0] [ 1  0  1] [-1  0  1]   Duplicated tetrahedron
  //[ 0  1  1] [ 0 -1  1] [ 0 -1  1] [ 1  0 -1]   for a total 16 vectors
  //[-1  0 -1] [ 0  1 -1] [ 0 -1 -1] [ 0 -1 -1] <-(as Perlin suggested)
  for(int i=0;i<256;i++) grad[i]=commonPermute256[i];
}

Noise::~Noise() {
}

void Noise::update() {
  //Scalar update
  for(int i=0;i<256;i++) {
    if ((grad[i]&0x7)==0x7) grad[i]&=0x7f;
    else if (!(grad[i]&0x7)) grad[i]|=0x80;
    grad[i]+=commonRandomBool()*(((grad[i]&0x80)>>6)-1);
  }
  //Gradient swap
  bool k=false;
  for(int i=0,j=0;i<256;i++) {
    if ((grad[i]&0x7)==0) {
      if (k) { unsigned char tmp=grad[i]; grad[i]=grad[j]; grad[j]=tmp; }
      else { k=true; }
      j=i;
    }
  }
}

//Output: signed 11-int
int Noise::get1D(int x,int xiMask) {
  return mmInterp(quickDot1D(grad[(x>>8)&xiMask],x&0xff),
                  quickDot1D(grad[((x>>8)+1)&xiMask],(x&0xff)|0xffffff00),
                  mmSCurve(x&0xff));
}

//Output: signed 11-int
int Noise::get2D(int x,int y,int xiMask,int yiMask) {
  int xi=(x>>8)&xiMask,x1=(xi+1)&xiMask,
      xf=x&0xff,yf=y&0xff,
      sx=mmSCurve(xf);
  //Permutation
  //Reusing variable names, actually are permutations
  x=commonPermute256[(y>>8)&yiMask];
  y=commonPermute256[((y>>8)+1)&yiMask];
  //Quick dot product, interpolation
  return mmInterp(mmInterp(quickDot2D(grad[xi^x],xf,yf),
                           quickDot2D(grad[x1^x],xf|0xffffff00,yf),
                           sx),
                  mmInterp(quickDot2D(grad[xi^y],xf,yf|0xffffff00),
                           quickDot2D(grad[x1^y],xf|0xffffff00,
                                                 yf|0xffffff00),
                           sx),
                  mmSCurve(yf));
}

//Output: signed 11-int
int Noise::get3D(int x,int y,int z,int xiMask,int yiMask,int ziMask) {
  int h[4];
  //Permutation
  h[2]=commonPermute256[(z>>8)&ziMask];
  h[3]=commonPermute256[((z>>8)+1)&ziMask];
  int yi=(y>>8)&yiMask,y1=(yi+1)&yiMask,
      xi=(x>>8)&xiMask,x1=(xi+1)&xiMask,
      xf=x&0xff,yf=y&0xff,zf=z&0xff,
      sx=mmSCurve(xf),sy=mmSCurve(yf);
  h[0]=commonPermute256[h[2]^yi];
  h[1]=commonPermute256[h[2]^y1];
  h[2]=commonPermute256[h[3]^yi];
  h[3]=commonPermute256[h[3]^y1];
  //Quick dot product, interpolation
  return mmInterp(mmInterp(mmInterp(quickDot3D(grad[xi^h[0]],
                                               xf,yf,zf),
                                    quickDot3D(grad[x1^h[0]],
                                               xf|0xffffff00,yf,zf),
                                    sx),
                           mmInterp(quickDot3D(grad[xi^h[1]],
                                               xf,yf|0xffffff00,zf),
                                    quickDot3D(grad[x1^h[1]],
                                               xf|0xffffff00,yf|0xffffff00,zf),
                                    sx),
                           sy),
                  mmInterp(mmInterp(quickDot3D(grad[xi^h[2]],
                                               xf,yf,zf|0xffffff00),
                                    quickDot3D(grad[x1^h[2]],
                                               xf|0xffffff00,yf,zf|0xffffff00),
                                    sx),
                           mmInterp(quickDot3D(grad[xi^h[3]],
                                               xf,yf|0xffffff00,zf|0xffffff00),
                                    quickDot3D(grad[x1^h[3]],xf|0xffffff00,
                                               yf|0xffffff00,zf|0xffffff00),
                                    sx),
                           sy),
                  mmSCurve(zf));
}

int Noise::getTurbulence2D(int x,int y,int xiMask,int yiMask,
                           unsigned char numOctaves) {
  int ret=0;
  for(int i=0;i<numOctaves;i++) {
    const int iComp=numOctaves-1-i;
    ret+=mmAbs(get2D(x<<i,y<<i,xiMask>>iComp,yiMask>>iComp))>>i;
  }
  return ret;
}

int Noise::getTurbulence3D(int x,int y,int z,int xiMask,int yiMask,int ziMask,
                           unsigned char numOctaves) {
  int ret=0;
  for(int i=0;i<numOctaves;i++) {
    const int iComp=numOctaves-1-i;
    ret+=mmAbs(get3D(x<<i,y<<i,z<<i,
                     xiMask>>iComp,yiMask>>iComp,ziMask>>iComp))>>i;
  }
  return ret;
}

int Noise::quickDot1D(int g,int dx) {
  return (g&0x8? -dx : dx)*(g&0x7);
}

//Decodes gradient vector, no multiply needed
int Noise::quickDot2D(int g,int dx,int dy) {
  int pa,pb;
  if (g&0x8) { pa=dx; pb=dy; }
  else { pa=dy; pb=dx; }
  return (((g&0x10? -pa : pa)<<1)+(g&0x20? -pb : pb))*(g&0x7);
}

//Decodes gradient vector, no multiply needed
int Noise::quickDot3D(int g,int dx,int dy,int dz) {
      int h=(g>>3)&0xf;
      int u=(h&0x10? dy : dx),
          v=(h<4? dy : (h==12||h==14? dx : dz));
      return ((h&1? -u : u)+(h&2? -v : v))*(g&0x7);
}
