#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <alloc.h>
#include <string.h>

#include "stdint.h"

/*
char[8] - archive magic "SZDD\x88\xF0\x27\x33"
char - compression type, always 'A' (0x41)
char - last char of uncompressed file name
uint32_t - uncompressed file size
*/

#pragma pack(push, 1)
typedef struct {
  uint8_t name[8];
  uint8_t zero; /* filename ASCIIZ zero byte */
  uint8_t flag; /* 1 - packed */
  uint16_t size; /* unpacked size */
  uint16_t pack; /* packed size */
} res_head;
#pragma pack(pop)

char *basename(char *s) {
char *r;
  if (s) {
    for (r = s; *r; r++) {
      if ((*r == '/') || (*r == '\\')) {
        s = &r[1];
      }
    }
  }
  return(s);
}

void str2up(uint8_t *s) {
  while (*s) {
    *s -= ((*s >= 'a') && (*s <= 'z')) ? ('a' - 'A') : 0;
    s++;
  }
}

int main(int argc, char *argv[]) {
res_head *rh;
uint16_t i;
uint8_t *p;
int fl;
char *s;
  if (argc != 2) { return(1); }
  s = basename(argv[1]);
  if (strlen(s) > 8) {
    printf("Error: name too long (size > 8).\n\n");
    return(2);
  }
  fl = open(argv[1], O_RDONLY | O_BINARY);
  if (fl == -1) {
    printf("Error: can not open input file.\n\n");
    return(3);
  }
  lseek(fl, 0, SEEK_END);
  if (tell(fl) >= (65528UL - 14)) {
    close(fl);
    printf("Error: file too big to fit in memory.\n\n");
    return(4);
  }
  i = tell(fl);
  p = (uint8_t *) malloc(sizeof(rh[0]) + i);
  if (p) {
    memset(p, 0, sizeof(rh[0]) + i);
    lseek(fl, 0, SEEK_SET);
    read(fl, &p[sizeof(rh[0])], i);
  }
  close(fl);
  if (!p) {
    printf("Error: not enough memory.\n\n");
    return(4);
  }
  if (!memcmp(&p[sizeof(rh[0])], "SZDD\x88\xF0\x27\x33", 8)) {
    rh = (res_head *) &p[sizeof(rh[0])];
    memset(rh, 0, 9);
    strcpy((char *) rh->name, s);
    str2up(rh->name);
    rh->flag = 1;
    rh->pack = i - sizeof(rh[0]);
  } else {
    rh = (res_head *) p;
    memset(rh, 0, 9);
    strcpy((char *) rh->name, s);
    str2up(rh->name);
    rh->flag = 0;
    rh->pack = i;
    rh->size = i;
    i += sizeof(rh[0]);
  }
  fl = open(s, O_RDWR | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
  if (fl != -1) {
    write(fl, rh, i);
    close(fl);
  }
  free(p);
  return(0);
}
