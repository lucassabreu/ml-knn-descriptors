//pra compilar
// gcc -w -fpermissive -Iift/include/ generate_base_sms.c -o ../generate_base_sms -lm

#include <stdlib.h>
#include <stdio.h>

#include "ift/src/mathematics3.c"
#include "ift/src/sort.c"
#include "ift/src/msfiltering.c"
#include "ift/src/adjacency3.c"
#include "ift/src/matrix.c"
#include "ift/src/realmatrix.c"
#include "ift/src/radiometric3.c"
#include "ift/src/border.c"
#include "ift/src/segmentation3.c"
#include "ift/src/heap.c"
#include "ift/src/morphology.c"
#include "ift/src/analysis.c"
#include "ift/src/queue.c"
#include "ift/src/gqueue.c"
#include "ift/src/annimg.c"
#include "ift/src/radiometric.c"
#include "ift/src/set.c"
#include "ift/src/segmentation.c"
#include "ift/src/scene.c"
#include "ift/src/adjacency.c"
#include "ift/src/filtering.c"
#include "ift/src/feature.c"
#include "ift/src/curve.c"
#include "ift/src/polynom.c"
#include "ift/src/mathematics.c"
#include "ift/src/color.c"
#include "ift/src/image.c"
#include "ift/src/dimage.c"
#include "ift/src/cimage.c"
#include "ift/src/common.c"
#include "ift/src/spectrum.c"
#include "ift/src/metrics.c"
#include "ift/src/descriptor.c"

FeatureVector1D *Steerable_ExtractionAlgorithm(CImage *cimg) {
   int i, j, cont;
   Features *lab,*stb;
   float mean, stddev, faux;
   FeatureVector1D *fv;

   fv = CreateFeatureVector1D(108);
   fv->n = 108;

   lab  = LabFeats(cimg);
   NormalizeLab(lab);
   stb = ShiftedSPFeats(lab,3,6);
   cont = 0;
   for (i=0; i < 54; i++) {
      mean = 0.0;
      for(j=0; j < stb->nelems; j++) {
         mean += stb->elem[j].feat[i] / stb->nelems;
      }
      fv->X[cont++] = (double)mean;

      stddev = 0.0;
      for(j=0; j < stb->nelems; j++) {
         faux = stb->elem[j].feat[i] - mean;
         stddev += faux*faux/stb->nelems;
      }
      stddev = sqrt(stddev);
      fv->X[cont++] = (double)stddev;
   }
   DestroyFeatures(&lab);
   DestroyFeatures(&stb);

   return(fv);
}

FeatureVector1D *BIC_ExtractionAlgorithm(CImage *cimg) {
   Curve *curve = BIC(cimg);
   FeatureVector1D *fvBIC = CurveTo1DFeatureVector(curve);
   DestroyCurve(&curve);
   return fvBIC;
}

double L1_Distance(FeatureVector1D *v1, FeatureVector1D *v2) {
  int i;
  double sum = 0.0;

  for (i = 0; i < v1->n ; i++){
    sum += fabs(v1->X[i] - v2->X[i]);
  }
  return (sum);
}

int main(int argc, char** argv)
{
   char filename[50];
   int current_class, n, id;
   unsigned count;
   unsigned i, j;
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

   FILE *fout = fopen("SMS.out","wt");
   for(count=0;count<n;count++) {
      if(feof(fin)) {
         fprintf(stderr,"Incomplete file? (%d images instead of %d)          \r",count, n);
         return 1;
      }
      fscanf(fin,"%d %d %s", &id, &current_class, filename);
      strcat(filename,".ppm");
      fprintf(stderr,"extracting %s (%d/%d)...          \r",filename, count, n);
      CImage *img1 = ReadCImage(filename);
      fv[count] = Steerable_ExtractionAlgorithm(img1);
      fprintf(fout,"%u %u 3 %u\n",1800000+count,count,fv[count]->n);
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

  fout = fopen("SMS.bin","wb");
  fwrite(&n, sizeof(unsigned), 1, fout);
  for(i=0;i<n;i++) {
      fprintf(stderr, "computing distances (%d/%d)...    \r", i+1, n);
      DistanceValue[i][i] = 0.0;
      for(j=i+1;j<n;j++)
        DistanceValue[i][j] = DistanceValue[j][i] = (float)L1_Distance(fv[i],fv[j]);
      fwrite(DistanceValue[i], sizeof(float), n, fout);
      DestroyFeatureVector1D(&fv[i]);
      free(DistanceValue[i]);
  }
  fclose(fout);
  free(DistanceValue);

  return(0);
}

