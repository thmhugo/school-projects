#ifndef TAR_H
#define TAR_H

/* tar Header Block, from POSIX 1003.1-1990.  */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define TAR_BLOCKSIZE 512
#define TAR_BLOCKBITS 9

/* POSIX header.  */

/* Note that sizeof(struct posix_header) == TAR_BLOCKSIZE */

typedef struct posix_header {                              /* byte offset */
  char name[100];               /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */
  char gid[8];                  /* 116 */
  char size[12];                /* 124 */
  char mtime[12];               /* 136 */
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */
  char linkname[100];           /* 157 */
  char magic[6];                /* 257 */
  char version[2];              /* 263 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char prefix[155];             /* 345 */
  char junk[12];                /* 500 */
} s_posix_header;                              /* Total: 512 */

/* Type flags. */
#define TAR_NORMALFILE_F  '0'
#define TAR_HLINK_F       '1'
#define TAR_SLINK_F       '2'
#define TAR_SPECIALCHAR_F '3'
#define TAR_SPECIALBLOC_F '4'
#define TAR_DIR_F         '5'
#define TAR_FIFO_F        '6'

#define TAR_NORMALFILE_V  '-'
#define TAR_HLINK_V       '-'
#define TAR_SLINK_V       'l'
#define TAR_SPECIALCHAR_V 'c'
#define TAR_SPECIALBLOC_V 'b'
#define TAR_DIR_V         'd'
#define TAR_FIFO_V        'p'


#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION "0"           /* 0 and no null */
#define TVERSLEN 2

#define TAR_DEFAULT_DIR_SIZE "00000000000"
#define TAR_DEFAULT_DIR_OCTAL_MODE 755

#define TAR_NAME_SIZE 100
#define TAR_U_G_NAME_SIZE 32

#define INT_TO_STRING_FORMAT "%.7d"
#define OCTAL_MTIME_FORMAT "%.11lo"
#define OCTAL_PERMISSION_FORMAT "%.7o"
#define OCTAL_FILESIZE_FORMAT "%011lo"

/* ... */

#define OLDGNU_MAGIC "ustar  "  /* 7 chars and a null */

/* ... */

/* Compute and write the checksum of a header, by adding all
   (unsigned) bytes in it (while hd->chksum is initially all ' ').
   Then hd->chksum is set to contain the octal encoding of this
   sum (on 6 bytes), followed by '\0' and ' '.
   */
void tar_set_checksum(s_posix_header *pheader);

/* Check that the checksum of a header is correct */
int tar_check_checksum(s_posix_header *pheader);

#endif
