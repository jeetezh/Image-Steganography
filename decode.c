#include "decode.h"

Status read_and_validate_decode(char **argv, decodeInfo *decode)
{
	if(strcmp(strstr(argv[2],"."),".bmp")==0)
	{
		decode->enc_image_fname=argv[2];
	}
	if(argv[3]!=NULL)
	{
		char *s=strtok(argv[3],".");
		decode->secret_fname=s;
	}
	else
	{
		decode->secret_fname="Output";
	}
	printf("the secret file name is %s :\n",decode->secret_fname);
	return e_success;
}

Status do_decoding(decodeInfo *decode)
{
	if(open_decode_files(decode)==e_success)
	{
		printf("ALl files are open\n");

		fseek(decode->fptr_enc_image,54,SEEK_SET);
		
		if(check_magic_string(decode)==e_success)
		{
			printf("Magic string is decoded\n");
			int data;
			if(decode_extn_size(decode,&data)==e_success)
			{
				decode->extn_size=data;
				printf("Extension is decoded successfully\n and extension is %d\n",decode->extn_size);
				if(decode_extn(decode)==e_success)
				{
					printf("Extension is decoded successfully\n");
					if(decode->fptr_secret==NULL)
					{
						printf("Failed to open file\n");
					}
					else
					{
						int d;
						if(decode_extn_size(decode,&d)==e_success)
						{
							decode->file_size=d;
							printf("File size is decoded successfully\n");
							printf("File size is %d\n",decode->file_size);
							if(decode_data(decode)==e_success)
							{
								printf("Data decoded successfully\n");
							}
							else
							{
								printf("Failed to decode data\n");
							}
						}
						else
						{
							printf("Failed to decode file size\n");
						}
					}

				}
				else
				{
					printf("Failed to decode extension\n");
				}
			}
		    else
		    {
				printf("Failed to decoded File extension size\n");
			}
		}
		else
		{
			printf("Failed to decode magic string\n");
		}
	}
	else
	{
		printf("Failed to open files\n");
	}

}

Status decode_extn(decodeInfo *decode)
{
	char buffer[8];
	int i;
	char ch[decode->extn_size];

	for(i=0;i<decode->extn_size;i++)
	{
		fread(buffer,8,1,decode->fptr_enc_image);
		decodebytes(buffer,&ch[i]);
	}
	ch[i]='\0';
	char new[100];
	strcpy(new,decode->secret_fname);
	strcat(new,ch);
	decode->secret_fname=new;
	printf("The file name is : %s\n",decode->secret_fname);
	decode->fptr_secret=fopen(decode->secret_fname,"w");
	return e_success;	
}

Status decode_extn_size(decodeInfo *decode,int *data)
{
	int num=0,i;
	char buffer[32];
	fread(buffer,32,1,decode->fptr_enc_image);
	for(i=0;i<32;i++)
	{
		num=((buffer[i] & (1))<<(31-i)) |num;
	}
	*data=num;
	return e_success;
}

Status check_magic_string(decodeInfo *decode)
{
	char magic_string[3];
	char buffer[8];
	int i;
	for(i=0;i<2;i++)
	{
		fread(&buffer,1,8,decode->fptr_enc_image);
		decodebytes(buffer,&magic_string[i]);
	}
	magic_string[i]='\0';
	printf("Magic string is %s\n ",magic_string);

	if(strcmp(magic_string,MAGIC_STRING)==0)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}	
}

Status decodebytes(char *buffer,char *data)
{
	char ch=0;
    for(int i=0;i < 8 ;i++)
    {
	ch =((buffer[i] & 0x01)<< (7 - i )) |ch;
    }
    *data= ch;
}




Status open_decode_files(decodeInfo *decode)
{
	decode->fptr_enc_image=fopen(decode->enc_image_fname,"r");

	if(decode->fptr_enc_image==NULL)
	{
		printf("Failed to open encrypted image\n");
		return e_failure;
	}
	return e_success;
}

Status decode_data(decodeInfo *decode)
{
	char new[100];
	int i,j;
	char buf[8];
	for(i=0;i<decode->file_size;i++)
	{
		fread(buf,8,1,decode->fptr_enc_image);
		decodebytes(buf,&new[i]);
		fwrite(&new[i],1,1,decode->fptr_secret);
	}
	return e_success;
}






