#include <stdio.h>
#include <cs50.h>
#include <math.h>

int main(void)
{

    // Get User Input

    string text = get_string("Text: ");
    // printf("%s\n", text);

    int char_counter = 0;
    int letter_counter = 0;
    int word_counter = 0;
    int sentence_counter = 0;

    // Loop on string until last character which should be \0.
    for (int i = 0; text[i] != '\0'; i++)
    {

        // Count letters (using a-z A-Z)
        if ((text[i] >= 'a' && text[i] <= 'z') || (text[i] >= 'A' && text[i] <= 'Z'))
        {
            letter_counter++;
        }

        // Count words (using whitespace)
        if (text[i] == ' ')
        {
            if (word_counter == 0)
            {
                word_counter++;
            }
            word_counter++;
        }

        // Count sentences (using punctuation . ! ?)
        if (text[i] == '.' || text[i] == '!' || text[i] == '?')
        {
            sentence_counter++;
        }

        // Not required but counting characters.
        char_counter++;
    }

    // printf("%i character(s).\n", char_counter);
    // printf("%i letter(s).\n", letter_counter);
    // printf("%i word(s).\n", word_counter);
    // printf("%i sentence(s).\n", sentence_counter);

    // Compute the Coleman-Liau index

    // L is the average number of letters per 100 words.
    float L = (float) letter_counter / ((float) word_counter / 100);
    //printf("%f\n", L);

    // S is the average number of sentences per 100 words.
    float S = (float) sentence_counter / ((float) word_counter / 100);
    // printf("%f\n", S);

    //printf("%f\n", 0.0588 * L - 0.296 * S - 15.8);
    int index = round(0.0588 * L - 0.296 * S - 15.8);
    // printf("%i\n", index);

    if (index < 1)
    {
        printf("Before Grade 1\n");
    }
    else if (index > 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %i\n", index);
    }

}
