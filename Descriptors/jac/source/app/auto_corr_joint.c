#include "plugin.h"
#include "../include/libcolordescriptors.h"


void Extraction(char *img_path, char *fv_path) {
	CImage *cimg=NULL;
	JointAutoCorrelogram *descr=NULL;

	cimg = ReadCImage(img_path);

	descr = JointAutoCorrelogramImage(cimg);

	WriteFileJointAutoCorrelogram(descr, fv_path);
	DestroyJointAutoCorrelogram(&descr);
	DestroyCImage(&cimg);

}

double Distance(char *fv1_path, char *fv2_path) {
	JointAutoCorrelogram *descr1=NULL;
	JointAutoCorrelogram *descr2=NULL;
	double distance;

	descr1 = ReadFileJointAutoCorrelogram(fv1_path);
	descr2 = ReadFileJointAutoCorrelogram(fv2_path);

	distance = (double) L1JointAutoCorrelogramDistance(descr1, descr2);

	DestroyJointAutoCorrelogram(&descr1);
	DestroyJointAutoCorrelogram(&descr2);

	return distance;
}
