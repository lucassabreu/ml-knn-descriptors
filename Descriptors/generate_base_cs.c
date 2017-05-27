//pra compilar
// gcc -w -fpermissive generate_base_cs.c MO445.c -o ../generate_base_cs -lm

#include <stdio.h>
#include <stdlib.h>

#include "MO445.h"

// image must be binary (0 or 1)
void ConvertImageToPB(Image *img)
{
   int i, n = img->ncols*img->nrows;
   for(i=0; i<n; i++)
      img->val[i] = (img->val[i]==0) ? 0 : 1;
}

int main(int argc, char** argv)
{
   char filename[50];
   int i, j, n, id, current_class;
   float **DistanceValue;
   unsigned count;
   FILE *fout;

   if(argc<2) {
      printf("Use: generate_base_bas <id_class_img file>\n");
      return 1;
   }

   FILE *fin = fopen(argv[1],"rt");
   if(fin == NULL) {
      printf("Cannot open file '%s'!\n", argv[1]);
      return 1;
   }

   // load number of images of database
   fscanf(fin,"%d", &n);
   FeatureVector2D *fv[n];

//   fout = fopen("SS.out","wt");
   for(count=0;count<n;count++) {
      if(feof(fin)) {
         fprintf(stderr,"Incomplete file? (%d images instead of %d)          \r",count, n);
         return 1;
      }
      fscanf(fin,"%d %d %s", &id, &current_class, filename);
      strcat(filename,".pgm");
      fprintf(stderr,"extracting %s (%d/%d)...          \r",filename, count, n);
      Image *img1 = ReadImage(filename);
      ConvertImageToPB(img1);
      fv[count] = CS_ExtractionAlgorithm(img1);
//      fprintf(fout,"%u %u 3 %u\n",1300000+count,count,fv[count]->n);
//      for(j=0;j<fv[count]->n;j++)
//         fprintf(fout,"%.1f\n",fv[count]->X[j]);
//      fprintf(fout,"\n");
      DestroyImage(&img1);
   }
//   fclose(fout);
   fclose(fin);
   
  //gera matriz de distancia
  DistanceValue = (float **)malloc(n*sizeof(float *));
  for(i=0;i<n;i++)
     DistanceValue[i] = (float *)malloc(n*sizeof(float));

  fout = fopen("CS.bin","wb");
  fwrite(&n, sizeof(unsigned), 1, fout);
  for(i=0;i<n;i++) {
      fprintf(stderr,"computing distances (%d/%d))...    \r", i+1, n);
      DistanceValue[i][i] = 0.0;
      for(j=i+1;j<n;j++)
        DistanceValue[i][j] = DistanceValue[j][i] = (float)CS_DistanceAlgorithm(fv[i],fv[j]);
      fwrite(DistanceValue[i], sizeof(float), n, fout);
      DestroyFeatureVector2D(&fv[i]);
      free(DistanceValue[i]);
  }
  fclose(fout);
  free(DistanceValue);

  return(0);
}

