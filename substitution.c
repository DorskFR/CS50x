#include <stdio.h>
#include <cs50.h>


// This command-line argument should be 26 letters. 0 repetition allowed and all 26 letters of the alphabet needed. Error return 1 if not met.
// uppercase or lowercase should not matter for key. a, A = z, Z ?
// Input plaintext:
// Output ciphertext: but conserve uppercase / lowercase.
// At the end \n and return 0.

int main(int argc, string argv[])
{

    // Obtain ONE command-line argument. Error return 1 is <2 or >2 (first one is the command).
    if (argc < 2 || argc > 2)
    {
        printf("You need to provide one and only one command line argument.\n");
        return 1;
    }

    // This command-line argument should be 26 letters. 0 repetition allowed and all 26 letters of the alphabet needed. Error return 1 if not met.

    int key_length = 0;
    char cipher_key[26];

    for (int i = 0; argv[1][i] != '\0'; i++)
    {
        key_length++;

        // Exit if not a letter. Uppercase is 65 to 90 and lowercase is 97 to 122 (Delta of 32).
        if ((int) argv[1][i] < 65 || ((int) argv[1][i] > 90 && (int) argv[1][i] < 97) || (int) argv[1][i] > 122)
        {
            printf("Your key is invalid (contains characters other than letters)\n");
            return 1;
        }

        // Convert the key to all capital letters. will check later if thats clever.
        if ((int) argv[1][i] >= 97 && (int) argv[1][i] <= 122)
        {
            argv[1][i] = argv[1][i] - 32;
        }

        // printf("argv[1][i] value is: %c\n", argv[1][i]);

        for (int j = 0; j < key_length; j++)
        {
            if (cipher_key[j] == argv[1][i])
            {
                printf("Duplicate character found: %c\n", argv[1][i]);
                return 1;
            }
        }
        cipher_key[i] = argv[1][i];
    }

    if (key_length > 26)
    {
        printf("Your key is invalid (more than 26 letters)\n");
        return 1;
    }

    if (key_length < 26)
    {
        printf("Your key is invalid (less than 26 letters)\n");
        return 1;
    }

    printf("The key you have provided is valid.\n");

    // Now that the key is valid, we go ahead and ask for input.

    string plaintext = get_string("Plaintext: ");

    int plaintext_length = 0;

    for (int i = 0; plaintext[i] != '\0'; i++)
    {
        plaintext_length++;
    }

    int ciphertext[plaintext_length];

    for (int i = 0; plaintext[i] != '\0'; i++)
    {
        if (plaintext[i] >= 65 && plaintext[i] <= 90)
        {
            // uppercase ciphering
            ciphertext[i] = cipher_key[plaintext[i] - 65];
        }
        else if (plaintext[i] >= 97 && plaintext[i] <= 122)
        {
            // lowercase ciphering
            ciphertext[i] = cipher_key[plaintext[i] - 97] + 32;
        }
        else
        {
            ciphertext[i] = plaintext[i];
        }
    }

    printf("ciphertext: ");

    for (int i = 0; i < plaintext_length; i++)
    {
        printf("%c", ciphertext[i]);
    }

    printf("\n");

}
