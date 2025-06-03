/*
NAME : BALRAJU M
DATE: 01/10/2024
DESCRIPTION : Steganography Project which is used hide the information in the Least significant bits in a picture or any kind of media
*/

#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Error:- Insufficient Arguments\n");
        printf("Usage:- For Encoding -> ./a.out -e <.bmp file> <.txt file>\n\tFor Decoding -> ./a.out -d <.bmp file> <output file>\n");
    }
    else
    {
        int type = check_operation_type(argv);
        if (type == e_encode)
        {
            printf("Encode is Started\n");

            EncodeInfo encInfo;
            uint img_size;

            if (read_and_validate_encode_args(argv, &encInfo) == e_success)
            {
                printf("INFO : DONE\n");

                // Test open_files
                if (open_files(&encInfo) == e_failure)
                {
                    printf("ERROR: %s function failed\n", "open_files");
                    return 1;
                }
                else
                {
                    printf("INFO : ## Encoding Procedure Started ##\n");
                    if (do_encoding(&encInfo) == e_success)
                    {
                        printf("INFO: ## Encodig Done Successfully. ##\n");
                    }
                    else
                    {
                        printf("ERROR: Encoding fail\n");
                    }
                }
            }
            else
            {
                printf("Error:- Insufficient Arguments\n");
                printf("Usage:- For Encoding -> ./a.out -e <.bmp file> <.txt file>\n\tFor Decoding -> ./a.out -d <.bmp file> <output file>\n");
            }

            // Test get_image_size_for_bmp
            // img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
            // printf("INFO: Image size = %u\n", img_size);
        }
        else if (type == e_decode)
        {
            DecodeInfo decInfo;

            /*// Fill with sample filenames
              encInfo.src_image_fname = "beautiful.bmp";
              encInfo.secret_fname = "secret.txt";
              encInfo.stego_image_fname = "stego_img.bmp";
             */

            printf("INFO: ## Decoding process started ##\n");
            if (read_and_validate_decode_args(argv, &decInfo) == e_success)
            {
                // function call for decoding
                if (do_decoding(&decInfo) == e_success)
                {
                    printf("INFO: ## Decoding Done Successfully ##\n");
                }
                else
                {
                    printf("Failed to decode\n");
                }
            }
            else
            {
                printf("Error:- Insufficient Arguments\n");
                printf("Usage:- For Encoding -> ./a.out -e <.bmp file> <.txt file>\n\tFor Decoding -> ./a.out -d <.bmp file> <output file>\n");
            }
        }
        else
        {
            printf("Error:- Insufficient Arguments\n");
            printf("Usage:- For Encoding -> ./a.out -e <.bmp file> <.txt file>\n\tFor Decoding -> ./a.out -d <.bmp file> <output file>\n");
        }
    }
    return 0;
}
