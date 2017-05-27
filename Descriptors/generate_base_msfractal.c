//pra compilar
// gcc -w -fpermissive -Iift/include/ generate_base_msfractal.c -o ../generate_base_msfractal -lm

#include <stdio.h>
#include <stdlib.h>

#include "ift/src/common.c"
#include "ift/src/curve.c"
#include "ift/src/set.c"
#include "ift/src/adjacency.c"
#include "ift/src/adjacency3.c"
#include "ift/src/segmentation.c"
#include "ift/src/segmentation3.c"
#include "ift/src/image.c"
#include "ift/src/analysis.c"
#include "ift/src/realmatrix.c"
#include "ift/src/scene.c"
#include "ift/src/opf.c"
#include "ift/src/queue.c"
#include "ift/src/gqueue.c"
#include "ift/src/border.c"
#include "ift/src/radiometric.c"
#include "ift/src/radiometric3.c"
#include "ift/src/annimg.c"
#include "ift/src/heap.c"
#include "ift/src/realheap.c"
#include "ift/src/polynom.c"
#include "ift/src/morphology.c"
#include "ift/src/mathematics.c"
#include "ift/src/mathematics3.c"
#include "ift/src/matrix.c"
#include "ift/src/cimage.c"
#include "ift/src/dimage.c"
#include "ift/src/color.c"
#include "ift/src/feature.c"
#include "ift/src/descriptor.c"
#include "ift/src/filtering.c"
#include "ift/src/msfiltering.c"
#include "ift/src/spectrum.c"
#include "ift/src/sort.c"
#include "ift/src/metrics.c"

// image must be binary (0 or 1)
void ConvertImageToPB(Image *img)
{
   int i, n = img->ncols*img->nrows;
   for(i=0; i<n; i++)
      img->val[i] = (img->val[i]==0) ? 0 : 1;
}

FeatureVector1D *MSFractal_ExtractionAlgorithm(Image *in)
{
 Curve *curve = NULL;
 FeatureVector1D *fv = NULL;
 
 curve = ContourMSFractal(in);
 fv = CurveTo1DFeatureVector(curve);
 
 DestroyCurve(&curve);
 return fv;
}

int main(int argc, char** argv)
{
   char filename[50];
   int i, j, n, id, current_class;
   float **DistanceValue;
   unsigned count;

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
   FeatureVector1D *fv[n];

   FILE *fout = fopen("MSF.out","wt");
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
      fv[count] = MSFractal_ExtractionAlgorithm(img1);
      fprintf(fout,"%u %u 3 %u\n",1300000+count,count,fv[count]->n);
      for(j=0;j<fv[count]->n;j++)
         fprintf(fout,"%.1f\n",fv[count]->X[j]);
      fprintf(fout,"\n");
      DestroyImage(&img1);
   }
   fclose(fout);
   fclose(fin);
   
  //gera matriz de distancia
  DistanceValue = (float **)malloc(n*sizeof(float *));
  for(i=0;i<n;i++)
     DistanceValue[i] = (float *)malloc(n*sizeof(float));

  fout = fopen("MSF.bin","wb");
  fwrite(&n, sizeof(unsigned), 1, fout);
  for(i=0;i<n;i++) {
      fprintf(stderr,"computing distances (%d/%d))...    \r", i+1, n);
      DistanceValue[i][i] = 0.0;
      for(j=i+1;j<n;j++)
        DistanceValue[i][j] = DistanceValue[j][i] = (float)EuclideanDistance(fv[i],fv[j]);
      fwrite(DistanceValue[i], sizeof(float), n, fout);
      DestroyFeatureVector1D(&fv[i]);
      free(DistanceValue[i]);
  }
  fclose(fout);
  free(DistanceValue);

  return(0);
}

