#include <stdio.h>
#include <cs50.h>

int main(void)
{
    int height = 0;
    do
    {
        height = get_int("Height: ");
    }
    while (height < 1 || height > 8);

    // Height is also equal to width of the base.
    // Width at the base is the same as the width at the top.
    // The filling is made of whitespace characters.
    // The first line is composed of n - 1 characters of white space and 1 character of #.
    // The second line is composed of n - 2 characters of white space and 2 characters of #.
    // So on and so forth until we reach n - n characters of white space and n characters of #.
    // At this point we have finished looping.

    // To make the double pyramid we do the same for the left side of the pyramid.
    // Then we add 2 white spaces in the middle.
    // Then we add the same number of character of # as the left side and skip the whitespace (because it is white by default).
    // We will already have this value of characters of # to write because it will be the same as the left side for symmetrical purposes.

    int n = height; // n is our number of whitespace to add. it is equal to height minus a number that is smaller than height.

    for (int i = 0; i < height; i++) // this is the loop for the number of rows. height is also our max width.
    {
        n--; // each row we remove one to n (removing one whitespace to allow room for one #)

        for (int j = 0; j < n; j++) // this is the loop for the number of whitespace characters in this row
        {
            printf(" "); // we print one char of whitespace at a time.
        }
        for (int k = 0; k < (height - n); k++) // this is the loop for the number of left side # characters in this row
        {
            printf("#");
        }
        printf("  "); // this is the central column
        for (int k = 0; k < (height - n); k++) // this is the loop for the number of right side # characters in this row
        {
            printf("#");
        }
        printf("\n"); // end of the row
    }
}