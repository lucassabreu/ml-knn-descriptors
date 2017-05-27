// gcc -L ../lib/ -I ../include/ generate_base_sasi.c -l texturedescriptors -lm -o ../../../../generate_base_sasi
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#include "../include/libtexturedescriptors.h"

FeatureVector1D * Extraction(CImage *cimg)
{

    Image *img=NULL;
    ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    FeatureVector1D *sasi=NULL;
    int i,j;
    int sizes[3];
    sizes[0]=3;
    sizes[1]=5;
    sizes[2]=7;
    FILE *fout;

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

    //liberando memoria imgHSV
    for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgHSV[i]);
    }
    free(imgHSV);

    DestroyImage(&img);
    return sasi;
}

int main(int argc, char** argv)
{
   char filename[50];
   int current_class, n, id;
   unsigned i, j;
   unsigned count;
   float **DistanceValue;

   if(argc<2) {
      printf("Use: generate_base_bic <id_class_img file>\n");
      return 1;
   }

   FILE *fin = fopen(argv[1],"rt");
   if(fin == NULL) {
      printf("Cannot open file '%s'!\n", argv[1]);
      return 1;
   }

   // load number of images of database
   fscanf(fin,"%d", &n);
   FeatureVector1D *fv[n];

   FILE *fout = fopen("SASI.out","wt");
   for(count=0;count<n;count++) {
      if(feof(fin)) {
         fprintf(stderr,"Incomplete file? (%d images instead of %d)          \r",count, n);
         return 1;
      }
      fscanf(fin,"%d %d %s", &id, &current_class, filename);
      strcat(filename,".ppm");
      fprintf(stderr,"extracting %s (%d/%d)...          \r",filename, count, n);
      CImage *img1 = ReadCImage(filename);
      fv[count] = Extraction(img1);
      fprintf(fout,"%u %u 3 %u\n",1700000+count,count,fv[count]->n);
      for(j=0;j<fv[count]->n;j++)
         fprintf(fout,"%f\n",fv[count]->X[j]);
      fprintf(fout,"\n");
      DestroyCImage(&img1);
   }
   fclose(fout);
   fclose(fin);
   
  //gera matriz de distancia
  DistanceValue = (float **)malloc(n*sizeof(float *));
  for(i=0;i<n;i++)
     DistanceValue[i] = (float *)malloc(n*sizeof(float));

  fout = fopen("SASI.bin","wb");
  fwrite(&n, sizeof(unsigned), 1, fout);
  for(i=0;i<n;i++) {
      fprintf(stderr, "computing distances (%d/%d)...    \r", i+1, n);
      DistanceValue[i][i] = 0.0;
      for(j=i+1;j<n;j++)
        DistanceValue[i][j] = DistanceValue[j][i] = (float)DotDistance(fv[i],fv[j]);
      fwrite(DistanceValue[i], sizeof(float), n, fout);
      DestroyFeatureVector1D(&fv[i]);
      free(DistanceValue[i]);
  }
  fclose(fout);
  free(DistanceValue);

  return(0);
}
