#include "LBPri.h"

inline uchar ComputeNorm(float value)
{
  return((uchar)(255. * value));
}

Histogram *ComputeAccHistogram (Image *img)
{
    int i;
    Histogram *h = NULL;
    ulong *acc = AllocULongArray(SIZE);

    //histograma
    for(i=0; i < img->nrows*img->ncols; i++)
        acc[img->val[i]]+=1;

    //histograma acumulado
    for(i=1; i < SIZE; i++)
        acc[i]+=acc[i-1];

    //histograma aumulado normalizado
    h = CreateHistogram(SIZE);

    for(i=0; i < SIZE; i++)
        h->v[i] = (uchar) ComputeNorm((float) acc[i] / (float) acc[SIZE-1]);

    free(acc);

    return(h);
}

int getLbpVal(Image *img, int i, int j)
{
    int lbpval = 0;
    int changes = 0;
    int k;

    // 0   1   2
    // 7   gc  3
    // 6   5   4

    int gc = img->val[img->tbrow[i]+j];
    int g[8];

    //nao interpolou para achar vizinhos que nao sao exatamente no centro do pixel
    g[0] = (img->val[ img->tbrow[i-1] +j-1] >= gc);
    g[1] = (img->val[ img->tbrow[i-1] +j  ] >= gc);
    g[2] = (img->val[ img->tbrow[i-1] +j+1] >= gc);
    g[3] = (img->val[ img->tbrow[i]   +j+1] >= gc);
    g[4] = (img->val[ img->tbrow[i+1] +j+1] >= gc);
    g[5] = (img->val[ img->tbrow[i+1] +j  ] >= gc);
    g[6] = (img->val[ img->tbrow[i+1] +j-1] >= gc);
    g[7] = (img->val[ img->tbrow[i]   +j-1] >= gc);


    //calcula qtde de variacoes 0/1 ou 1/0
    for(k=0;k<7;k++)
        changes += (g[k]!=g[k+1]);
    changes += (g[7]!=g[0]);

    //se foram 2 ou menos variacoes, acumula os g's
    if (changes<=2)
        for (k=0;k<8;k++)
            lbpval += g[k];
    else lbpval = SIZE-1; //senao, usa P+1

    //printf(" %d",lbpval);
    return (lbpval);
}

Histogram *LBPri(Image *img)
{
  Histogram *histogram = NULL;
  int i,j;
  Image *lbpRep;
  int lbpval;
  
  lbpRep = CreateImage(img->ncols-2, img->nrows-2);
  
	
  for( i = 1; i < img->nrows-1; i++ ){
	for( j = 1; j < img->ncols-1; j++ ){
		lbpval = getLbpVal(img, i, j);
		lbpRep->val[lbpRep->tbrow[i-1]+j-1] = lbpval;
	}
  }

  histogram = ComputeAccHistogram(lbpRep);
  //printf("\n");
  DestroyImage(&lbpRep);
  
  return(histogram);
}

/********************/
//Encontra o minimo valor entre 3
float min(float x, float y, float z) {

    if ( (x<=y) && (x<=z) ) {
        return x;
    } else if ( (y<=x) && (y<=z) ) {
        return y;
    } else if ( (z<=x) && (z<=y) ) {
        return z;
    }
    return -1;
}

//Encontra o maximo valor entre 3
float max(float x, float y, float z) {

    if ( (x>=y) && (x>=z) ) {
        return x;
    } else if ( (y>=x) && (y>=z) ) {
        return y;
    } else if ( (z>=x) && (z>=y) ) {
        return z;
    }
    return -1;
}

void RGB2HSV_lbp(CImage *RGB, ImageHSV **HSV) {

    float r, g, b;
    float minVal, maxVal, delta;
    int i, j;

    for (i = 0; i < RGB->C[0]->nrows; i++) {
        for (j = 0; j < RGB->C[0]->ncols; j++) {

            //normaliza de 0 a 1
            r = (float) RGB->C[0]->val[j+RGB->C[0]->tbrow[i]]/255;
            g = (float) RGB->C[1]->val[j+RGB->C[1]->tbrow[i]]/255;
            b = (float) RGB->C[2]->val[j+RGB->C[2]->tbrow[i]]/255;

            minVal = min(r, g, b);
            maxVal = max(r, g, b);
            delta = maxVal - minVal;

            //H
            if (delta == 0) {
                HSV[i][j].H = 0;
            } else if (maxVal==r && g>=b) {
                HSV[i][j].H = 60*((g-b)/delta);
            } else if (maxVal==r && g<b) {
                HSV[i][j].H = 60*((g-b)/delta) + 360;
            } else if (maxVal==g) {
                HSV[i][j].H = 60*((b-r)/delta) + 120;
            } else if (maxVal==b) {
                HSV[i][j].H = 60* ((r-g)/delta) + 240;
            }

            //S
            if (maxVal==0) {
                HSV[i][j].S = 0;
            } else {
                HSV[i][j].S = delta/maxVal;
            }

            //V
            HSV[i][j].V = maxVal;

            //normalizando S e V entre 0 e 255
            HSV[i][j].S *= 255;
            HSV[i][j].V *= 255;

            //colocando H de 0 a 1
            //HSV[i][j].H /= 360;

        }
    }
}
