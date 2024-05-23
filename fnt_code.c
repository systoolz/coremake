#include <dos.h>
#include <string.h>

#include "fnt_code.h"

void fnt_char(uint8_t far *d, int16_t x, int16_t y, uint8_t c, fnt_data *f) {
uint16_t cr, cw, cx;
uint16_t i;
uint8_t *p;
  if (c && f && f->data) {
    cr = 0;
    cw = 0;
    cx = 0;
    for (i = 0; i < f->info->clen; i++) {
      if (f->line[i] == c) {
        if (!f->info->crow) {
          /* proportional */
          cx = f->pack[i];
          cr = (cx >> 14) & 0x0003;
          cw = (cx >>  9) & 0x001F;
          cx = (cx      ) & 0x01FF;
        } else {
          /* monospace */
          cw = f->info->wchr;
          cr = i / f->info->crow;
          cx = (i % f->info->crow) * cw;
        }
        break;
      }
    }
    if (cw) {
      d = d ? d : MK_FP(0xA000, 0);
      d += (y * 320) + x;
      p = &f->body[(cr * f->info->hchr * f->info->wimg) + cx];
      for (i = 0; i < f->info->hchr; i++) {
        memcpy(d, p, cw);
        d += 320; /*WIDTH*/
        p += f->info->wimg;
      }
    }
  }
}

/*#define FNT_PROP 1*/
void fnt_text(uint8_t far *d, int x, int y, char *s, fnt_data *f) {
uint16_t cw, i, j;
  if (s && f && f->data) {
    /* centered */
    if (x < 0) {
      /* string length */
      #ifdef FNT_PROP
      x = 0;
      for (i = 0; s[i]; i++) {
        cw = f->info->wchr;
        /* proportional */
        if (!f->info->crow) {
          for (j = 0; j < f->info->clen; j++) {
            if (f->line[j] == ((uint8_t) s[i])) {
              if (s[i] != '*') {
                cw = (f->pack[j] >> 9) & 0x001F;
              }
              break;
            }
          }
        }
        x += cw;
      }
      #else
      x = strlen(s) * f->info->wchr;
      #endif
      x = (x <= 319) ? x : 319;
      x = (319 - x) / 2;
    }
    /* draw */
    for (i = 0; s[i]; i++) {
      cw = f->info->wchr;
      /* proportional */
      if (!f->info->crow) {
        for (j = 0; j < f->info->clen; j++) {
          if (f->line[j] == ((uint8_t) s[i])) {
            cw = (f->pack[j] >> 9) & 0x001F;
            break;
          }
        }
      }
      if ((x + cw) > 319) { break; }
      fnt_char(d, x +
        #ifdef FNT_PROP
        ((s[i] == '*') ? ((f->info->wchr - cw) / 2) : 0)
        #else
        ((f->info->wchr - cw) / 2)
        #endif
        , y, (uint8_t) s[i], f);
      #ifdef FNT_PROP
      x += (s[i] == '*') ? f->info->wchr : cw;
      #else
      x += f->info->wchr;
      #endif
      if (x > 319) { break; }
    }
  }
}
