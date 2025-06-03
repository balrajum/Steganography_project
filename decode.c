/*
NAME : BALRAJU M
DATE: 01/10/2024
DESCRIPTION : Steganography Project which is used hide the information in the Least significant bits in a picture or any kind of media
*/

#include <stdio.h>
#include <string.h>
#include "common.h"
#include "types.h"
#include "decode.h"
int flag = 0; // global variable declaration
/*Read and validation of arguments*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        // storing encoded file name
        decInfo->stego_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    if (argv[3] != NULL)
    {
        flag = 1;
        decInfo->decode_fname = argv[3];
    }
    else
    {
        // if arg 3 is not present by default set to decode
        strcpy(decInfo->decode_fname, "decode");
    }
    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo)
{
    // openning encoded image in reading mode
    decInfo->fptr_stego = fopen(decInfo->stego_fname, "r");
    // checking encode image is present or not
    if (decInfo->fptr_stego == NULL)
    {
        printf("ERROR: Missing image.bmp file\n");
        return e_failure;
    }
    return e_success;
}
// fetching lsb bits to char buffer
char decode_lsb_to_char(DecodeInfo *decInfo)
{
    char buffer[8];
    fread(buffer, 8, 1, decInfo->fptr_stego);
    char ch = 0;
    for (int i = 0; i < 8; i++)
    {
        ch = ch | (buffer[i] & 1);
        if (i != 7)
            ch = ch << 1;
    }
    return ch;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    int magic_len = strlen(MAGIC_STRING);
    fseek(decInfo->fptr_stego, 54, SEEK_SET); // skip the header so setting offset position to 54
    char magic_str[5];
    int i;
    for (i = 0; i < magic_len; i++)
    {
        magic_str[i] = decode_lsb_to_char(decInfo);
    }
    magic_str[i] = '\0'; // adding null character to get string
    if (strcmp(magic_str, MAGIC_STRING) == 0)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

int decode_lsb_to_int(DecodeInfo *decInfo)
{
    char intbuffer[32];
    int size = 0;
    fread(intbuffer, 32, 1, decInfo->fptr_stego);
    for (int i = 0; i < 32; i++)
    {
        size = size | (intbuffer[i] & 1);
        if (i != 31)
            size = size << 1;
    }
    return size;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    decInfo->extn_size = decode_lsb_to_int(decInfo);
    return e_success;
}

Status decode_secret_extn(DecodeInfo *decInfo)
{
    char extention[decInfo->extn_size + 1]; // size+1 to store null character
    for (int i = 0; i < decInfo->extn_size; i++)
    {
        extention[i] = decode_lsb_to_char(decInfo);
    }
    extention[decInfo->extn_size] = '\0';
    char *temp = strstr(decInfo->decode_fname, ".");
    if (temp != NULL)
    {
        int i = 0;
        char str[100];
        while (decInfo->decode_fname[i] != '.')
        {
            str[i] = decInfo->decode_fname[i];
            i++;
        }
        str[i] = '\0';
        strcpy(decInfo->decode_fname, str);
    }
    strcat(decInfo->decode_fname, extention); // concatinating the file name with extention
    if (flag == 1)
        printf("INFO: Output File is mentioned. Creating %s\n", decInfo->decode_fname);
    else
        printf("INFO: Output File not mentioned. Creating %s as default\n", decInfo->decode_fname);

    decInfo->fptr_decode = fopen(decInfo->decode_fname, "w");
    if (decInfo->fptr_decode == NULL)
    {
        printf("ERROR: Unable to open output file\n");
        return e_failure;
    }
    printf("INFO: Opened %s\n", decInfo->decode_fname);
    printf("INFO: DOne. Opened all required files\n");

    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    decInfo->size_secretfile = decode_lsb_to_int(decInfo);
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;
    for (int i = 0; i < decInfo->size_secretfile; i++)
    {
        ch = decode_lsb_to_char(decInfo);
        fwrite(&ch, 1, 1, decInfo->fptr_decode); // writing char by char to decoded file or output file
    }
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: Openning required files\n");
    if (open_decode_files(decInfo) == e_success)
    {
        printf("INFO: Opened stego.bmp\n");
        printf("INFO: Decoding Magic String Signature\n");
        if (decode_magic_string(decInfo) == e_success)
        {
            printf("INFO: DONE\n");
            if (decode_secret_file_extn_size(decInfo) == e_success)
            {
                printf("INFO: Decoding Output File Extenstion\n");
                if (decode_secret_extn(decInfo) == e_success)
                {
                    printf("INFO: Decoding File Size\n");
                    if (decode_secret_file_size(decInfo) == e_success)
                    {
                        printf("INFO: Done\n");
                        printf("INFO: Decoding File Data\n");
                        if (decode_secret_file_data(decInfo) == e_success)
                        {
                            printf("INFO: Done\n");
                            return e_success;
                        }
                        printf("ERROR: Failed to copy secret file data\n");
                        return e_failure;
                    }
                    printf("ERROR: Failed to copy secret file size\n ");
                    return e_failure;
                }
                return e_failure;
            }
            printf("ERROR: Failed to copy secret file extention");
            return e_failure;
        }
        printf("ERROR : Failed to decode magic string\n");
        return e_failure;
    }
    printf("ERROR: Failed to open decoding files\n");
    return e_failure;
}