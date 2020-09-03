#include "helpers.h"
#include <math.h>
#include <stdio.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (image[i][j].rgbtBlue != image[i][j].rgbtGreen || image[i][j].rgbtGreen != image[i][j].rgbtRed)
            {
                int average_value = (int) round(((float) image[i][j].rgbtBlue + (float) image[i][j].rgbtGreen + (float) image[i][j].rgbtRed) / 3);
                image[i][j].rgbtBlue = average_value;
                image[i][j].rgbtGreen = average_value;
                image[i][j].rgbtRed = average_value;
            }
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j <= (width - j - 1); j++)
        {
            int tmp_pixBlue = image[i][width - j - 1].rgbtBlue;
            int tmp_pixGreen = image[i][width - j - 1].rgbtGreen;
            int tmp_pixRed = image[i][width - j - 1].rgbtRed;

            image[i][width - j - 1].rgbtBlue = image[i][j].rgbtBlue;
            image[i][width - j - 1].rgbtGreen = image[i][j].rgbtGreen;
            image[i][width - j - 1].rgbtRed = image[i][j].rgbtRed;

            image[i][j].rgbtBlue = tmp_pixBlue;
            image[i][j].rgbtGreen = tmp_pixGreen;
            image[i][j].rgbtRed = tmp_pixRed;
        }
    }
    return;
}

void add_pixel_blur(int *neighbour_pix_count, int new_pix[3], int height, int width, RGBTRIPLE image_copy[height][width], int r, int c)
{
    *neighbour_pix_count += 1;
    new_pix[0] += image_copy[r][c].rgbtBlue;
    new_pix[1] += image_copy[r][c].rgbtGreen;
    new_pix[2] += image_copy[r][c].rgbtRed;
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE image_copy[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image_copy[i][j].rgbtBlue = image[i][j].rgbtBlue;
            image_copy[i][j].rgbtGreen = image[i][j].rgbtGreen;
            image_copy[i][j].rgbtRed = image[i][j].rgbtRed;
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int neighbour_pix_count = 1;
            int new_pix[3] = { image_copy[i][j].rgbtBlue, image_copy[i][j].rgbtGreen, image_copy[i][j].rgbtRed };

            // checking top row to see if it exists. if it does checking if corners exist.
            if (i - 1 >= 0)
            {
                add_pixel_blur(&neighbour_pix_count, new_pix, height, width, image_copy, i - 1, j);
                if (j - 1 >= 0) { add_pixel_blur(&neighbour_pix_count, new_pix, height, width, image_copy, i - 1, j - 1); }
                if (j + 1 < width) { add_pixel_blur(&neighbour_pix_count, new_pix, height, width, image_copy, i - 1, j + 1); }
            }

            // checking middle row to see if it exists.
            if (j - 1 >= 0) { add_pixel_blur(&neighbour_pix_count, new_pix, height, width, image_copy, i, j - 1); }
            if (j + 1 < width)  { add_pixel_blur(&neighbour_pix_count, new_pix, height, width, image_copy, i, j + 1); }

            // checking bottom row to see if it exists. if it does checking if corners exist.
            if (i + 1 < height)
            {
                add_pixel_blur(&neighbour_pix_count, new_pix, height, width, image_copy, i + 1, j);
                if (j - 1 >= 0) { add_pixel_blur(&neighbour_pix_count, new_pix, height, width, image_copy, i + 1, j - 1); }
                if (j + 1 < width) { add_pixel_blur(&neighbour_pix_count, new_pix, height, width, image_copy, i + 1, j + 1); }
            }

            image[i][j].rgbtBlue = (int) round((float) new_pix[0] / (float) neighbour_pix_count);
            image[i][j].rgbtGreen = (int) round((float) new_pix[1] / (float) neighbour_pix_count);
            image[i][j].rgbtRed = (int) round((float) new_pix[2] / (float) neighbour_pix_count);
        }
    }
    return;
}

void add_pixel_edges(int g_values[6], int height, int width, RGBTRIPLE image_copy[height][width], int r, int c, int a, int Gx[8], int Gy[8])
{
    g_values[0] += (image_copy[r][c].rgbtBlue * Gx[a]);
    g_values[1] += (image_copy[r][c].rgbtBlue * Gy[a]);
    g_values[2] += (image_copy[r][c].rgbtGreen * Gx[a]);
    g_values[3] += (image_copy[r][c].rgbtGreen * Gy[a]);
    g_values[4] += (image_copy[r][c].rgbtRed * Gx[a]);
    g_values[5] += (image_copy[r][c].rgbtRed * Gy[a]);
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE image_copy[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image_copy[i][j].rgbtBlue = image[i][j].rgbtBlue;
            image_copy[i][j].rgbtGreen = image[i][j].rgbtGreen;
            image_copy[i][j].rgbtRed = image[i][j].rgbtRed;
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int Gx[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
            int Gy[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
            int g_values[6] = { 0, 0, 0, 0, 0, 0 };

            if (i - 1 >= 0) // if there is a top row
            {
                if (j - 1 >= 0) { add_pixel_edges(g_values, height, width, image_copy, i - 1, j - 1, 0, Gx, Gy); }
                add_pixel_edges(g_values, height, width, image_copy, i - 1, j, 1, Gx, Gy);
                if (j + 1 < width) { add_pixel_edges(g_values, height, width, image_copy, i - 1, j + 1, 2, Gx, Gy); }
            }

            // checking middle row to see if it exists.
            if (j - 1 >= 0) { add_pixel_edges(g_values, height, width, image_copy, i, j - 1, 3, Gx, Gy); }
            if (j + 1 < width) { add_pixel_edges(g_values, height, width, image_copy, i, j + 1, 5, Gx, Gy); }

            // checking bottom row to see if it exists. if it does checking if corners exist.
            if (i + 1 < height)
            {
                if (j - 1 >= 0) { add_pixel_edges(g_values, height, width, image_copy, i + 1, j - 1, 6, Gx, Gy); }
                add_pixel_edges(g_values, height, width, image_copy, i + 1, j, 7, Gx, Gy);
                if (j + 1 < width) { add_pixel_edges(g_values, height, width, image_copy, i + 1, j + 1, 8, Gx, Gy); }
            }

            int new_pix_blue = round(sqrt(pow(g_values[0], 2) + pow(g_values[1], 2)));
            if(new_pix_blue > 255) { new_pix_blue = 255; }
            image[i][j].rgbtBlue = new_pix_blue;

            int new_pix_green = round(sqrt(pow(g_values[2], 2) + pow(g_values[3], 2)));
            if(new_pix_green > 255) { new_pix_green = 255; }
            image[i][j].rgbtGreen = new_pix_green;

            int new_pix_red = round(sqrt(pow(g_values[4], 2) + pow(g_values[5], 2)));
            if(new_pix_red > 255) { new_pix_red = 255; }
            image[i][j].rgbtRed = new_pix_red;

        }
    }
    return;
}
