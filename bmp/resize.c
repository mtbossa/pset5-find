/****************************************************************************
 * copy.c
 *
 * CC50
 * Pset 5
 *
 * Copies a BMP piece by piece, just because.
 ***************************************************************************/
       
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "bmp.h"


int
main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: resize n infile outfile\n");
        return 1;
    }

    // transform char* num to int
    int n = atoi(argv[1]);

    // checks n
    if (n < 1 || n > 100)
    {
        fprintf(stderr, "Resize only 1-100. Try again.\n");
        return 4;
    }

    // remember filenames    
    char *infile = argv[2];
    char *outfile = argv[3];    

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // determines padding before resize
    int padding_infile =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // new bf and bi values
    BITMAPFILEHEADER new_bf = bf;
    BITMAPINFOHEADER new_bi = bi;

    // changes new bf with n   
    new_bi.biWidth = bi.biWidth * n;
    new_bi.biHeight = bi.biHeight * n;

    // determine padding for scanlines using new_bi Width and Height values
    int padding_outfile =  (4 - (new_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // biSizeIamge is the scanline + padding * the height
    new_bi.biSizeImage = (new_bi.biWidth * sizeof(RGBTRIPLE) + padding_outfile) * abs(new_bi.biHeight);
    // bibfSize (full file size) is the biSizeImage + headers
    new_bf.bfSize = new_bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&new_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&new_bi, sizeof(BITMAPINFOHEADER), 1, outptr);

     // temporary storage
    RGBTRIPLE triple;
    RGBTRIPLE *sline = malloc(new_bi.biWidth * sizeof(RGBTRIPLE));

    // read RGB triple from infile
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        for(int j = 0; j < bi.biWidth; j++)
        {
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            for(int m = 0; m < n; m++)            
                sline[j * n + m] = triple;
                            
        }

        fseek(inptr, padding_infile, SEEK_CUR);

        //write new scanline to file
        for (int k = 0; k < n; k++)
        {
            fwrite(sline, new_bi.biWidth * 3, 1, outptr);

            // add padding if any
            for (int h = 0; h < padding_outfile; h++)
            {
                fputc(0x00, outptr);
            }
        }
    }

    free(sline);

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
