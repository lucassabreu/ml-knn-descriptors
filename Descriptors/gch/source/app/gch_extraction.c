#include "libcolordescriptors.h"

int main(int argc, char** argv)
{
  CImage *cimg=NULL;
  Image *mask=NULL;
  Histogram *gch=NULL;

  if (argc != 3) {
    fprintf(stderr,"usage: gch_extraction <image_path> <fv_path>\n");
    exit(-1);
  }

  cimg = ReadCImage(argv[1]);
  mask = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

  gch = GCH(cimg, mask);

  WriteFileHistogram(gch,argv[2]);
  DestroyHistogram(&gch);
  DestroyCImage(&cimg);
  DestroyImage(&mask);

  return(0);
}
