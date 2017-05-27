#ifndef _MPEG7HTD_H_
#define _MPEG7HTD_H_

#include "cimage.h"
#include "matrix.h"

typedef struct {
	int scale, orientation;
	float *media, *desvio;
} Mpeg7HTDDescriptor;

Mpeg7HTDDescriptor *ExtractMpeg7HTDDescriptor(CImage *rgbImage);
void WriteFileMpeg7HTDDescriptor(Mpeg7HTDDescriptor *descr, char *fv_path);
void DestroyMpeg7HTDDescriptor(Mpeg7HTDDescriptor **descr);
Mpeg7HTDDescriptor *ReadFileMpeg7HTDDescriptor(char *fv_path);
double DistanceMpeg7HTDDescriptor(Mpeg7HTDDescriptor *descr1, Mpeg7HTDDescriptor *descr2);

#endif /*_MPEG7HTD_H_*/

