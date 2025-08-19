#include "osc.h"

#include <math.h>

Osc osc_init(double sr) {
  Osc osc = {0};
  osc.inv_sr = 1.0 / sr;
  return osc;
}

double osc_tick(Osc* osc) {
  osc->phase += osc->freq * osc->inv_sr;
  if (osc->phase >= 1) {
    osc->phase -= 1;
  } 
  double cont_index = osc->phase * (osc->wt.size-1);
  
  double integral = 0;
  double frac = modf(cont_index, &integral);
  double indexed_val_1 = osc->wt.wt[(int)integral];
  double indexed_val_2 = osc->wt.wt[(int)(integral+1)];
  double interpolated_value = indexed_val_1 + (indexed_val_2 - indexed_val_1) * frac;
  
  return interpolated_value;
}

TabPlay tabplay_init(double sr) {
  TabPlay tp = {0};
  tp.inv_sr = 1.0 / sr;
  return tp;
}

double tabplay_tick(TabPlay* tp) {
  tp->phase += (1.0 / tp->s) * tp->inv_sr;
  if (tp->phase >= 1) {
    tp->phase = 1;
  } 
  double cont_index = tp->phase * (tp->wt.size-1);

  double integral = 0;
  double frac = modf(cont_index, &integral);
  double indexed_val_1 = tp->wt.wt[(int)integral];
  double indexed_val_2 = tp->wt.wt[(int)(integral+1)];
  double interpolated_value = indexed_val_1 + (indexed_val_2 - indexed_val_1) * frac;

  return interpolated_value;
}
