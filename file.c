#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *read_file(const char *filename) {
  long int size = 0;
  FILE *file = fopen(filename, "r");

  if (!file) {
    fputs("File error.\n", stderr);
    exit(1);
  }

  fseek(file, 0, SEEK_END);
  size = ftell(file);
  rewind(file);

  char *result = (char *)malloc(size + 1);
  if (!result) {
    fputs("Memory error.\n", stderr);
    fclose(file);
    exit(1);
  }

  if (fread(result, 1, size, file) != size) {
    fputs("Read error.\n", stderr);
    fclose(file);
    exit(1);
  }

  result[size] = 0;

  fclose(file);
  return result;
}