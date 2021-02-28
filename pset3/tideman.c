#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            // Here we iterate over the number of candidates available because we need to compare the value of the vote to every candidate.
            // If we have a candidate that matches the name then we update this voters ranking with the index of the candidate.
            // Since candidates have been recorded in order i their index is actually i also.
            // We can therefore associate in our array ranks, this voters ranking "rank" with the candidate index "i".

            // printf("rank = %i, ranks[rank] = %i (%s)\n", rank, i, candidates[i]);
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // We are working on an array which contains in order the personal ranking of a voter.
    // Because we need to compare the first element with the 2nd element and the 3rd element and the nth element
    // But also because we then need to compare the 2nd element with the 3rd element and so on and so forth we need a nested loop (or something like recursivity)
    // Taking A B C D we therefore need to loop n times or actually n-1 times since the last one doesnt matter to record
    // First loop should record A over B, A over C, A over D
    // Second loop should record B over C, B over D
    // Third and last loop should record C over D
    // We have therefore an outer loop of candidate number minus one.
    // And an inner loop that is removing one element every run.
    // One way to do this is to use candidate_count for the outer loop (or candidate_count - 1?). For example above that is 4 runs or 3 runs.
    // And candidate_count - 1 - i for the inner loop. For example above that is 3 pass, 2 pass, 1 pass.

    for (int i = 0; i < candidate_count; i++)
    {
        // Maybe should record a preference of 0 for every [i][i] item since that cannot be a real preference.
        preferences[i][i] = 0;

        for (int j = 0; j < (candidate_count - i - 1); j++)
        {
            // Would need to check if same voter is voting two times for same candidate. but ignored here.
            preferences[ranks[i]][ranks[i + j + 1]]++;

            // printf("references[ranks[i]][ranks[i + 1]]++ with ranks[i] = %i and ranks[i + 1] = %i\n", ranks[i], ranks[i + j + 1]);
            // printf("preferences[%i or %s][%i or %s] is now equal to %i\n\n", ranks[i], candidates[ranks[i]], ranks[i + j + 1], candidates[ranks[i + j + 1]], preferences[ranks[i]][ranks[i + j + 1]]);
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // In the previous function we have graded the preferences of one over the other and stored the value in an array that has the name of one candidate over the other.
    // We should now be able to iterate over the possible combinations and to determine which is the winning one.
    // For example comparing the score of preferences[A][B] and the score of preferences[B][A] to understand if A is the winner and B the loser or the opposite or none.
    // Here we only want to record actual clear preferences of one over the other "edge" and not pairs that are equal.

    // Edges being in only one direction there is no need to compare B to A if we have compared A to B already.
    // Therefore when we reach B since we have already compared to A and comparing to B is useless we only need to compare to C D and E.

    for (int i = 0; i < candidate_count; i++)
    {
        // We need to loop for the number of rows in the array so basically candidate_count - 1 number of rows.
        // But we need to remove gradually the first columns of the array because comparing A to A or B to A when we have checked A to B is useless.
        // For this reason j will be equal to i + 1 because in any case we are skipping the first case : A compared to A.

        for (int j = i + 1; j < candidate_count; j++)
        {
            // First comparison is therefore A(0) to B(1) versus B(1) to A(0).
            // Second comparison in this loop will be A(0) versus C(2) until we reach the end of the candidates.
            // At the second run of the outer loop, i will be 1 so j will be 2. Therefore comparing B to C and not B to A or B to B.
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
                // printf("1st case - pairs[i].winner = %i or %s and pairs[i].loser = %i or %s\n", i, candidates[i], j, candidates[j]);
            }
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
                // printf("2nd case - pairs[i].winner = %i or %s and pairs[i].loser = %i or %s\n", j, candidates[j], i, candidates[i]);
            }
            // nothing if == because its not an edge.
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // Here we have pairs of winner and losers stored in a struct called pairs.
    // Here we want to calculate the difference between the winner and loser value of each pair.
    // And sort the pairs starting with the pair with the highest difference.

    // First trying with bubblesort.
    // First pass is comparing pair in position 0 and in position 1.
    // If position 1 is bigger, swap and increment a swap counter by 1.
    // Then should compare position 1 and position 2.
    // If position 2 is bigger, swap.
    // Then should compare position 2 and position 3.
    // So on and so forth until there are no elements left.
    // When there are no elements left, we can consider that the last element is sorted.
    // At this point we restart the outer loop, reset our swap counter to 0 and start again comparing position 1 and position 2 but our overall array is -1 item.
    // We stop the algorithm when the swap counter is 0.

    int swap_counter = 0;
    int sorted = 0;
    do
    {
        swap_counter = 0;
        for (int i = 0; i < pair_count - sorted - 1; i++)
        {
            // effectively pushing the lower number to the end of the array.
            if ((preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner]) <
                (preferences[pairs[i + 1].winner][pairs[i + 1].loser] - preferences[pairs[i + 1].loser][pairs[i + 1].winner]))
            {
                int swap_winner = pairs[i].winner;
                int swap_loser = pairs[i].loser;
                pairs[i].winner = pairs[i + 1].winner;
                pairs[i].loser = pairs[i + 1].loser;
                pairs[i + 1].winner = swap_winner;
                pairs[i + 1].loser = swap_loser;
                swap_counter++;
            }
        } // this is one pass.
        sorted++;
    }
    while (swap_counter != 0); // hopefully when swap_counter remains 0 at the end of the loop that means the array is sorted already.

    return;

    // Another idea would be to take the first item in pairs and add them to a new array.
    // Then read the second item and if the second item is bigger than the first item then insert it in first position and shift the other one by one to the right.
    // Then read the third item and compare to the first item. If bigger repeat the above.
    // If not, compare with the next element and repeat the above.

    // Another stupid idea would be that we only have a certain number of pairs possible.
    // We could therefore imagine creating an array with all the possible values.
    // Then placing the pairs in their possible positions.
    // Then read the result array and build a third array removing the empty positions.
    // For exemple sorting 8 3 5 1 2 4 would go on like this :
    // create an array with 8 positions because the biggest item is 8
    // position each item at their position na 1 2 3 4 5 na na 8
    // read the array and skip the empty boxes to create a new array : 1 2 3 4 5 8
}

// Lock pairs into the candidate graph in order, without creating cycles

void lock_pairs(void)
{
    // // Print preferences

    // for (int i = 0; i < candidate_count; i++)
    // {
    //     for (int j = 0; j < candidate_count; j++)
    //     {
    //         printf("preferences[%i or %s][%i or %s] = %i\n", i, candidates[i], j, candidates[j], preferences[i][j]);
    //     }
    // }

    // // Print pairs who happen to be sorted

    // for (int i = 0; i < candidate_count; i++)
    // {
    //     printf("pairs[%i or %s].winner = %i or %s\n", i, candidates[i], pairs[i].winner, candidates[pairs[i].winner]);
    //     printf("pairs[%i or %s].loser = %i or %s\n", i, candidates[i], pairs[i].loser, candidates[pairs[i].loser]);
    // }

    for (int i = 0; i < pair_count; i++) // iterating over all pairs.
    {
        // printf("\nTrying to lock the pair %i -> %i.\n", pairs[i].winner, pairs[i].loser);
        int current_winner = pairs[i].winner;
        int current_loser = pairs[i].loser;
        bool cycle_exists = false;
        bool path_exists = false;

        do
        {
            path_exists = false;

            // printf("\n-Beginning the do while loop with pair %i -> %i.\n", current_winner, current_loser);

            for (int j = 0; j < candidate_count; j++) // iterating over every row of the locked array
            {
                // printf("--Now iterating over every row of the locked array.\n");

                // simply to avoid looping when path is found. We restart at the "do" level if only a path exists and no cycle.
                if (path_exists == true)
                {
                    break;
                }
                // simply to avoid looping when cycle exists and we can expect to exit the do loop too.
                if (cycle_exists == true)
                {
                    break;
                }

                for (int k = 0; k < candidate_count; k++) // iterating over every column of the locked array
                {
                    // printf("---Now iterating over every column of the locked array.\n");

                    if (locked[j][k] == true) // we only need to compare with pairs that are already locked in.
                    {
                        // printf("----Found locked[j][k] == true with the following pair %i -> %i.\n", j, k);
                        // printf("----Now comparing pairs[j].loser (%i) with current_winner (%i).\n", k, current_winner);
                        if (k == current_winner) // here we have found a path.
                        {
                            // printf("====Found a path while trying to add %i -> %i. The path goes from %i -> %i then %i -> %i.\n", current_winner, current_loser, current_loser, current_winner, k, j);
                            path_exists = true;
                            if (j == current_loser) // here we have found a cycle
                            {
                                // printf("====Found a cycle while trying to add %i -> %i. The path goes from %i -> %i then %i -> %i.\n", current_winner, current_loser, current_loser, current_winner, k, j);
                                cycle_exists = true;
                                break;
                            }
                            else
                            {
                                current_winner = j;
                                // here since we have found a path but it is not a cycle we reuse the current pairs winner to search again one level up.
                                // printf("====Found a path and no cycle, new pair is now winner %i and loser %i. Path value is %d.\n", current_winner, current_loser, path_exists);
                                break;
                            }
                        }
                    }
                }
            }

            // printf("-Current Pair %i -> %i.\n", current_winner, current_loser);
            // printf("-Reached the end of the do while loop.\n");
        }
        while (path_exists == true && cycle_exists == false);
        // if path is false then cycle cannot be true. therefore path will be false and cycle will be false so the loop will exit.
        // if path exists and cycle does not exist then the condition is met to check again the next node of the path.
        // if path exists or doesnt exist but cycle exists then the loop exits for another reason (there is a cycle) and so we should not add the pair.

        if (cycle_exists == false)
        {
            // printf("Locking pair %i -> %i.\n\n", pairs[i].winner, pairs[i].loser);
            locked[pairs[i].winner][pairs[i].loser] = true;
            // if the previous loop was exited and no cycle exists then we can safely add the pair to the locked array.
        }
        // else
        // printf("Do not lock pair %i -> %i.\n\n", pairs[i].winner, pairs[i].loser);
    }

    // TIDEMAN!
    return;
}

// Print the winner of the election
void print_winner(void)
{
    bool winner_array[candidate_count];
    bool loser_array[candidate_count];

    for (int i = 0; i < candidate_count; i++)
    {
        winner_array[i] = false;
        loser_array[i] = false;
    }

    for (int j = 0; j < candidate_count; j++) // iterating over every row of the locked array
    {
        for (int k = 0; k < candidate_count; k++) // iterating over every column of the locked array
        {
            if (locked[j][k] == true)
            {
                if (loser_array[j] == true) // the winner is already in the losers so we demote it.
                {
                    // printf("locked[j][k] == true && loser_array[j] == true for %i\n", j);
                    winner_array[j] = false;
                }
                else // the winner is not in the losers so lets add it to the winner array.
                {
                    // printf("winner_array[j] = true for %i\n", j);
                    winner_array[j] = true;
                }
                if (winner_array[k] == true) // the current loser is in the winner array so we demote it.
                {
                    // printf("locked[j][k] == true && winner_array[k] == true for %i\n", k);
                    winner_array[k] = false;
                    loser_array[k] = true;
                }
                else // the current loser is not in the winner array so we simply add it to the losers.
                {
                    // printf("loser_array[k] = true for %i\n", k);
                    loser_array[k] = true;
                }
            }
        }
    }

    for (int i = 0; i < candidate_count; i++)
    {
        if (loser_array[i] == true) // making sure there is no value both in losers and winners array.
        {
            winner_array[i] = false;
        }

        else if (winner_array[i] == true)
        {
            printf("%s\n", candidates[i]);
        }

    }

    return;
}
