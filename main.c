#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include<string.h>
#include<stdlib.h>

int main(int argc, char *argv[])
{
	if(argc<3)
	{
		printf("Please Enter required number of argument\n");
		return 1;
	}
	
    int res=check_operation_type(argv);

	if(res==e_encode)
	{
		printf("Encoding selected\n");
		EncodeInfo encInfo;
		
		if(read_and_validate_encode_args(argv,&encInfo)==e_success)
		{
			printf("Read and Validate is done succesfully\n");

			do_encoding(&encInfo);
		}
		else
		{
			printf("Read and Validate is Failed\n");
			exit(0);
		}

	}
	else if(res==e_decode)
	{
		decodeInfo decode;
		printf("Decoding is selected\n");
		if(read_and_validate_decode(argv,&decode)==e_success)
		{
			printf("Read and validate is done successfully\n");
			do_decoding(&decode);
		}
		else
		{
			printf("Read and validate is failed\n");
			exit(0);
		}
	}
	else
	{
		printf("Error:: Enter correct option either -e for encoding or -d for decoding\n");
		exit(1);
	}

		



    return 0;
}


OperationType check_operation_type(char *argv[])
{
	if(strcmp(argv[1],"-e")==0)
	{
		return e_encode;
	}
	else if(strcmp(argv[1],"-d")==0)
	{
		return e_decode;
	}
	else
	{
		return e_unsupported;
	}
}

	




