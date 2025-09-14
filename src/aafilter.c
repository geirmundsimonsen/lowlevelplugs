#include "aafilter.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "log.h"

void instanceClearLowpassLR4x2(LowpassLR4x2* dsp) {
  {
    int l0;
    for (l0 = 0; l0 < 2; l0 = l0 + 1) {
      dsp->fRec9[l0] = 0.0;
    }
  }
  {
    int l1;
    for (l1 = 0; l1 < 2; l1 = l1 + 1) {
      dsp->fRec10[l1] = 0.0;
    }
  }
  {
    int l2;
    for (l2 = 0; l2 < 2; l2 = l2 + 1) {
      dsp->fRec6[l2] = 0.0;
    }
  }
  {
    int l3;
    for (l3 = 0; l3 < 2; l3 = l3 + 1) {
      dsp->fRec7[l3] = 0.0;
    }
  }
  {
    int l4;
    for (l4 = 0; l4 < 2; l4 = l4 + 1) {
      dsp->fRec3[l4] = 0.0;
    }
  }
  {
    int l5;
    for (l5 = 0; l5 < 2; l5 = l5 + 1) {
      dsp->fRec4[l5] = 0.0;
    }
  }
  {
    int l6;
    for (l6 = 0; l6 < 2; l6 = l6 + 1) {
      dsp->fRec0[l6] = 0.0;
    }
  }
  {
    int l7;
    for (l7 = 0; l7 < 2; l7 = l7 + 1) {
      dsp->fRec1[l7] = 0.0;
    }
  }
}

void instanceConstantsLowpassLR4x2(LowpassLR4x2* dsp, int sample_rate) {
  dsp->fSampleRate = sample_rate;
  dsp->fConst0 = tan(62831.853071795864 / (double)(dsp->fSampleRate));
  dsp->fConst1 = dsp->fConst0 * (dsp->fConst0 + 1.4142135623730951) + 1.0;
  dsp->fConst2 = 2.0 / dsp->fConst1;
  dsp->fConst3 = dsp->fConst0 / dsp->fConst1;
}

void instanceInitLowpassLR4x2(LowpassLR4x2* dsp, int sample_rate) {
  instanceConstantsLowpassLR4x2(dsp, sample_rate);
  instanceClearLowpassLR4x2(dsp);
}

void initLowpassLR4x2(LowpassLR4x2* dsp, int sample_rate) {
  instanceInitLowpassLR4x2(dsp, sample_rate);
}

void frameLowpassLR4x2(LowpassLR4x2* dsp, double* inputs, double* outputs) {
  double fTemp0 = (double)(inputs[0]);
  double fTemp1 = dsp->fRec9[1] + dsp->fConst0 * (fTemp0 - dsp->fRec10[1]);
  dsp->fRec9[0] = dsp->fConst2 * fTemp1 - dsp->fRec9[1];
  double fTemp2 = dsp->fRec10[1] + dsp->fConst3 * fTemp1;
  dsp->fRec10[0] = 2.0 * fTemp2 - dsp->fRec10[1];
  double fRec11 = fTemp2;
  double fTemp3 = dsp->fRec6[1] + dsp->fConst0 * (fRec11 - dsp->fRec7[1]);
  dsp->fRec6[0] = dsp->fConst2 * fTemp3 - dsp->fRec6[1];
  double fTemp4 = dsp->fRec7[1] + dsp->fConst3 * fTemp3;
  dsp->fRec7[0] = 2.0 * fTemp4 - dsp->fRec7[1];
  double fRec8 = fTemp4;
  double fTemp5 = dsp->fRec3[1] + dsp->fConst0 * (fRec8 - dsp->fRec4[1]);
  dsp->fRec3[0] = dsp->fConst2 * fTemp5 - dsp->fRec3[1];
  double fTemp6 = dsp->fRec4[1] + dsp->fConst3 * fTemp5;
  dsp->fRec4[0] = 2.0 * fTemp6 - dsp->fRec4[1];
  double fRec5 = fTemp6;
  double fTemp7 = dsp->fRec0[1] + dsp->fConst0 * (fRec5 - dsp->fRec1[1]);
  dsp->fRec0[0] = dsp->fConst2 * fTemp7 - dsp->fRec0[1];
  double fTemp8 = dsp->fRec1[1] + dsp->fConst3 * fTemp7;
  dsp->fRec1[0] = 2.0 * fTemp8 - dsp->fRec1[1];
  double fRec2 = fTemp8;
  outputs[0] = (double)(fRec2);
  dsp->fRec9[1] = dsp->fRec9[0];
  dsp->fRec10[1] = dsp->fRec10[0];
  dsp->fRec6[1] = dsp->fRec6[0];
  dsp->fRec7[1] = dsp->fRec7[0];
  dsp->fRec3[1] = dsp->fRec3[0];
  dsp->fRec4[1] = dsp->fRec4[0];
  dsp->fRec0[1] = dsp->fRec0[0];
  dsp->fRec1[1] = dsp->fRec1[0];
}

static double LowpassBW8P_faustpower2_f(double value) {
  return value * value;
}

void instanceClearLowpassBW8P(LowpassBW8P* dsp) {
  {
    int l0;
    for (l0 = 0; l0 < 3; l0 = l0 + 1) {
      dsp->fRec3[l0] = 0.0;
    }
  }
  {
    int l1;
    for (l1 = 0; l1 < 3; l1 = l1 + 1) {
      dsp->fRec2[l1] = 0.0;
    }
  }
  {
    int l2;
    for (l2 = 0; l2 < 3; l2 = l2 + 1) {
      dsp->fRec1[l2] = 0.0;
    }
  }
  {
    int l3;
    for (l3 = 0; l3 < 3; l3 = l3 + 1) {
      dsp->fRec0[l3] = 0.0;
    }
  }
}

void instanceConstantsLowpassBW8P(LowpassBW8P* dsp, int sample_rate) {
  dsp->fSampleRate = sample_rate;
  dsp->fConst0 = tan(62831.853071795864 / (double)(dsp->fSampleRate));
  dsp->fConst1 = 2.0 * (1.0 - 1.0 / LowpassBW8P_faustpower2_f(dsp->fConst0));
  dsp->fConst2 = 1.0 / dsp->fConst0;
  dsp->fConst3 = (dsp->fConst2 + -0.39018064403225594) / dsp->fConst0 + 1.0;
  dsp->fConst4 = 1.0 / ((dsp->fConst2 + 0.39018064403225594) / dsp->fConst0 + 1.0);
  dsp->fConst5 = (dsp->fConst2 + -1.111140466039204) / dsp->fConst0 + 1.0;
  dsp->fConst6 = 1.0 / ((dsp->fConst2 + 1.111140466039204) / dsp->fConst0 + 1.0);
  dsp->fConst7 = (dsp->fConst2 + -1.66293922460509) / dsp->fConst0 + 1.0;
  dsp->fConst8 = 1.0 / ((dsp->fConst2 + 1.66293922460509) / dsp->fConst0 + 1.0);
  dsp->fConst9 = (dsp->fConst2 + -1.9615705608064609) / dsp->fConst0 + 1.0;
  dsp->fConst10 = 1.0 / ((dsp->fConst2 + 1.9615705608064609) / dsp->fConst0 + 1.0);
}

void instanceInitLowpassBW8P(LowpassBW8P* dsp, int sample_rate) {
  instanceConstantsLowpassBW8P(dsp, sample_rate);
  instanceClearLowpassBW8P(dsp);
}

void initLowpassBW8P(LowpassBW8P* dsp, int sample_rate) {
  instanceInitLowpassBW8P(dsp, sample_rate);
}

void frameLowpassBW8P(LowpassBW8P* dsp, double* __restrict__ inputs, double* __restrict__ outputs) {
  dsp->fRec3[0] = (double)(inputs[0]) - dsp->fConst10 * (dsp->fConst9 * dsp->fRec3[2] + dsp->fConst1 * dsp->fRec3[1]);
  dsp->fRec2[0] = dsp->fConst10 * (dsp->fRec3[2] + dsp->fRec3[0] + 2.0 * dsp->fRec3[1]) - dsp->fConst8 * (dsp->fConst7 * dsp->fRec2[2] + dsp->fConst1 * dsp->fRec2[1]);
  dsp->fRec1[0] = dsp->fConst8 * (dsp->fRec2[2] + dsp->fRec2[0] + 2.0 * dsp->fRec2[1]) - dsp->fConst6 * (dsp->fConst5 * dsp->fRec1[2] + dsp->fConst1 * dsp->fRec1[1]);
  dsp->fRec0[0] = dsp->fConst6 * (dsp->fRec1[2] + dsp->fRec1[0] + 2.0 * dsp->fRec1[1]) - dsp->fConst4 * (dsp->fConst3 * dsp->fRec0[2] + dsp->fConst1 * dsp->fRec0[1]);
  outputs[0] = (double)(dsp->fConst4 * (dsp->fRec0[2] + dsp->fRec0[0] + 2.0 * dsp->fRec0[1]));
  dsp->fRec3[2] = dsp->fRec3[1];
  dsp->fRec3[1] = dsp->fRec3[0];
  dsp->fRec2[2] = dsp->fRec2[1];
  dsp->fRec2[1] = dsp->fRec2[0];
  dsp->fRec1[2] = dsp->fRec1[1];
  dsp->fRec1[1] = dsp->fRec1[0];
  dsp->fRec0[2] = dsp->fRec0[1];
  dsp->fRec0[1] = dsp->fRec0[0];
}
