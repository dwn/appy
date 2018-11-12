////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
////////////////////////////////////////////////////////////////////////////////
#include "MoreMath.h"
#include <stdbool.h>
#include <limits.h>

//Floor of binary logarithm (i.e. truncated to basic integer)
//Input: unsigned 32-int
//Output: signed 32-int
int mmFloorLog2(unsigned int n) {
  if (!n) return -32768;
  int pos=0;
  if (n>=1<<16) { n>>=16; pos+=16; }
  if (n>=1<< 8) { n>>= 8; pos+= 8; }
  if (n>=1<< 4) { n>>= 4; pos+= 4; }
  if (n>=1<< 2) { n>>= 2; pos+= 2; }
  if (n>=1<< 1) {         pos+= 1; }
  return pos;
}

//Ceiling of binary logarithm
//Input: unsigned 32-int
//Output: signed 32-int
int mmCeilingLog2(unsigned int n) {
  unsigned char shift=mmFloorLog2(n);
  return (n>(1<<shift)? shift+1 : shift);
}

//Inverse tangent with quadrant determination
//Input: signed 16.16-fixed
//Output: unsigned 11-int (512 halfticks=90 deg; high-precision angle!)
unsigned int mmAtan(int y,int x) {
   if (mmAbs(x)<0x100) {
     if (x>=0) x=0x100;
     else x=-0x100;
   }
   int t=mmAbs((y/(x>>8))<<8);
   int a,b,c,d;
   a=0; b=511;
   do { //Perform binary search on tan table
     c=(a+b)>>1;
     d=t-mmTanTable[c];
     if (d>0) a=c+1;
     else if (d<0) b=c-1;
   } while((a<=b)&&(d));
   c=(x<0? 1024-c : c);
   return (y<0? 2048-c : c);
}

//Inverse cosine
//Input: signed 1.8-fixed
//Output: unsigned 9-int (256 ticks=90 deg; range [0,512))
int mmAcos(int t) {
  return (t>=0x100? 0 :
                    (t<=-0x100? 512 :
                      (t>=0? mmAcosTable[t]+1 : (512-1)-mmAcosTable[-t])));
}

//Sine
//Input: unsigned 32-int (256 ticks=90 deg)
//Output: signed 2.16-fixed
int mmSin(int t) {
  int r=
    ((t&0x1ff)==0x100? 65536 :
                       mmSinTable[t&0x100? 
                                  //Hardware: ((t&0xff)^0xff)+1
                                  256-(t&0xff) :
                                  (t&0xff)]);

  return ((t&0x200)>0? -r : r);
}

//Sine that clamps into a short
//Input: unsigned 32-int (256 ticks=90 deg)
//Output: signed 2.14-fixed
short mmShortSin(int t) {
  int r=
    ((t&0x1ff)==0x100? 32767 :
                       mmSinTable[t&0x100? 
                                  //Hardware: ((t&0xff)^0xff)+1
                                  256-(t&0xff) :
                                  (t&0xff)]>>1);

  return ((t&0x200)>0? -r : r);
}

//Cosine
//Input: unsigned 32-int (256 ticks=90 deg)
//Output: signed 2.16-fixed
int mmCos(int t) {
  return mmSin(t+256);
}

//Linear interpolate
//Input: signed 14.8-fixed {a,b},unsigned .8-fixed {t}
//Output: signed 14.8-fixed
int mmInterp(int a,int b,int t) {
  return a+((t*(b-a))>>8);
}

//Square root
//If performed on a fixed-point value, remember to consider the decimal place of
//the result to be at a position half that of the input
//Input: unsigned 32-int
//Output: unsigned 16-int
unsigned int mmSqrRt(unsigned int val) {
  unsigned int tmp,g=0;
  if (val>=0x40000000) {
    g=0x8000;
    val-=0x40000000;
  }

  #define MM_TMP(x) \
    tmp=(g << (x))+(1 << (((x)<<1)-2)); \
    if (val>=tmp) { \
      g+=1<<((x)-1); \
      val-=tmp; \
    }

  MM_TMP(15); MM_TMP(14); MM_TMP(13); MM_TMP(12); MM_TMP(11); MM_TMP(10);
  MM_TMP(9); MM_TMP(8); MM_TMP(7); MM_TMP(6); MM_TMP(5); MM_TMP(4); MM_TMP(3);
  MM_TMP(2);

  #undef MM_TMP

  if (val>=((g<<1)+1)) g++;
  return g;
}

//Approximate square root
//If performed on a fixed-point value, remember to consider the decimal place of
//the result to be at a position half that of the input
//Input: unsigned 32-int
//Output: unsigned 16-int
unsigned int mmApproxSqrRt(unsigned int val) {
  union {float f; unsigned int i;} flt,flt2;
  flt.f=(float)val;
  flt.i=0x5f375a86-(flt.i>>1); //Initial estimate
  flt2.f=flt.f+2.; //Convert float to 10.22-fixed, exploiting the mechanism
  flt2.i&=0x7fffff;
  //Single iteration of Newton approximation
  flt2.i=(flt2.i>>11)*((0x600000-(((flt2.i>>15)*(flt2.i>>15))*(val>>9)))>>11);
  return (val*flt2.i+0x1e3c68)>>23;
}

//Approximate reciprocal square root (1/sqrt(val))
//With max error around 10%, mmApproxRecipSqrRt(x>>3)==0x20000000/mmSqrRt(x<<10)
//Input: unsigned 32-int
//Output: unsigned 16-int
unsigned int mmApproxRecipSqrRt(unsigned int val) {
  union {float f; unsigned int i;} flt,flt2;
  flt.f=(float)val;
  flt.i=0x5f375a86-(flt.i>>1); //Initial estimate
  flt2.f=flt.f+2.; //Convert float to 10.22-fixed, exploiting the mechanism
  flt2.i&=0x7fffff;
  //Single iteration of Newton approximation
  return (flt2.i>>11)*((0x600000-(((flt2.i>>15)*(flt2.i>>15))*(val>>9)))>>11);
}

//Input: signed .16-fixed
//Output: signed .16-fixed
short mmShortMul(short a,short b) {
  int c=((int)a*(int)b)>>14;
  return (c>>1)+(c&0x01); //With rounding
}

//In-place forward/inverse fast Fourier transform
//Good only for m<=10, where there are pow2(m) samples
//Input: signed .16-fixed
//Output: signed .16-fixed
int mmFFT(short* fr,short* fi,short m,bool inverse) {
  int mr,nn,i,j,l,k,iStep,n,scale,shift;
  short qr,qi,tr,ti,wr,wi;
  n=1<<m;
  mr=0;
  nn=n-1;
  scale=0;
  for(m=1;m<=nn;++m) {
    l=n;
    do {
      l>>=1;
    } while(mr+l>nn);
    mr=(mr&(l-1))+l;
    if (mr<=m) continue;
    tr=fr[m];
    fr[m]=fr[mr];
    fr[mr]=tr;
    ti=fi[m];
    fi[m]=fi[mr];
    fi[mr]=ti;
  }
  l=1;
  k=9;
  while(l<n) {
    if (inverse) {
      shift=0;
      for (i=0;i<n;++i) {
        j=fr[i];
        if (j<0) j=-j;
        m=fi[i];
        if (m<0) m=-m;
        if (j>16383||m>16383) {
          shift=1;
          break;
        }
      }
      if (shift) scale++;
    }
	else {
      shift=1;
    }
    iStep=l<<1;
    for(m=0;m<l;++m) {
      j=m<<k;
      wr=mmShortSin(j+256);
      wi=-mmShortSin(j);
      if (inverse) wi=-wi;
      if (shift) {
        wr>>=1;
        wi>>=1;
      }
      for(i=m;i<n;i+=iStep) {
        j=i+l;
        tr=mmShortMul(wr,fr[j])-mmShortMul(wi,fi[j]);
        ti=mmShortMul(wr,fi[j])+mmShortMul(wi,fr[j]);
        qr=fr[i];
        qi=fi[i];
        if (shift) {
          qr>>=1;
          qi>>=1;
        }
        fr[j]=qr-tr;
        fi[j]=qi-ti;
        fr[i]=qr+tr;
        fi[i]=qi+ti;
      }
    }
    k--;
    l=iStep;
  }
  return scale;
}

//In-place forward/inverse FFT on array of real numbers
//Good only for m<=10, where there are pow2(m) samples
//Input: signed .16-fixed
//Output: signed .16-fixed
int mmFFTR(short* f,int m,bool inverse) {
  int i,n=1<<(m-1),scale=0;
  short tt,*fr=f,*fi=f+n;
  if (inverse) scale=mmFFT(fi,fr,m-1,inverse); //Intentional swapped arguments
  for(i=1;i<n;i+=2) {
    tt=f[n+i-1];
    f[n+i-1]=f[i];
    f[i]=tt;
  }
  if (!inverse) scale=mmFFT(fi,fr,m-1,inverse); //Intentional swapped arguments
  return scale;
}

//Clear vector
//Fills the first n entries in vector v with 0
void mmClearVec(int* v,int n) {
  for(int i=0;i<n;i++) *v++=0;
}

//Fill vector
//Fills the first n entries in vector v with a given value
void mmFillVec(int* v,int val,int n) {
  for(int i=0;i<n;i++) *v++=val;
}

//3D vector dot product
int mmDot(const int* va,const int* vb) {
  return ((va[0]>>7)*(vb[0]>>7)+
          (va[1]>>7)*(vb[1]>>7)+
          (va[2]>>7)*(vb[2]>>7))>>2;
}

//3D vector cross product
void mmCross(int* vr,const int* va,const int* vb) {
  vr[0]=((va[1]>>7)*(vb[2]>>7)-(va[2]>>7)*(vb[1]>>7))>>2;
  vr[1]=((va[2]>>7)*(vb[0]>>7)-(va[0]>>7)*(vb[2]>>7))>>2;
  vr[2]=((va[0]>>7)*(vb[1]>>7)-(va[1]>>7)*(vb[0]>>7))>>2;
}

//2D vector magnitude
//Performs the real 2D 2-norm operation, i.e. the Pythagoras theorem
unsigned int mmNorm2Vec2(const int* v) {
  return mmSqrRt(v[0]*v[0]+v[1]*v[1]);
}

//Approximate 2D vector magnitude
//Linearly approximates mmNorm2Vec2()
//Use with caution
unsigned int mmApproxNorm2Vec2(const int* v) {
  int sm=(v[0]<0? -v[0] : v[0]),
      lg=(v[1]<0? -v[1] : v[1]);
  if (sm>lg) {int tmp=sm; sm=lg; lg=tmp;}
  return ( (((((lg<<4)-lg)<<1)+lg)<<2)-lg +
           (((((sm<<2)-sm)<<2)+sm)<<2)-sm   )>>7; //123/128*lg+51/128*sm
}

//3D vector magnitude
//Performs the real 3D 2-norm operation, i.e., the Pythagoras theorem
unsigned int mmNorm2Vec3(const int* v) {
  return mmSqrRt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

//3D vector magnitude by components
//Performs the real 3D 2-norm operation, i.e., the Pythagoras theorem
unsigned int mmNorm2Coord3(const int x,const int y,const int z) {
  return mmSqrRt(x*x+y*y+z*z);
}

//4D vector magnitude
//Performs the real 4D 2-norm operation
unsigned int mmNorm2Vec4(const int* v) {
  return mmSqrRt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]+v[3]*v[3]);
}

//Linear space
//Fills vector v with n uniformly spaced values from begin to end
//Similar to Matlab's linspace(), but doesn't operate on arbitrary types
void mmLinSpace(int* v,int begin,int end,int n) {
  for(int i=0;i<n;i++) {
    //TODO: improve
    v[i]=begin+i*(end-begin)/(n-1);
  }
}

//Evaluate space
//Fills vector v with the results of f(x) where x=begin..end uniformly spaced
void mmEvalVec(int* v,int (*f)(int),int begin,int end,int n) {
  for(int i=0;i<n;i++) {
    //TODO:
    //Improve
    v[i]=f(begin+i*(end-begin)/(n-1));
  }
}

//Affect space
//Performs a function f() on the values contained in vector v, and overwrites
//the original values in v with the results
void mmVecFunc(int* v,int (*f)(int),int n) {
  for(int i=0;i<n;i++) {
    v[i]=f(v[i]);
  }
}

void mmTransposeMat2x2(int* m) {
  int a=m[1];
  m[1]=m[2];
  m[2]=a;
}

bool mmInvertMat2x2(int* mr,int* m) {
  int det=((m[0]>>8)*(m[3]>>8)-(m[1]>>8)*(m[2]>>8))>>2;
  if (!det) return false;
  det=0x40000000/det;
  mr[0]=(det>>8)*(m[3]>>8); mr[1]=-(det>>8)*(m[1]>>8);
  mr[2]=-(det>>8)*(m[2]>>8); mr[3]=(det>>8)*(m[0]>>8);
  return true;
}

void mmClearMat3x4(int* m) {
  for(int i=0;i<12;i++) {
    m[i]=0;
  }
}

void mmCopyMat3x4(int* mr,int* m) {
  for(int i=0;i<12;i++) {
    mr[i]=m[i];
  }
}

void mmSetIdentityMat3x4(int* m) {
  m[0]=0x10000; m[1]=      0; m[ 2]=      0; m[ 3]=0;
  m[4]=      0; m[5]=0x10000; m[ 6]=      0; m[ 7]=0;
  m[8]=      0; m[9]=      0; m[10]=0x10000; m[11]=0;
}

void mmSetRotXMat3x4(int* mr,const int ang) {
  int cosAng=mmCos(ang),sinAng=mmSin(ang);
  mr[0]=0x10000; mr[1]=0;      mr[ 2]=0;
  mr[4]=0;       mr[5]=cosAng; mr[ 6]=-sinAng;
  mr[8]=0;       mr[9]=sinAng; mr[10]=cosAng;
}

void mmSetRotYMat3x4(int* mr,const int ang) {
  int cosAng=mmCos(ang),sinAng=mmSin(ang);
  mr[0]=cosAng; mr[1]=0;       mr[ 2]=-sinAng;
  mr[4]=0;      mr[5]=0x10000; mr[ 6]=0;
  mr[8]=sinAng; mr[9]=0;       mr[10]=cosAng;
}

void mmSetRotZMat3x4(int* mr,const int ang) {
  int cosAng=mmCos(ang),sinAng=mmSin(ang);
  mr[0]=cosAng;  mr[1]=-sinAng; mr[ 2]=0;
  mr[4]=sinAng;  mr[5]=cosAng;  mr[ 6]=0;
  mr[8]=0;       mr[9]=0;       mr[10]=0x10000;
}

void mmSetRotMat3x4(int* mr,const int tta,const int phi,const int psi) {
  int sinTta=mmSin(tta),cosTta=mmCos(tta),
      sinPhi=mmSin(phi),cosPhi=mmCos(phi),
      sinPsi=mmSin(psi),cosPsi=mmCos(psi);
  mr[ 0]= ((cosPsi>>2)*(cosPhi>>2))>>12;
  mr[ 1]=-(((sinPsi>>2)*(cosTta>>2))>>12)
         -( ((((cosPsi>>2)*(sinPhi>>2))>>14) * (sinTta>>2)) >> 12 );
  mr[ 2]= (((sinPsi>>2)*(sinTta>>2))>>12)
         -( ((((cosPsi>>2)*(sinPhi>>2))>>14) * (cosTta>>2)) >> 12 );
  mr[ 4]= ((sinPsi>>2)*(cosPhi>>2))>>12;
  mr[ 5]= (((cosPsi>>2)*(cosTta>>2))>>12)
         -( ((((sinPsi>>2)*(sinPhi>>2))>>14) * (sinTta>>2)) >> 12 );
  mr[ 6]=-(((cosPsi>>2)*(sinTta>>2))>>12)
         -( ((((sinPsi>>2)*(sinPhi>>2))>>14) * (cosTta>>2)) >> 12 );
  mr[ 8]= sinPhi;
  mr[ 9]= ((cosPhi>>2)*(sinTta>>2))>>12;
  mr[10]= ((cosPhi>>2)*(cosTta>>2))>>12;
}

void mmSetPosMat3x4(int* mr,const int* v) {
  mr[ 3]=v[0];
  mr[ 7]=v[1];
  mr[11]=v[2];
}

//Converts quaternion to rotation matrix
//Preserves any values that may already be in the position column
//Input: signed 16.16-fixed quaternion (x,y,z,w)
void mmQuatToMat3x4(int* q,int* m) {
  const int x=q[0]>>7,y=q[1]>>7,z=q[2]>>7,w=q[3]>>7, //Equal to 2*q_x, etc.
            xx=x*(x>>1),  //Equal to 2*q_x*q_x, etc.
            yy=y*(y>>1),
            zz=z*(z>>1),
            xy=x*(y>>1),
            xz=x*(z>>1),
            yz=y*(z>>1),
            wx=w*(x>>1),
            wy=w*(y>>1),
            wz=w*(z>>1);
  m[0]=0x10000-(yy+zz);
  m[1]=xy+wz;
  m[2]=xz-wy;
  m[4]=xy-wz;
  m[5]=0x10000-(xx+zz);
  m[6]=yz+wx;
  m[8]=xz+wy;
  m[9]=yz-wx;
  m[10]=0x10000-(xx+yy);
}

//Multiplies two quaternions
//Input: signed 16.16-fixed quaternions as (x,y,z,w)
void mmQuatMul(int* qr,int* qa,int* qb) {
  const int qax=qa[0]>>8,
            qay=qa[1]>>8,
            qaz=qa[2]>>8,
            qaw=qa[3]>>8,
            qbx=qb[0]>>8,
            qby=qb[1]>>8,
            qbz=qb[2]>>8,
            qbw=qb[3]>>8;
  qr[0]=qaw*qbx+qax*qbw+qay*qbz-qaz*qby;
  qr[1]=qaw*qby+qay*qbw+qaz*qbx-qax*qbz;
  qr[2]=qaw*qbz+qaz*qbw+qax*qby-qay*qbx;
  qr[3]=qaw*qbw-qax*qbx-qay*qby-qaz*qbz;
}

void mmMat3x4FakeMul(int* mr,const int* ma,const int* mb) {
  int k;
  for(int j=0;j<3;j++) {
    k=j<<2;
    for(int i=0;i<4;i++) { //Rotation portion has extra precision
      mr[k+i]=( ((ma[k]>>4)  *(mb[i]>>3)  ) >> 9 )+
              ( ((ma[k+1]>>4)*(mb[4+i]>>3)) >> 9 )+
              ( ((ma[k+2]>>4)*(mb[8+i]>>3)) >> 9 );
    }
    mr[k+3]+=ma[k+3];
  }
}

//Multiplies the top-left 3x3 submatrix of 3x4 matrix m by 3D column vector v,
//then adds the rightmost column vector in m to the result
//This is used to rotate and translate a vector
void mmMat3x4FakeMulVec3(int* vr,const int* m,const int* v) {
  vr[0]=(((m[ 0]>>2)*(v[0]>>6)) >> 8)+
        (((m[ 1]>>2)*(v[1]>>6)) >> 8)+
        (((m[ 2]>>2)*(v[2]>>6)) >> 8)+m[ 3];
  vr[1]=(((m[ 4]>>2)*(v[0]>>6)) >> 8)+
        (((m[ 5]>>2)*(v[1]>>6)) >> 8)+
        (((m[ 6]>>2)*(v[2]>>6)) >> 8)+m[ 7];
  vr[2]=(((m[ 8]>>2)*(v[0]>>6)) >> 8)+
        (((m[ 9]>>2)*(v[1]>>6)) >> 8)+
        (((m[10]>>2)*(v[2]>>6)) >> 8)+m[11];
}

//Multiplies the top-left 3x3 submatrix of 3x4 matrix m by 3D column vector v,
//without adding the displacement vector
//This is used to rotate without translation
void mmMat3x4MulVec3NoTranslate(int* vr,const int* m,const int* v) {
  vr[0]=(((m[ 0]>>2)*(v[0]>>6)) >> 8)+
        (((m[ 1]>>2)*(v[1]>>6)) >> 8)+
        (((m[ 2]>>2)*(v[2]>>6)) >> 8);
  vr[1]=(((m[ 4]>>2)*(v[0]>>6)) >> 8)+
        (((m[ 5]>>2)*(v[1]>>6)) >> 8)+
        (((m[ 6]>>2)*(v[2]>>6)) >> 8);
  vr[2]=(((m[ 8]>>2)*(v[0]>>6)) >> 8)+
        (((m[ 9]>>2)*(v[1]>>6)) >> 8)+
        (((m[10]>>2)*(v[2]>>6)) >> 8);
}

//Evaluates a point along a smooth curve that has a derivative of 0 at the
//endpoints and a derivative of 1 at the midpoint
//No lookup table is required for this s-curve function, although it is slightly
//different from mmSCurve()
//Input: unsigned .8-fixed
//Output: unsigned .8-fixed
unsigned char mmApproxSCurve(const unsigned char t) {
  //Hardware implementation:
  //s=(t*t)>>8;
  //return (t&0x80? (((t<<1)-s)<<1)-1 : s);
  unsigned char s=(t*t)>>7;
  return (t&0x80? ((t<<2)-s)-1:s);
}

//Base-2 exponential function
//This function is only assured good for x in range [-0x10000,0]
//Input: signed 1.16-fixed
//Output: unsigned .16-fixed
unsigned int mmExp2(int t) {
  t=(t+0x20000)>>6; //Add 2
  return ((((t*t)>>4)*0xaa)+0x1555555)>>10; //Approximate exp2(x-0x20000)
}

//Natural exponential function
//Input: signed 1.16-fixed
//Output: unsigned .16-fixed
unsigned int mmExp(int t) {
  bool negative=(t<0);
  t=mmAbs(t);
  int x,y;
  y=0x10000;
  x=t-0x58b91; if (x>=0) t=x,y<<=8;
  x=t-0x2c5c8; if (x>=0) t=x,y<<=4;
  x=t-0x162e4; if (x>=0) t=x,y<<=2;
  x=t-0x0b172; if (x>=0) t=x,y<<=1;
  x=t-0x067cd; if (x>=0) t=x,y+=y>>1;
  x=t-0x03920; if (x>=0) t=x,y+=y>>2;
  x=t-0x01e27; if (x>=0) t=x,y+=y>>3;
  x=t-0x00f85; if (x>=0) t=x,y+=y>>4;
  x=t-0x007e1; if (x>=0) t=x,y+=y>>5;
  x=t-0x003f8; if (x>=0) t=x,y+=y>>6;
  x=t-0x001fe; if (x>=0) t=x,y+=y>>7;
  if (t&0x100)               y+=y>>8;
  if (t&0x080)               y+=y>>9;
  if (t&0x040)               y+=y>>10;
  if (t&0x020)               y+=y>>11;
  if (t&0x010)               y+=y>>12;
  if (t&0x008)               y+=y>>13;
  if (t&0x004)               y+=y>>14;
  if (t&0x002)               y+=y>>15;
  if (t&0x001)               y+=y>>16;
  return (negative? (0x1000000/(y>>8)) : y);
}

//Convolves two 15-element arrays and outputs to an array 15 convolution
//evaluations at different positions as rev(vb[]) is right-shifted across va[],
//keeping only the 15 central evaluations (truncating 7 evaluations on each
//side)
//Input: signed 16.16-int
//Output: signed 16.16-fixed
void mmTrunShiftConvolveVec15(int* vr,int* va,int* vb) {
  for(int j=8;j<=15;j++) {
    *vr=0;
    vb+=j;
    for(int i=0;i<j;i++) {
      vb--;
      *vr+=(*va>>8)*(*vb>>8);
      va++;
    }
    va-=j;
    vr++;
  }
  for(int j=14;j>=8;j--) {
    *vr=0;
    vb+=j+1;
    for(int i=0;i<j;i++) {
      va++;
      vb--;
      *vr+=(*va>>8)*(*vb>>8);
    }
    va-=j-1;
    vr++;
  }
}

//Correlates two 15-element arrays and outputs to an array 15 correlation
//evaluations at different positions as vb[] is right-shifted across va[],
//keeping only the 15 central evaluations (truncating 7 evaluations on each
//side)
//Input: signed 16.16-int
//Output: signed 16.16-fixed
void mmTrunShiftCorrelateVec15(int* vr,int* va,int* vb) {
  for(int j=8;j<15;j++) {
    *vr=0;
    va+=15-j;
    for(int i=0;i<j;i++) {
      *vr+=(*va>>8)*(*vb>>8);
      va++;
      vb++;
    }
    va-=15;
    vb-=j;
    vr++;
  }
  for(int j=15;j>=8;j--) {
    *vr=0;
    for(int i=0;i<j;i++) {
      *vr+=(*va>>8)*(*vb>>8);
      va++;
      vb++;
    }
    va-=j;
    vb-=j-1;
    vr++;
  }
}

//Normalizes 15-vector
//Input: signed 8.16-int
//Output: signed .16-fixed
void mmNormalizeVec15(int* v) {
  int mag=0;
  for(int i=0;i<15;i++) {
    mag+=(*v>>8)*(*v>>8);
    v++;
  }
  mag=mmSqrRt(mag);
  v-=15;
  for(int i=0;i<15;i++) {
    *v=(*v<<8)/mag;
    v++;
  }
}
