#include "log.h"

#include <stdio.h>
#include <time.h>

void write_log(const char* msg) {
  FILE* f = fopen("/p1/code/c/lowlevelplugs/plugin.log", "a");
  if (f) {
    time_t t = time(nullptr);
    fprintf(f, "[%ld] %s\n", t, msg);
    fclose(f);
  }
}

void write_log_double(double d) {
  FILE* f = fopen("/p1/code/c/lowlevelplugs/plugin.log", "a");
  if (f) {
    time_t t = time(nullptr);
    fprintf(f, "[%ld] %f\n", t, d);
    fclose(f);
  }
}

void clear_log() {
  FILE* f = fopen("/p1/code/c/lowlevelplugs/plugin.log", "w");
  if (f) {
    fclose(f);
  }
}