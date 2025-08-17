#include "osc.h"

#include <math.h>

double osc_tick(osc_t* osc) {
  osc->phase += osc->freq / 48000;
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

double tabplay_tick(tabplay_t* tp) {
  tp->phase += (1.0 / tp->s) / 48000;
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
