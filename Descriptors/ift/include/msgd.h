#ifndef _MSGD_H_
#define _MSGD_H_

#include "morphology.h"
#include "segmentation.h"
#include "cimage.h"
#include "ctree.h"
#include "adjacency.h"
#include "set.h"

typedef struct _msgdreg {
  float  *theta;  // vector of parameters
  float **delta;  // matrix of displacements
  int n,m;        // number of parameters and number of scales 
  Image *grad1;   // gradient of the fixed image
  Set *S;         // set of points extracted from the movable image
  int   ncols2,nrows2;   // number of rows and columns of the movable image
} MSGDReg;

MSGDReg *CreateMSGDReg(int n, int m, Image *grad1, Set *S, \
		       int ncols2, int nrows2);
void     DestroyMSGDReg(MSGDReg **msgd);
void     SetThetaMSGDReg(MSGDReg *msgd, char *theta);
void     SetDeltaMSGDReg(MSGDReg *msgd, char *delta);
Set    *WatershedLines(Image *label);
void MatchingTransform(float T[3][3], float *theta, MSGDReg *msgd);
void MatchingInverseTransform(float T[3][3], float *theta, MSGDReg *msgd);


int    MSGDMatching(float *theta, float *delta, int i, int dir, MSGDReg *msgd);
float  *MSGDSearchTransform(MSGDReg *msgd);
CImage *RegisterCImages(CImage *cimg1, CImage *cimg2, float perc_volume);







#endif





