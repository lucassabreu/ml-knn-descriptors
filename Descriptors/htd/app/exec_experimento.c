#include "plugin.h"
#include "../include/libcolordescriptors.h"
timer *Tic() { /* It marks the initial time */
	timer *tic=NULL;
	tic = (timer *)malloc(sizeof(timer));
	gettimeofday(tic, NULL);
	return (tic);
}

timer *Toc() { /* It marks the final time */
	timer *toc=NULL;
	toc = (timer *)malloc(sizeof(timer));
	gettimeofday(toc, NULL);
	return (toc);
}

float CTime(timer *tic, timer *toc) /* It computes the time difference */
{
	float t=0.0;
	if ((tic!=NULL)&&(toc!=NULL)) {
		t = (toc->tv_sec-tic->tv_sec)*1000.0 + (toc->tv_usec-tic->tv_usec)
				*0.001;
		free(tic);
		free(toc);
		tic=NULL;
		toc=NULL;
	}
	return (t);
}


#define NIMG (3906)
int main(int argc, char *argv[]) {
        int Q[] = {10,18,103,158,166,225,287,342,463,486,503,516,551,567,568,617,674,674,711,780,800,804,824,852,882,934,1009,1113,1128,1200,1351,1379,1425,1437,1495,1514,1539,1558,1582,1690,1838,1889,1910,1988,1990,2016,2137,2149,2196,2197,2222,2243,2274,2282,2362,2375,2423,2441,2445,2465,2494,2512,2522,2545,2556,2584,2699,2720,2727,2752,2799,2825,2834,2837,2860,2891,2913,2969,3042,3045,3063,3093,3095,3149,3171,3205,3223,3282,3340,3367,3410,3425,3426,3518,3546,3584,3609,3644,3689,3799};
        
        char buff1[256];
	void *descr[NIMG];
        int q_,i;
        FILE *out;
		
	if(argc < 2) {
		printf("exec_exp <joint|indep_cl1|indep_l1p|color>\n");
		exit(0);
	}
	int op = 0;
	if(!strcmp(argv[1],"joint")){
		op = 1;
		out = fopen("./joint.dists","wt");
	} else if(!strcmp(argv[1],"indep_cl1")){
		op = 2;
		out = fopen("./indep_cl1.dists","wt");
	} else if(!strcmp(argv[1],"indep_l1p")){
		op = 3;
		out = fopen("./indep_l1p.dists","wt");
	} else if(!strcmp(argv[1],"color")) {
		op = 4;
		out = fopen("./color.dists","wt");
	} else {
		printf("exec_exp <joint|indep_cl1|indep_l1p|color>\n");
		exit(0);
	}
		
	printf("loading in memory...");
	for(i=0;i<NIMG;i++) {
		switch(op){
			case 1:
				sprintf(buff1,"c:/cbir/Corel3906/joint/%d.key",i);
				descr[i] = ReadFileJointAutoCorrelogram(buff1);
			break;
			case 2:
			case 3:
				sprintf(buff1,"c:/cbir/Corel3906/indep_b/%d.key",i);
				descr[i] = ReadFileIndependentAutoCorrelogram(buff1);
			break;
			case 4:
				sprintf(buff1,"c:/cbir/Corel3906/color/%d.key",i);
				descr[i] = ReadFileAutoCorrelogram(buff1);
			break;
		}

	}
	printf("starting computation...\n");
	
        for(q_=0;q_<100;q_++) {
          int q = Q[q_];
		timer *t0 = Tic();
		switch(op) {
			case 1:
				for(i=0;i<NIMG;i++)
					fprintf(out,"%d %d %f\n",q,i,L1JointAutoCorrelogramDistance(descr[q], descr[i]));
				break;
			case 2:
				for(i=0;i<NIMG;i++)					
					fprintf(out,"%d %d %f\n",q,i,SumL1IndependentAutoCorrelogramDistance(descr[q], descr[i]));
				break;
			case 3:
				for(i=0;i<NIMG;i++)					
					fprintf(out,"%d %d %f\n",q,i,L1IndependentAutoCorrelogramDistance(descr[q], descr[i]));
				break;
			case 4:
				for(i=0;i<NIMG;i++)
					fprintf(out,"%d %d %f\n",q,i,L1AutoCorrelogramDistance(descr[q], descr[i]));
				break;
		}
	  timer *tf = Toc();
	  printf("%d %f\n",q,CTime(t0,tf));
        }
        fclose(out);

	return 0;
}
