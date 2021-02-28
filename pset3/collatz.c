#include <cs50.h>
#include <stdio.h>

int collatz(int n);

int even_counter = 0;
int odd_counter = 0;

int main(void)
{
    int n = get_int("Integer: ");
    collatz(n);
}

int collatz(int n)
{
    if (n == 1)
    {
        printf("Finished with odd_counter %i and even counter %i for a total of %i\n", odd_counter, even_counter, odd_counter + even_counter);
        return 0;
    }
    else if (n % 2 == 0)
    {
        even_counter++;
        return 1 + collatz(n / 2);
    }
    else {
        odd_counter++;
        return 1 + collatz(3 * n + 1);
    }
    return 1;
}
