typedef struct {
  int fSampleRate;
  double fConst0;
  double fConst1;
  double fConst2;
  double fRec9[2];
  double fConst3;
  double fRec10[2];
  double fRec6[2];
  double fRec7[2];
  double fRec3[2];
  double fRec4[2];
  double fRec0[2];
  double fRec1[2];
} LowpassLR4x2;

void initLowpassLR4x2(LowpassLR4x2* dsp, int sample_rate);
void frameLowpassLR4x2(LowpassLR4x2* dsp, double* inputs, double* outputs);

typedef struct {
  int fSampleRate;
  double fConst0;
  double fConst1;
  double fConst2;
  double fConst3;
  double fConst4;
  double fConst5;
  double fConst6;
  double fConst7;
  double fConst8;
  double fConst9;
  double fConst10;
  double fRec3[3];
  double fRec2[3];
  double fRec1[3];
  double fRec0[3];
} LowpassBW8P;

void initLowpassBW8P(LowpassBW8P* dsp, int sample_rate);
void frameLowpassBW8P(LowpassBW8P* dsp, double* __restrict__ inputs, double* __restrict__ outputs);