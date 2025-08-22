#pragma once

#include <stdbool.h>

typedef struct {
  double in, freq, q, saturation;
  bool nonlinear;
  double Tdiv2, two_div_T;
  double z1, z2, z3, last_cut, last_q, g, G, K, S35, alpha, lpf2_beta, hpf1_beta;
} K35_LPF;

K35_LPF k35_lpf_init(double sr);
double k35_lpf_tick(K35_LPF* p);

typedef struct  {
  double in, freq;
  double last_freq;
  double pidsr;
  double a[8];
} Butterlp;

Butterlp butterlp_init(double sr);
double butterlp_tick(Butterlp* p);

typedef struct  {
  double in;
  double pidsr;
  double a[20];
} FixedBLP8;

FixedBLP8 fixedblp8_init(double sr, double freq);
double fixedblp8_tick(FixedBLP8* p);
