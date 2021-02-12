#include <stdio.h>
#include <stdlib.h>

int isAJpg(unsigned char blockBytes[]);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: ./recover filename\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Could not open file");
        return 2;
    }
    
    // variable for naming files
    char filename[10];    
    // variable used to count which file we're in
    int count = 0;
    // variable used to store all 512 Bytes from block. Using char beucase 1 char = 1 byte, and we want 512 bytes
    unsigned char blockBytes[512];
    // creates output file pointer
    FILE *output;
    // jpgfound control
    int jpgfound = 0;
   
    // loop that reads all 512 Bytes blocks from file. Reads complete file. Will return 0 when not able to read 512 bytes, meaning file is over (cursor keeps going foward for next block)
    while (fread(blockBytes, sizeof(blockBytes), 1, file) != 0) {       
        // when isAJpg() returns 1, so we know this block as a JPG, this if statement will be executed
        if (isAJpg(blockBytes) == 1) {
            if (jpgfound == 1) {
                fclose(output);
            } else {
                jpgfound = 1;
            }
            // name file
            sprintf(filename, "%03d.jpg", count);           
            // open file
            output = fopen(filename, "a"); 
            if(output == NULL) {
                printf("Error creating file");
                fclose(file);
                return 3;                
            }          
            count++;
        }

        if (jpgfound == 1) {
            // writes to the current file
            fwrite(&blockBytes, sizeof(blockBytes), 1, output);
        }
    }

    //close the files
    fclose(output);
    fclose(file);
}

// check's if it's a JPG
int isAJpg(unsigned char blockBytes[]) {
    if (blockBytes[0] == 0xff && blockBytes[1] == 0xd8 && blockBytes[2] == 0xff && (blockBytes[3] & 0xf0) == 0xe0)
        return 1;

    return 0;
}