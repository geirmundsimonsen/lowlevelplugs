#pragma once

#include <stdbool.h>

typedef struct {
  double *out;
  double *in, *cutoff, *q, *saturation;
  bool nonlinear;
  double z1, z2, z3, last_cut, last_q, g, G, K, S35, alpha, lpf2_beta, hpf1_beta;
} K35_LPF;

void k35_lpf_init(K35_LPF* p);
void k35_lpf_perf(K35_LPF* p);