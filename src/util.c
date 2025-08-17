#include "util.h"

#include <math.h>

double midipitch2freq(int midipitch) {
  return 440.0 * pow(2.0, (midipitch - 69) / 12.0);
}