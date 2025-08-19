#pragma once

#include <stdbool.h>

typedef struct {
  double out;
  double in, cutoff, q, saturation;
  bool nonlinear;
  double Tdiv2, two_div_T;
  double z1, z2, z3, last_cut, last_q, g, G, K, S35, alpha, lpf2_beta, hpf1_beta;
} K35_LPF;

K35_LPF k35_lpf_init(double sr);
void k35_lpf_tick(K35_LPF* p);

typedef struct {
  double *out;
  double *in, *cutoff, *q, *saturation;
  bool nonlinear;
  double z1, z2, z3, last_cut, last_q, g, G, K, S35, alpha, lpf2_beta, hpf1_beta;
} K35_LPF_old;

void k35_lpf_old_init(K35_LPF_old* p);
void k35_lpf_old_perf(K35_LPF_old* p);