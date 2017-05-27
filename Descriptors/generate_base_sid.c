// gcc -w -fpermissive -Iift/include/ generate_base_sid.c -lm -o ../generate_base_sid

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

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


FeatureVector1D * Extraction(CImage *cimg)
{

    Features *img=NULL;
    Features *steerable=NULL;
    FeatureVector1D *featurevector=NULL;
    int i,j;

    img = CreateFeatures(cimg->C[0]->ncols, cimg->C[0]->nrows, 1);

    //translate to HSV space

    for (i=0; i<(img->ncols*img->nrows); i++)
        img->elem[i].feat[0] = (cimg->C[0]->val[i])+(cimg->C[1]->val[i])+(cimg->C[2]->val[i])/3;

    steerable = SteerablePyramidFeats(img,3,6);

    featurevector = CreateFeatureVector1D(2*steerable->nfeats);
    for (j=0; j<featurevector->n; j++)
        featurevector->X[j] = 0;

    //mean of the steerable features
    for (i=0; i<steerable->nelems; i++)
        for (j=0; j<steerable->nfeats; j++)
            featurevector->X[j] += (double) (steerable->elem[i].feat[j]);
    for (j=0; j<steerable->nfeats; j++)
        featurevector->X[j] /= steerable->nelems;

    //standad deviation of the steerable features
    for (i=0; i<steerable->nelems; i++)
        for (j=0; j<steerable->nfeats; j++){
            featurevector->X[j+steerable->nfeats] += (double) ((steerable->elem[i].feat[j]-featurevector->X[j])*(steerable->elem[i].feat[j]-featurevector->X[j]));
        }
    for (j=0; j<steerable->nfeats; j++){
        featurevector->X[j+steerable->nfeats] /= steerable->nelems;
        featurevector->X[j+steerable->nfeats] = sqrt(featurevector->X[j+steerable->nfeats]);
    }


  //
  // Sort feature vector to achive rotation and scale invariance
  //
/**/
    int maxpos;
    double max, aux;

    for (i=0; i<steerable->nfeats-1; i++){
        max = featurevector->X[i];
        maxpos = i;
        for (j=i+1; j<steerable->nfeats; j++){
            if (featurevector->X[j]>max){
                max = featurevector->X[j];
                maxpos = j;
            }
        }
        aux = featurevector->X[maxpos+steerable->nfeats];
        featurevector->X[maxpos] = featurevector->X[i];
        featurevector->X[maxpos+steerable->nfeats] = featurevector->X[i+steerable->nfeats];
        featurevector->X[i] = max;
        featurevector->X[i+steerable->nfeats] = aux;
    }
/**/

  //
  // End sort. Comment lines above if rotation or scale invariance isn't desired.
  //

 //   WriteFileFeatureVector1D_bin(featurevector,fv_path);

    DestroyFeatures(&img);
    DestroyFeatures(&steerable);
    return featurevector;
}

// L1_Distance
float ManhattanDistance(FeatureVector1D *v1, FeatureVector1D *v2)
{
    int i;
    float dist=0.0f;

    for (i=0; i < v1->n; i++)
        dist += fabs(v1->X[i] - v2->X[i]);

    return(dist);
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

   FILE *fout = fopen("SID.out","wt");
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
      fprintf(fout,"%u %u 3 %u\n",1900000+count,count,fv[count]->n);
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

  fout = fopen("SID.bin","wb");
  fwrite(&n, sizeof(unsigned), 1, fout);
  for(i=0;i<n;i++) {
      fprintf(stderr, "computing distances (%d/%d)...    \r", i+1, n);
      DistanceValue[i][i] = 0.0;
      for(j=i+1;j<n;j++)
        DistanceValue[i][j] = DistanceValue[j][i] = (float)ManhattanDistance(fv[i],fv[j]);
      fwrite(DistanceValue[i], sizeof(float), n, fout);
      DestroyFeatureVector1D(&fv[i]);
      free(DistanceValue[i]);
  }
  fclose(fout);
  free(DistanceValue);

  return(0);
}
