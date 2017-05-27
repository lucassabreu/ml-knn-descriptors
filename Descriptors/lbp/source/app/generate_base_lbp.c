// gcc -L ../lib/ -I ../include/ generate_base_lbp.c -l texturedescriptors -lm -o ../../../../generate_base_lbp

#include <string.h>

#include "../include/libtexturedescriptors.h"

Histogram* Extraction(char *img_path)
{
  Histogram *lbp;
  CImage *cimg=NULL;
  Image *img=NULL;
  int i;

  cimg = ReadCImage(img_path);
  img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);
  
  //translate to HSV space

  for (i=0; i<(img->ncols*img->nrows); i++)
    img->val[i] = (cimg->C[0]->val[i])+(cimg->C[1]->val[i])+(cimg->C[2]->val[i])/3;

  lbp = LBPri(img);

  DestroyCImage(&cimg);
  DestroyImage(&img);

  return lbp;
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
   Histogram *fv[n];

   FILE *fout = fopen("LBP.out","wt");
   for(count=0;count<n;count++) {
      if(feof(fin)) {
         fprintf(stderr,"Incomplete file? (%d images instead of %d)          \r",count, n);
         return 1;
      }
      fscanf(fin,"%d %d %s", &id, &current_class, filename);
      strcat(filename,".ppm");
      fprintf(stderr,"extracting %s (%d/%d)...          \r",filename, count, n);
      fv[count] = Extraction(filename);
      fprintf(fout,"%u %u 3 %u\n",2100000+count,count,fv[count]->n);
      for(j=0;j<fv[count]->n;j++)
         fprintf(fout,"%f\n",(float)fv[count]->v[j]);
      fprintf(fout,"\n");
   }
   fclose(fout);
   fclose(fin);
   
  //gera matriz de distancia
  DistanceValue = (float **)malloc(n*sizeof(float *));
  for(i=0;i<n;i++)
     DistanceValue[i] = (float *)malloc(n*sizeof(float));

  fout = fopen("LBP.bin","wb");
  fwrite(&n, sizeof(unsigned), 1, fout);
  for(i=0;i<n;i++) {
      fprintf(stderr, "computing distances (%d/%d)...    \r", i+1, n);
      DistanceValue[i][i] = 0.0;
      for(j=i+1;j<n;j++)
        DistanceValue[i][j] = DistanceValue[j][i] = (float)L1Distance(fv[i],fv[j]);
      fwrite(DistanceValue[i], sizeof(float), n, fout);
      DestroyHistogram(&fv[i]);
      free(DistanceValue[i]);
  }
  fclose(fout);
  free(DistanceValue);

  return(0);
}

