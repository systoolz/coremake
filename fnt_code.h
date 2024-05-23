#ifndef __FNT_CODE_H
#define __FNT_CODE_H

#include "stdint.h"
#include "res_code.h"

#pragma pack(push, 1)
typedef struct {
  uint8_t wchr;
  uint8_t hchr;
  uint8_t crow; /* 0 - proportional font; non-zero - monospace font (chars per row) */
  uint8_t clen;
  uint16_t wimg;
  uint16_t himg;
/*  uint8_t line[clen];*/ /* ABCDEF... */
/*  uint16_t data[clen]; *//* only for proportional font */
} fnt_head;

typedef struct {
  fnt_head *info;
  uint8_t  *line;
  uint16_t *pack;
  tga_head *head;
  uint8_t  *body;
  /* --- */
  uint8_t  *data;
  uint16_t ilen;
  uint16_t hlen;
} fnt_data;
#pragma pack(pop)

void fnt_char(uint8_t far *d, int16_t x, int16_t y, uint8_t c, fnt_data *f);
void fnt_text(uint8_t far *d, int x, int y, char *s, fnt_data *f);

#endif
