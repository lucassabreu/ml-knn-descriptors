#include "libtexturedescriptors.h"

void Extraction(char *img_path, char *fv_path)
{

    CImage *cimg=NULL;
    Image *img=NULL;
    ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    FeatureVector1D *sasi=NULL;
    int i,j;
    int sizes[3];
    sizes[0]=3;
    sizes[1]=5;
    sizes[2]=7;
    FILE *fout;

    cimg = ReadCImage(img_path);
    img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

    //Alocacao da imagem no espaco HSV
    imgHSV = (ImageHSV**) calloc(cimg->C[0]->nrows, sizeof(ImageHSV*));
    for (i=0; i<cimg->C[0]->nrows; i++) {
        imgHSV[i] = (ImageHSV*) calloc(cimg->C[0]->ncols, sizeof(ImageHSV));
    }

    //converte para HSV
    RGB2HSV_sasi(cimg, imgHSV);

    //copia canal V do HSV para a imagem cinza
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = imgHSV[i][j].V;
        }
    }

    sasi = SASI(img, 3, sizes);

    //WriteFileFeatureVector1D(sasi,fv_path);
    if ((fout = fopen(fv_path, "wb")) == NULL) {
        printf("ERRO CRIANDO ARQUIVO DE FV\n");
        exit(0);
    }
    fwrite(&sasi->n,sizeof(int),1,fout);
    //fprintf(fout,"%d\n",sasi->n);
    fwrite(sasi->X,sizeof(double),sasi->n,fout);
    fclose(fout);

    //liberando memoria imgHSV
    for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgHSV[i]);
    }
    free(imgHSV);

    DestroyFeatureVector1D(&sasi);
    DestroyCImage(&cimg);
    DestroyImage(&img);

}


int main(int argc, char** argv)
{

  if (argc != 3) {
    fprintf(stderr,"usage: gch_extraction <image_path> <fv_path>\n");
    exit(-1);
  }

  Extraction(argv[1], argv[2]);

  return(0);
}
