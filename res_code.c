#include <io.h>
#include <fcntl.h>
#include <alloc.h>
#include <string.h>

#include "res_code.h"

#pragma pack(push, 1)
typedef struct {
  uint8_t  name[8];
  uint8_t  zero; /* filename ASCIIZ zero byte */
  uint8_t  flag; /* 1 - packed */
  uint16_t size; /* unpacked size */
  uint16_t pack; /* packed size */
} res_head;
#pragma pack(pop)

#define BUFFSIZE 0xFFF

void lzss_unpack(uint8_t *p, uint8_t *u, uint16_t ps, uint16_t us) {
uint8_t *buff/*[BUFFSIZE + 1]*/, b, m;
uint16_t i, j, k;
  /* sanity checks */
  if (p && u && ps && us) {
    /* dynamic allocation - not enough space on stack in real mode DOS */
    buff = (uint8_t *) malloc(BUFFSIZE + 1);
    if (buff) {
      memset(buff, 32, BUFFSIZE + 1);
      i = (BUFFSIZE + 1) - 16;
      while ((ps) && (us)) {
        b = *p; p++; ps--;
        for (m = 0; m < 8; m++) {
          if (!(b & 1)) {
            if (ps < 2) { break; }
            j = *p; p++; ps--;
            k = *p; p++; ps--;
            j += ((k & 0xF0) << 4);
            k = (k & 0x0F) + 3;
            while ((k > 0) && (us)) {
              buff[i] = buff[j];
              *u = buff[i]; u++; us--;
              i = (i + 1) & BUFFSIZE;
              j = (j + 1) & BUFFSIZE;
              k--;
            }
          } else {
            if (!ps) { break; }
            if (!us) { break; }
            buff[i] = *p; p++; ps--;
            *u = buff[i]; u++; us--;
            i = (i + 1) & BUFFSIZE;
          }
          b >>= 1;
        }
      }
      free(buff);
    }
  }
}

int res_init(char *s) {
uint16_t hexe[14];
uint32_t flen;
char b;
int fl;
  b = 0;
  fl = s ? open(s, O_RDONLY | O_BINARY) : (-1);
  if (fl != -1) {
    memset(&hexe, 0, sizeof(hexe));
    flen = read(fl, &hexe, sizeof(hexe));
    if ((flen == sizeof(hexe)) && (hexe[0] == 0x5A4D) && (hexe[2])) {
      flen = hexe[1] ? (512 - hexe[1]) : 0;
      flen = (512UL * hexe[2]) - flen;
      lseek(fl, 0, SEEK_END);
      if (flen < tell(fl)) {
        lseek(fl, flen, SEEK_SET);
        flen = 0;
        read(fl, &flen, sizeof(flen));
        /* CORE9798 */
        if (flen == 0x98979EC0UL) {
          b = 1;
        }
      }
    }
  }
  if ((!b) && (fl != -1)) {
    close(fl);
    fl = -1;
  }
  return(fl);
}

void res_free(int res) {
  if (res != -1) {
    close(res);
  }
}

uint16_t res_read(int res, char *s, uint8_t *u, uint16_t pu) {
res_head rh;
uint16_t r;
uint32_t o;
uint8_t *p;
char b;
  r = 0;
  if ((res != -1) && (s)) {
    o = tell(res);
    do {
      memset(&rh, 0, sizeof(rh));
      b = (read(res, &rh, sizeof(rh)) == sizeof(rh)) ? 1 : 0;
      if (b) {
        if (!strcmp((char *) rh.name, s)) {
          if (u && pu) {
            memset(u, 0, pu);
            if (rh.flag & 1) {
              /* packed */
              p = (uint8_t *) malloc(rh.pack);
              if (p) {
                memset(p, 0, rh.pack);
                rh.pack = read(res, p, rh.pack);
                r = (pu < rh.size) ? pu : rh.size;
                lzss_unpack(p, u, rh.pack, r);
                free(p);
              }
            } else {
              /* unpacked */
              r = (pu < rh.pack) ? pu : rh.pack;
              r = read(res, u, r);
            }
          } else {
            /* get resource size only */
            r = rh.size;
          }
          /* done */
          b = 0;
        } else {
          /* skip this file */
          lseek(res, tell(res) + rh.pack, SEEK_SET);
        }
      }
    } while (b);
    lseek(res, o, SEEK_SET);
  }
  return(r);
}

uint8_t far *res_load(int res, char *s, uint16_t *len) {
uint8_t far *p;
uint16_t l;
  p = NULL;
  l = res_read(res, s, NULL, 0);
  if (l) {
    p = (uint8_t *) malloc((len ? *len : 0) + l);
    if (p) {
      l = res_read(res, s, &p[len ? *len : 0], l);
      if (!l) {
        free(p);
        p = NULL;
      } else {
        if (len) {
          memset(p, 0, *len);
          *len += l;
        }
      }
    }
  }
  return(p);
}

int crc_core(char *s) {
uint16_t i, k;
uint32_t l, c;
uint8_t p[256];
int fl;
  k = 1;
  fl = open(s, O_RDONLY | O_BINARY);
  if (fl != -1) {
    lseek(fl, 0, SEEK_END);
    l = tell(fl);
    /* original CORE code limited to max DOS memory block size (65536 - 8) */
    if ((l >= 19) && (l <= 65528U)) {
      lseek(fl, 0, SEEK_SET);
      c = 0;
      for (i = 0; i <= (l - 19); i++) {
        /* avoid unnecessary memory allocation - use tiny static buffer */
        if (!(i % sizeof(p))) { read(fl, p, sizeof(p)); }
        k = p[i % sizeof(p)];
        /* CORE hash method */
        k *= 13;
        k += i + 1;
        k ^= 0xBB;
        k %= 1001;
        c += k;
        c *= 17;
      }
      lseek(fl, l - 11, SEEK_SET);
      read(fl, p, 9);
      for (i = 0; i < 9; i++) {
        /* skip ':' - original CORE code doesn't check for separator */
        if (i != 4) {
          k = (c >> 28) & 0x0F;
          k += (k < 10) ? '0' : ('A' - 10);
          c <<= 4;
          k ^= p[/*l - 11 + */i];
          if (k) { break; }
        }
      }
    }
    close(fl);
  }
  return(k ? 0 : 1);
}
