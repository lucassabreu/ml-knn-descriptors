#include "libtexturedescriptors.h"

void Extraction(char *img_path, char *fv_path)
{
    CImage *cimg=NULL;
    Image *img=NULL;
    ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    Histogram *lbp=NULL;
    int i,j;

    cimg = ReadCImage(img_path);

    //Alocacao da imagem no espaco HSV
    imgHSV = (ImageHSV**) calloc(cimg->C[0]->nrows, sizeof(ImageHSV*));
    for (i=0; i<cimg->C[0]->nrows; i++) {
        imgHSV[i] = (ImageHSV*) calloc(cimg->C[0]->ncols, sizeof(ImageHSV));
    }
    //converter para HSV
    RGB2HSV_lbp(cimg, imgHSV);

    img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

    //copia canal V do HSV para a imagem cinza
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = imgHSV[i][j].V;
        }
    }
    lbp = LBPri(img);
    WriteImage(img, "HSV.pgm");
    WriteFileHistogram(lbp,fv_path);

    //////////////////////////////////////
    //liberando memoria imgHSV
    for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgHSV[i]);
    }
    free(imgHSV);
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
