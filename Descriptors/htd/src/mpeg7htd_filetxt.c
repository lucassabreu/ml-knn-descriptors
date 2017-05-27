#include "../include/mpeg7htd.h"
#include "../include/common.h"
#include "../include/cimage.h"
#include "../include/matrix.h"
#include "../include/gabor.h"


/* INFELIZMENTE DA FORMA COMO A BIBLIOTECA ESTÁ PROJETADA NAO É POSSIVEL
 * CALCULAR O DESVIO GLOBAL DE ALGUNS VALORES DE TODOS FEATURES EXTRAIDOS
 * DESTA FORMA DEIXEI ESTAS FUNCOES PARA SEREM PREENCHIDAS COM OS VALORES
 * REAIS, ENTRETANTO, DEIXEI RETORNANDO UMA CONSTANTE = 1 PARA RODAR
 */
float GlobalStandardDesviationMediaHTD() {
	return 1.0;
}
float GlobalStandardDesviationDesvioHTD() {
	return 1.0;
}
/**************************************************************************/

float ComplexMagnitude(float real, float imag) {
	return sqrt(real*real + imag*imag);
}


Mpeg7HTDDescriptor *Mpeg7HTDDescriptorInit(int scale, int orientation) {
	Mpeg7HTDDescriptor *descr = NULL;
	descr = (Mpeg7HTDDescriptor *) malloc(sizeof(Mpeg7HTDDescriptor));
	descr->scale = scale;
	descr->orientation = orientation;
	return descr;
}

Mpeg7HTDDescriptor *CreateMpeg7HTDDescriptorReady(int scale, int orientation, float *media, float *desvio) {
	Mpeg7HTDDescriptor *descr = Mpeg7HTDDescriptorInit(scale,orientation);
	descr->media = media;
	descr->desvio = desvio;
	return descr;
}

Mpeg7HTDDescriptor *CreateMpeg7HTDDescriptor(int hei, int wid, int side, int scale, int orientation, 
		int flag, double Ul, double Uh, Matrix *F_r, Matrix *F_i) {
	
	Mpeg7HTDDescriptor *descr = Mpeg7HTDDescriptorInit(scale,orientation);
	
	float *media  = (float *) malloc(sizeof(float) * scale * orientation);
	float *desvio = (float *) malloc(sizeof(float) * scale * orientation);
	
	int s,o;
	float sum = 0;
	int i = 0;
	int j = 0;
	int h,w;
	int n = hei * wid;
	
	for(s=0,i=0,j=0;s<scale;s++) {
		for(o=0;o<orientation;o++) {
			sum = 0.0;
			for(h=0;h<hei;h++) {
				for(w=0;w<wid;w++) {
					int b = j %  F_r->width;
					int a = (j-b) / F_r->width;
					float v = ComplexMagnitude(F_r->data[a][b],F_i->data[a][b]);					
					sum += v;
					j++;
				}
			}
			media[i] = ((float) sum / (float) n);
			i++;
		}
	}
	for(s=0,i=0,j=0;s<scale;s++) {
		for(o=0;o<orientation;o++) {
			sum = 0.0;
			for(h=0;h<hei;h++) {
				for(w=0;w<wid;w++) {
					int b = j %  F_r->width;
					int a = (j-b) / F_r->width;
					float v = ComplexMagnitude(F_r->data[a][b],F_i->data[a][b]);					
					sum += (v - media[i]) * (v - media[i]);
					j++;
				}
			}
			desvio[i] = sqrt((float) sum / (float) n);
			i++;
		}
	}
	descr->media = media;
	descr->desvio = desvio;
	
	return descr;	
}
void DestroyMpeg7HTDDescriptor(Mpeg7HTDDescriptor **c) {
	Mpeg7HTDDescriptor *aux;

	aux = *c;
	if (aux != NULL) {
		if(aux->media!=NULL)free(aux->media);
		if(aux->desvio!=NULL)free(aux->desvio);
		free(aux);
		*c = NULL;
	}	
}

Mpeg7HTDDescriptor *ExtractMpeg7HTDDescriptor(CImage *rgbImage) {
	int hei, wid, i, j, side, scale, orientation, flag;
	Matrix *img, *F_r, *F_i;	
	double Ul, Uh;
	
	wid = rgbImage->C[0]->ncols;
	hei = rgbImage->C[0]->nrows;
	CreateMatrix(&img, hei, wid);
	
	//converte para gray-scale
	for (i=0;i<hei;i++)
		for (j=0;j<wid;j++)  {
			int p = i*wid+j;
			int R = rgbImage->C[0]->val[p];
			int G = rgbImage->C[1]->val[p];
			int B = rgbImage->C[2]->val[p];
			//de acordo com http://www.mathworks.com/support/solutions/data/1-1ASCU.html
			double intensity = ((double)0.2989 * R + 0.5870 * G + 0.1140 * B);
			//faz rounding (truncando mesmo! podia somar 0.5 mas achei melhor assim)
			img->data[i][j] =(int)(intensity);		
		}
	
	//parametros do artigo da PAMI
	scale = 4;
	orientation = 6;
	Ul = 0.05;
	Uh = 0.4;
	flag = 0;
	side = 60;
	
	//cria as imagens filtradas para todas orientacoes e escalas
	CreateMatrix(&F_r, hei*scale, wid*orientation);
	CreateMatrix(&F_i, hei*scale, wid*orientation);
	GaborFilteredImg(F_r, F_i, img, side, Ul, Uh, scale, orientation, flag);
	
	//faz o descritor
	Mpeg7HTDDescriptor *descr = CreateMpeg7HTDDescriptor(hei,wid,side, scale, orientation, flag, Ul, Uh, F_r, F_i);	

	FreeMatrix(F_r);
	FreeMatrix(F_i);
	FreeMatrix(img);
	return descr;
}


void WriteStreamMpeg7HTDDescriptor(Mpeg7HTDDescriptor *c, FILE *fp) {
	if(c == NULL) return;
	if (fp != NULL) {
		float *media = c->media;
		float *desvio = c->desvio;
		int i;
		int n = c->scale * c->orientation;
		fprintf(fp,"%d %d ",c->scale, c->orientation);
		for(i=0;i<n;i++)
			fprintf(fp,"%f %f ",media[i],desvio[i]);
	}
}
void WriteFileMpeg7HTDDescriptor(Mpeg7HTDDescriptor *c, char *filename) {
	FILE *fp;
	if(c == NULL) return;

	fp = fopen(filename, "wt");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open %s\n", filename);
		exit(-1);
	}
	WriteStreamMpeg7HTDDescriptor(c, fp);
	fclose(fp);
}

Mpeg7HTDDescriptor *ReadStreamMpeg7HTDDescriptor(FILE *fp) {
	Mpeg7HTDDescriptor *descr = NULL; 

	if (fp != NULL) {
		
		int scale,orientation;
		fscanf(fp,"%d %d",&scale,&orientation);
		float *media = (float *) malloc(sizeof(float) * scale * orientation);
		float *desvio = (float *) malloc(sizeof(float) * scale * orientation);
		int i;
		int n = scale * orientation;
		for(i=0;i<n;i++)
			fscanf(fp,"%f %f",&media[i],&desvio[i]);		
		descr = CreateMpeg7HTDDescriptorReady(scale, orientation, media, desvio);			
	}
	return descr;
}

Mpeg7HTDDescriptor *ReadFileMpeg7HTDDescriptor(char *fv_path) {
	FILE *fp;
	fp = fopen(fv_path, "rt");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open %s\n", fv_path);
		exit(-1);
	}
	Mpeg7HTDDescriptor *c = ReadStreamMpeg7HTDDescriptor(fp);
	fclose(fp);
	return c;
}
double DistanceMpeg7HTDDescriptor(Mpeg7HTDDescriptor *descr1, Mpeg7HTDDescriptor *descr2) {
	int s,o;
	int scale = descr1->scale;
	int orientation = descr1->orientation;
	
	assert(descr2->orientation == orientation);
	assert(descr2->scale == scale);
	
	int i = 0;
	float globalDesvMedia  = GlobalStandardDesviationMediaHTD();
	float globalDesvDesvio = GlobalStandardDesviationDesvioHTD();
	float d = 0.0;
	for(s=0;s<scale;s++) {
		for(o=0;o<orientation;o++) {
			float m1 = descr1->media[i];
			float m2 = descr2->media[i];
			float d1 = descr1->desvio[i];
			float d2 = descr2->desvio[i];
			d += fabs((m1-m2)/globalDesvMedia) + fabs((d1-d2) / globalDesvDesvio);
			i++;
		}
	}
	return d;
}
