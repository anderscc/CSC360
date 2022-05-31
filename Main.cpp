*******************
//
// Includes and Defines
//
//*******************
#include <stdio.h>
#include <fstream>
#include "Matrix.h"

                  using namespace std;

//*******************
//
// Function Declarations
//
//*******************
// Check if in safe state
bool inSafeState(Matrix &, Matrix &, Matrix &, int);

//*******************
//
// Main Function
// Takes an agrument as filename path
//
//*******************
int main(int argc, char *argv[])
{
    // Number of arguments
    if (argc != 2)
    {
        printf("Error: MUST take one and only one argument as input file path\n");
        return -1;
    }

    ifstream infile;          // file
    int row, col, processNum; // # of processes, # of resources, process # of request

    // Starting to read data from file................................
    infile.open(argv[1]); // Open file
    // Check error opening file
    if (infile.fail())
    {
        perror(argv[1]);
        return -1;
    }

    infile >> row; // number of processes
    infile >> col; // number of resources

    // nxm matrices
    Matrix allocation(row, col), max(row, col), mrequest(row, col), available(1, col), request(1, col);

    //Read data into matrices and vectors
    allocation.read(infile);
    max.read(infile);
    available.read(infile);
    infile >> processNum;
    infile.ignore(255, ':');
    request.read(infile);
    infile.close(); // Closing file
    Matrix need = (max - allocation);
    mrequest.setToZeroExcept(processNum, request);

    // Print the report.....................................
    printf("\nTHE REPORT STARTS HERE.........................\n\n");
    // The static data from input
    printf("There are %d processes in the system.\n\n", row);
    printf("There are %d resource types.\n\n", col);
    // Matrices
    printf("The Allocation Matrix is...\n");
    allocation.print(0);
    printf("\nThe Max Matrix is...\n");
    max.print(0);
    printf("\nThe Need Matrix is...\n");
    need.print(0);
    // Available vector
    printf("\nThe Available Vector is...\n");
    //allocation.printResource();
    available.print(-1);

    // print status of current state
    if (inSafeState(need, allocation, available, row))
        printf("\nTHE SYSTEM IS IN A SAFE STATE!\n");
    else
        printf("\nTHE SYSTEM IS NOT SAFE!\n");

    // Request Vector
    printf("\nThe Request Vector is...\n");
    //allocation.printResource();
    //printf("%*d: ", 3, processNum);
    request.print(processNum);

    // determine if the request is granted
    if (mrequest <= need)
    {

        if (request <= available)
        {
            need -= mrequest;
            available -= request;
            allocation += mrequest;
            if (inSafeState(need, allocation, available, row))
            {
                printf("3\n");
                printf("\nTHE REQUEST CAN BE GRANTED!\n");
                // calculate and print new available vector
                printf("\nThe New Available Vector is...\n");
                available.print(-1);
            }
            else
                printf("\nTHE REQUEST CANNOT BE GRANTED!\nTHE SYSTEM IS NOT IN SAFE STATE IF REQUEST IS GRANTED!\n");
        }
        else
            printf("\nTHE REQUEST CANNOT BE GRANTED!\nTHE PROCESS MUST WAIT SINCE RESOURCES ARE NOT AVAILABLE!\n");
    }
    else
        printf("\nTHE REQUEST CANNOT BE GRANTED!\nTHE PROCESS HAS EXCEEDED ITS MAXIMUM CLAIM!\n");

    printf("\nEND REPORT......................................\n\n");

    return 0;
}

//************************
//
// inSafeState Function
//
// Check if system in safe state
//
//***********************
bool inSafeState(Matrix &need, Matrix &allocation, Matrix &available, int row)
{
    // Initialize variables
    Matrix work = available; // Work = Available
    bool finished[row];      // Finished = {False}
    for (int i = 0; i < row; i++)
        finished[i] = false;
    int last = 0;      // quantitizer
    int current = row; // quantitizer
    // Loop starts to determine if the current state safe
    while (last != current)
    {
        last = current; // update undone
        for (int i = 0; i < row; i++)
        {
            // looking for undone process and has need </= work
            if (need.at(i) <= work && !finished[i])
            {
                current--;                // update update
                finished[i] = true;       // set process to finished
                work += allocation.at(i); // update work
            }
        }
    }
    if (current == 0)
        return true;
    else
        return false;
}
