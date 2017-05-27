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


int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr,"usage: lbp_extraction <image_path> <fv_path>\n");
        exit(-1);
    }

    Extraction(argv[1], argv[2]);

    return(0);
}
