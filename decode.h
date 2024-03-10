#ifndef DECODE_H
#define DECODE_H
#include "types.h"
#include "common.h"
#include<stdio.h>
#include<string.h>

typedef struct _decodeInfo
{
	char *enc_image_fname;
	FILE *fptr_enc_image;

	char *extn;
	int extn_size;
	int file_size;
	char *secret_fname;
	FILE *fptr_secret;
}decodeInfo;

Status read_and_validate_decode(char **argv,decodeInfo *decode);
Status do_decoding(decodeInfo *decode);
Status open_decode_files(decodeInfo *decode);
Status check_magic_string(decodeInfo *decode);
Status decodebytes(char *buffer,char *data);
Status decode_extn_size(decodeInfo *decode,int *data);
Status decode_extn(decodeInfo *decode);
Status decode_data(decodeInfo *decode);

#endif
