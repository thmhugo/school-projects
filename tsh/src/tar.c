#include "inc/tar.h"

void tar_set_checksum(s_posix_header *hd) {
	memset(hd->chksum,' ',8);
	unsigned int sum = 0;
	char *p = (char *)hd;
	for (int i=0; i < TAR_BLOCKSIZE; i++) { sum += p[i]; }
	sprintf(hd->chksum,"%06o",sum);
}

int tar_check_checksum(s_posix_header *hd) {
	unsigned int checksum;
	sscanf(hd->chksum,"%o ", &checksum);
	unsigned int sum = 0;
	char *p = (char *)hd;
	for (int i=0; i < TAR_BLOCKSIZE; i++) { sum += p[i]; }
	for (int i=0;i<8;i++) { sum += ' ' - hd->chksum[i]; }
	return (checksum == sum);
}
