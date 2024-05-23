#ifndef __KEY_CODE_H
#define __KEY_CODE_H

#include "stdint.h"

enum {
  KEY_READ = 0,
  KEY_TEST = 1,
  KEY_FLAG = 2,
  KEY_FAST = 3,
  KEY_DROP = (4 | KEY_TEST),
  KEY_WAIT = (8 | KEY_TEST)
};

uint16_t key_read(uint8_t what);

#endif
