#include <dos.h>
#include <math.h>
#include <stdio.h>
#include <alloc.h>
#include <string.h>

#include "stdint.h"
#include "res_code.h"
#include "vga_code.h"
#include "fnt_code.h"
#include "key_code.h"
#include "tik_code.h"
#include "hsc_code.h"

/*#define NO_ANSI 1*/

static uint16_t y320[200];
static uint8_t far *texture;
static uint8_t far *composite;
static uint8_t far *strlines;
static uint8_t far *songcode;
static uint8_t far *songdata;
static LPHSCPLAYER HSCPlayer;
static int res;

int loadfont(int res, uint8_t num, fnt_data *font) {
char info[9], head[9];
int i;
  if (font) { memset(font, 0, sizeof(font[0])); }
  if (info && head && font) {
    memcpy(info, "FONT0DAT", sizeof(info));
    memcpy(head, "FONT0TGA", sizeof(head));
    info[4] += num;
    head[4] += num;
    for (i = 0; i < 2; i++) {
      font->ilen = res_read(res, info, i ? font->data : NULL, font->ilen);
      font->hlen = res_read(res, head, i ? &font->data[font->ilen] : NULL, font->hlen);
      if (!i) {
        if (font->ilen && font->hlen) {
          font->data = (uint8_t *) malloc(font->ilen + font->hlen);
        }
      } else {
        if ((!font->ilen) || (!font->hlen)) {
          free(font->data);
          memset(font, 0, sizeof(font[0]));
        }
      }
      if (!font->data) { break; }
    }
    if (font->data) {
      /* skip image header */
      font->info = (fnt_head *) font->data;
      font->line = (uint8_t *) &font->data[sizeof(font->info[0])];
      font->pack = font->info->crow ? NULL :
        (uint16_t *) &font->data[sizeof(font->info[0]) + font->info->clen];
      font->head = (tga_head *) &font->data[font->ilen];
      font->body = (uint8_t *) &font->data[font->ilen + sizeof(font->head[0]) + font->head->IDLength];
    }
  }
  return((font && font->data) ? 1 : 0);
}

void InitRotate(void) {
uint16_t i;
  for (i = 0; i < 200; i++) {
    y320[i] = i * 320;
  }
}

/* This is it. This rotates and scales the image into a 200x200 window. */
void FastRotate(float scale, float ang) {
int32_t xscale, yscale, xc, yc, xlong, ylong;
int16_t x, y, *tempx, *tempy;
uint8_t far *c, texel;
  xscale = ceil((sin(ang) * 65536.0) * scale);
  yscale = ceil((cos(ang) * 65536.0) * scale);
  xc = 0x00A00000L/*(160 * 65536L)*/ - (100 * (yscale + xscale));
  yc = 0x00640000L/*(100 * 65536L)*/ - (100 * (yscale - xscale));

  tempx = (int16_t *) &xlong; tempx++;
  tempy = (int16_t *) &ylong; tempy++;
  c = composite;
  for (y = 0; y < 200; y++) {
    /* up and bottom lines for text fadeout: 44 + 112 + 44 = 200 */
    if ((y >= 44) && (y <= (200 - 44))) {
      xlong = xc;
      ylong = yc; /* init x/ylong to topleft of square */
      for (x = 60; x < (320 - 60); x++) {
        /* normally from 0 to 319 */
        /*tempx = xlong >> 16;*/
        /*tempy = ylong >> 16;*/

        if ((*tempx < 0) || (*tempx >= 320 /*WIDTH*/) || (*tempy < 0) || (*tempy >= 200 /*HEIGHT*/)) {
          texel = 0;
        } else {
          texel = texture[y320[*tempy] + *tempx];
        }
        c[x] = texel;

        xlong += yscale;
        ylong -= xscale;
      }
    }
    c += 320; /*WIDTH*/
    xc += xscale;
    yc += yscale;
  }
}

void TextFadeOut(fnt_data *f) {
uint8_t far *d, far *c;
uint16_t i, j, k;
  if (f) {
    k = 1 + f->info->hchr + 1;
    c = composite;
    d = &composite[320 * (200 - k)];
    for (j = 1; j < (k - 1); j++) {
      d += 320;
      c += 320;
      for (i = 1; i < 319; i++) {
        /* original CORE dissolve text routine */
        d[i] = (((c[i - 1] + c[i + 1] + c[i - 320] + c[i + 320]) >> 2) + c[i]) >> 1;
        /* since restored original font a bit brighter - speed up fade out a bit */
        d[i] = (d[i] * 127) >> 7; /* /128 but faster */
      }
    }
    memcpy(composite, &composite[320 * (200 - k)], 320 * k);
  }
}

void scr_init(char t) {
fnt_data font;
uint8_t *s;
uint16_t i;
  if (loadfont(res, 1, &font)) {
    s = (uint8_t *) (t ? "CORE1998" : "CORE1997"); /* BOTH:7/8 */
    for (i = 0; i < 4; i++) {
      fnt_char(NULL, 16 + (10 * i), 40 + (37 * i), (uint8_t) s[i], &font);
      fnt_char(NULL,
        319 - 16 - font.info->wchr - (10 * i)
          /* special case for center aligned "1" character */
          + (i ? 0 : 5),
        40 + (37 * i), (uint8_t) s[i + 4], &font
      );
    }
    free(font.data);
  }
}

#define __PLAYBACK_C
#include "playback.c"

int main(int argc, char *argv[]) {
uint8_t far *p, k;
uint16_t key, l;
uint32_t t;
fnt_data font;
  if (!vga_test()) {
    puts("VGA Required");
    return(1);
  }
  res = res_init(argc ? argv[0] : NULL);
  if (res == -1) {
    puts("Invalid Resources");
    return(2);
  }
  texture = (uint8_t *) malloc((sizeof(tga_head) + 255) + (320 * 200));
  composite = (uint8_t *) malloc(320 * 200);
  if ((!texture) || (!composite)) {
    if (texture) { free(texture); }
    if (composite) { free(composite); }
    puts("Low Memory");
    return(3);
  }
#ifndef NO_ANSI
  vga_mode(0x03); /* clear screen and go to 80x25 mode to display ANSI artwork */
  vga_hcur(HCUR_HIDE);
  vga_wait();
  if (!crc_core("CORE.NFO")) {
    puts(
      "!!! Maybe this is not a CORE release...\n"
      "!!! No NFO found or the NFO-CRC failed.\n\n"
      "Nevertheless press any key to start the INTRO"
    );
    key_read(KEY_WAIT);
  } else {
    puts("NFO-CRC passed - this is a offical CORE release...");
    l = tik_init(&t, 2);
    while (l) {
      l = tik_wait(&t, l);
    }
  }
  vga_fade(FADE_OUT);
  if (res_read(res, "ANSI1SCR", MK_FP(0xB800, 0), 80 * 25 * 2)) {
    vga_fade(FADE_IN);
    key = 0;
    l = tik_init(&t, 4);
    key_read(KEY_DROP);
    while ((l) && (!key)) {
      l = tik_wait(&t, l);
      key = key_read(KEY_TEST);
    }
    vga_fade(FADE_OUT);
  }
#endif
  vga_mode(0x13);
  vga_fill(NULL);
  vga_fade(FADE_IN | FADE_FAST | FADE_GREY);
  scr_init((argc > 1) ? 1 : 0);
  /* load font */
  loadfont(res, 2, &font);
  /* load texture */
  vga_fill(composite);
  vga_fill(texture);
  /* BOTH:7/8 */
  if (res_read(res, (argc > 1) ? "LOGO8TGA" : "LOGO7TGA", texture, (sizeof(tga_head) + 255) + (320 * 200))) {
    /* skip image header */
    memmove(texture, &texture[sizeof(tga_head) + texture[0]], 320 * 200);
  }
  /* BOTH:7/8 */
  strlines = res_load(res, (argc > 1) ? "TEXT8BIN" : "TEXT7BIN", NULL);
  HSCPlayer = NULL;
  /* wrapper code size and align bytes */
  key = sizeof(hsc_wrap) + 15;
  songcode = res_load(res, "HSCOBJ14", &key);
  songdata = NULL;
  HSCPlayer = NULL;
  if (songcode) {
    /* write down wrapper code */
    memcpy(songcode, hsc_wrap, sizeof(hsc_wrap));
    /* align player code position - it must start at CS:0000 to work properly */
    p = &songcode[sizeof(hsc_wrap)];
    while (FP_OFF(p) & 0x0F) { p++; }
    /* move player code */
    memmove(p, &songcode[sizeof(hsc_wrap) + 15], key - (sizeof(hsc_wrap) + 15));
    /* fix correct address in wrapper code */
    p = MK_FP(FP_SEG(p) + (FP_OFF(p) >> 4), FP_OFF(p) & 0x0F);
    memmove(&songcode[0x22], &p, 4);
    /* load song if player ok */
    songdata = res_load(res, "LIVE2HSC", NULL);
    /* create C-function wrapper for player only if song loaded too */
    if (songdata) {
      HSCPlayer = (LPHSCPLAYER) songcode;
    } else {
      /* free space for other files */
      free(songcode);
      songcode = NULL;
    }
  }
  InitRotate();
  p = strlines;
  k = 1;
  /* [playback] */
  angle = M_PI / 256; /* add /256 */
  angle_v = 0;
  scale = 100;
  scale_v = 10;
  bSeqNum = 1;
  bState = 1;
  /* [/playback] */
  /* start song */
  if (HSCPlayer) {
    /* find AdLib port */
    HSCPlayer(HSC_FIND, 0, NULL, NULL);
    /* play song */
    HSCPlayer(HSC_PLAY, 0, songdata, NULL);
  }
  key_read(KEY_DROP);
  l = tik_wait(&t, 0);
  do {
    /* [playback] */
    anim_calc();
    /* BOTH:7/8 */
    FastRotate(scale * ((argc > 1) ? 1.5 : 1), angle);
    angle += angle_v;
    /* [/playback] */
    if (p) {
      switch (k) {
        /* show */
        case 0:
          /* show current text */
          fnt_text(composite, -1, 1, (char *) p, &font);
          /* next string */
          p += strlen((char *) p) + 1;
          /* rewind back */
          if (!*p) { p = strlines; }
          l = tik_wait(&t, tik_time(1));
          k = 1;
          break;
        /* wait */
        case 1:
          l = tik_wait(&t, l);
          if (!l) {
            l = tik_time(2);
            k = 2;
          }
          break;
        /* fade */
        case 2:
          TextFadeOut(&font);
          l = tik_wait(&t, l);
          if (!l) {
            k = 0;
          }
          break;
      }
    }
    vga_wait();
    vga_flip(&font, composite);
    /* read first presseed key */
    key = (key_read(KEY_DROP) >> 8);
    /* only Escape (0x01), Enter (0x1C) or Space (0x39) allowed for exit */
  } while ((key != 0x01) && (key != 0x1C) && (key != 0x39));
  /* cleanup */
  if (strlines) { free(strlines); }
  if (HSCPlayer) { HSCPlayer(HSC_STOP, 0, NULL, NULL); }
  if (songdata) { free(songdata); }
  if (songcode) { free(songcode); }
  if (font.data) { free(font.data); }
  /* this one to avoid ugly colors flickering on display mode change */
  vga_fill(NULL);
  /* restore palette */
  vga_fade(FADE_IN | FADE_FAST);
  /* restore text mode */
  vga_mode(0x03);
  free(composite);
  free(texture);
  /* move cursor to screen bottom */
  for (key = 0; key < (25 - 3); key++) { puts(""); }
#ifndef NO_ANSI
  res_read(res, "ANSI2SCR", MK_FP(0xB800, 0), 80 * 25 * 2);
  vga_hcur(HCUR_SHOW);
#endif
  res_free(res);
  /* clear keyboard buffer */
  key_read(KEY_DROP);
  return(0);
}
