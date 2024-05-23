/* code below restored from disassembled CORE'97 executable */

#ifdef __PLAYBACK_C

#include <math.h>
#include "stdint.h"

float scale, scale_v, angle, angle_v;
uint8_t bSeqNum, bState;

void anim_01(void) {
  if (scale_v > 0) {
    scale = (scale_v * scale_v * scale_v * 0.056) + 1.8;
    scale_v -= 0.03;
  } else {
    scale_v = 10;
    bSeqNum++;
  }
}

void anim_02(void) {
  if (scale < 20) {
    scale += 0.5;
  } else {
    bSeqNum++;
  }
}

void anim_03(void) {
  if (scale > 2.0) {
    scale -= 0.5;
  } else {
    bSeqNum++;
  }
}

void anim_06(float r) {
  if (scale_v > 0) {
    scale_v -= r;
  } else {
    scale_v = 10;
    bSeqNum++;
  }
}

void anim_07(float r, int16_t i) {
  if (scale_v > 0) {
    angle_v -= M_PI / i;
    scale_v -= r;
  } else {
    scale_v = 10;
    bSeqNum++;
  }
}

void anim_08(float r, int16_t i) {
  if (scale_v > 0) {
    angle_v += M_PI / i;
    scale_v -= r;
  } else {
    scale_v = 10;
    bSeqNum++;
  }
}

void anim_1F(void) {
  if (bState == 1) {
    if (scale < 12) {
      scale += 0.5;
    } else {
      bState = 2;
    }
  }
  if (bState == 2) {
    if (scale > 2) {
      scale -= 0.5;
    } else {
      bState = 1;
    }
  }
}

void anim_22(void) {
  if (bState == 1) {
    if (scale < 12) {
      scale += 0.5;
    } else {
      scale_v = 10;
      bSeqNum++;
    }
  }
  if (bState == 2) {
    if (scale > 2) {
      scale -= 0.5;
    } else {
      scale_v = 10;
      bSeqNum++;
    }
  }
}

void anim_23(void) {
  if (scale_v > 0) {
    scale = ((10 - scale_v) * (10 - scale_v) * (10 - scale_v) * 0.06) + 1.8;
    scale_v -= 0.03;
  } else {
    angle_v = 0;
    angle = M_PI / 256; /* add /256 */
    scale = 100;
    scale_v = 10;
    bSeqNum = 1;
  }
}

void anim_calc(void) {
  switch (bSeqNum) {
    case  1:
      anim_01();
      break;
    case  2:
      anim_02();
      break;
    case  3:
      anim_03();
      break;
    case  4:
      anim_02();
      break;
    case  5:
      anim_03();
      break;
    case  6:
      anim_06(0.2);
      break;
    case  7:
      anim_07(0.5, 1024);
      break;
    case  8:
      anim_08(0.5, 1024);
      break;
    case  9:
      anim_08(0.5, 1024);
      break;
    case 10:
      anim_07(0.5, 1024);
      break;
    case 11:
      anim_08(0.5, 1024);
      break;
    case 12:
      anim_07(0.5, 1024);
      break;
    case 13:
      anim_07(0.5, 1024);
      break;
    case 14:
      anim_08(0.5, 1024);
      break;
    case 15:
      anim_07(0.5, 256);
      break;
    case 16:
      anim_06(0.05);
      break;
    case 17:
      anim_08(0.5, 256);
      break;
    case 18:
      anim_08(0.5, 256);
      break;
    case 19:
      anim_06(0.05);
      break;
    case 20:
      anim_07(0.5, 256);
      break;
    case 21:
      anim_07(0.5, 256);
      break;
    case 22:
      anim_06(0.1);
      break;
    case 23:
      anim_02();
      break;
    case 24:
      anim_03();
      break;
    case 25:
      anim_02();
      break;
    case 26:
      anim_03();
      break;
    case 27:
      anim_02();
      break;
    case 28:
      anim_03();
      break;
    case 29:
      anim_02();
      break;
    case 30:
      anim_03();
      bState = 1;
      break;
    case 31:
      anim_07(0.03, 512);
      anim_1F();
      break;
    case 32:
      anim_08(0.015, 512);
      anim_1F();
      break;
    case 33:
      anim_07(0.03, 512);
      anim_1F();
      break;
    case 34:
      anim_22();
      break;
    case 35:
      anim_23();
      break;
  }
}

#endif
