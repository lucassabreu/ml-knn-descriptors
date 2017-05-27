// gcc -L ../lib/ -I ../include/ generate_base_lucolor.c -l colordescriptors -lm -o ../../../../generate_base_lucolor

#include <string.h>

#include "../include/libcolordescriptors.h"

int main(int argc, char** argv)
{
   char filename[50];
   int current_class, n, id;
   unsigned i, j;
   unsigned count;
   float **DistanceValue;
   long int tam;

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
   Vetor *fv[n];
   double media[n][3],desvio[n][3];

   FILE *fout = fopen("ColorBitmap.out","wt");
   for(count=0;count<n;count++) {
      if(feof(fin)) {
         fprintf(stderr,"Incomplete file? (%d images instead of %d)          \r",count, n);
         return 1;
      }
      fscanf(fin,"%d %d %s", &id, &current_class, filename);
      strcat(filename,".ppm");
      fprintf(stderr,"extracting %s (%d/%d)...          \r",filename, count, n);
      CImage *img1 = ReadCImage(filename);
      tam = img1->C[0]->ncols * img1->C[0]->nrows;

      fv[count] = LuColor2(img1,tam,media[count],desvio[count]);

      fprintf(fout,"%u %u 3 %u\n",1100000+count,count,fv[count]->tam[0]);
      fprintf(fout,"%f\n",(float)fv[count]->tam[0]);
      fprintf(fout,"%f %f %f\n",(float)media[count][0],(float)media[count][1],(float)media[count][2]);
      fprintf(fout,"%f %f %f\n",(float)desvio[count][0],(float)desvio[count][1],(float)desvio[count][2]);
      for(j=0;j<fv[count]->tam[0];j++)
         fprintf(fout,"%d %d %d\n",(int)fv[count]->r[j],(int)fv[count]->g[j],(int)fv[count]->b[j]);
      fprintf(fout,"\n");
      DestroyCImage(&img1);
   }
   fclose(fout);
   fclose(fin);
   
  //gera matriz de distancia
  DistanceValue = (float **)malloc(n*sizeof(float *));
  for(i=0;i<n;i++)
     DistanceValue[i] = (float *)malloc(n*sizeof(float));

  fout = fopen("ColorBitmap.bin","wb");
  fwrite(&n, sizeof(unsigned), 1, fout);
  for(i=0;i<n;i++) {
      fprintf(stderr, "computing distances (%d/%d)...    \r", i+1, n);
      DistanceValue[i][i] = 0.0;
      for(j=i+1;j<n;j++)
        DistanceValue[i][j] = DistanceValue[j][i] = (float)Hamming(fv[i],fv[j],media[i],media[j],desvio[i],desvio[j]);
      fwrite(DistanceValue[i], sizeof(float), n, fout);
      DestroyVetor(&fv[i]);
      free(DistanceValue[i]);
  }
  fclose(fout);
  free(DistanceValue);

  return(0);
}

