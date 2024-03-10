#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"


/* Function Definitions */
Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
	if(strcmp(strstr(argv[2],"."),".bmp")==0)
	{
		encInfo->src_image_fname=argv[2];
	}
	else
	{
		return e_failure;
	}

	if(strcmp(strstr(argv[3],"."),".txt")==0 ||strcmp(strstr(argv[3],"."),".c")==0)
	{
		encInfo->secret_fname=argv[3];
		strcpy(encInfo->extn_secret_file,strstr(argv[3],"."));
	}
	else
	{
		return e_failure;
	}

	if(argv[4]!=NULL)
	{
		if(strcmp(strstr(argv[4],"."),".bmp")==0)
     	{
			encInfo->stego_image_fname=argv[4];
    	}
	    else
     	{
	    	return e_failure;
       	}
	}
	else
	{
		encInfo->stego_image_fname="Output.bmp";
	}

	
	return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{
	if(open_files(encInfo)==e_success)
	{
		printf("All Files are opened Successfully\n");
		if(check_capacity(encInfo)==e_success)
		{
			printf("Image capacity is Checked \n");

			if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
			{
				printf("Header is copied is successfuly\n");
				if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
				{
					printf("Magic string encoded successfully\n");
					if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_success)
					{
						printf("secret file extension is encoded successfully\n");
			        	if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
				        {
							printf("secret file extension encoded succesfully\n");
							if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
							{
								printf("Secret file size is encoded successfully\n");
								if(encode_secret_file_data(encInfo)==e_success)
								{
									printf("Scret file data are encoded successfully\n");
									if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
									{
										printf("Encoding is completed Successfully\n");
										return e_success;
									}
									else
									{
										printf("Failed to encode remaining data\n");
									}

								}
								else
								{
									printf("Encoding secret file data is failed\n");
								}

							}
							else
							{
								printf("failed to encode secret file size\n");
						    }

				        }
						else
			        	{
							printf("Encoding secret file extension failed\n");
						}	   
					}
					else
					{
						printf("encode secret file extn is failed\n");	
					}
				}
				else
				{
					printf("Failed to encode magic string\n");
				}
			}
			else
			{
				printf("Failed to copy header\n");
			}

		}
		else
		{
			printf("Image capacity is not Enough!!\n");
		}

	}
	else
	{
		printf("Opening the Files operation is failed\n");
	}
}

Status copy_remaining_img_data(FILE *fptr_src_image,FILE *fptr_stego_image)
{
	char ch;
	while(fread(&ch,1,1,fptr_src_image)>0)
	{
		fwrite(&ch,1,1,fptr_stego_image);
	}
	return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char arr[encInfo->size_secret_file];
	fseek(encInfo->fptr_secret,0,SEEK_SET);

	fread(arr,encInfo->size_secret_file,1,encInfo->fptr_secret);
		encode_data_to_image(arr,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image);

}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
	char str[32];
	fread(str,32,1,encInfo->fptr_src_image);
	encode_size_to_lsb(file_size,str);
	fwrite(str,32,1,encInfo->fptr_stego_image);
	return e_success;
}


Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
	encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image);
	return e_success;
}


Status encode_secret_file_extn_size(long size, EncodeInfo *encInfo)
{
	char str[32];
	fread(str,32,1,encInfo->fptr_src_image);
	encode_size_to_lsb(size,str);
	fwrite(str,32,1,encInfo->fptr_stego_image);
	return e_success;
}

Status encode_size_to_lsb(int size, char *image_buffer)
{
	for(int i=0;i<32;i++)
	{
		image_buffer[i]=(image_buffer[i]&(~1)) |((size & (1<<(31-i)))>>(31-i));
	}
}
		

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
	if(encode_data_to_image(magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

Status encode_data_to_image(char *data,int size,FILE *fptr_src_image,FILE *fptr_stego_image)
{
	int i;
	char str[8];
	for(i=0;i<size;i++)
	{
		fread(str,8,1,fptr_src_image);
		encode_byte_to_lsb(data[i],str);
		fwrite(str,8,1,fptr_stego_image);
	}
	return e_success;
}

Status encode_byte_to_lsb(char data,char *image_buffer)
{
	for(int i=0;i<8;i++)
	{
		image_buffer[i]=(image_buffer[i]&0XFE) |((data & (1<<(7-i)))>>(7-i));
	}
}






Status copy_bmp_header(FILE *fptr_source,FILE *fptr_dest)
{
	 char arr[54];
     //Bring back source pointer to the 0th position
	 fseek(fptr_source,0,SEEK_SET);
	 fread(arr,54,1,fptr_source);
	 fwrite(arr,54,1,fptr_dest);
	 return e_success;
}




Status check_capacity(EncodeInfo *encInfo)
{
	//To get the size of the source image
	encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
	//To get the size of the secret data
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);

	printf("Size of secret file is : %lu\n",encInfo->size_secret_file);

	if(encInfo->image_capacity>(54+16+32+32+32+(encInfo->size_secret_file*8)))
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

			

uint get_file_size(FILE *fptr_image)
{
	//We will move file pointer to the end 
	// using ftell() find the size of secret file
	fseek(fptr_image,0,SEEK_END);
	return ftell(fptr_image);
}



/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
