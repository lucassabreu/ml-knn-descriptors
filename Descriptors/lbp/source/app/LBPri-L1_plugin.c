#include "libtexturedescriptors.h"

void Extraction(char *img_path, char *fv_path)
{

  CImage *cimg=NULL;
  Image *img=NULL;
  Histogram *lbp=NULL;
  int i;

  cimg = ReadCImage(img_path);
  img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);
  
  //translate to HSV space

  for (i=0; i<(img->ncols*img->nrows); i++)
    img->val[i] = (cimg->C[0]->val[i])+(cimg->C[1]->val[i])+(cimg->C[2]->val[i])/3;

  lbp = LBPri(img);

  WriteFileHistogram(lbp,fv_path);
  DestroyHistogram(&lbp);
  DestroyCImage(&cimg);
  DestroyImage(&img);

}


double Distance(char *fv1_path, char *fv2_path)
{
  
  Histogram *lbp1=NULL;
  Histogram *lbp2=NULL;
  double distance;

  lbp1 = ReadFileHistogram(fv1_path);
  lbp2 = ReadFileHistogram(fv2_path);

  distance=(double)L1Distance(lbp1, lbp2);

  DestroyHistogram(&lbp1);
  DestroyHistogram(&lbp2);

  return (distance);
}
