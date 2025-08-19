#include "tables.h"

typedef struct {
  double phase;
  double freq;
  double inv_sr;
  table_t wt;
} Osc;

Osc osc_init(double sr);
double osc_tick(Osc* osc);

typedef struct {
  double phase;
  double freq;
  double inv_sr;
  double duty;
} Pulse;

Pulse pulse_init(double sr);
double pulse_tick(Pulse* p);

typedef struct tabplay {
  double phase;
  double s;
  double inv_sr;
  table_t wt;
} TabPlay;

TabPlay tabplay_init(double sr);
double tabplay_tick(TabPlay* tp);