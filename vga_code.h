#ifndef __VGA_CODE_H
#define __VGA_CODE_H

#include "stdint.h"
#include "fnt_code.h"

#define FADE_OUT  0
#define FADE_IN   1
#define FADE_FAST 2
#define FADE_GREY 4

#define HCUR_HIDE 0
#define HCUR_SHOW 1

char vga_test(void);
void vga_wait(void);
void vga_mode(uint8_t mode);
void vga_fade(uint8_t flag);
void vga_fill(uint8_t far *f);
void vga_flip(fnt_data *f, uint8_t far *c);
void vga_hcur(uint8_t cvis);
void vga_tclr(void);

#endif
