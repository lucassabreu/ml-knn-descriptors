// gcc -L ../lib/ -I ../include/ generate_base_las.c -l descriptors -lm -o ../../../../generate_base_las

#include <string.h>

#include "../include/libdescriptors.h"

const int qtd_bins4 = QTD_BINS*QTD_BINS*QTD_BINS*QTD_BINS;

float* Extraction(char *img_path)
{
    CImage *cimg=NULL;
    Image *img=NULL;
    ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    Image *g1=NULL, *g2=NULL, *g3=NULL, *g4=NULL; //imgs dos gradientes LAS
    int i, j;
    float *hist=NULL;

    //leitura da imagem de entrada
    cimg = ReadCImage(img_path);

    //cria imagem HSV
    imgHSV = (ImageHSV**) calloc(cimg->C[0]->nrows, sizeof(ImageHSV*));
    for (i=0; i<cimg->C[0]->nrows; i++) {
        imgHSV[i] = (ImageHSV*) calloc(cimg->C[0]->ncols, sizeof(ImageHSV));
    }

    //converter para HSV
    RGB2HSV_las(cimg, imgHSV);
    //cria imagem cinza
    img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);
    //copia canal V do HSV para a imagem cinza
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = imgHSV[i][j].V;
        }
    }

    //calcula g1, g2, g3, g4
    g1 = CreateImage(img->ncols, img->nrows);  //cria as imgs
    g2 = CreateImage(img->ncols, img->nrows);
    g3 = CreateImage(img->ncols, img->nrows);
    g4 = CreateImage(img->ncols, img->nrows);
    CalculaGradientes(img, g1, g2, g3, g4);

    //hist = (float*) calloc(qtd_bins4, sizeof(float));
    hist = HistogramaGradiente(g1, g2, g3, g4);
    NormalizaHistograma(hist, qtd_bins4, img->nrows*img->ncols);

    //liberando memoria
    for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgHSV[i]);
    }
    free(imgHSV);
    DestroyCImage(&cimg);
    DestroyImage(&img);
    DestroyImage(&g1);
    DestroyImage(&g2);
    DestroyImage(&g3);
    DestroyImage(&g4);

    return hist;
}

int main(int argc, char** argv)
{
   char filename[50];
   int current_class, n, id;
   unsigned i, j;
   unsigned count;
   float **DistanceValue;

   if(argc<2) {
      printf("Use: generate_base_las <id_class_img file>\n");
      return 1;
   }

   FILE *fin = fopen(argv[1],"rt");
   if(fin == NULL) {
      printf("Cannot open file '%s'!\n", argv[1]);
      return 1;
   }

   // load number of images of database
   fscanf(fin,"%d", &n);
   float *fv[n];

   FILE *fout = fopen("LAS.out","wt");
   for(count=0;count<n;count++) {
      if(feof(fin)) {
         fprintf(stderr,"Incomplete file? (%d images instead of %d)          \r",count, n);
         return 1;
      }
      fscanf(fin,"%d %d %s", &id, &current_class, filename);
      strcat(filename,".ppm");
      fprintf(stderr,"extracting %s (%d/%d)...          \r",filename, count, n);
      fv[count] = Extraction(filename);
      fprintf(fout,"%u %u 3 %u\n",2100000+count,count,qtd_bins4);
      for(j=0;j<qtd_bins4;j++)
         fprintf(fout,"%f\n",(float)fv[count][j]);
      fprintf(fout,"\n");
   }
   fclose(fout);
   fclose(fin);
   
  //gera matriz de distancia
  DistanceValue = (float **)malloc(n*sizeof(float *));
  for(i=0;i<n;i++)
     DistanceValue[i] = (float *)malloc(n*sizeof(float));

  fout = fopen("LAS.bin","wb");
  fwrite(&n, sizeof(unsigned), 1, fout);
  for(i=0;i<n;i++) {
      fprintf(stderr, "computing distances (%d/%d)...    \r", i+1, n);
      DistanceValue[i][i] = 0.0;
      for(j=i+1;j<n;j++)
        DistanceValue[i][j] = DistanceValue[j][i] = (float)L1FloatDistance(fv[i],fv[j],qtd_bins4);
      fwrite(DistanceValue[i], sizeof(float), n, fout);
      free(fv[i]);
      free(DistanceValue[i]);
  }
  fclose(fout);
  free(DistanceValue);

  return(0);
}

