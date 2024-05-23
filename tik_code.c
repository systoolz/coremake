#include <dos.h>

#include "tik_code.h"

uint16_t tik_time(uint16_t s) {
  return((s * 182UL) / 10UL);
}

uint16_t tik_wait(uint32_t *p, uint16_t l) {
uint32_t far *t, c;
  if (p) {
    /* timer ticks (~18.2 per second) */
    t = MK_FP(0x40, 0x6C);
    c = *t;
    if (*p != c) {
      *p = c;
      l -= l ? 1 : 0;
    }
  }
  return(l);
}

uint16_t tik_init(uint32_t *p, uint16_t l) {
  /* seconds to ticks */
  l = (l * 182UL) / 10UL;
  if (p) {
    /* sync timer to start of the tick */
    while (tik_wait(p, 1));
  }
  return(l);
}
