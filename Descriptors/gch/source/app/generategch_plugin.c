#include "libcolordescriptors.h"

void Extraction(char *img_path, char *fv_path)
{
  //  timer *tic, *toc;
  CImage *cimg=NULL;
  Image *mask=NULL;
  Histogram *gch=NULL;

  cimg = ReadCImage(img_path);
  //mask = ReadImage(img_path);
  mask = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);


  //  tic = Tic();
  gch = GCH(cimg, mask);
  //  toc = Toc();
  //  printf("CreateGch in %f milliseconds\n\n",CTime(tic, toc));

  WriteFileHistogram(gch,fv_path);
  DestroyHistogram(&gch);
  DestroyCImage(&cimg);
  DestroyImage(&mask);

}


double Distance(char *fv1_path, char *fv2_path)
{
//  timer *tic, *toc;
  Histogram *gch1=NULL;
  Histogram *gch2=NULL;
  //ulong distance;
  double distance;

  gch1 = ReadFileHistogram(fv1_path);
  gch2 = ReadFileHistogram(fv2_path);

//  tic = Tic();
  distance = (double) L1Distance(gch1, gch2);
//  toc = Toc();

  //#printf("%f\n",distance);

  DestroyHistogram(&gch1);
  DestroyHistogram(&gch2);

  return distance;
}
