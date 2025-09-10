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

typedef struct {
  double in;
  double pidsr;
  double a[20];
} FixedBLP8;

FixedBLP8 fixedblp8_init(double sr, double freq);
double fixedblp8_tick(FixedBLP8* p);

typedef struct {
  double inCo;
  double inMin1Co;
  double inMin2Co;
  double inMin3Co;
  double inMin4Co;
  double outMin1Co;
  double outMin2Co;
  double outMin3Co;
  double outMin4Co;

  double in;
  double inMin1;
  double inMin2;
  double inMin3;
  double inMin4;
  double outMin1;
  double outMin2;
  double outMin3;
  double outMin4;
} IIRTest;

IIRTest iirtest_init();
double iirtest_tick(IIRTest* p);
