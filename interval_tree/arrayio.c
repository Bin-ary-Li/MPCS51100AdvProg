#include "arrayio.h"

// int main(int argc, char **argv){
//   char          *filename = "arrays.txt";
//   array2D* array_list = read(filename);
//   printf("%f\n", array_list->array[2][10000]);

//   destroy_arr(array_list);
//   return 0;
// }


array2D* read(char *filename) { 
  char*  string = NULL; 
  size_t strsize = 0;   
  ssize_t nread; 

  FILE *file = fopen(filename, "r");
  double **array_list = malloc(sizeof(double *)*NUM_ARRAY);
  int * array_size = malloc(sizeof(int)*NUM_ARRAY);

  int arrcnt = 0;
  while ((nread = getline(&string, &strsize, file)) != -1) {
      int valcnt = 0;
      int arrsize = 0;
      char* token = NULL;
      token = strtok(string, " "); // get the first token, which represents size
      arrsize = atoi(token);
      array_list[arrcnt] = malloc(sizeof(double)*arrsize);

      token = strtok(NULL, " "); // get next token

      while (token != NULL && token != "\n") {
        double val = 0.0;
        int ret = sscanf(token, "%lf", &val);
        if (ret == EOF) {
          // printf("Error, encounter token '%s' not valid double\n", token);
          token = strtok(NULL, " ");
          continue;
        }
        array_list[arrcnt][valcnt] = val;
        token = strtok(NULL, " ");
        ++valcnt;
      }
      array_size[arrcnt] = valcnt;
      ++arrcnt;
  }
  free(string);
  fclose(file);

  array2D * arr = malloc(sizeof(array2D));
  arr->array = array_list;
  arr->innersize = array_size;
  arr->outersize = arrcnt;

  return arr;
}

void destroy_arr(array2D** arr) {
  for (int i=0; i<(*arr)->outersize; i++) {
    free((*arr)->array[i]);
  }
  free((*arr)->array);
  free((*arr)->innersize);
  free(*arr);
}

