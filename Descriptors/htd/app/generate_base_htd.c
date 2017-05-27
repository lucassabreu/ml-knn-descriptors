// gcc -L ../lib/ -I ../include/ generate_base_htd.c -l colordescriptors -lm -o ../../../generate_base_htd
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#include "../include/libcolordescriptors.h"

int main(int argc, char** argv)
{
   char filename[50];
   int current_class, n, id;
   unsigned i, j, s, o;
   float **DistanceValue;
   unsigned count;

   if(argc<2) {
      printf("Use: eth80_htd_distance <id_class_img file> \n");
      return 1;
   }

   FILE *fin = fopen(argv[1],"rt");
   if(fin == NULL) {
      printf("Cannot open file '%s'!\n", argv[1]);
      return 1;
   }

   // load number of images of database
   fscanf(fin,"%d", &n);

   Mpeg7HTDDescriptor *fv[n];

   FILE *fout = fopen("HTD.out","wt");
   for(count=0;count<n;count++) {
      if(feof(fin)) {
         fprintf(stderr,"Incomplete file? (%d images instead of %d)          \r",count, n);
         return 1;
      }
      fscanf(fin,"%d %d %s", &id, &current_class, filename);
      strcat(filename,".ppm");
      fprintf(stderr,"extracting %s (%d/%d)...          \r",filename, count, n);
      CImage *img1 = ReadCImage(filename);
      fv[count] = ExtractMpeg7HTDDescriptor(img1);
      fprintf(fout,"%u %u %u %u\n", 1500000+count, count, fv[count]->scale, 2*fv[count]->scale*fv[count]->orientation);
      j = 0;
      for(s=0;s<fv[count]->scale;s++) {
         for(o=0;o<fv[count]->orientation;o++) {
            fprintf(fout,"%f\n",fv[count]->media[j]);
            fprintf(fout,"%f\n",fv[count]->desvio[j]);
            j++;
         }
      }
      fprintf(fout,"\n");
      DestroyCImage(&img1);
   }
   fclose(fout);
   fclose(fin);
   
  //gera matriz de distancia
  DistanceValue = (float **)malloc(n*sizeof(float *));
  for(i=0;i<n;i++)
     DistanceValue[i] = (float *)malloc(n*sizeof(float));

  fout = fopen("HTD.bin","wb");
  fwrite(&n, sizeof(unsigned), 1, fout);
  for(i=0;i<n;i++) {
      fprintf(stderr, "computing distances (%d/%d)...    \r", i+1, n);
      DistanceValue[i][i] = 0.0;
      for(j=i+1;j<n;j++)
        DistanceValue[i][j] = DistanceValue[j][i] = (float)DistanceMpeg7HTDDescriptor(fv[i],fv[j]);
      DestroyMpeg7HTDDescriptor(&fv[i]);
      fwrite(DistanceValue[i], sizeof(float), n, fout);
      free(DistanceValue[i]);
  }
  fclose(fout);
  free(DistanceValue);

  return(0);
}

