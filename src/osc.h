#include "tables.h"

typedef struct osc {
  double phase;
  double freq;
  table_t wt;
} osc_t;

double osc_tick(osc_t* osc);

typedef struct tabplay {
  double phase;
  double s;
  table_t wt;

} tabplay_t;

double tabplay_tick(tabplay_t* tabplay);