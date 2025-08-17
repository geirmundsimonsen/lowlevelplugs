#include "filter.h"

#include <math.h>
#include "log.h"

#define TWOPI 3.14159265358979323846 * 2
#define MYFLT2LONG(x) ((int)(x))

void k35_lpf_init(K35_LPF* p) {
  if (*p->skip == 0.0) {
    p->z1 = 0.0;
    p->z2 = 0.0;
    p->z3 = 0.0;
    p->last_cut = -1.0;
    p->last_q = -1.0;
    p->g = 0.0;
    p->G = 0.0;
    p->S35 = 0.0;
    p->alpha = 0.0;
    p->lpf2_beta = 0.0;
    p->hpf1_beta = 0.0;
  }
}

void k35_lpf_perf(K35_LPF* p) {
  double z1 = p->z1;
  double z2 = p->z2;
  double z3 = p->z3;
  double last_cut = p->last_cut;
  double last_q = p->last_q;
  double g = p->g;
  double G = p->G;
  double K = p->K;
  double S35 = p->S35;
  double alpha = p->alpha;
  double lpf2_beta = p->lpf2_beta;
  double hpf1_beta = p->hpf1_beta;
  
  //uint32_t offset = p->h.insdshead->ksmps_offset;
  //uint32_t early = p->h.insdshead->ksmps_no_end;
  //uint32_t n, nsmps = CS_KSMPS;
  int n, nsmps = 1; // should be adjusted for performance later.

  //double T = CS_ONEDSR;
  double T = 1.0 / 48000; // sample rate should not be permanent
  double Tdiv2 = T / 2.0;
  double two_div_T = 2.0 / T;

  //int32_t cutoff_arate = IS_ASIG_ARG(p->cutoff);
  int cutoff_arate = 1; // always a-rate
  //int32_t q_arate = IS_ASIG_ARG(p->q);
  int q_arate = 1; // always a-rate

  double cutoff = cutoff_arate ? 0.0 : *p->cutoff;
  double q = q_arate ? 0.0 : *p->q;

  int nonlinear = MYFLT2LONG(*p->nonlinear);
  double saturation = *p->saturation;

  /*if (UNLIKELY(offset)) {
    memset(p->out, '\0', offset*sizeof(MYFLT));
  }
  if (UNLIKELY(early)) {
    nsmps -= early;
    memset(&p->out[nsmps], '\0', early*sizeof(MYFLT));
  }*/
  //for (n = offset; n < nsmps; n++) {
  for (n = 0; n < nsmps; n++) {
    double in = p->in[n];

    if (cutoff_arate) {
      cutoff = p->cutoff[n];
    }
    if (q_arate) {
      q = p->q[n];
      // clamp from 1.0 to 10.0
      q = (q > 10.0) ? 10.0 : (q < 1.0) ? 1.0 : q;
    }

    if (cutoff != last_cut) {
      double wd = TWOPI * cutoff;
      double wa = two_div_T * tan(wd * Tdiv2);
      g = wa * Tdiv2;
      G = g / (1.0 + g);
    }

    if (q != last_q) {
      K = 0.01 + ((2.0 - 0.01) * (q / 10.0));
    }

    if ((cutoff != last_cut) || (q != last_q)) {
      lpf2_beta = (K - (K * G)) / (1.0 + g);
      hpf1_beta = -1.0 / (1.0 + g);
      alpha = 1.0 / (1.0 - (K * G) + (K * G * G));
    }

    last_cut = cutoff;
    last_q = q;

    // LPF1
    double v1 = (in - z1) * G;
    double lp1 = v1 + z1;
    z1 = lp1 + v1;

    double u = alpha * (lp1 + S35);

    if (nonlinear) {
      u = tanh(u * saturation);
    }

    // LPF2
    double v2 = (u - z2) * G;
    double lp2 = v2 + z2;
    z2 = lp2 + v2;
    double y = K * lp2;

    // HPF1

    double v3 = (y - z3) * G;
    double lp3 = v3 + z3;
    z3 = lp3 + v3;
    // double hp1 = y - lp3; /* FIXME: not used */

    S35 = (lpf2_beta * z2) + (hpf1_beta * z3);
    double out = (K > 0) ? (y / K) : y;

    p->out[n] = out;
  }

  p->z1 = z1;
  p->z2 = z2;
  p->z3 = z3;
  p->last_cut = last_cut;
  p->last_q = last_q;
  p->g = g;
  p->G = G;
  p->K = K;
  p->S35 = S35;
  p->alpha = alpha;
  p->lpf2_beta = lpf2_beta;
  p->hpf1_beta = hpf1_beta;
}
