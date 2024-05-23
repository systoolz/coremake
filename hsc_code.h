#ifndef __HSC_CODE_H
#define __HSC_CODE_H

#include "stdint.h"

/*
  ah - function number [0..7] (ah)
  bx - additional arguments (bx / bh & bl)
  es_si - song or buffer for data (es:si)
  ds_dx - callback function (ds:dx)
*/
enum {
  HSC_PLAY = 0,
  HSC_POLL = 1,
  HSC_STOP = 2,
  HSC_FADE = 3,
  HSC_FIND = 4,
  HSC_RBAR = 5,
  HSC_FUNC = 6,
  HSC_STAT = 7
};

typedef void far (*LPHSCPLAYER)(unsigned char ah, unsigned short bx, void far *es_si, void far *ds_dx);

/* wrapper for non PIC (Position Independed Code) friendly code */
static uint8_t hsc_wrap[] = {
  /* push bp */
  0x55,
  /* mov bp, sp */
  0x89, 0xE5,
  /* push ax, bx, cx, dx, ds, si, es, di */
  0x50, 0x53, 0x51, 0x52, 0x1E, 0x56, 0x06, 0x57,
  /* mov ds, [bp + 10h] */
  0x8E, 0x5E, 0x10,
  /* mov ds, dx */
  0x8E, 0xDA,
  /* mov dx, [bp + 0Eh] */
  0x8B, 0x56, 0x0E,
  /* mov si, [bp + 0Ch] */
  0x8B, 0x76, 0x0C,
  /* mov es, si */
  0x8E, 0xC6,
  /* mov si, [bp + 0Ah] */
  0x8B, 0x76, 0x0A,
  /* mov bx, [bp + 08h] */
  0x8B, 0x5E, 0x08,
  /* mov ah, [bp + 06h] */
  0x8A, 0x66, 0x06,
  /* call far 0000:0000 */
  0x9A, 0x00, 0x00, 0x00, 0x00, /* 0x22: wofs[2], wseg[2] */
  /* pop di, es, si, ds, dx, cx, bx, ax */
  0x5F, 0x07, 0x5E, 0x1F, 0x5A, 0x59, 0x5B, 0x58,
  /* pop bp */
  0x5D,
  /* retf */
  0xCB
};

#endif
