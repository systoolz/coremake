#ifndef __RES_CODE_H
#define __RES_CODE_H

#include "stdint.h"

#pragma pack(push, 1)
typedef struct {
  uint8_t  IDLength;
  uint8_t  ColorMapType;
  uint8_t  ImageType;
  uint16_t CMapStart;
  uint16_t CMapLength;
  uint8_t  CMapDepth;
  uint16_t XOffset;
  uint16_t YOffset;
  uint16_t Width;
  uint16_t Height;
  uint8_t  PixelDepth;
  uint8_t  ImageDescriptor;
} tga_head;
#pragma pack(pop)

int res_init(char *s);
void res_free(int res);
uint16_t res_read(int res, char *s, uint8_t *u, uint16_t pu);
uint8_t far *res_load(int res, char *s, uint16_t *len);
int crc_core(char *s);

#endif
