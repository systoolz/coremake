#include <dos.h>

#include "key_code.h"

uint16_t key_read(uint8_t what) {
struct SREGS sr;
union REGS rs;
  rs.x.bx = 0; /* only for KEY_FAST */
  rs.h.ah = what & 0x03;
  segread(&sr);
  int86x(0x16, &rs, &rs, &sr);
  switch (what & 0x03) {
    case KEY_READ:
      break;
    case KEY_TEST:
      /* check zero flag */
      rs.x.ax = (rs.x.flags & 0x40) ? 0 : rs.x.ax;
      /* check other what bits */
      what ^= KEY_TEST;
      if (what & (KEY_DROP | KEY_WAIT)) {
        if (rs.x.ax) {
          /* read first pressed key if any */
          rs.x.ax = key_read(KEY_READ);
        }
        /* drop everything else */
        while (key_read(KEY_TEST)) {
          key_read(KEY_READ);
        }
        if (what & KEY_WAIT) {
          rs.x.ax = key_read(KEY_READ);
        }
      }
      break;
    case KEY_FLAG:
      rs.x.ax &= 0xFF;
      break;
    case KEY_FAST:
      rs.x.ax = 0;
      break;
  }
  return(rs.x.ax);
}
