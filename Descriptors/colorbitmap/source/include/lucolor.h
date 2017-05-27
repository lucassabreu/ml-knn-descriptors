#ifndef _LUCOLOR2_H_
#define _LUCOLOR2_H_

#include "cimage.h"
#include "image.h"
#include "common.h"

#define QTD_BLOCOS 100

typedef struct vetor {
    float *r;
    float *g;
    float *b;
    int *tam;
}Vetor;

Vetor *CreateVetor(long int blocos);
void DestroyVetor(Vetor **v);
Vetor *ReadFileVetor(char *filename, double *media, double *desvio);
Vetor *ReadFileVetorBin(char *filename, double *media, double *desvio);
void WriteFileVetor(Vetor *h,char *filename,double *media, double *desvio);
void WriteStreamVetor(Vetor *h,FILE *fp, double *media, double *desvio);
void WriteFileVetorBin(Vetor *h,char *filename,double *media, double *desvio);
void WriteStreamVetorBin(Vetor *h,FILE *fp, double *media, double *desvio);

void MediaImage(CImage *image, double *media, long int tam);
void DesvioImage(CImage *image, double *media,double *desvio, long int tam);
Vetor *LuColor2(CImage *image, long int tam, double *media, double *desvio);
void Bitmap2(double *media,Vetor *mbloco,Vetor *bitmap, long int blocos);
void MediaBlocos2(CImage *imagem,Vetor *mbloco,long int tam,long int n, int qtde);

double Hamming(Vetor *Ta,Vetor *Tb, double *ma, double *mb, double *da, double *db);

#endif
