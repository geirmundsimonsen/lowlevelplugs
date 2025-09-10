










typedef struct {
	int iVec0[2];
	int fSampleRate;
	double fConst0;
	double fConst1;
	double fConst2;
	double lpf_freq;
	double lpf_q;
	double freq;
	double fConst3;
	double fRec5[2];
	double fRec1[2];
	double fRec2[2];
	double fRec3[2];
	double fRec4[2];
	double fConst4;
	double fRec0[2];
} mydsp;

mydsp* newmydsp() { 
	mydsp* dsp = (mydsp*)calloc(1, sizeof(mydsp));
	return dsp;
}

void deletemydsp(mydsp* dsp) { 
	free(dsp);
}



int getSampleRatemydsp(mydsp* __restrict__ dsp) {
	return dsp->fSampleRate;
}

int getNumInputsmydsp(mydsp* __restrict__ dsp) {
	return 0;
}
int getNumOutputsmydsp(mydsp* __restrict__ dsp) {
	return 1;
}

void classInitmydsp(int sample_rate) {
}

void instanceResetUserInterfacemydsp(mydsp* dsp) {
	dsp->lpf_freq = (double)(0.0);
	dsp->lpf_q = (double)(0.0);
	dsp->freq = (double)(0.0);
}

void instanceClearmydsp(mydsp* dsp) {
	
	{
		int l0;
		for (l0 = 0; l0 < 2; l0 = l0 + 1) {
			dsp->iVec0[l0] = 0;
		}
	}
	
	{
		int l1;
		for (l1 = 0; l1 < 2; l1 = l1 + 1) {
			dsp->fRec5[l1] = 0.0;
		}
	}
	
	{
		int l2;
		for (l2 = 0; l2 < 2; l2 = l2 + 1) {
			dsp->fRec1[l2] = 0.0;
		}
	}
	
	{
		int l3;
		for (l3 = 0; l3 < 2; l3 = l3 + 1) {
			dsp->fRec2[l3] = 0.0;
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
}

void instanceConstantsmydsp(mydsp* dsp, int sample_rate) {
	dsp->fSampleRate = sample_rate;
	dsp->fConst0 = (double)(dsp->fSampleRate);
	dsp->fConst1 = 6.283185307179586 / dsp->fConst0;
	dsp->fConst2 = 1.0 - dsp->fConst1;
	dsp->fConst3 = 1.0 / dsp->fConst0;
	dsp->fConst4 = 1.0 / (dsp->fConst1 + 1.0);
}
	
void instanceInitmydsp(mydsp* dsp, int sample_rate) {
	instanceConstantsmydsp(dsp, sample_rate);
	instanceResetUserInterfacemydsp(dsp);
	instanceClearmydsp(dsp);
}

void initmydsp(mydsp* dsp, int sample_rate) {
	classInitmydsp(sample_rate);
	instanceInitmydsp(dsp, sample_rate);
}



void framemydsp(mydsp* dsp, double* __restrict__ inputs, double* __restrict__ outputs) {
	double fSlow0 = tan(dsp->fConst1 * pow(1e+01, 3.0 * (double)(dsp->lpf_freq) + 1.0));
	double fSlow1 = fSlow0 + 1.0;
	double fSlow2 = fSlow0 / fSlow1;
	double fSlow3 = 1.0 - fSlow2;
	double fSlow4 = (double)(dsp->lpf_q) + -0.7071067811865475;
	double fSlow5 = 0.21521822675751856 * fSlow4 * fSlow3;
	double fSlow6 = dsp->fConst3 * (double)(dsp->freq);
	double fSlow7 = 1.0 / fSlow1;
	double fSlow8 = 1.0 / (1.0 - 0.21521822675751856 * (fSlow0 * fSlow4 * fSlow3 / fSlow1));
	double fSlow9 = 2.0 * fSlow2;
	double fSlow10 = 0.21521822675751856 * fSlow4;
	dsp->iVec0[0] = 1;
	double fTemp0 = ((1 - dsp->iVec0[1]) ? 0.0 : fSlow6 + dsp->fRec5[1]);
	dsp->fRec5[0] = fTemp0 - floor(fTemp0);
	double fTemp1 = 0.5 * (2.0 * (double)(dsp->fRec5[0] <= 0.1) + -1.0) - dsp->fRec4[1];
	double fTemp2 = fSlow8 * (dsp->fRec4[1] + fSlow7 * (fSlow0 * fTemp1 + fSlow5 * dsp->fRec1[1] - dsp->fRec3[1])) - dsp->fRec1[1];
	dsp->fRec1[0] = dsp->fRec1[1] + fSlow9 * fTemp2;
	double fTemp3 = dsp->fRec1[1] + fSlow2 * fTemp2;
	dsp->fRec2[0] = fTemp3;
	dsp->fRec3[0] = dsp->fRec3[1] + fSlow9 * (fSlow10 * fTemp3 - dsp->fRec3[1]);
	dsp->fRec4[0] = dsp->fRec4[1] + fSlow9 * fTemp1;
	dsp->fRec0[0] = dsp->fConst4 * (dsp->fRec2[0] - dsp->fRec2[1] + dsp->fConst2 * dsp->fRec0[1]);
	outputs[0] = (double)(dsp->fRec0[0]);
	dsp->iVec0[1] = dsp->iVec0[0];
	dsp->fRec5[1] = dsp->fRec5[0];
	dsp->fRec1[1] = dsp->fRec1[0];
	dsp->fRec2[1] = dsp->fRec2[0];
	dsp->fRec3[1] = dsp->fRec3[0];
	dsp->fRec4[1] = dsp->fRec4[0];
	dsp->fRec0[1] = dsp->fRec0[0];
}

