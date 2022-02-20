/*
+=============================================================================
|
| To Compile: gcc checksum.c
|
| To Execute: ./a.out textfile.txt checksum_size
| where the files in the command line are in the current directory.
|
| The text file contained text is mixed case with spaces, punctuation,
| and is terminated by the hexadecimal ‘0A’, inclusive.
| (The 0x’0A’ is included in the checksum calculation.)
|
| The checksum_size contains digit(s) expressing the checksum size
| of either 8, 16, or 32 bits
|
+=============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 100
#define MAX_LENGTH 10000

int createBinaryValues(int checksumSize, char *plaintext, int *returnValues)
{
    int i, j, k, ascii, binaryHold[8], endNode = 0, len = strlen(plaintext);

    // Travel down the length of the plaintext and convert each character into a binary value
    for (i = 0; i < len; i++)
    {
        ascii = (int)plaintext[i];

        // Taking the decimal of the ASCII value, determine the binary value, padding with 0s
        for (j = 0; j < 8; j++)
        {
            if (ascii % 2 != 0)
                binaryHold[j] = 1;
            else
                binaryHold[j] = 0;

            ascii /= 2;
        }

        // Insert the acquired binary value in reverse into the original array
        for (k = 7; k >= 0; k--)
            returnValues[endNode++] = binaryHold[k];
    }

    return endNode;
}

void print_plaintext(char *input)
{
    int n = strlen(input);

    for (int i = 0; i < n; i++)
    {
        // Each line should be no more than 80 characters long
        if (i % 80 == 0)
            printf("\n");
        printf("%c%s", input[i], (i + 1 == n) ? "\n" : "");
    }
}

long unsigned int createChecksum(int checksumSize, int *binaryValues, int endNode)
{
    int i = 0, j, shiftByArraySize, shiftByBlockSize, currentIndex, carryOver = 0, tempVal = 0;
    long long unsigned int binToDecVal = 0, baseVal = 1;
    int sumValue[32] = {0};

    // The method of iteration is dependent on the checksum size
    if (checksumSize == 8)
    {
        shiftByArraySize = 7;
        shiftByBlockSize = 8;
    }
    else if (checksumSize == 16)
    {
        shiftByArraySize = 15;
        shiftByBlockSize = 16;
    }
    else if (checksumSize == 32)
    {
        shiftByArraySize = 31;
        shiftByBlockSize = 32;
    }

    // Iterates through the binary values until it hits the end of the array
    while (i != endNode)
    {
        currentIndex = i + shiftByArraySize;
        j = shiftByArraySize;

        // Adds two sets of binary values together
        while (j >= 0)
        {
            tempVal = sumValue[j] + binaryValues[currentIndex--];

            if (carryOver == 1)
            {
                tempVal += carryOver;
                carryOver = 0;
            }

            if (tempVal == 2)
            {
                tempVal = 0;
                carryOver = 1;
            }
            else if (tempVal == 3)
            {
                tempVal = 1;
                carryOver = 1;
            }

            sumValue[j--] = tempVal;
        }

        i += shiftByBlockSize;
        carryOver = 0;
    }

    // Converts the binary value of the checksum into a decimal value
    for (i = shiftByArraySize; i >= 0; i--)
    {
        binToDecVal += sumValue[i] * baseVal;
        baseVal *= 2;
    }

    return binToDecVal;
}

int main(int argc, char **argv)
{
    int checksumSize = atoi(argv[2]), characterCount, endNode;
    int *binaryValues = calloc(MAX_LENGTH, sizeof(int));
    long unsigned int checksum;
    char buffer[MAX_LENGTH];

    FILE *ifp = fopen(argv[1], "r");

    // Ends the program if the user does not input a valid checksum bit-size
    if (checksumSize != 8 && checksumSize != 16 && checksumSize != 32)
    {
        fprintf(stderr, "Valid checksum sizes are 8, 16, or 32\n");
        return 1;
    }

    // Transfers the provided text into plaintext, to be utilized by the program
    fgets(buffer, 10000, (FILE*)ifp);
    characterCount = strlen(buffer);

    // Pad the text with "X" as is necessary
    while(characterCount % (checksumSize / 8) != 0)
    {
        strcat(buffer, "X");
        characterCount++;
    }

    // Generates the length of the combined binary values and the checksum
    endNode = createBinaryValues(checksumSize, buffer, binaryValues);
    checksum = createChecksum(checksumSize, binaryValues, endNode);

    // Prints the plaintext and the converted values
    print_plaintext(buffer);
    printf("%2d bit checksum is %8lx for all %4d chars\n", checksumSize, checksum, characterCount);

    // Cleans up the program
    fclose(ifp);
    free(binaryValues);

    return 0;
}

/*=============================================================================
| I Alexa Sanders-Laird (al406652) affirm that this program is
| entirely my own work and that I have neither developed my code together with
| any another person, nor copied any code from any other person, nor permitted
| my code to be copied or otherwise used by any other person, nor have I
| copied, modified, or otherwise used programs created by others. I acknowledge
| that any violation of the above terms will be treated as academic dishonesty.
+=============================================================================*/
