#include "plugin.h"
#include "../include/libcolordescriptors.h"

void Extraction(char *img_path, char *fv_path) {
	CImage *cimg=NULL;
	
	Mpeg7HTDDescriptor *descr=NULL;
	
	cimg = ReadCImage(img_path);

	descr = ExtractMpeg7HTDDescriptor(cimg);

	WriteFileMpeg7HTDDescriptor(descr, fv_path);
	DestroyMpeg7HTDDescriptor(&descr);
	DestroyCImage(&cimg);

}

double Distance(char *fv1_path, char *fv2_path) {
	Mpeg7HTDDescriptor *descr1=NULL;
	Mpeg7HTDDescriptor *descr2=NULL;
	double distance;

	descr1 = ReadFileMpeg7HTDDescriptor(fv1_path);
	descr2 = ReadFileMpeg7HTDDescriptor(fv2_path);

	distance = (double) DistanceMpeg7HTDDescriptor(descr1, descr2);

	DestroyMpeg7HTDDescriptor(&descr1);
	DestroyMpeg7HTDDescriptor(&descr2);

	return distance;
}
