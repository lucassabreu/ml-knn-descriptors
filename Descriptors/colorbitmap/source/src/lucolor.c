#include "lucolor.h"

Vetor *CreateVetor(long int blocos)
{
  Vetor *v=NULL;
  

  v = (Vetor *) calloc(1,sizeof(Vetor));
  if (v == NULL){
    printf("ERRO na criacao do Vetor");
  }

  v->r   = AllocFloatArray(blocos);
  v->g   = AllocFloatArray(blocos);
  v->b   = AllocFloatArray(blocos);
  v->tam = AllocIntArray(blocos);
  
  v->tam[0] = blocos;
 
 return(v);
}

void DestroyVetor(Vetor **v)
{
  Vetor *aux;
  
  aux = *v;
  if(aux != NULL){
    if (aux->r != NULL) free(aux->r); 
    if (aux->g != NULL)   free(aux->g);
    if (aux->b != NULL)   free(aux->b);
    
    if (aux->tam != NULL)   free(aux->tam);
    free(aux);    
    *v = NULL;
  }   
}

Vetor *ReadFileVetor(char *filename, double *media, double *desvio)
//Vetor *ReadFileVetor(char *filename)
{
  Vetor *h = NULL;
  int i, nblocos;
  FILE *fp;
  int c;
  double x;

  fp = fopen(filename,"r");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }

  fscanf(fp, "%d\n", &nblocos); //le o numero de blocos
  h = CreateVetor(nblocos);
  
  fscanf(fp, "%lf\t", &x);//media
  media[0] = x;
  fscanf(fp, "%lf\t", &x);
  media[1] = x;
  fscanf(fp, "%lf\n", &x);
  media[2] = x;

  fscanf(fp, "%lf\t", &x);//desvio
  desvio[0] = x; 
  fscanf(fp, "%lf\t", &x);
  desvio[1] = x;
  fscanf(fp, "%lf\n", &x);
  desvio[2] = x;

  for (i=0; i<nblocos; i++){
    fscanf(fp, "%d\t", &c);
    h->r[i] = c;
    fscanf(fp, "%d\t", &c);
    h->g[i] = c;
    fscanf(fp, "%d\n", &c);
    h->b[i] = c; 
  }
  fclose(fp);
  
  return(h);
}

Vetor *ReadFileVetorBin(char *filename, double *media, double *desvio)
//Vetor *ReadFileVetor(char *filename)
{
    Vetor *h = NULL;
    //int nblocos;
    FILE *fp;

    //copia a estrutura h->x para um vetor de inteiros
    int i;
    int *auxR=NULL, *auxG=NULL, *auxB=NULL;

    fp = fopen(filename,"rb");
    if (fp == NULL){
        fprintf(stderr,"Cannot open %s\n",filename);
        exit(-1);
    }

    //fscanf(fp, "%d", &nblocos); //le o numero de blocos
    //fread(&nblocos, sizeof(int), 1, fp);
    //printf("\n\nQTD_BLOCOS=%d\n", QTD_BLOCOS);
    auxR = (int*) calloc(QTD_BLOCOS, sizeof(int));
    auxG = (int*) calloc(QTD_BLOCOS, sizeof(int));
    auxB = (int*) calloc(QTD_BLOCOS, sizeof(int));

    fread(media, sizeof(double), 3, fp);
    fread(desvio, sizeof(double), 3, fp);
    fread(auxR, sizeof(int), QTD_BLOCOS, fp);
    fread(auxG, sizeof(int), QTD_BLOCOS, fp);
    fread(auxB, sizeof(int), QTD_BLOCOS, fp);

    //printf("VALORES LIDOS:\n");
    //printf("media=%lf\t%lf\t%lf\n", media[0], media[1], media[2]);
    //printf("desvio=%lf\t%lf\t%lf\n", desvio[0], desvio[1], desvio[2]);

    h = CreateVetor(QTD_BLOCOS);
    for (i=0; i<h->tam[0]; i++) {
        h->r[i] = auxR[i];
        h->g[i] = auxG[i];
        h->b[i] = auxB[i];
    }
    free(auxR);
    free(auxG);
    free(auxB);
    fclose(fp);

    return(h);
}


void WriteFileVetor(Vetor *h,char *filename,double *media, double *desvio)
{
  FILE *fp;
  
  fp = fopen(filename,"w");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  WriteStreamVetor(h, fp, media,desvio);
  fclose(fp);
}

void WriteStreamVetor(Vetor *h,FILE *fp, double *media, double *desvio)
{
  int i;
  
 
  if (fp != NULL){
    fprintf(fp,"%d\n",h->tam[0]);
    fprintf(fp,"%lf\t",*(media+0));//media
    fprintf(fp,"%lf\t",*(media+1));
    fprintf(fp,"%lf\n",*(media+2));

    fprintf(fp,"%lf\t",*(desvio+0));//desvio
    fprintf(fp,"%lf\t",*(desvio+1));
    fprintf(fp,"%lf\n",*(desvio+2));

    for (i=0; i < h->tam[0]; i++)
      {
       fprintf(fp,"%d\t",(int)h->r[i]);
       fprintf(fp,"%d\t",(int)h->g[i]);
       fprintf(fp,"%d\n",(int)h->b[i]);
      } 
   fprintf(fp,"\n");
  }
}

void WriteFileVetorBin(Vetor *h, char *filename, double *media, double *desvio)
{
  FILE *fp;

  fp = fopen(filename,"wb");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  WriteStreamVetorBin(h, fp, media, desvio);
  fclose(fp);
}

void WriteStreamVetorBin(Vetor *h, FILE *fp, double *media, double *desvio)
{
    //copia a estrutura h->x para um vetor de inteiros
    int i;
    int *auxR=NULL, *auxG=NULL, *auxB=NULL;
    auxR = (int*) calloc(h->tam[0], sizeof(int));
    auxG = (int*) calloc(h->tam[0], sizeof(int));
    auxB = (int*) calloc(h->tam[0], sizeof(int));

    for (i=0; i<h->tam[0]; i++) {
        auxR[i] = h->r[i];
        auxG[i] = h->g[i];
        auxB[i] = h->b[i];
    }

    if (fp != NULL){
        //fprintf(fp,"%d",h->tam[0]);  //NAO ESTOU ESCREVENDO A QTD DE BLOCOS NO ARQUIVO! USO A CONSTANTE QUE FICA OK
        fwrite(media, sizeof(double), 3, fp);
        fwrite(desvio, sizeof(double), 3, fp);
        //printf("VALORES ESCRITOS:\n");
        //printf("media=%lf\t%lf\t%lf\n", media[0], media[1], media[2]);
        //printf("desvio=%lf\t%lf\t%lf\n", desvio[0], desvio[1], desvio[2]);
        //fwrite(h->r, sizeof(int), h->tam[0], fp);
        //fwrite(h->g, sizeof(int), h->tam[0], fp);
        //fwrite(h->b, sizeof(int), h->tam[0], fp);
        fwrite(auxR, sizeof(int), h->tam[0], fp);
        fwrite(auxG, sizeof(int), h->tam[0], fp);
        fwrite(auxB, sizeof(int), h->tam[0], fp);
    }

    free(auxR);
    free(auxG);
    free(auxB);
}

/////////////////////////////////////////////////////////////////

void MediaImage(CImage *image, double *media, long int tam)

{
  int i;
  long long int m1=0, m2=0, m0=0;

  for (i=0; i < tam; i++) { 
       m0+=image->C[0]->val[i];
       m1+=image->C[1]->val[i];
       m2+=image->C[2]->val[i];
   }
  media[0] = (double)m0/tam;// medias das bandas RGB
  media[1] = (double)m1/tam;
  media[2] = (double)m2/tam;

}

void DesvioImage(CImage *image, double *media, double *desvio, long int tam)

{
  int i;
  double aux1=0,aux2=0,aux3=0;

  for (i=0; i < tam; i++) { 
       aux1 += pow(image->C[0]->val[i] - media[0],2);
       aux2 += pow(image->C[1]->val[i] - media[1],2);
       aux3 += pow(image->C[2]->val[i] - media[2],2);

   }
   desvio[0]= sqrt( (aux1 / (tam-1)) );// desvio padrao das bandas RGB
   desvio[1]= sqrt( (aux2 / (tam-1)) );
   desvio[2]= sqrt( (aux3 / (tam-1)) );

}

void MediaBlocos2(CImage *image,Vetor *mbloco,long int tam,long int n, int qtde)
{
  long int j,i,aux=0;
  double mr,mg,mb;


  for(i=0; i < QTD_BLOCOS ;i++) {// 10 blocos 
      mbloco->r[i]= 0;//zerar posicoes
      mbloco->g[i]= 0;
      mbloco->b[i]= 0;
      mr=0;
      mg=0;
      mb=0;
      if(i != QTD_BLOCOS-1) {//senao for o ultimo bloco pq ele pode ter menos elementos
          for (j=aux; j < (aux+n); j++) {
              mr += (image->C[0]->val[j]);//printf("R:%d",image->C[0]->val[j]);
              mg += (image->C[1]->val[j]);//printf("G:%d",image->C[1]->val[j]);
              mb += (image->C[2]->val[j]);//printf("B:%d",image->C[2]->val[j]);
          }
          mbloco->r[i] = (double)(mr/n);// medias das bandas RGB
          mbloco->g[i] = (double)(mg/n);
          mbloco->b[i] = (double)(mb/n);//img->val[i]
      } else {
          if(qtde==0) qtde=n; //divisao certa
          for (j=aux; j < tam; j++) { //percorrer até final da imagem
              mr += (image->C[0]->val[j]);//printf("R:%d",image->C[0]->val[j]);
              mg += (image->C[1]->val[j]);//printf("G:%d",image->C[1]->val[j]);
              mb += (image->C[2]->val[j]);//printf("B:%d",image->C[2]->val[j]);
          }
          mbloco->r[i] = (double)(mr/qtde);// medias das bandas RGB
          mbloco->g[i] = (double)(mg/qtde);
          mbloco->b[i] = (double)(mb/qtde);//img->val[i]
      }
      aux=aux+n;
  } //for
}

void Bitmap2(double *media,Vetor *mbloco,Vetor *bitmap, long int blocos)// Forma os TR's, TG's e TB's... 
{
    int i;

    for (i=0; i < mbloco->tam[0]; i++) { 
        if(mbloco->r[i] > *(media+0) ) {//se media do blocoR e maior que a media da imagemR
            bitmap->r[i]=1;
        } else {
            bitmap->r[i]=0;
        }

        if(mbloco->g[i] > *(media+1) ) { //se media do blocoG e maior que a media da imagemG
            bitmap->g[i]=1;
        } else {
            bitmap->g[i]=0;
        }

        if(mbloco->b[i] > *(media+2) ) {//se media do blocoG e maior que a media da imagemB
            bitmap->b[i]=1;
        } else {
            bitmap->b[i]=0;
        }
    }
}

Vetor *LuColor2(CImage *image, long int tam, double *media, double *desvio)
{
    int resto=0,nelem=0;

    nelem = tam / QTD_BLOCOS; //numero de elementos
    resto = tam % QTD_BLOCOS;

    Vetor *mblocos = CreateVetor(QTD_BLOCOS);
    Vetor *trgb = CreateVetor(QTD_BLOCOS);

    MediaImage(image,media,tam);
    DesvioImage(image,media,desvio,tam);
    MediaBlocos2(image,mblocos,tam,nelem,resto);
    Bitmap2(media,mblocos,trgb,QTD_BLOCOS);
    DestroyVetor(&mblocos);
    return(trgb);
}

double Hamming(Vetor *Ta,Vetor *Tb, double *ma, double *mb, double *da, double *db)
{
    int i,Hab=0,tr=0,tg=0,tb=0;
    double m=0,d=0,dab=0;

    for (i=0; i < Ta->tam[0]; i++) { 
        tr+=( abs( (Ta->r[i])-(Tb->r[i]) ) );
        tg+=( abs( (Ta->g[i])-(Tb->g[i]) ) );
        tb+=( abs( (Ta->b[i])-(Tb->b[i]) ) );   
    }
    Hab += tr + tg + tb;

    for (i=0; i < 3; i++) {
        m += pow(ma[i] - mb[i],2);
        d += pow(da[i] - db[i],2);
        //printf("ma=%lf\tmb=%lf\tm=%lf\n", ma[i], mb[i], m);
        //printf("da=%lf\tdb=%lf\td=%lf\n\n", da[i], db[i], d);
    }
    //printf("m=%lf\t d=%lf\n", m, d);
    dab = ((double)Hab/(3.0*QTD_BLOCOS)) + sqrt( m + d );

    return dab;
}

