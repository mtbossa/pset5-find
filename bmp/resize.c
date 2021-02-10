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
        return 2;
    }

    // remember filenames    
    char *infile = argv[2];
    char *outfile = argv[3];    

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
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
        return 5;
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

     // temporary storage for infile's pixel RGB struct values
    RGBTRIPLE triple;

    // creates variable for the size of an complete scanline for better coding understanding
    int sizeScanline = new_bi.biWidth * sizeof(RGBTRIPLE);

    // creates new RGBTRIPLE variable type as pointer, because we need to allocate memory with malloc, which returns an address
    RGBTRIPLE *scanline = malloc(sizeScanline); /* we need to allocate the sizeScanline because we are going to store it as a complete scanline and not just a single pixel */
    if(scanline == NULL)
    {
        fclose(inptr);
        fclose(outptr);
        fprintf(stderr, "Error while allocating memory.\n");
        return 6;
    }

    // read RGB triple from infile
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++) // changes the infile's row (scanline)
    {
        // infile's pixel
        for(int j = 0; j < bi.biWidth; j++) // changes the infile's pixel in current scanline
        {
            // read first infile's pixel and stores in triple
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // stores the same infile's pixel (j) n times (loop with int m = 0 will repeat n times for each infile's pixel)
            for(int m = 0; m < n; m++)
            {                
                // index has this formula ([j * n + m]) so we can create one complete outfile's scanline, based on n.
                scanline[j * n + m] = triple; 
            }                            
        }

        // fseek is here beacuse when we get here, it means we finished infile's scaline (row), so we have to skip padding
        fseek(inptr, padding_infile, SEEK_CUR);

        // write new scanline to file n times
        for (int k = 0; k < n; k++) // will repeate n times, writing the scanline (scanline created for outfile) n times.
        {
            fwrite(scanline, sizeScanline, 1, outptr); // we need to write it with the size of a complete scanline

            // add padding if any
            for (int h = 0; h < padding_outfile; h++)
            {
                fputc(0x00, outptr);
            }
        }
    }

    free(scanline);

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
