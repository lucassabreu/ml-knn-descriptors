/*                     csd.h                                                      */
/* Funções auxiliares para extração e cálculo de distância do descritor CSD-MPEG7 */
/* Fernando Cesar Akune RA 970709                                                 */
/* Última alteração: 28/10/2008 18:10                                             */

#ifndef _CSD_H_
#define _CSD_H_

#include "cimage.h"
#include "histogram.h"

typedef struct _Property {
  int color;
} Property;

typedef struct _VisualFeature {
  ulong *colorH;
  int n;
} VisualFeature;

typedef struct _CompressedVisualFeature {
  uchar *colorH;
  int n;
} CompressedVisualFeature;

CImage *CImageRGBtoHMMD(CImage *cimg);
int RGB2HMMD(int vi);
Histogram *CSD(CImage *cimg);
CompressedVisualFeature *ExtractCompressedVisualFeatures(CImage *cimg);
Property *ComputePixelsProperties(CImage *cimg);
int *QuantizeColors(CImage *cimg);
void CompressHistogram(uchar *ch, ulong *h, ulong max, int size);
CompressedVisualFeature *CompressHistograms(VisualFeature *vf, int npixels);
void DestroyCompressedVisualFeature(CompressedVisualFeature **cvf);
inline uchar ComputeNorm(float value);
VisualFeature *ComputeHistograms(Property *p, int nrows, int ncols, int npixels, int *npoints);
CompressedVisualFeature *CreateCompressedVisualFeature(int n);
void DestroyVisualFeature(VisualFeature **vf);
VisualFeature *CreateVisualFeature(int n);
Property *AllocPropertyArray(int n);
double CSDDistance(Histogram *h1, Histogram *h2);

#endif
