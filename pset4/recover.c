#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage is ./recover [filename to recover].\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        return 1;
    }

    BYTE *buffer = malloc(512 * sizeof(BYTE));
    int img_counter = 0;
    char *filename = malloc(8*sizeof(char));
    FILE *img;

    while(fread(buffer, 1, 512, file) == 512)
    {
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            if(img_counter == 0)
            {
                sprintf(filename, "%03i.jpg", img_counter);
                img = fopen(filename, "w");
                img_counter++;
            }
            else
            {
                fclose(img);
                sprintf(filename, "%03i.jpg", img_counter);
                img = fopen(filename, "w");
                img_counter++;
            }
        }
        if (img_counter != 0)
        {
            fwrite(buffer, 512, 1, img);
        }
    }

    fclose(img);
    fclose(file);
    free(filename);
    free(buffer);
    return 0;
}
