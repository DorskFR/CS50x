#include <stdio.h>
#include <cs50.h>
#include <math.h>

// Luhn's Algorithm :
// Multiply every other digit by 2 starting with the SECOND TO LAST.
// Add those digits (not the products) to each other = sum 1
// Add the digits not multiplied by 2 = sum 2
// Add sum 1 and sum 2 and if the last digit is 0 (or modulo 10 = 0) then = valid

// To be able to do Luhn's algorithm we need to separate the digits from the products as for example 9 * 2 = 18 must read as 1 and 8 = 9.
// In addition to checking Luhn's algorithm we also want to detect the Credit Card issuers special codes and special lengths.
// American Express card number starts with 34 or 37 and is 15 in length.
// MasterCard starts with 51, 52, 53, 54, 55 and is 16 in length.
// VISA starts with 4 and is 13 or 16 in length.

// We must reject any number that does not fit any of the above formats.
// And reject any other entry that isn't a valid card number.

int main(void)
{

    long cardnumber = 0;

    do
    {
        cardnumber = get_long("Number: ");
    }
    while (cardnumber < 1);
    // while (cardnumber < 999999999999 || cardnumber > 10000000000000000); // limiting to numbers between 13 and 16 in length. Edit: not because it bugs the checkcs50

    // calculate the number of digits of the cardnumber
    int numberlen = floor(1 + log10(cardnumber));

    // printf("Card number length: %i\n", numberlen);

    // array to store individual digits length is the length of the cardnumber
    int digit[numberlen - 1];

    // creating a copy not to alter the original cardnumber
    long cardnumber_copy = cardnumber;

    // storing the individual digits in the array (which will therfore be reverse order of the cardnumber)
    for (int i = 0; i < numberlen; i++)
    {
        digit[i] = cardnumber_copy % 10;
        cardnumber_copy /= 10;
    }

    // calculating the first sum by adding the digits of the odd position numbers

    int sum1 = 0;

    for (int i = 1; i < numberlen; i += 2)
    {
        int currentdigit = digit[i] * 2;
        // printf("Current digit for sum1 is : %i (%i)\n", digit[i], currentdigit);

        if (currentdigit > 9)
        {
            sum1 += currentdigit % 10;
            sum1 += 1;
        }
        else
        {
            sum1 += currentdigit;
        }
    }

    // same thing for even position numbers for second sum

    int sum2 = 0;

    for (int i = 0; i < numberlen; i += 2)
    {
        // printf("Current digit for sum2 is : %i\n", digit[i]);
        sum2 += digit[i];
    }

    // Validating the Luhn's Algorithm

    // printf("Sum1 : %i\nSum2 : %i\nModulo of the sum of both: %i\n", sum1, sum2, (sum1 + sum2) % 10);

    if ((sum1 + sum2) % 10 == 0)
    {
        // Cards here have passed the Luhn's Algorithm test. We now check for specific card masks.

        if ((numberlen == 13 || numberlen == 16) && digit[numberlen - 1] == 4)
        {
            printf("VISA\n");
        }
        else if (numberlen == 16 && digit[numberlen - 1] == 5 && digit[numberlen - 2] > 0 && digit[numberlen - 2] < 6)
        {
            printf("MASTERCARD\n");
        }
        else if (numberlen == 15 && digit[numberlen - 1] == 3 && (digit[numberlen - 2] == 4 || digit[numberlen - 2] == 7))
        {
            printf("AMEX\n");
        }
        else
        {
            printf("INVALID\n");
        }
    }
    else
    {
        printf("INVALID\n");
    }


}
