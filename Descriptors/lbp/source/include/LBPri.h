#ifndef _LBPRI_H_
#define _LBPRI_H_

#include "cimage.h"
#include "histogram.h"
#include "common.h"
#include "image.h"

#define SIZE 10

typedef struct _imageHSV {
  float H,S,V;
} ImageHSV;

Histogram *LBPri(Image *img);
void RGB2HSV_lbp(CImage *RGB, ImageHSV **HSV);

#endif
