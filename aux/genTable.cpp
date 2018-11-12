////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
//Used to generate tables in project files.
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>

#define sqr(x) ((x)*(x))
#define M_E 2.71828182845904523536
#define M_LOG2E 1.44269504088896340736
#define M_LOG10E 0.434294481903251827651
#define M_LN2 0.693147180559945309417
#define M_LN10 2.30258509299404568402
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_PI_4 0.785398163397448309616
#define M_1_PI 0.318309886183790671538
#define M_2_PI 0.636619772367581343076
#define M_1_SQRTPI 0.564189583547756286948
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2 1.41421356237309504880
#define M_SQRT_2 0.707106781186547524401

int main() {
  FILE* fileOut;
  fileOut=fopen("table.txt","w");
/*
  //Used to generate a percentage table where "128"=100%
  for(int i=0;i<100;i++) {
    fprintf(fileOut,"%d o/o = %d\n",i,(int)(((128.*i)/100.)+.5));
  }
*/
/*
  //Used to generate table weight index categories in "snookx.h":
  double t;
  int initial[22]={65,96,59,96,40,77,51,87,51,75,51,73,130,130,80,105,130,130,
                   76,105,73,99};
  int diff[22]={3,4,3,4,2,3,-1,3,-1,3,-1,3,3,3,4,5,3,3,4,5,3,-2};
  int detail[2][18]={{51,48,45,43,41,39,37,35,33,31,29,27,25,23,21,18,15,12},
                     {99,94,89,85,81,77,73,69,65,61,57,53,49,45,41,36,31,-1}};
  int res;
  for(int j=0;j<22;j++) {
    fprintf(fileOut,"\n\nTABLE %d %c",(j>>1)+1,(j&1? 0x4d : 0x46));
    fprintf(fileOut,"\n\nIN POUNDS:");
    for(int i=0;i<(((j>>1)==6||(j>>1)==8)+1)*18-((j>>1)==10);i++) {
      if (i%19==0) fprintf(fileOut,"\n  ");
      if (diff[j]<0) res=detail[-diff[j]-1][i];
      else res=(initial[j]-diff[j]*i);
      fprintf(fileOut,"%3d,",res);
    }
    fprintf(fileOut,"\n\nIN NEWTONS:");
    for(int i=0;i<(((j>>1)==6||(j>>1)==8)+1)*18-((j>>1)==10);i++) {
      if (i%19==0) fprintf(fileOut,"\n  ");      
      if (diff[j]<0) res=detail[-diff[j]-1][i];
      else res=(initial[j]-diff[j]*i);
      fprintf(fileOut,"%3d,",(int)(res*4.44822162+.5));
    }
  }
*/
/*
  //Used to generate mmSCurveTable[] in "moremath.h":
  double t;
  for(int i=0;i<256;i++) {
    t=i/256.;
    if (i%16==0) fprintf(fileOut,"\n  ");
    fprintf(fileOut,"%3d,",(int)(256*t*t*t*(t*(t*6-15)+10)));
  }
*/
/*
  //Used to generate mmSinTable[] in "moremath.h":
  double t;
  for(int i=0;i<256;i++) {
    t=i/256.;
    if (i%8==0) fprintf(fileOut,"\n  ");
    fprintf( fileOut,"%5d,",(int)(65536.*sin(i*3.141592653/512.)) );
  }
*/
/*
  //Used to generate mmSinAcosTable[] in "moremath.h":
  double t;
  for(int i=0;i<256;i++) {
    t=i/256.;
    if (i%8==0) fprintf(fileOut,"\n  ");
    fprintf(fileOut,"%3d,",(int)(sin(acos((float)i/256))*0x10000));
  }
*/
/*
  //Used to generate mmAcosTable[] in "moremath.h":
  double t;
  for(int i=0;i<256;i++) {
    t=i/256.;
    if (i%16==0) fprintf(fileOut,"\n  ");
    //Note values in table must be increased by a value of 1 before use
    //They are reduced to fit into an unsigned char
    fprintf(fileOut,"%3d,",(int)(acos((float)i/256)*0x100*2/3.141592653)-1);
  }*/
/*
  //Used to generate mmTanTable[] in "moremath.cpp":
  double t;
  for(int i=0;i<512;i++) {
    t=i/512.;
    if (i%8==0) fprintf(fileOut,"\n  ");
    fprintf(fileOut,"%8d,",(int)(65536.*tan(t*.5*3.141592653)));
  }
*/
/*
  //Used to generate commonSymbol[] in "common.h":
  FILE *fileIn=fopen("0.bmp","rb");
  unsigned char buff[3*256*8*16],*p=buff,h;
  fread(buff,0x36,1,fileIn); //Eat header
  fread(buff,3*256*8*16,1,fileIn);
  for(int k=0;k<256;k++) {
    for(int j=0;j<16;j++) {
      h=0;
      for(int i=0;i<8;i++) {
        h|=(*p<200)<<i;
        p+=3;
      }
      fprintf(fileOut,"0x");
      if (h<0x10) fprintf(fileOut,"0");
      fprintf(fileOut,"%x,",h);
      p+=3*255*8;
    }
    fprintf(fileOut,"\n");
    p=buff+3*k*8;
  }
*/
///*
  //Used to generate quantization table for normals in "Splat.h":
  //Because we quantize normals so severely, there are often banding artifacts
  //in regions of very low curvature.  To try to minimize the effect of this,
  //we dither the normals by adding a bit of random noise before we quantize
  #define DITHER_NORMALS
  #define N 52
  //Defines warping so that quantized normals cover sphere more evenly
  #define WARP(x) (min(max(float(atan(x)*4.0f/M_PI), -1.0f), 1.0f))
  #define UNWARP(x) (min(max(float(tan((x)*M_PI*0.25f)), -1.0f), 1.0f))
  float *normquant_table = new float[3*16384];
	static const float twoNths = 2.0f / (float)N;
	int index = 0;
	for (unsigned y = 0; y < N; y++) {
	  for (unsigned z = 0; z < N; z++) {
		float ny = UNWARP((((float)y + 0.5f) * twoNths) - 1.0f);
		float nz = UNWARP((((float)z + 0.5f) * twoNths) - 1.0f);
		float nx = sqrtf(1.0f-ny*ny-nz*nz);
		normquant_table[index++] = nx;
		normquant_table[index++] = ny;
		normquant_table[index++] = nz;
	  }
	}
	int i;
	for (i=0; i < N*N; i++) {
		normquant_table[index++] = -normquant_table[3*i  ];
		normquant_table[index++] =  normquant_table[3*i+1];
		normquant_table[index++] =  normquant_table[3*i+2];
	}
	for (unsigned z = 0; z < N; z++) {
	  for (unsigned x = 0; x < N; x++) {
		float nz = UNWARP((((float)z + 0.5f) * twoNths) - 1.0f);
		float nx = UNWARP((((float)x + 0.5f) * twoNths) - 1.0f);
		float ny = sqrtf(1.0f-nz*nz-nx*nx);
		normquant_table[index++] = nx;
		normquant_table[index++] = ny;
		normquant_table[index++] = nz;
	  }
	}
	for (i=N*N*2; i < N*N*3; i++) {
		normquant_table[index++] =  normquant_table[3*i  ];
		normquant_table[index++] = -normquant_table[3*i+1];
		normquant_table[index++] =  normquant_table[3*i+2];
	}
	for (unsigned x = 0; x < N; x++) {
	  for (unsigned y = 0; y < N; y++) {
		float nx = UNWARP((((float)x + 0.5f) * twoNths) - 1.0f);
		float ny = UNWARP((((float)y + 0.5f) * twoNths) - 1.0f);
		float nz = sqrtf(1.0f-nx*nx-ny*ny);
		normquant_table[index++] = nx;
		normquant_table[index++] = ny;
		normquant_table[index++] = nz;
	  }
	}
	for (i=N*N*4; i < N*N*5; i++) {
		normquant_table[index++] =  normquant_table[3*i  ];
		normquant_table[index++] =  normquant_table[3*i+1];
		normquant_table[index++] = -normquant_table[3*i+2];
	}
	for (i=N*N*6; i < 16384; i++) {
		normquant_table[index++] = 0.0f;
		normquant_table[index++] = 0.0f;
		normquant_table[index++] = 1.0f;
	}
  for(i=0;i<N*N*6*3;i++) {
    if (!(i%9)) fprintf(fileOut,"\n ");
    if (!(i%3)) fprintf(fileOut,"   ");
    int z=(int)(32767.99*normquant_table[i]);
    if (z<-32768||z>32767) z=0;
    fprintf(fileOut,"%6d,",z);
  }
//*/
/*
  //Used to generate quantization table for spheres (radius,xPos,yPos,zPos) in "Splat.h":
  #define N 13
  #define INVALID 0xffff
  #undef PARANOIA
  //Set up the quantization tables
	static const float oneNth = 1.0f / float(N);
	static const float sqrt3 = sqrtf(3.0f);
	int i;
  float *spherequant_table = new float[4 * (1 << 13)];
	unsigned short *spherequant_enc_table = new unsigned short[N * N * N * N];
	float *tbl = spherequant_table;
	for (i = 0; i < N*N*N*N; i++) spherequant_enc_table[i] = INVALID;
	i = 0;
	for (int size = 1; size <= N; size++) {
		for (int x = 0; x < N+1-size; x++)
		  for (int y = 0; y < N+1-size; y++)
		    for (int z = 0; z < N+1-size; z++) {
			    float X = (size+2*x)*oneNth - 1.0f;
			    float Y = (size+2*y)*oneNth - 1.0f;
			    float Z = (size+2*z)*oneNth - 1.0f;
			    float R = size*oneNth;
			    if (X*X + Y*Y + Z*Z > sqr(1.0f+sqrt3*R)) continue;
			    *tbl++ = X; *tbl++ = Y; *tbl++ = Z; *tbl++ = R;
			    spherequant_enc_table[size-1+N*(x+N*(y+N*z))] = i++;
		    }
	}
  for(i=0;i<(4*(1<<13));i++) {
    if (!(i%8)) fprintf(fileOut,"\n ");
    if (!(i%4)) fprintf(fileOut,"   ");
    int z=(int)(32767.99*spherequant_table[i]);
    if (z<-32768||z>32767) z=0;
    fprintf(fileOut,"%6d,",z);
  }
*/
  printf("\n\nOutput written\n\n");
  getch();
  return 0;
}
