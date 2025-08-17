#include <math.h>
#include <stdio.h>

#define M_PI 3.14159265358979323846

void table(FILE* h, FILE* cpp, const char* name, int size, double(*fn)(int, int)) {
  fprintf(h, "extern const Table %s;\n", name);
  fprintf(cpp, "const double %s_t[] = {\n", name);

  for (int i = 0; i < size; i++) {
    double sample = fn(i, size);
    fprintf(cpp, "%.15f,\n", sample);
  }
  fprintf(cpp, "};\n");
  fprintf(cpp, "extern const Table %s = { %s_t, %d };", name, name, size);
}

double wt_sin(int i, int size) {
  return sin(((double)i / (size-1)) * 2 * M_PI);
}

double et_fall_lin(int i, int size) {
  return 1 - ((double)i / (size-1));
}

double et_fall_exp_2(int i, int size) {
  return pow(1 - ((double)i / (size-1)), 2);
}

double et_fall_exp_3(int i, int size) {
  return pow(1 - ((double)i / (size-1)), 3);
}

double et_fall_exp_4(int i, int size) {
  return pow(1 - ((double)i / (size-1)), 4);
}

int main() {
  FILE* h = fopen("src/tables.h", "w");
  FILE* cpp = fopen("src/tables.cpp", "w");
  
  fprintf(h, "#pragma once\n\n");
  fprintf(h, "struct Table { const double* wt; int size; };\n\n");
  
  fprintf(cpp, "#include \"tables.h\"\n\n");
  
  table(h, cpp, "wt_sin", 257, wt_sin);
  table(h, cpp, "et_fall_lin", 256, et_fall_lin);
  table(h, cpp, "et_fall_exp_2", 256, et_fall_exp_2);
  table(h, cpp, "et_fall_exp_3", 256, et_fall_exp_3);
  table(h, cpp, "et_fall_exp_4", 256, et_fall_exp_4);

  fclose(h);
  fclose(cpp);

  return 0;
}