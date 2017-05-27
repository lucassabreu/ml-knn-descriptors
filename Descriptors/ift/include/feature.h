#ifndef _FEATURE_H_
#define _FEATURE_H_

#include "common.h"
#include "dimage.h"
#include "cimage.h"

typedef struct _felem {
  float *feat;
} FElem;


typedef struct _features {
  FElem *elem;
  int  nfeats;
  int  nelems;
  int  nrows,ncols;
  int  Imax;
} Features;

/// Allocates memory for features
Features* CreateFeatures(int ncols, int nrows, int nfeats);

/// Deallocates memory for features
void      DestroyFeatures(Features **f);

Features* CopyFeatures(Features* feat);

///Gets the given feature for all image pixels
///Returns a double image with the features
DImage* GetFeature(Features* feats, int index);

int SetFeature(Features* feats, int index, DImage* imagefeats);

///Concatenates 2 features.
///If either features are NULL, or they are of different sizes,
///it will be returned NULL
///IMPORTANT: The features are not deallocated
Features* ConcatFeatures(Features* f1, Features* f2);

/// Used to return the image(Features*) to its original size,
/// after it is power-of-2-sided by the Steerable
/// Pyramid filter. Actually, the image is trimmed until
/// it becomes ncols width and nrows height.
Features* TrimFeatures(Features* feats,int ncols,int nrows);

/// Normalize features
void FNormalizeFeatures(Features *f);


// using reconstructions
Features *MSImageFeats(Image *img, int nscales);
Features *MSCImageFeats(CImage *cimg, int nscales);
// using convolution with gaussian filters
Features *LMSImageFeats(Image *img, int nscales);
Features *LMSCImageFeats(CImage *cimg, int nscales);

Features *MarkovImageFeats(Image *img, float dm);
Features *KMarkovImageFeats(Image *img); // for image compression
Features *MarkovCImageFeats(CImage *cimg, float dm);

/// Lab extraction
Features* LabFeats(CImage* cimg);
/// Shifts Lab negative values
void NormalizeLab(Features *lab);

///Extracts Multi-scale Low-Pass features
Features* MSLowPassFeats(Features* feats, int nscales);

///Extracts Steerable Pyramid Features from the feats
Features* SteerablePyramidFeats(Features* feats, int nscales, int norientations);
///Extracts Steerable Pyramid features from feats and shifts them according the energy
///computed for the scales and orientations
Features* ShiftedSPFeats(Features* feats, int nscales, int norientations);
///Extracts Lab and uses Steerable Pyramid on each band
Features* CLabShiftedSPFeats(CImage* cimg, int nscales, int norientations);


#endif // _FEATURE_H_
