#ifndef __TIK_CODE_H
#define __TIK_CODE_H

#include "stdint.h"

uint16_t tik_time(uint16_t s);
uint16_t tik_wait(uint32_t *p, uint16_t l);
uint16_t tik_init(uint32_t *p, uint16_t l);

#endif
