
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ift.h"



int main(int argc, char **argv) {
  Queue *q;
  int i,color[40];
  char cs[41];

  q = CreateQueue(10,40);

  /* teste de ordem e coerencia de cores */

  for(i=0;i<40;i++)
    color[i] = WHITE;

  memset(cs,0,41);
  for(i=0;i<40;i++)
    switch(q->L.elem[i].color) {
    case WHITE: cs[i]='W'; break;
    case GRAY:  cs[i]='G'; break;
    case BLACK: cs[i]='B'; break;
    }
  printf("color %s\n",cs);
  
  for(i=39;i>=10;i--) {
    printf("INS %d %d\n",i/4,i);
    InsertQueue(q, i/4, i);
    color[i] = GRAY;
  }

  memset(cs,0,41);
  for(i=0;i<40;i++)
    switch(q->L.elem[i].color) {
    case WHITE: cs[i]='W'; break;
    case GRAY:  cs[i]='G'; break;
    case BLACK: cs[i]='B'; break;
    }
  printf("color %s\n",cs);

  while(!EmptyQueue(q)) {
    i = RemoveQueue(q);
    printf("rem %d (%d)\n",i,i/4);
  }

  memset(cs,0,41);
  for(i=0;i<40;i++)
    switch(q->L.elem[i].color) {
    case WHITE: cs[i]='W'; break;
    case GRAY:  cs[i]='G'; break;
    case BLACK: cs[i]='B'; break;
    }
  printf("color %s\n",cs);
   
  DestroyQueue(&q);
  return 0;
}
