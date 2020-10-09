#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "list.h"

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
  Point **pa = (Point **) a;
  Point **pb = (Point **) b;

  float dista = dist(*pa);
  float distb = dist(*pb);

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

int tostr(const void * ptr, char * res) {
  /*char out[18];
  *const Point * p;
  *size_t size;
  *p = (const Point*) ptr;
  *size = snprintf(NULL, 0, "%5.2f %5.2f %5.2f", p->x,p->y,p->z);
  *out = (char *) malloc(size + 1);
  *snprintf(out, size, "%5.2f %5.2f %5.2f", p->x,p->y,p->z);
  *return out;*/

  const Point * p;
  p = (const Point*) ptr;
  sprintf(res, "%5.2f %5.2f %5.2f", p->x,p->y,p->z);
  return 0;
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
  p = (Point *) a;
  free(p);
  return 0;
}

int main(int argc, char **argv){
  int i,n, strsz;
  List * ptlist;
  Point * rm;

  n = atoi(argv[1]);
  /* an array of 2*n pointers to Point */

  strsz = 18*sizeof(char);

  ptlist = list_create(sizeof(Point));
  
  srand(time(NULL));

  /* allocates memory and assigns a valid pointer value to each
     of the first n values of the pointer array p */
  for (i=0;i<n;++i){
    Point * p;
    p = create_ran_point();
    list_append(ptlist, p);
  }

  list_print(ptlist, tostr, strsz);

  printf("%d\n", ptlist->size);

  rm = (Point *) list_remove(ptlist, 2);
  destroy(rm);

  list_print(ptlist, tostr, strsz);

  int success = list_destroy(ptlist);

  printf("%d\n", success);

  list_clear(ptlist, destroy);

  list_print(ptlist, tostr, strsz);

  printf("run to here\n");

  list_destroy(ptlist);

  return 0;
}