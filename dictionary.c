// Implements a dictionary's functionality

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Number of buckets in hash table
// Going with simple first letter system.
const unsigned int N = 4096;
int number_words = 0;

// Hash table
// Hash table is an array of linked lists
node *table[N];

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    char word_copy[LENGTH + 1] = "";

    for (int i = 0; word[i] != '\0'; i++)
    {
        word_copy[i] = tolower(word[i]);
    }
    // const char endchar = '\0';
    // strncat(word_copy, &endchar, 1);

    int index = hash(word_copy);

    node *cursor = table[index];

    while (cursor != NULL)
    {
        if (strcmp(cursor->word, word_copy) == 0) // and not cursor->word == word
        {
            return true;
        }
        else
        {
            cursor = cursor->next;
        }
    }
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    int length = strlen(word);
    unsigned int index = 0;
    for (int i = 0; i < length; i++)
    {
        index += word[i];
        index = (index * word[i]) % N;
    }
    return index;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // Open dictionary file
    // Remember to check if return value is NULL


    // Before being able to use the hashtable and insert a node in it appears we have to initialize the first N nodes of the array.
    // Otherwise I get a Segmentation fault
    for (int i = 0; i < N; i++)
    {
        table[i] = malloc(sizeof(node));
        if (table[i] == NULL)
        {
            return false;
        }
        strcpy(table[i]->word, "\0");
        table[i]->next = NULL;
    }

    FILE *file = fopen(dictionary, "r");
    if (file == NULL)
    {
        return false;
    }

    char *word = malloc(sizeof(char) * LENGTH + 1);

    if (word == NULL)
    {
        return false;
    }

    // create a node cursor to track where the last node in a specific index is.
    node *cursor = NULL;

    // Read strings from file one at a time
    while (fscanf(file, "%s", word) != EOF)
    {
        // Hash word to obtain a hash value (returns an index)
        int index = hash(word);

        // Create a new node for each word
        // Allocate enough memory using malloc to every node
        node *n = malloc(sizeof(node));

        if (n == NULL)
        {
            return false;
        }

        // Copy word in the node
        strcpy(n->word, word);
        n->next = NULL;

        if (table[index]->next != NULL)
        {
            n->next = table[index]->next;
        }

        table[index]->next = n;
        number_words += 1;
    }

    // Return true if successful or false if unsuccessful
    // not sure how to check yet
    free(word);
    fclose(file);
    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return number_words;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    node *cursor = NULL;
    for (int i = 0; i < N; i++)
    {
        cursor = table[i];
        while (cursor != NULL)
        {
            node *tmp = cursor;
            cursor = cursor->next;
            free(tmp);
        }
    }

    // how to check for the return condition ?
    return true;
}
