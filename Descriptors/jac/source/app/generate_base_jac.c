// gcc -L ../lib/ -I ../include/ generate_base_jac.c -l colordescriptors -lm -o ../../../../generate_base_jac

#include <string.h>

#include "../include/libcolordescriptors.h"

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
   JointAutoCorrelogram *fv[n];

   FILE *fout = fopen("JAC.out","wt");
   for(count=0;count<n;count++) {
      if(feof(fin)) {
         fprintf(stderr,"Incomplete file? (%d images instead of %d)          \r",count, n);
         return 1;
      }
      fscanf(fin,"%d %d %s", &id, &current_class, filename);
//if(feof(fin)) break;
      strcat(filename,".ppm");
      fprintf(stderr,"extracting %s (%d/%d)...          \r",filename, count, n);
      CImage *img1 = ReadCImage(filename);
      fv[count] = JointAutoCorrelogramImage(img1);

		int n = fv[count]->nColors * fv[count]->nTexturedness * fv[count]->nGradient * fv[count]->nRank * fv[count]->maxDistance;
      fprintf(fout,"%u %u 3 %u\n",1400000+count,count,n);
		for (i=0; i<n; i++) {
			if(fv[count]->h[i] > 0.0){
				fprintf(fout, "%d %f ", i,fv[count]->h[i]);
			}
		}
		fprintf(fout, "-1 -1\n\n");

      DestroyCImage(&img1);
   }
   fclose(fout);
   fclose(fin);
   
  //gera matriz de distancia
  DistanceValue = (float **)malloc(n*sizeof(float *));
  for(i=0;i<n;i++)
     DistanceValue[i] = (float *)malloc(n*sizeof(float));

  fout = fopen("JAC.bin","wb");
  fwrite(&n, sizeof(unsigned), 1, fout);
  for(i=0;i<n;i++) {
      fprintf(stderr, "computing distances (%d/%d)...    \r", i+1, n);
      DistanceValue[i][i] = 0.0;
      for(j=i+1;j<n;j++)
        DistanceValue[i][j] = DistanceValue[j][i] = (float)L1JointAutoCorrelogramDistance(fv[i],fv[j]);
      fwrite(DistanceValue[i], sizeof(float), n, fout);
      DestroyJointAutoCorrelogram(&fv[i]);
      free(DistanceValue[i]);
  }
  fclose(fout);
  free(DistanceValue);

  return(0);
}

