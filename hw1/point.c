#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "list.h"

const int STRLENG = 18;
const int STRSZ = STRLENG*sizeof(char);
const int TESTN = 5;

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
  int i,j,n, ntrial;
  List * ptlist;
  Point ** ptarr;
  Point * rm, *comparing, *comparee, *p, *ap, *prep;
  char* res;
  clock_t t;
  double *processor_t, ave_t;
  Iterator * iter;
  float ssDis, sumDis, sd, num;

  n = atoi(argv[1]);
  assert(n>0); /* check for valid input */

  /* randomly create TESTN of point and append to the List for the unit test. */
  srand(time(NULL));

  ptlist = list_create(tostr, destroy, Point_comparer, STRSZ);
  ptarr = (Point**) malloc(n*sizeof(Point*));
  for (i=0;i<TESTN;++i){
    Point * p;
    p = create_ran_point();
    list_append(ptlist, p);
  }

  /* Funtion unit test */

  printf("\n\n\n---------Start of Test------------\n");

  printf("\nChecking list_insert function...\n");
  p = (Point *) malloc(sizeof(Point));
  p->x = 1.0; p->y = 1.0; p->z = 1.0;
  i=4;
  printf("Before insertion: \n");
  list_print(ptlist);
  printf("After insert p = (1.0,1.0,1.0) to index %d: \n", i);
  list_insert(ptlist, i, p);
  list_print(ptlist);

  printf("\nChecking list_append and list_prepend function...\n");
  ap = (Point *) malloc(sizeof(Point));
  prep = (Point *) malloc(sizeof(Point));
  ap->x = -2.0; ap->y = -2.0; ap->z = -2.0;
  prep->x = 2.0; prep->y = 2.0; prep->z = 2.0;
  printf("After prepend p = (2.0,2.0,2.0) and append p = (-2.0,-2.0,-2.0) to the list: \n");
  list_append(ptlist, ap);
  list_prepend(ptlist, prep);
  list_print(ptlist);

  printf("\nChecking list_remove function...\n");
  printf("After removed the first element: \n");
  rm = (Point *) list_remove(ptlist, 0);
  destroy(rm);
  list_print(ptlist);
  printf("Try remove element at an out of bound index (expected: index out of bound): \n");
  rm = (Point *) list_remove(ptlist, 20);
  destroy(rm);


  printf("\nChecking toStr and list_val_at function...\n");
  res = malloc(STRLENG*sizeof(char));
  tostr(list_val_at(ptlist, 0), res);
  printf("list_val_at index 0 is: %s\n\n", res);
  free(res);

  printf("\nChecking comparer and list_find_first function...\n");
  i = 3;
  comparing = (Point *) malloc(sizeof(Point));
  comparee = (Point *) list_val_at(ptlist,i);
  if (comparee != NULL){
    comparing->x = comparee->x;
    comparing->y = comparee->y;
    comparing->z = comparee->z;
  }
  assert(list_find_first(ptlist, comparing) == i);
  free(comparing);

  printf("\nChecking list_destroy on non-empty list...\n");
  assert(list_destroy(&ptlist) == -1);

  printf("\nChecking list_clear on the list...\n");
  list_clear(ptlist);
  printf("\nChecking list_clear on the cleared list...\n");
  list_clear(ptlist);

  printf("\n---------End of Test------------\n\n\n");


  /* Performance Comparison */

  printf("\n\n---------Start of Performance Comparison------------\n\n\n");


  /* randomly create n point and store them in the List and the Array for the performance comparison. */
  for (i=0;i<n;++i) {
    Point * p;
    p = create_ran_point();
    list_append(ptlist, p);
    ptarr[i] = p; 
  }
  ntrial = 20;
  ave_t = 0;
  processor_t = malloc(ntrial*sizeof(double));
  for (j=0;j<ntrial;++j) {
    ssDis = sumDis = 0;
    t = clock();
    for (i=0;i<n;++i) {
      num = dist(ptarr[i]);
      sumDis += num;
      ssDis += num*num;
    }
    t = clock() - t;
    processor_t[j] = ((double) t)/CLOCKS_PER_SEC;
  }
  for (i=0;i<ntrial;++i) {
    ave_t += processor_t[i];
  }
  ave_t = ave_t/ntrial;
  sd = sqrt((ssDis/n) - (sumDis/n)*(sumDis/n));
  printf("SD = %f. On average of %d trials, Array took about %f seconds to calculate the standard deviation of %d data. \n", sd, ntrial, ave_t, n);

  ave_t = 0;
  t = clock();
  for (i=0;i<ntrial;++i) {
    iter = list_getIterator(ptlist);
    ssDis = sumDis = 0;
    while (hasNext(iter)) {
      Point *p;
      p = (Point *)next(iter);
      num = dist(p);
      sumDis += num;
      ssDis += num*num;
    }
    t = clock() - t;
    processor_t[i] = ((double) t)/CLOCKS_PER_SEC;
  }
  for (i=0;i<ntrial;++i) {
    ave_t += processor_t[i];
  }
  ave_t = ave_t/ntrial;
  sd = sqrt((ssDis/n) - (sumDis/n)*(sumDis/n));
  printf("SD = %f. On average of %d trials, List took about %f seconds to calculate the standard deviation of %d data. \n", sd, ntrial, ave_t, n);

  list_clear(ptlist);
  list_destroy(&ptlist);
  free(ptarr);
  free(processor_t);

  printf("\n---------End of Performance Comparison------------\n\n\n");

  return 0;
}