#include "filter.h"

#include <math.h>
#include "log.h"

#define PI 3.14159265358979323846
#define TWOPI 3.14159265358979323846 * 2
#define ROOT2 1.414213562373095048801688724209698078569
#define MYFLT2LONG(x) ((int)(x))

K35_LPF k35_lpf_init(double sr) {
  K35_LPF k35 = {0};

  k35.freq = 400;
  k35.saturation = 1.0;
  k35.q = 1.0;
  k35.nonlinear = false;

  k35.last_cut = -1.0;
  k35.last_q = -1.0;

  double inv_sr = 1.0 / sr;
  k35.Tdiv2 = inv_sr / 2.0;
  k35.two_div_T = 2.0 / inv_sr;

  return k35;
}

double k35_lpf_tick(K35_LPF* p) {
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

  double cutoff = p->freq;
  double q = p->q;
  q = (q > 10.0) ? 10.0 : (q < 1.0) ? 1.0 : q;

  double saturation = p->saturation;
  double in = p->in;

  if (cutoff != last_cut) {
    double wd = TWOPI * cutoff;
    double wa = p->two_div_T * tan(wd * p->Tdiv2);
    g = wa * p->Tdiv2;
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

  double v1 = (in - z1) * G;
  double lp1 = v1 + z1;
  z1 = lp1 + v1;

  double u = alpha * (lp1 + S35);

  if (p->nonlinear) {
    u = tanh(u * saturation);
  }

  double v2 = (u - z2) * G;
  double lp2 = v2 + z2;
  z2 = lp2 + v2;
  double y = K * lp2;


  double v3 = (y - z3) * G;
  double lp3 = v3 + z3;
  z3 = lp3 + v3;

  S35 = (lpf2_beta * z2) + (hpf1_beta * z3);
  double out = (K > 0) ? (y / K) : y;

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

  return out;
}

Butterlp butterlp_init(double sr) {
  Butterlp blp = {0};
  blp.freq = 400;
  blp.pidsr = PI / sr;
  return blp;
}

double butterlp_tick(Butterlp *p) {
  if (p->freq != p->last_freq) {
    double *a, c;
    a = p->a;
    p->last_freq = p->freq;
    c = 1.0 / tan((double)(p->pidsr * p->last_freq));
    a[1] = 1.0 / ( 1.0 + ROOT2 * c + c * c);
    a[2] = a[1] + a[1];
    a[3] = a[1];
    a[4] = 2.0 * ( 1.0 - c*c) * a[1];
    a[5] = ( 1.0 - ROOT2 * c + c * c) * a[1];
  }

  double* a = p->a;
  double t, y;
  t = p->in - a[4] * a[6] - a[5] * a[7];
  y = t * a[1] + a[2] * a[6] + a[3] * a[7];
  a[7] = a[6];
  a[6] = t;
  
  return y;
}

FixedBLP8 fixedblp8_init(double sr, double freq) {
  FixedBLP8 blp = {0};
  blp.pidsr = PI / sr;

  double c;

  c = 1.0 / tan((double)(blp.pidsr * freq));
  blp.a[1] = 1.0 / ( 1.0 + ROOT2 * c + c * c);
  blp.a[2] = blp.a[1] + blp.a[1];
  blp.a[3] = blp.a[1];
  blp.a[4] = 2.0 * ( 1.0 - c*c) * blp.a[1];
  blp.a[5] = ( 1.0 - ROOT2 * c + c * c) * blp.a[1];
  return blp;
}

double fixedblp8_tick(FixedBLP8 *p) {
  double* a = p->a;
  double t, y;

  /*
  t = p->in - a[4] * a[6] - a[5] * a[7];
  y = t * a[1] + a[2] * a[6] + a[3] * a[7];
  a[7] = a[6];
  a[6] = t;
  */
  y = p->in;

  // 12 db
  t = y - a[4] * a[6] - a[5] * a[7];
  y = t * a[1] + a[2] * a[6] + a[3] * a[7];
  a[7] = a[6];
  a[6] = t;

  // 24 db
  t = y - a[4] * a[8] - a[5] * a[9];
  y = t * a[1] + a[2] * a[8] + a[3] * a[9];
  a[9] = a[8];
  a[8] = t;

  // 36 db
  t = y - a[4] * a[10] - a[5] * a[11];
  y = t * a[1] + a[2] * a[10] + a[3] * a[11];
  a[11] = a[10];
  a[10] = t;

  // 48 db
  t = y - a[4] * a[12] - a[5] * a[13];
  y = t * a[1] + a[2] * a[12] + a[3] * a[13];
  a[13] = a[12];
  a[12] = t;

  // 60 db
  t = y - a[4] * a[14] - a[5] * a[15];
  y = t * a[1] + a[2] * a[14] + a[3] * a[15];
  a[15] = a[14];
  a[14] = t;

  // 72 db
  t = y - a[4] * a[16] - a[5] * a[17];
  y = t * a[1] + a[2] * a[16] + a[3] * a[17];
  a[17] = a[16];
  a[16] = t;

  // 84 db
  t = y - a[4] * a[18] - a[5] * a[19];
  y = t * a[1] + a[2] * a[18] + a[3] * a[19];
  a[19] = a[18];
  a[18] = t;
  
  return y;
}
