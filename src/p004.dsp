import("stdfaust.lib");
freq = hslider("freq", 0, 0, 0, 0);
lpf_freq = hslider("lpf_freq", 0, 0, 0, 0);
lpf_q = hslider("lpf_q", 0, 0, 0, 0);

// korg35LPF (q: [0.707, 10] and sallenKey makes clicking sounds sometimes
process = os.lf_pulsetrain(freq, 0.1) * 0.5 : ve.korg35LPF(lpf_freq, lpf_q) : fi.dcblockerat(2);
//process = os.oscsin(freq) * 0.06 : ve.korg35LPF(lpf_freq, lpf_q) : fi.dcblockerat(2);
//process = os.lf_pulsetrain(freq, 0.1) : ve.sallenKey2ndOrderLPF(lpf_freq, lpf_q) : fi.dcblockerat(2);

// resonlp, lowpass (butterworth), lowpass3/6e (elliptic) doesn't behave well at fast modulations. LowpassLR4 behaves well at fast modulations.
//process = os.lf_pulsetrain(freq, 0.1) : fi.lowpassLR4(lpf_freq) : fi.dcblockerat(2);

// svf works. A bit boring - also has clicks but no misbehaving on sudden changes to freq. freq: [0-20000], Q: (0-35+]
//process = os.lf_pulsetrain(freq, 0.1) : fi.svf.lp(lpf_freq, lpf_q) : fi.dcblockerat(2);
// SVFTFT (Topology-preserving transform, Zavalishin method). A bit more bold.
//process = os.lf_pulsetrain(freq, 0.1) : fi.SVFTPT.LP2(lpf_freq, lpf_q) : fi.dcblockerat(2);

// not behaving at fast modulations
//process = os.lf_pulsetrain(freq, 0.1) : ve.moog_vcf(lpf_q, lpf_freq) : fi.dcblockerat(2);
//process = os.lf_pulsetrain(freq, 0.1) : ve.moog_vcf_2b(lpf_q, lpf_freq) : fi.dcblockerat(2);

// not behaving at fast modulation, but to a lesser degree
//process = os.lf_pulsetrain(freq, 0.1) : ve.moog_vcf_2bn(lpf_q, lpf_freq) : fi.dcblockerat(2);

// freq [0, 1], q [0.707-25]
//process = os.lf_pulsetrain(freq, 0.1) : ve.moogLadder(lpf_freq, lpf_q) : fi.dcblockerat(2);
//process = os.oscsin(freq) * 0.06: ve.moogLadder(lpf_freq, lpf_q) : fi.lowpass(12, 140);//, no.noise * 0 :> /(1);
//process = os.pulsetrain(freq, 0.5) * 0.06 : fi.lowpass(4, 1000);//, no.noise * 0 :> /(1);
