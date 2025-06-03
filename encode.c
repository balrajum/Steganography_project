/*
NAME : BALRAJU M
DESCRIPTION : Steganography Project which is used hide the information in the Least significant bits in a picture or any kind of media
*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "common.h"
#include "types.h"

/* Function Definitions */
OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0)
        return e_encode;
    else if (strcmp(argv[1], "-d") == 0)
        return e_decode;
    return e_unsupported;
}
/*READ AND VALIDATING THE ARGUMENTS*/
/*Input : ARGUMENTS AND STRUCTURE*/
/*Here the comparision with the extenstions with the user given arguments
 if its okay as per the processing the encode and decode then storing those argumrnts in the specified structures members*/
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    char *temp = strstr(argv[3], ".");
    // Copying the extenstion
    strcpy(encInfo->extn_secret_file, temp);
    if (strcmp(temp, ".txt") == 0)
    {
        encInfo->secret_fname = argv[3];
    }
    else if (strcmp(temp, ".sh") == 0)
    {
        encInfo->secret_fname = argv[3];
    }
    else if (strcmp(temp, ".c") == 0)
    {
        encInfo->secret_fname = argv[3];
    }
    else
        return e_failure;

    if (argv[4] != NULL)
    {
        if ((strcmp(strstr(argv[4], "."), ".bmp")) == 0)
        {
            encInfo->stego_image_fname = argv[4];
        }
        else
            return e_failure;
    }
    else
        // default output file name if arg 4 is not present
        encInfo->stego_image_fname = "stego.bmp";

    return e_success;

    printf("INFO : Validation completed\n");
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

Status check_capacity(EncodeInfo *encInfo)
{
    // magic string length
    int len = strlen(MAGIC_STRING);
    // printf("Magic string length %d\n", len);

    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    // to get size of the secret file
    uint size_secret_file = get_file_size(encInfo->fptr_secret);

    // storing in structure data member
    encInfo->size_secret_file = size_secret_file;
    // find the encoding data size, mul with 8.cause for 1 bit we need 1 byte to place the data in lsb position
    uint encode_size = 54 + (len + 4 + 4 + strlen(encInfo->extn_secret_file) + size_secret_file) * 8;

    // comparing the to the image size with encode size if encode size is more than the image we can't hide information
    if (encInfo->image_capacity > encode_size)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/*TO GET FILE SIZE
INPUT : FILE POINTER
returing the offset position to find the size of the file*/
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}
// Copying the image header from source file to destination file header is common
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];
    rewind(fptr_src_image);  // makesure offset position at 0
    rewind(fptr_dest_image); // changed offset position to 0
    fread(buffer, 54, 1, fptr_src_image);
    fwrite(buffer, 54, 1, fptr_dest_image);
    printf("INFO: Copying Image Header\n");
    return e_success;
}
/*ENCODING THE MAGIC STRING
input : magic string and structure*/
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    // to get size of the magic string
    int magicstr_len = strlen(MAGIC_STRING);
    // passing parameters to the function magic string and its size, source and destination struct members
    encode_data_to_image(magic_string, magicstr_len, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

// encoding the data in the lsb position
Status encode_byte_to_lsb(char data, char image_buffer[])
{
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7 - i)) & 1);
    }
    return e_success;
}

// Encoding data to image
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    // char buffer to store data and write in the destination file
    char image_buffer[8];
    for (int i = 0; i < size; i++) // loop will run size times
    {
        fread(image_buffer, 8, 1, fptr_src_image);
        encode_byte_to_lsb(data[i], image_buffer); // passing data and char buffer array to store data
        fwrite(image_buffer, 8, 1, fptr_stego_image);
    }
    return e_success;
}

// encode the the to the lsb positions
Status encode_size_to_lsb(long size, char image_buffer[])
{
    // printf("Data size is %ld\n", size);
    for (int i = 0; i < 32; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((size >> (31 - i)) & 1);
    }
}

// to encode the file extenstion size
Status encode_file_extn_size(char *file_extn, EncodeInfo *encInfo)
{
    // In int 32 bits will be there so taking char buffer size is 32 to store bits
    char image_buffer[32];
    fread(image_buffer, 32, 1, encInfo->fptr_src_image);
    // to get file extenstion length
    long size = strlen(file_extn);
    encode_size_to_lsb(size, image_buffer);
    fwrite(image_buffer, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}
Status encode_secret_file_size(uint size_secret_file, EncodeInfo *encInfo)
{
    char image_buffer[32];
    fread(image_buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(size_secret_file, image_buffer);
    fwrite(image_buffer, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char sec_file[encInfo->size_secret_file];
    rewind(encInfo->fptr_secret);
    fread(sec_file, encInfo->size_secret_file, 1, encInfo->fptr_secret);
    if (encode_data_to_image(sec_file, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        return e_success;
    }
    return e_failure;
}

Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src_image) != 0)
    {
        fwrite(&ch, 1, 1, fptr_stego_image);
    }
    return e_success;
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
    printf("INFO: Openning required files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }
    else
    {
        printf("INFO: Opened Skeleton/beautiful.bmp\n");
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
    else
    {
        printf("INFO: Opened Secret.txt\n");
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
    else
    {
        printf("INFO: Opened Stego.bmp\n");
    }

    // No failure return e_success
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{

    if (check_capacity(encInfo) == e_success)
    {
        printf("INFO: Done. Found OK\n");
        if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
        {
            printf("INFO: Done\n");
            printf("INFO: Encoding Magic String Signature\n");
            printf("INFO: Done\n");
            if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
            {
                printf("INFO: Copying Extention size.\n");
                // printf("%s\n", encInfo->extn_secret_file);
                if (encode_file_extn_size(encInfo->extn_secret_file, encInfo) == e_success)
                {
                    printf("INFO: Secret.txt File Extenstion\n");
                    printf("INFO: Done\n");
                    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                    {
                        printf("INFO: Encoding Secret.txt File Size.\nINFO: Done\n");
                        if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                        {
                            printf("INFO: Encoding secret.txt File Data\n");
                            if (encode_secret_file_data(encInfo) == e_success)
                            {
                                printf("INFO: Done\n");
                                printf("INFO: Copying left over Data\n");
                                if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                {
                                    printf("INFO: Done.\n");
                                    return e_success;
                                }
                            }
                            else
                            {
                                printf("ERROR: secret file data not copied\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("ERROR: Secret file size not copied\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("ERROR: Secret file extention is not stored.\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("ERROR: File extention not copied\n");
                    return e_failure;
                }
            }
            else
            {
                printf("ERROR: Magic string is not encoded\n");
                return e_failure;
            }
        }
        else
        {
            printf("ERROR: Copying image header is not done\n");
            return e_failure;
        }
    }
    else
    {
        printf("ERROR: Capicity is not enough to encode\n");
        return e_failure;
    }
}
