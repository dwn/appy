////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
////////////////////////////////////////////////////////////////////////////////
#include "Splat.h"

Splat::Splat(unsigned short* buffSet,unsigned short* zInvBuffSet) {
  tta=0;
  buff=buffSet;
  zInvBuff=zInvBuffSet;
  mat[3]=0<<16; mat[7]=0<<16; mat[11]=3200<<SPLAT_LENSCALE;
  //Build fragment list
  //File just looks like files for individual fragments concatenated
  float xMin=3.3e33f,xMax=-3.3e33f;
  float yMin=3.3e33f,yMax=-3.3e33f;
  float zMin=3.3e33f,zMax=-3.3e33f;
  numLeafPoints=0;
  const unsigned char *ptr=splatModel;
  while(ptr<splatModel+splatModelSize) {
    //Data in fragment header:
    //QSplat 6B
    //Version 2B
    //lenFrag 4B
    //numLeafPtsInFrag 4B
    //options 4B
    //cx 4B
    //cy 4B
    //cz 4B
    //r 4B
    //numChildren 4B
    int lenFrag=*(int*)(ptr+8); FIX_LONG(lenFrag);
    if ((*(unsigned char*)(ptr+19))&2) {
      // comments.append((const char*)(ptr+20),lenFrag-20);
      ptr+=lenFrag;
      continue;
    }
    int numLeafPtsInFrag=*(int*)(ptr+12); FIX_LONG(numLeafPtsInFrag);
    numLeafPoints+=numLeafPtsInFrag;
    float cx=*(float*)(ptr+20); FIX_FLOAT(cx);
    float cy=*(float*)(ptr+24); FIX_FLOAT(cy);
    float cz=*(float*)(ptr+28); FIX_FLOAT(cz);
    float r=*(float*)(ptr+32); FIX_FLOAT(r);
    xMin=fmin(xMin,cx-r); xMax=fmax(xMax,cx+r);
    yMin=fmin(yMin,cy-r); yMax=fmax(yMax,cy+r);
    zMin=fmin(zMin,cz-r); zMax=fmax(zMax,cz+r);
    fragments.push_back(ptr);
    ptr+=lenFrag;
  }
  center[0]=0.5f*(xMin+xMax);
  center[1]=0.5f*(yMin+yMax);
  center[2]=0.5f*(zMin+zMax);
  radius=0.5f*sqrtf(sqr(xMax-xMin)+sqr(yMax-yMin)+sqr(zMax-zMin));
  // char buf[255];
  // sprintf(buf,"%d leaf pts\n",numLeafPoints);
  // comments+=buf;
}

Splat::~Splat() {
}

void Splat::update() {
  // setZInvBufferClear(); //TODO: Remove this
  mmSetRotMat3x4(mat,tta,tta<<1,tta<<2);
  tta++;
  // int xScreen,yScreen,width,height,vRes[3],nRes[3],*vCurr,zInv,tmp;
  int numChildren;
  //Draw each fragment
  for (int f=0;f<fragments.size();f++) {
    float cx,cy,cz,r;
    const unsigned char *ptr=splatModel;
    unsigned int options=*(unsigned int*)(ptr+16); FIX_LONG(options);
    bool hasColor=options&1;
    int nodeSize=(hasColor? 6 : 4);
    cx=*(float*)(ptr+20); FIX_FLOAT(cx);
    cy=*(float*)(ptr+24); FIX_FLOAT(cy);
    cz=*(float*)(ptr+28); FIX_FLOAT(cz);
    r=*(float*)(ptr+32); FIX_FLOAT(r);
    numChildren=*(int*)(ptr+36); FIX_LONG(numChildren);
    ptr+=40;
   // splatDrawHierarchy(here,numChildren,cx,cy,cz,r,true,true);
  }
}

// void splatSphereQuantLookup(const unsigned char* ptr,
//   float pcx,float pcy,float pcz,float pr,
//   float& cx,float& cy,float& cz,float& r) {
//   unsigned short index=*(const unsigned short*)ptr; FIX_SHORT(index);
//   float *rq=(float*)(
//     (const unsigned char*)splatSphereQuantTable+
//     ((unsigned(index)<<1)&0x1fff0u));
//   cx=pcx+pr*(*rq++);
//   cy=pcy+pr*(*rq++);
//   cz=pcz+pr*(*rq++);
//   r=pr*(*rq);
// }

// const float* splatNormalQuantLookup(const unsigned char* ptr) {
//   unsigned short index=*(const unsigned short*)ptr; FIX_SHORT(index);
//   return splatNormalQuantTable+3*(index>>2);
// }

// float splatNormalQuantLookupCone(const unsigned char* ptr) {
//   return -0.0625f*(1+(ptr[1]&3))*(1+(ptr[1]&3));
// }

// #ifdef NEED_ALIGNED_ACCESSES
// // XXXXXX - assumes p has 2-byte alignment, and assumes big endian
// #define UNALIGNED_DEREFERENCE_INT(p) (int( \
//   (*((unsigned short*)(p))<<16) | *(((unsigned short*)(p))+1)))
// #else
// #define UNALIGNED_DEREFERENCE_INT(p) (*(int*)(p))
// #endif

// #define FAST_CUTOFF (2.3f*minSize)

// #define TMP_TO_FLT (1./32768.);

// //Local variables
// static float pixels_per_radian, recursion_thresh;
// static float frustum[4][4];
// static float zproj[4];
// static float minSize;
// static bool bail;
// static bool hasColor;
// static int nodeSize;

// //At lowest level of hierarchy, so draw a bunch of leaf nodes without testing sizes
// void splatDrawHierarchyLeaves(const unsigned char* ptr,int numNodes,
//   float cx,float cy,float cz,float r,float approxSplatSizeScale) {
//   for(int i=0;i<numNodes;i++,ptr+=nodeSize) {
//     float cx,cy,cz,r;
//     splatSphereQuantLookup(ptr,cx,cy,cz,r,cx,cy,cz,r);
//     float splatSize=approxSplatSizeScale? r*approxSplatSizeScale : minSize;
//     // GUI->drawpoint(cx,cy,cz,r,splatSize,
//     //                splatNormalQuantLookup(ptr+2),
//     //                NULL);/////////////hasColor? splatColorQuantLookup(ptr+4):NULL);
//  }
// }

// //Fast version of the draw routine, for when size is only a few pixels
// void splatDrawHierarchyFast(const unsigned char *ptr,int numNodes,
//                             float cx,float cy,float cz,float r) {
//  int childoffset=UNALIGNED_DEREFERENCE_INT(ptr); FIX_LONG(childoffset);
//  const unsigned char *childPtr=ptr+childoffset;
//  ptr+=4;
//  int numChildren=0;
//  int grandchildren=0;
//  for (int i=0;i<numNodes;i++,ptr+=nodeSize,childPtr+=nodeSize*numChildren+grandchildren) {
//    numChildren=ptr[1] & 3;
//    if (numChildren) {
//      numChildren++;
//      grandchildren=ptr[1] & 4;
//    } else { grandchildren=0; }
//    float mycx,mycy,mycz,myr;
//    splatSphereQuantLookup(ptr,cx,cy,cz,r,mycx,mycy,mycz,myr);
//    float z=zproj[0]*mycx+zproj[1]*mycy+zproj[2]*mycz+zproj[3];
//    float splatSize_scale=2.0f*pixels_per_radian/z;
//    float splatSize=myr*splatSize_scale;
//    if (!numChildren||(splatSize<=minSize)) {
//      // GUI->drawpoint(mycx,mycy,mycz,myr,splatSize,
//      //                splatNormalQuantLookup(ptr+2),
//      //                NULL);///////////////////hasColor? splatColorQuantLookup(ptr+4):NULL);
//    } else if (!grandchildren) {
//      splatDrawHierarchyLeaves(childPtr,numChildren,mycx,mycy,mycz,myr,
//                            splatSize_scale);
//    } else if (splatSize<=FAST_CUTOFF) {
//      splatDrawHierarchyLeaves(childPtr+4,numChildren,mycx,mycy,mycz,myr,0.0f);
//    } else { splatDrawHierarchyFast(childPtr,numChildren,mycx,mycy,mycz,myr); }
//  }
// }

// void splatDrawHierarchy(const unsigned char *ptr,int numNodes,
//                         float cx,float cy,float cz,float r,bool backfacecull,
//                         bool frustumcull) {
//   //Wptr are the children of these nodes stored?
//   int childoffset=UNALIGNED_DEREFERENCE_INT(ptr); FIX_LONG(childoffset);
//   const unsigned char *childPtr=ptr+childoffset;
//   ptr+=4;
//   int numChildren=0;
//   int grandchildren=0;
//   //For each node in this group of siblings
//   for(int i=0;i<numNodes;i++,ptr+=nodeSize,childPtr+=nodeSize*numChildren+grandchildren) {
//     //Find number of children
//     numChildren=ptr[1]&3;
//     if (numChildren) {
//       //Value of 0 means no children, but since 1 child never happens, 1 really
//       //means 2 children and so on
//       numChildren++;
//       //This bit is set if this node has any grandchildren, and hence has the
//       //extra pointer; note that because this uses bit 2, we automatically
//       //get the right increment to the child offset
//       grandchildren = ptr[1] & 4;
//     } else { grandchildren = 0; }
//     // Determine our position and radius
//     float mycx,mycy,mycz,myr;
//     splatSphereQuantLookup(ptr,cx,cy,cz,r,mycx,mycy,mycz,myr);
//     // Determine perpendicular distance to screen plane
//     float z=zproj[0]*mycx+zproj[1]*mycy+zproj[2]*mycz+zproj[3];
//     // Frustum culling
//     bool frustumcull_children=frustumcull;
//     if (frustumcull) {
//       if ((z <= -myr) ||
//           (mycx*frustum[0][0] + mycy*frustum[0][1] +
//            mycz*frustum[0][2] + frustum[0][3] <= -myr) ||
//           (mycx*frustum[1][0] + mycy*frustum[1][1] +
//            mycz*frustum[1][2] + frustum[1][3] <= -myr) ||
//           (mycx*frustum[2][0] + mycy*frustum[2][1] +
//            mycz*frustum[2][2] + frustum[2][3] <= -myr) ||
//           (mycx*frustum[3][0] + mycy*frustum[3][1] +
//            mycz*frustum[3][2] + frustum[3][3] <= -myr))
//         continue;
//       if ((z > myr) &&
//           (mycx*frustum[0][0] + mycy*frustum[0][1] +
//            mycz*frustum[0][2] + frustum[0][3] >= myr) &&
//           (mycx*frustum[1][0] + mycy*frustum[1][1] +
//            mycz*frustum[1][2] + frustum[1][3] >= myr) &&
//           (mycx*frustum[2][0] + mycy*frustum[2][1] +
//            mycz*frustum[2][2] + frustum[2][3] >= myr) &&
//           (mycx*frustum[3][0] + mycy*frustum[3][1] +
//            mycz*frustum[3][2] + frustum[3][3] >= myr))
//         frustumcull_children = false;
//     }
//     // Backface culling
//     bool backfacecull_children=backfacecull;
//     float camdotnorm=0.0f; //If we're not doing backface culling, this gets
//                            //left set to a safe value for the sake of code
//                            //later on
//     if (backfacecull && ((ptr[3]&3)!=3)) {
//       const short *norm=splatNormalQuantLookup(ptr+2);
//       float camx=0.-mycx;
//       float camy=0.-mycy;
//       float camz=0.-mycz;
//       camdotnorm=camx*(norm[0]*TMP_TO_FLT)+camy*(norm[1]*TMP_TO_FLT)+camz*(norm[2]*TMP_TO_FLT);
//       if (camdotnorm < -myr) {
//         float camdist2=sqr(camx)+sqr(camy)+sqr(camz);
//         float cone=splatNormalQuantLookupCone(ptr+2);
//         if (sqr(camdotnorm+myr)>camdist2*sqr(cone)) {
//           continue;
//         }
//       } else if (camdotnorm>myr) {
//         float camdist2=sqr(camx)+sqr(camy)+sqr(camz);
//         float cone=splatNormalQuantLookupCone(ptr+2);
//         if (sqr(camdotnorm-myr)>camdist2*sqr(cone)) {
//           backfacecull_children=false;
//         }
//       }
//     }
//     // Yes, we actually have to do a divide
//     float splatSize_scale=2.0f*pixels_per_radian/z;
//     float splatSize=myr*splatSize_scale;
//     //Check whether we recurse
//     if (!numChildren||((z>0.0f) && (splatSize<=minSize))) {
//       //No - draw now
//       if ((z>0.0f) && (camdotnorm>=0.0f)) {
//         // GUI->drawpoint(mycx,mycy,mycz,myr,splatSize,
//         //                splatNormalQuantLookup(ptr+2),
//         //                NULL);////////////hasColor?QSplatColorQuantLookup(ptr+4):NULL);
//       }
//     } else if (!grandchildren) {
//       //We recurse, but children are all leaf nodes
//       splatDrawHierarchyLeaves(childPtr,numChildren,mycx,mycy,mycz,myr,
//                             splatSize_scale);
//     } else if ((!frustumcull_children && !backfacecull_children) ||
//                ((splatSize<=FAST_CUTOFF) && (z>0.0f))) {
//       // We recurse, but switch to fast mode
//       if (splatSize<=FAST_CUTOFF) {
//         //If we are this small, the next round of recursion is going to be
//         //close to minSize, so we use the leaves function to just draw the
//         //children
//         splatDrawHierarchyLeaves(childPtr+4, numChildren,mycx,mycy,mycz,myr,0.0f);
//       } else { splatDrawHierarchyFast(childPtr,numChildren,mycx,mycy,mycz,myr); }
//     } else {
//       //Basic slow-mode recursion
//       splatDrawHierarchy(childPtr,numChildren,mycx,mycy,mycz,myr,backfacecull_children,
//                          frustumcull_children);
//       if (bail) return;
//     }
//   }
// }
