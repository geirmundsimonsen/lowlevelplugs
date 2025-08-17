#include <math.h>
#include <stdio.h>

#define M_PI 3.14159265358979323846

void table(FILE* h, FILE* cpp, const char* name, int size, double(*fn)(int, int)) {
  fprintf(h, "extern const table_t %s;\n", name);
  fprintf(cpp, "const double %s_t[] = {\n", name);

  for (int i = 0; i < size; i++) {
    double sample = fn(i, size);
    fprintf(cpp, "%.15f,\n", sample);
  }
  fprintf(cpp, "};\n");
  fprintf(cpp, "const table_t %s = { %s_t, %d };\n", name, name, size);
}

double wt_sin_fn(int i, int size) {
  return sin(((double)i / (size-1)) * 2 * M_PI);
}

double et_fall_lin_fn(int i, int size) {
  return 1 - ((double)i / (size-1));
}

double et_fall_exp_2_fn(int i, int size) {
  return pow(1 - ((double)i / (size-1)), 2);
}

double et_fall_exp_3_fn(int i, int size) {
  return pow(1 - ((double)i / (size-1)), 3);
}

double et_fall_exp_4_fn(int i, int size) {
  return pow(1 - ((double)i / (size-1)), 4);
}

int main() {
  FILE* h = fopen("src/tables.h", "w");
  FILE* cpp = fopen("src/tables.c", "w");
  
  fprintf(h, "#pragma once\n\n");
  fprintf(h, "typedef struct table { const double* wt; int size; } table_t;\n\n");
  
  fprintf(cpp, "#include \"tables.h\"\n\n");
  
  table(h, cpp, "wt_sin", 257, wt_sin_fn);
  table(h, cpp, "et_fall_lin", 256, et_fall_lin_fn);
  table(h, cpp, "et_fall_exp_2", 256, et_fall_exp_2_fn);
  table(h, cpp, "et_fall_exp_3", 256, et_fall_exp_3_fn);
  table(h, cpp, "et_fall_exp_4", 256, et_fall_exp_4_fn);

  fclose(h);
  fclose(cpp);

  return 0;
}