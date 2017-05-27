#include <sys/types.h>
#include <dirent.h>

#include "ift.h"

int main(int argc, char **argv)
{
  CImage *cimg;
  Features *lab,*stb;
  int i, j;
  float mean, stddev, faux;

  /*------- -------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/
  
  if (argc != 2) {
    fprintf(stderr,"usage: steerable <image.ppm> \n");
    exit(-1);
  }

  FILE *fout;
  DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(".")) == NULL) {
      printf("Error opening directory");
      return 1;
  }

  fout = fopen("base_s.bin", "wb");
  if(fout == NULL) {
      printf("Error opening base_s.bin");
      return 2;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if(*dirp->d_name == '2') { // All files MUST begin whith 2!!! (Ex.: 2007_...)

      printf("%s   \r", dirp->d_name);
      fwrite(dirp->d_name, 15, sizeof(char), fout);
      cimg = ReadCImage(dirp->d_name);

      lab  = LabFeats(cimg);
      NormalizeLab(lab);
      stb = ShiftedSPFeats(lab,3,6);
      for (i=0; i < 54; i++) {
         mean = 0.0;
         for(j=0; j < stb->nelems; j++) {
            mean += stb->elem[j].feat[i] / stb->nelems;
         }
         fwrite(&mean, 1, sizeof(float), fout);

         stddev = 0.0;
         for(j=0; j < stb->nelems; j++) {
            faux = stb->elem[j].feat[i] - mean;
            stddev += faux*faux/stb->nelems;
         }
         stddev = sqrt(stddev);
         fwrite(&stddev, 1, sizeof(float), fout);
//         printf("%f %f\n", mean, stddev);
      }

      DestroyCImage(&cimg);
      DestroyFeatures(&lab);
      DestroyFeatures(&stb);

    }
  }
  fclose(fout);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}
