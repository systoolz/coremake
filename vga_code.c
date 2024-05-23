#include <dos.h>
#include <string.h>

#include "vga_code.h"

char vga_test(void) {
struct SREGS sr;
union REGS rs;
uint8_t data[64];
  memset(data, 0, sizeof(data));
  rs.h.al = 0;
  rs.h.ah = 0x1B;
  rs.x.bx = 0;
  rs.x.di = FP_OFF(data);
  segread(&sr);
  sr.es = FP_SEG(data);
  int86x(0x10, &rs, &rs, &sr);
  return((rs.h.al == 0x1B) ? 1 : 0);
}

void vga_wait(void) {
  while (inp(0x3DA) & 8);
  while (!(inp(0x3DA) & 8));
}

/* http://www.osdever.net/FreeVGA/vga/extreg.htm#3xAR */
void vga_mode(uint8_t mode) {
struct SREGS sr;
union REGS rs;
  rs.h.al = mode;
  rs.h.ah = 0x00;
  segread(&sr);
  int86x(0x10, &rs, &rs, &sr);
  /* put it in square mode */
  outp(0x3C2, 0xE3);
}

/*
http://www.osdever.net/FreeVGA/vga/colorreg.htm
http://www.osdever.net/FreeVGA/vga/attrreg.htm#10 - 8BIT palette
https://web.archive.org/web/20070101125116/http://thorkildsen.no/faqsys/
*/
static uint8_t vga_cpal[256][3];

void vga_fade(uint8_t flag) {
uint16_t i, j;
uint8_t b;
  /* fadeout - read and save palette */
  if (!(flag & FADE_IN)) {
    for (i = 0; i < 256; i++) {
      /* color index to get */
      outp(0x3C7, i);
      /* get RGB colors */
      vga_cpal[i][0] = inp(0x3C9) & 0x3F;
      vga_cpal[i][1] = inp(0x3C9) & 0x3F;
      vga_cpal[i][2] = inp(0x3C9) & 0x3F;
    }
  }
  /* fade out/in (64 shades for 60 FPS (vsync) executes around 1 second) */
  for (j = 0; j <= 0x3F; j++) {
    /* fast fade - basically instant load or clear palette */
    b = (flag & FADE_FAST) ? 0x3F : j;
    /* linear scale coeff */
    b = (flag & FADE_IN) ? b : (0x3F - b);
    /* wait for sync */
    vga_wait();
    for (i = 0; i < 256; i++) {
      /* color index to set */
      outp(0x3C8, i);
      if (flag & FADE_GREY) {
        /* grescale */
        b = i >> 2;
        outp(0x3C9, b);
        outp(0x3C9, b);
        outp(0x3C9, b);
      } else {
        /* palette */
        outp(0x3C9, (vga_cpal[i][0] * b) / 0x3F);
        outp(0x3C9, (vga_cpal[i][1] * b) / 0x3F);
        outp(0x3C9, (vga_cpal[i][2] * b) / 0x3F);
      }
    }
    /* only once for fast or greyscale */
    if (flag & (FADE_GREY | FADE_FAST)) { break; }
  }
}

void vga_fill(uint8_t far *f) {
  f = f ? f : MK_FP(0xA000, 0);
  memset(f, 0, 320 * 200);
}

void vga_flip(fnt_data *f, uint8_t far *c) {
uint8_t far *d;
uint16_t i;
  d = MK_FP(0xA000, 0);
  if (f && f->data) {
    memcpy(d, c, 320 * (1 + f->info->hchr + 1));
  }
  d += 104 + ((24 + 32) * 320);
  c += 104 + (((200 - 112) / 2) * 320);
  for (i = 0; i < 112; i++) {
    memcpy(d, c, 112);
    d += 320;
    c += 320;
  }
}

static uint16_t hcur;

void vga_hcur(uint8_t cvis) {
struct SREGS sr;
union REGS rs;
  if (cvis == HCUR_HIDE) {
    /* read cursor size */
    rs.h.ah = 0x03;
    rs.h.bh = 0;
    segread(&sr);
    int86x(0x10, &rs, &rs, &sr);
    /* save cursor size */
    hcur = rs.x.cx;
    /* hide cursor */
    rs.x.cx = 0x2020;
  } else {
    /* show cursor */
    rs.x.cx = hcur;
  }
  rs.h.ah = 0x01;
  segread(&sr);
  int86x(0x10, &rs, &rs, &sr);
}
