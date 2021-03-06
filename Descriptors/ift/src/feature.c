#include "feature.h"
#include "msfiltering.h"

Features* CreateFeatures(int ncols, int nrows, int nfeats)
{
    Features *f=(Features *)calloc(1,sizeof(Features));

    f->ncols  = ncols;
    f->nrows  = nrows;
    f->nelems = ncols*nrows;
    f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));
    f->nfeats = nfeats;
    f->Imax = 0;

    int i;

    for (i=0; i < f->nelems; i++)
        f->elem[i].feat = AllocFloatArray(nfeats);

    return f;
}

void DestroyFeatures(Features **f)
{
    int i;

    if ((*f)!=NULL)
    {
        for (i=0; i < (*f)->nelems; i++)
            free((*f)->elem[i].feat);
        free((*f)->elem);
        free(*f);
        (*f)=NULL;
    }
}

Features* CopyFeatures(Features* feat)
{
    if(feat == NULL)
    {
        Error("Features* feat NULL ","CopyFeatures");
    }

    Features* result = CreateFeatures(feat->ncols, feat->nrows, feat->nfeats);

    result->Imax = feat->Imax;

    int i;
    int size = feat->nfeats*sizeof(float);

    for(i = 0; i < feat->nelems; i++)
        memcpy(result->elem[i].feat, feat->elem[i].feat,size);

    return result;
}

DImage* GetFeature(Features* feats, int index)
{
    if (feats == NULL) return NULL;
    if (index >= feats->nfeats) return NULL;

    DImage* imgfeats = CreateDImage(feats->ncols, feats->nrows);

    int i;

    for (i = 0; i < feats->nelems; i++)
    {
        if (feats->elem == NULL || feats->elem[i].feat == NULL)
        {
            DestroyDImage(&imgfeats);
            perror("Error! Pixels or feature with NULL values in GetFeature!");
            return NULL;
        }

        double value = (double)feats->elem[i].feat[index];
        imgfeats->val[i] = value;
    }

    return imgfeats;
}


int SetFeature(Features* feats, int index, DImage* imagefeats)
{

    if (feats == NULL || imagefeats == NULL) return 0;;
    if (index >= feats->nfeats) return 0;

    int i;

    for (i = 0; i < feats->nelems; i++)
    {
        if (feats->elem == NULL || feats->elem[i].feat == NULL)
        {
            fprintf(stderr,"Error! Pixels or feature with NULL values!\n");
            return 0;
        }
        feats->elem[i].feat[index] = (float)imagefeats->val[i];
    }
    return 1;
}

Features* ConcatFeatures(Features* f1, Features* f2)
{
    if (f1 == NULL || f2 == NULL)
    {
        Error("Feature f1 or feature f2 is NULL","ConcatFeatures");
    }
    if (f1->nelems != f2->nelems)
    {
        Error("Distinct number of elements between "
              "features f1 and f2","ConcatFeatures");
    }
    if (f1->ncols != f2->ncols || f1->nrows != f2->nrows)
    {
        Error("Number of rows, or columns, is different between"
              " features f1 and f2", "ConcatFeatures");
    };

    int ncols = f1->ncols;
    int nrows = f1->nrows;
    int nfeats = f1->nfeats + f2->nfeats;

    Features* result = CreateFeatures(ncols, nrows, nfeats);

    int i, j;

    for (i=0; i < f1->nelems; i++)
    {
        for (j = 0; j < nfeats; j++)
        {
            if (j < f1->nfeats)
                result->elem[i].feat[j] = f1->elem[i].feat[j];
            else
                result->elem[i].feat[j] = f2->elem[i].feat[j-f1->nfeats];
        }
    }

    result->Imax = MAX(f1->Imax, f2->Imax);

    return result;

}

Features* TrimFeatures(Features* feats,int ncols, int nrows)
{
    if (feats == NULL)
    {
        Error("Null Features", "ExcludeFeatures\n");
    }
    if (ncols > feats->ncols || nrows > feats->nrows)
    {
        Error("Ncols/Nrows parameter is greater than  feats->ncols/feats->nrows","CutFeats");
    }

    Features* result = CreateFeatures(ncols,nrows,feats->nfeats);

    int p,t;
    int size = feats->nfeats*sizeof(float);
    int x,y;

    for (y = 0; y < nrows; y++)
    {
        for (x = 0; x < ncols; x++)
        {
            p = x + y*ncols;

            t = x + y*feats->ncols;

            memcpy(result->elem[p].feat,feats->elem[t].feat,size);
        }
    }
    return result;
}

/*normalize features*/
void FNormalizeFeatures(Features *f)
{
    float *mean = (float *)calloc(f->nfeats,sizeof(float)), *std = (float *)calloc(f->nfeats, sizeof(int));
    int i,j;

    for (i = 0; i < f->nfeats; i++)
    {
        for (j = 0; j < f->nelems; j++)
            mean[i]+=f->elem[j].feat[i]/f->nelems;
        for (j = 0; j < f->nelems; j++)
            std[i]+=pow(f->elem[j].feat[i]-mean[i],2)/f->nelems;
        std[i]=sqrt(std[i]);
    }

    for (i = 0; i < f->nfeats; i++)
    {
        for (j = 0; j < f->nelems; j++)
        {
            float v =(f->elem[j].feat[i]-mean[i]);
            /// Preventing division by zero
            if (std[i] != 0.0)
                f->elem[j].feat[i] = v/std[i];
            else
                f->elem[j].feat[i] = v;
        }
    }

    free(mean);
    free(std);
}


Features *MSImageFeats(Image *img, int nscales)
{
    Features *f=(Features *)calloc(1,sizeof(Features));
    AdjRel   *A=NULL;
    int       s,i;
    Image    *img1,*img2;

    f->Imax=MaximumValue(img);
    f->ncols = img->ncols;
    f->nrows = img->nrows;
    f->nelems = f->ncols*f->nrows;
    f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));
    for (i=0; i < f->nelems; i++)
    {
        f->elem[i].feat = AllocFloatArray(nscales);
        f->nfeats       = nscales;
    }

    img1  = CopyImage(img);

    for (s=1; s <= nscales; s=s+1)
    {
        A    = Circular(s);
        img2 = AsfOCRec(img1,A);
        for (i=0; i < f->nelems; i++)
        {
            f->elem[i].feat[s-1] = (float)img2->val[i]/(float)f->Imax;
        }
        DestroyImage(&img2);
        DestroyAdjRel(&A);
    }
    DestroyImage(&img1);
    return(f);
}

Features *MSCImageFeats(CImage *cimg, int nscales)
{
    Features *f=(Features *)calloc(1,sizeof(Features));
    AdjRel   *A=NULL;
    int       s,i,j;
    Image    *img;

    f->Imax = MAX(MAX(MaximumValue(cimg->C[0]),MaximumValue(cimg->C[1])),MaximumValue(cimg->C[2]));

    f->ncols  = cimg->C[0]->ncols;
    f->nrows  = cimg->C[1]->nrows;
    f->nelems = cimg->C[0]->ncols*cimg->C[0]->nrows;
    f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));
    for (i=0; i < f->nelems; i++)
    {
        f->elem[i].feat = AllocFloatArray(3*nscales);
        f->nfeats       = 3*nscales;
    }

    for (j=0; j < 3; j=j+1)
    {
        for (s=1; s <= nscales; s=s+1)
        {
            A   = Circular(s);
            img = AsfCORec(cimg->C[j],A);
            for (i=0; i < f->nelems; i++)
            {
                f->elem[i].feat[s-1+(j*nscales)] = (float)img->val[i]/f->Imax;
            }
            DestroyImage(&img);
            DestroyAdjRel(&A);
        }
    }
    return(f);
}

// Using Linear Convolution with Gaussian filters

Features *LMSImageFeats(Image *img, int nscales)
{
    Features *f=(Features *)calloc(1,sizeof(Features));
    AdjRel   *A=NULL;
    int       s,i,p,q;
    Pixel     u,v;
    Image    *img1;
    float    *w,d,K1,K2,sigma2,val;

    f->Imax = MaximumValue(img);
    f->ncols  = img->ncols;
    f->nrows  = img->nrows;
    f->nelems = img->ncols*img->nrows;
    f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));
    for (i=0; i < f->nelems; i++)
    {
        f->elem[i].feat = AllocFloatArray(nscales);
        f->nfeats       = nscales;
    }

    for (s=1; s <= nscales; s=s+1)
    {
        A  = Circular(s);
        w  = AllocFloatArray(A->n);
        sigma2 = (s/3.0)*(s/3.0);
        K1     =  2.0*sigma2;
        K2     = 1.0/sqrt(2.0*PI*sigma2);
        //compute kernel coefficients

        for (i=0; i < A->n; i++)
        {
            d    = A->dx[i]*A->dx[i]+A->dy[i]*A->dy[i];
            w[i] = K2 * exp(-d/K1); // Gaussian
        }

        // Convolution

        img1 = CreateImage(img->ncols,img->nrows);

        for (p=0; p < f->nelems; p++)
        {
            u.x = p%f->ncols;
            u.y = p/f->ncols;
            val = 0.0;
            for (i=0; i < A->n; i++)
            {
                v.x = u.x + A->dx[i];
                v.y = u.y + A->dy[i];
                if (ValidPixel(img,v.x,v.y))
                {
                    q   = v.x + img->tbrow[v.y];
                    val += (float)img->val[q]*w[i];
                }
            }
            img1->val[p]=(int)val;
        }
        free(w);

        // Copy features and reinitialize images

        for (p=0; p < f->nelems; p++)
        {
            f->elem[p].feat[s-1] = (float)img1->val[p]/f->Imax;
        }
        DestroyImage(&img1);
        DestroyAdjRel(&A);
    }
    return(f);
}

// Using Linear Convolution with Gaussian filters

Features *LMSCImageFeats(CImage *cimg, int nscales)
{
    Features *f=(Features *)calloc(1,sizeof(Features));
    AdjRel   *A=NULL;
    int       s,i,j,p,q;
    Pixel     u,v;
    Image    *img1,*img2;
    float    *w,d,K,sigma2,val;

    f->Imax = MAX(MAX(MaximumValue(cimg->C[0]),MaximumValue(cimg->C[1])),MaximumValue(cimg->C[2]));

    f->ncols  = cimg->C[0]->ncols;
    f->nrows  = cimg->C[1]->nrows;
    f->nelems = cimg->C[0]->ncols*cimg->C[0]->nrows;
    f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));
    for (i=0; i < f->nelems; i++)
    {
        f->elem[i].feat = AllocFloatArray(3*nscales);
        f->nfeats       = 3*nscales;
    }

    for (j=0; j < 3; j=j+1)
    {
        img1 = CopyImage(cimg->C[j]);
        for (s=1; s <= nscales; s=s+1)
        {
            A  = Circular(s);
            w  = AllocFloatArray(A->n);
            sigma2 = (s/3.0)*(s/3.0);
            K      =  2.0*sigma2;

            //compute kernel coefficients

            for (i=0; i < A->n; i++)
            {
                d    = A->dx[i]*A->dx[i]+A->dy[i]*A->dy[i];
                w[i] = 1.0/sqrt(2.0*PI*sigma2) * exp(-d/K); // Gaussian
            }

            // Convolution

            img2 = CreateImage(img1->ncols,img1->nrows);

            for (p=0; p < f->nelems; p++)
            {
                u.x = p%f->ncols;
                u.y = p/f->ncols;
                val = 0.0;
                for (i=0; i < A->n; i++)
                {
                    v.x = u.x + A->dx[i];
                    v.y = u.y + A->dy[i];
                    if (ValidPixel(img1,v.x,v.y))
                    {
                        q   = v.x + img1->tbrow[v.y];
                        val += (float)img1->val[q]*w[i];
                    }
                }
                img2->val[p]=(int)val;
            }
            free(w);

            // Copy features and reinitialize images

            for (p=0; p < f->nelems; p++)
            {
                f->elem[p].feat[s-1+(j*nscales)] = (float)img2->val[p]/f->Imax;
            }
            DestroyImage(&img2);
            DestroyAdjRel(&A);
        }
        DestroyImage(&img1);
    }
    return(f);
}

Features *MarkovImageFeats(Image *img, float dm)
{
    Features *f=(Features *)calloc(1,sizeof(Features));
    AdjRel   *A=NULL;
    int       p,q,i,j;
    //  float gx,gy;
    Pixel     u,v;

    f->Imax = MaximumValue(img);

    if (dm >= 1.0)
    {
        A=Circular(dm);
        f->ncols  = img->ncols;
        f->nrows  = img->nrows;
        f->nelems = f->ncols*f->nrows;
        f->nfeats = A->n-1;
        f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));
        for (i=0; i < f->nelems; i++)
        {
            f->elem[i].feat = AllocFloatArray(f->nfeats);
        }

        for (p=0; p < f->nelems; p++)
        {
            u.x = p%img->ncols;
            u.y = p/img->ncols;
            for (j=1; j < A->n; j++)
            {
                v.x = u.x + A->dx[j];
                v.y = u.y + A->dy[j];
                if (ValidPixel(img,v.x,v.y))
                {
                    q = v.x + img->tbrow[v.y];
                    f->elem[p].feat[j-1]=(float)img->val[q]/f->Imax;
                }
                else
                {
                    f->elem[p].feat[j-1]=(float)img->val[p]/f->Imax;
                }
            }
            /*
            gx = fabs(f->elem[p].feat[0]-f->elem[p].feat[2]);
            gy = fabs(f->elem[p].feat[1]-f->elem[p].feat[3]);
            if (gx < gy){
            f->elem[p].feat[1]=f->elem[p].feat[3]=(f->elem[p].feat[0]+f->elem[p].feat[2])/2.0;
            }else{
            if (gx > gy){
            f->elem[p].feat[0]=f->elem[p].feat[2]=(f->elem[p].feat[1]+f->elem[p].feat[3])/2.0;
            }
            }
            */
        }

        DestroyAdjRel(&A);
    }
    else
    {
        f->ncols  = img->ncols;
        f->nrows  = img->nrows;
        f->nelems = f->ncols*f->nrows;
        f->nfeats = 1;
        f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));
        for (i=0; i < f->nelems; i++)
        {
            f->elem[i].feat = AllocFloatArray(f->nfeats);
        }

        for (p=0; p < f->nelems; p++)
        {
            f->elem[p].feat[0]=(float)img->val[p]/f->Imax;
        }
    }

    return(f);
}

// for image compression
Features *KMarkovImageFeats(Image *img)
{
    Features *f=(Features *)calloc(1,sizeof(Features));
    AdjRel   *A=NULL;
    int       p,q,i,j;
    Pixel     u,v;

    A=KAdjacency();
    f->Imax   = MaximumValue(img);
    f->ncols  = img->ncols;
    f->nrows  = img->nrows;
    f->nelems = f->ncols*f->nrows;
    f->nfeats = A->n;
    f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));
    for (i=0; i < f->nelems; i++)
    {
        f->elem[i].feat = AllocFloatArray(f->nfeats);
    }

    for (p=0; p < f->nelems; p++)
    {
        u.x = p%img->ncols;
        u.y = p/img->ncols;
        for (j=0; j < A->n; j++)
        {
            v.x = u.x + A->dx[j];
            v.y = u.y + A->dy[j];
            if (ValidPixel(img,v.x,v.y))
            {
                q = v.x + img->tbrow[v.y];
                f->elem[p].feat[j]=(float)img->val[q]/f->Imax;
            }
            else
            {
                f->elem[p].feat[j]=(float)img->val[p]/f->Imax;
            }
        }
    }

    DestroyAdjRel(&A);
    return(f);
}

Features *MarkovCImageFeats(CImage *cimg, float dm)
{
    Features *f=(Features *)calloc(1,sizeof(Features));
    AdjRel   *A=NULL;
    int       p,q,i,j,k;
    Pixel     u,v;
    Image    *img=cimg->C[0];

    f->Imax = MAX(MAX(MaximumValue(cimg->C[0]),MaximumValue(cimg->C[1])),MaximumValue(cimg->C[2]));

    if (dm >= 1.0)
    {
        A=Circular(dm);
        f->ncols  = img->ncols;
        f->nrows  = img->nrows;
        f->nelems = f->ncols*f->nrows;
        f->nfeats = 3*A->n-3;
        f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));
        for (i=0; i < f->nelems; i++)
        {
            f->elem[i].feat = AllocFloatArray(f->nfeats);
        }

        for (p=0; p < f->nelems; p++)
        {
            u.x = p%img->ncols;
            u.y = p/img->ncols;
            for (j=1, k=0; j < A->n; j++, k=k+3)
            {
                v.x = u.x + A->dx[j];
                v.y = u.y + A->dy[j];
                if (ValidPixel(img,v.x,v.y))
                {
                    q = v.x + img->tbrow[v.y];
                    f->elem[p].feat[k]=cimg->C[0]->val[q]/(float)f->Imax;
                    f->elem[p].feat[k+1]=cimg->C[1]->val[q]/(float)f->Imax;
                    f->elem[p].feat[k+2]=cimg->C[2]->val[q]/(float)f->Imax;
                }
                else
                {
                    f->elem[p].feat[k]=cimg->C[0]->val[p]/(float)f->Imax;
                    f->elem[p].feat[k+1]=cimg->C[1]->val[p]/(float)f->Imax;
                    f->elem[p].feat[k+2]=cimg->C[2]->val[p]/(float)f->Imax;
                }
            }
        }
        DestroyAdjRel(&A);
    }
    else
    {
        f->ncols  = img->ncols;
        f->nrows  = img->nrows;
        f->nelems = f->ncols*f->nrows;
        f->nfeats = 3;
        f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));
        for (i=0; i < f->nelems; i++)
        {
            f->elem[i].feat = AllocFloatArray(f->nfeats);
        }

        for (p=0; p < f->nelems; p++)
        {
            f->elem[p].feat[0]=cimg->C[0]->val[p]/(float)f->Imax;
            f->elem[p].feat[1]=cimg->C[1]->val[p]/(float)f->Imax;
            f->elem[p].feat[2]=cimg->C[2]->val[p]/(float)f->Imax;
        }
    }
    return(f);
}


Features* LabFeats(CImage* cimg)
{
    int nrows=cimg->C[0]->nrows;
    int ncols=cimg->C[0]->ncols;

    int tam = nrows * ncols;

    Features* f = CreateFeatures(ncols,nrows,3);

    int z;
    float Imax = 1;

    for (z = 0;z < tam;z++)
    {
        //******* RGB original******//
        float R = (float)(cimg->C[0])->val[z];
        float G = (float)(cimg->C[1])->val[z];
        float B = (float)(cimg->C[2])->val[z];

        RGB2Lab(&R,&G,&B);

        f->elem[z].feat[0] = R;
        f->elem[z].feat[1] = G;
        f->elem[z].feat[2] = B;

        Imax = MAX(Imax,MAX(R,MAX(G,B)));
    }

    f->Imax = (int)Imax;

    return f;
}


void NormalizeLab(Features *lab)
{
  float min[lab->nfeats],max;
  int i,p;

  for (i=0; i < lab->nfeats; i++) {
    min[i]=FLT_MAX;
  }

  for (i=0; i < lab->nfeats; i++) {
    for (p=0; p < lab->nelems; p++) {
      if (lab->elem[p].feat[i] < min[i])
	min[i]=lab->elem[p].feat[i];
    }
  }

  max=FLT_MIN;
  for (i=0; i < lab->nfeats; i++) {
    min[i]=fabs(min[i]);
    for (p=0; p < lab->nelems; p++) {
      lab->elem[p].feat[i] = lab->elem[p].feat[i] + min[i];
      if (lab->elem[p].feat[i] > max)
	max = lab->elem[p].feat[i];
    }
  }

  for (i=0; i < lab->nfeats; i++) {
    for (p=0; p < lab->nelems; p++) {
      lab->elem[p].feat[i] = 255*lab->elem[p].feat[i]/max;
    }
  }

  lab->Imax=255;
}

Features* MSLowPassFeats(Features* feats, int nscales)
{
    if(feats == NULL)
    {
        Error("Features null","MSLowPassFeats");
    }
    if(feats->nfeats == 0)
    {
        Error("The must be at least one feat. Features->nfeats == 0!!","SteerablePyramidFeats");
    }

    int i,j;
    bool fillwzeroes = false;
    int nfeats = feats->nfeats;
    DImage* featsArray[feats->nfeats];

    /// Getting all image Features and turning them into power-of-2 sided DImages
    for( i = 0; i < nfeats; i++)
    {
        DImage* feat = GetFeature(feats,i);

        int x = feats->ncols;
        int y = feats->nrows;
        /// Checks if x is power of two: (x&(x-1)) == 0
        /// Algorithm from http://aggregate.org/MAGIC/#Is%20Power%20of%202

        if(!IsPowerOf2(x) || !IsPowerOf2(y))
        {
            featsArray[i] = DImagePower2(feat);
            DestroyDImage(&feat);
            fillwzeroes = true;
        }else{
            featsArray[i] = feat;
        }
    }

    int index = 0;
    int ncols = featsArray[0]->ncols;
    int nrows = featsArray[0]->nrows;

    Filterbank* bank = CreateLPFilterBank(ncols, nrows, nscales, nfeats);

    for( i = 0; i < nfeats; i++)
    {
        ApplyMSLowPass(featsArray[i], bank, index);

        /** Updates the index where the features should be placed in f,
         *  i.e 0, 3, 6, etc.
         **/
        index += nscales;
    }

    Features* result = NULL;

    /// If image was filled with zeroes, remove zeroes **/
    if(fillwzeroes)
    {
        result = TrimFeatures(bank->m_pFeatures,feats->ncols, feats->nrows);
    }else{
        result = CopyFeatures(bank->m_pFeatures);
    }

    result->Imax = feats->Imax;

    /// "Normalizing" features
    for( i = 0; i < result->nelems; i++)
        for(j = 0; j < result->nfeats; j++)
            result->elem[i].feat[j] /= result->Imax;

    for( i = 0; i < nfeats; i++) DestroyDImage(&(featsArray[i]));

    DestroyFilterbank(&bank);

    return result;
}


Features* SteerablePyramidFeats(Features* feats, int nscales, int norientations)
{
    if (feats == NULL)
    {
        Error("Features null","SteerablePyramidFeats");
    }
    if (feats->nfeats == 0)
    {
        Error("The must be at least one feat. Features->nfeats == 0!!","SteerablePyramidFeats");
    }

    int i;
    bool fillwzeroes = false;
    int nfeats = feats->nfeats;
    DImage* featsArray[feats->nfeats];

    /// Getting all image Features and turning them into power-of-2 sided DImages
    for ( i = 0; i < nfeats; i++)
    {
        DImage* feat = GetFeature(feats,i);
        if (!IsPowerOf2(feats->ncols) || !IsPowerOf2(feats->nrows))
        {
            featsArray[i] = DImagePower2(feat);
            DestroyDImage(&feat);
            fillwzeroes = true;
        }
        else
        {
            featsArray[i] = feat;
        }
    }

    int index = 0;
    int ncols = featsArray[0]->ncols;
    int nrows = featsArray[0]->nrows;

    Filterbank* bank = CreateSPFilterBank(ncols, nrows, nscales, norientations, nfeats);

    for ( i = 0; i < nfeats; i++)
    {
        ApplySPFilterBank(featsArray[i], bank, index);

        /** Updates the index where the features should be placed in bank->m_pFeatures,
         *  i.e 0, 24, 48, etc.
         **/
        index += nscales*norientations;
    }

    Features* result = NULL;

    /// If image was filled with zeroes, remove zeroes **/
    if (fillwzeroes)
    {
        result = TrimFeatures(bank->m_pFeatures,feats->ncols, feats->nrows);
    }else{
        result = CopyFeatures(bank->m_pFeatures);
    }

    for(i = 0; i < nfeats; i++) DestroyDImage(&(featsArray[i]));

    DestroyFilterbank(&bank);

    return result;
}

double* SPComputeEnergy(Features* feats)
{
    int n = feats->nfeats;
    double* energy = AllocDoubleArray(n);

    int i, j;

    for (i = 0; i < n; i++)
        for (j = 0; j < feats->nelems; j++)
            energy[i] += abs((double)feats->elem[j].feat[i]);

    return energy;
}

int* SPOriShiftMax(double* energy, int nbands, int nscales, int norientations)
{
    int* ori_shift = AllocIntArray(norientations*nbands);
    double* dorientations = AllocDoubleArray(norientations*nbands);
    int s,o,b,i,itmp;
    int bandfeats = nscales*norientations;
    double dtmp;

    for(b = 0; b < nbands; b++)
        for (o = 0; o < norientations; o++)
            for (s = 0; s < nscales; s++)
                dorientations[o + b*norientations] += energy[o + s*norientations + b*bandfeats];

    for(b = 0; b < nbands; b++)
        for (o = 0; o < norientations; o++)
            ori_shift[o + b*norientations] = o;

    for(b = 0; b < nbands; b++)
    {
        for (o = 0; o < norientations; o++)
        {
            double dmax = dorientations[o + b*norientations];
            int maxindex = o;
            int omax = ori_shift[o];

            for (i = o + 1; i < norientations; i++)
            {
                if (dorientations[i + b*norientations] > dmax)
                {
                    dmax = dorientations[i + b*norientations];
                    maxindex = i;
                    omax = ori_shift[i + b*norientations];
                }
            }

            dtmp = dorientations[o + b*norientations];
            dorientations[o + b*norientations] = dmax;
            dorientations[maxindex + b*norientations] = dtmp;

            itmp = ori_shift[o + b*norientations];
            ori_shift[o + b*norientations] = omax;
            ori_shift[maxindex + b*norientations] = itmp;
        }
    }

    free(dorientations);

    return ori_shift;
}

int* SPSclShiftMax(double* energy, int nbands, int nscales, int norientations)
{
    int* scale_shift = AllocIntArray(nscales*nbands);
    double* dscales = AllocDoubleArray(nscales*nbands);
    int s,o,b,i,itmp;
    int bandfeats = nscales*norientations;
    double dtmp;

    for(b = 0; b < nbands; b++)
        for (s = 0; s < nscales; s++)
            for (o = 0; o < norientations; o++)
                dscales[s + b*nscales] += energy[o + s*nscales + b*bandfeats];

    for(b = 0; b < nbands; b++)
        for (s = 0; s < nscales; s++)
            scale_shift[s + b*nscales] = s;

    for(b = 0; b < nbands; b++)
    {
        for (s = 0; s < nscales; s++)
        {
            double dmax = dscales[s + b*nscales];
            int maxindex = s;
            int smax = scale_shift[s + b*nscales];

            for (i = s + 1; i < nscales; i++)
            {
                if (dscales[i + b*nscales] > dmax)
                {
                    dmax = dscales[i + b*nscales];
                    maxindex = i;
                    smax = scale_shift[i + b*nscales];
                }
            }

            dtmp = dscales[s + b*nscales];
            dscales[s + b*nscales] = dmax;
            dscales[maxindex + b*nscales] = dtmp;

            itmp = scale_shift[s + b*nscales];
            scale_shift[s + b*nscales] = smax;
            scale_shift[maxindex + b*nscales] = itmp;
        }
    }
    free(dscales);

    return scale_shift;
}

Features* SPShiftFeaturesBySclOri(Features* feats, int* scale_shift, int nscales, int* ori_shift, int norientations)
{
    int i,j,b;
    int bandfeats = nscales*norientations;
    int nbands = feats->nfeats/(bandfeats);
    Features* shifted_feats = CreateFeatures(feats->ncols, feats->nrows, feats->nfeats);

    for (i = 0; i < feats->nelems; i++)
    {
        for(b = 0; b < nbands; b++)
        {
            for (j = 0; j < bandfeats; j++)
            {
                int sindex = (j/norientations)%nscales;
                int oindex = j%norientations;
                int dest = scale_shift[sindex + b*nscales]*norientations + ori_shift[oindex + b*norientations] + b*bandfeats;
                shifted_feats->elem[i].feat[j + b*bandfeats] = feats->elem[i].feat[dest];
            }
        }
    }

    return shifted_feats;

}

Features* ShiftedSPFeats(Features* feats, int nscales, int norientations)
{
    Features* result = SteerablePyramidFeats(feats, nscales, norientations);

    double* energy = SPComputeEnergy(result);
    int* ori_shift = SPOriShiftMax(energy, feats->nfeats, nscales, norientations);
    int* scale_shift = SPSclShiftMax(energy, feats->nfeats, nscales, norientations);

    Features* final = SPShiftFeaturesBySclOri(result, scale_shift, nscales, ori_shift, norientations);

    free(ori_shift);
    free(scale_shift);
    free(energy);

    DestroyFeatures(&result);

    return final;
}

Features* CLabShiftedSPFeats(CImage* cimg, int nscales, int norientations)
{
    Features* lab  = LabFeats(cimg);

    Features* stb = ShiftedSPFeats(lab,nscales,norientations);

    DestroyFeatures(&lab);

    return stb;
}

