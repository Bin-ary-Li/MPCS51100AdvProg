#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "list.h"

const int STRLENG = 18;

struct Point_{
  char *color;
  float x;
  float y;
  float z;
};

typedef struct Point_ Point;

float dist (Point *a){
  return sqrt( (a->x)*(a->x)+(a->y)*(a->y)+(a->z)*(a->z));
}

int Point_comparer(const void *a, const void *b){
  Point *pa = (Point *) a;
  Point *pb = (Point *) b;

  float dista = dist(pa);
  float distb = dist(pb);

  if (dista < distb)
    return 1;
  else if (dista == distb) 
    return 0;
  else 
    return -1;
}

Point * create_ran_point(){
  Point * p;
  float x,y,z;
  float a = 10.0;

  p = (Point *) malloc(sizeof(Point));
  x = ((float)rand()/(float)(RAND_MAX)) * a - a/2.0;
  y = ((float)rand()/(float)(RAND_MAX)) * a - a/2.0;
  z = ((float)rand()/(float)(RAND_MAX)) * a - a/2.0;

  p->x = x; p->y = y; p->z = z;

  return p;
}

char * tostr(const void * ptr, char * res) {
  const Point * p;
  if (ptr == NULL || res == NULL) {
    return NULL;
  }
  p = (const Point*) ptr;
  sprintf(res, "%5.2f %5.2f %5.2f", p->x,p->y,p->z);
  return res;
}

void write_file(Point **p, int n){
  int i;
  FILE *file = fopen("data.txt", "w");
  for (i=0;i<n;++i)
    fprintf(file, "%f %f %f\n", p[i]->x, p[i]->y, p[i]->z);
  fclose(file);
}

int destroy(void * a) {
  Point * p;
  if (a == NULL) {
    return -1;
  }
  p = (Point *) a;
  free(p);
  return 0;
}

int main(int argc, char **argv){
  int i,n, strsz;
  List * ptlist;
  Point ** ptarr;
  // Point * rm, *comparing, *comparee;

  n = atoi(argv[1]);

  strsz = STRLENG*sizeof(char);

  ptlist = list_create(sizeof(Point));
  ptarr = (Point**) malloc(n*sizeof(Point*));
  
  // srand(time(NULL));
  srand(24);

  /* allocates memory and assigns a valid pointer value to each
     of the first n values of the pointer array p */
  for (i=0;i<n;++i){
    Point * p;
    p = create_ran_point();
    list_append(ptlist, p);
    ptarr[i] = p; 
  }

  clock_t t;
  double processor_t;
  float ssDis, sumDis, sd, num;
  ssDis = sumDis = 0;

  t = clock();
  for (i=0;i<n;++i) {
    num = dist(ptarr[i]);
    sumDis += num;
    ssDis += num*num;
  }
  t = clock() - t;
  processor_t = ((double) t)/CLOCKS_PER_SEC;
  sd = sqrt((ssDis/n) - (sumDis/n)*(sumDis/n));
  printf("SD = %f. Array took %f seconds to calculate the standard deviation of %d data.\n", sd, processor_t, n);

  ssDis = sumDis = 0;
  t = clock();
  Point * p;
  for (i=0, p = list_val_at(ptlist,0);i<n;++i, p = list_val_at(ptlist,i)) {
    num = dist(p);
    sumDis += num;
    ssDis += num*num;
  }
  t = clock() - t;
  processor_t = ((double) t)/CLOCKS_PER_SEC;
  sd = sqrt((ssDis/n) - (sumDis/n)*(sumDis/n));
  printf("SD = %f. List took %f seconds to calculate the standard deviation of %d data.\n", sd, processor_t, n);

  ssDis = sumDis = 0;
  t = clock();
  for (i=0;i<n;++i) {
    p = (Point *)list_remove(ptlist, 0);
    num = dist(p);
    sumDis += num;
    ssDis += num*num;
    destroy(p);
  }
  t = clock() - t;
  processor_t = ((double) t)/CLOCKS_PER_SEC;
  sd = sqrt((ssDis/n) - (sumDis/n)*(sumDis/n));
  printf("SD = %f. List took %f seconds to calculate the standard deviation of %d data by using destructive method (continuously popping the first element).\n", sd, processor_t, n);






  // comparing = (Point *) malloc(sizeof(Point));

  // comparee = (Point *) list_val_at(ptlist,0);

  // if (comparee != NULL){
  //   comparing->x = comparee->x;
  //   comparing->y = comparee->y;
  //   comparing->z = comparee->z;
  // }

  

  // list_print(ptlist, tostr, strsz);

  // printf("%d\n", list_find_first(ptlist, comparing, Point_comparer));

  // rm = (Point *) list_remove(ptlist, 0);
  // destroy(rm);

  // list_print(ptlist, tostr, strsz);

  // char* res = malloc(STRLENG*sizeof(char));
  // printf("run to here\n");
  
  // tostr(list_val_at(ptlist, 0), res);

  // printf("%s\n",res);

  // printf("%d\n", list_destroy(&ptlist));

  // printf("%d\n", ptlist == NULL);

  list_clear(ptlist, destroy);
  list_destroy(&ptlist);
  // for(i=0;i<n;++i)
  //   free(ptarr[i]);
  free(ptarr);

  printf("----end----\n");

  return 0;
}