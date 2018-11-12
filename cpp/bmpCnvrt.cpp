////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
//Used to red-blue-swap colors in bitmap images.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <conio.h>

void main() {
  printf("File to convert? ");
  char file=getche();
  char nameOut[7];
  char *np=nameOut;
  *np++=file; *np++='0'; *np++='.'; *np++='b'; *np++='m'; *np++='p'; *np=NULL;
  char name[6];
  np=name;
  *np++=file; *np++='.'; *np++='b'; *np++='m'; *np++='p'; *np=NULL;
  FILE *f=fopen(name,"rb");
  FILE *fOut=fopen(nameOut,"wb");
  unsigned char in[3*100];
  unsigned char out[3*100];
  unsigned short width,height;
  fread(in,18,1,f); //Eat header
  fread(&width,2,1,f);
  fread(&height,2,1,f);
  fread(&height,2,1,f);
  fclose(f);
  fclose(fOut);
//  width=640; height=480;
  f=fopen(name,"rb");
  fOut=fopen(nameOut,"wb");
  fread(in,0x36,1,f);
  fwrite(in,0x36,1,fOut);  
  for(int i=0;i<width*height;i++) {
    fread(in,3,1,f);
    out[0]=in[2];
    out[1]=in[1];
    out[2]=in[0];
    fwrite(out,3,1,fOut);
  }
  fclose(f);
  fclose(fOut);
  printf("\nDone.\n",nameOut);
//  getch();
}
