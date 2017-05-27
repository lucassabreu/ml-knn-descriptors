#ifndef _CCD_H_
#define _CCD_H_

#include "cimage.h"
#include "correlogram.h"

AutoCorrelogram *CAD64(CImage *cimg);
IndependentAutoCorrelogram *IndependentAutoCorrelogramImage(CImage *cimgRGB);
JointAutoCorrelogram *JointAutoCorrelogramImage(CImage *cimgRGB);

#endif
