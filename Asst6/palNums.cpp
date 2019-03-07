#include <mutex>
#include <chrono>
#include <iostream>
#include <thread>
#include <string>
#include <string.h>
using namespace std;

static unsigned long long MIN_THREAD_COUNT = 1;
static unsigned long long MAX_THREAD_COUNT = 64;
static unsigned long long MIN_LIMIT = 10;
static unsigned long long MAX_LIMIT = 100000000000;
static unsigned long long PAL_STEP = 10000;

bool getArguments(int, char*[], unsigned long long&, unsigned long long&);
unsigned long long getBlockNum();   // Have the counter in here. Lock it with mutex.
void calcPalindrome(int);           // Calculate palindrome and update the palindrome index.
                                    // The passed in parameter is the array index the thread function will update.


mutex mutexVariable;
int *PAL_COUNT_ARRAY = nullptr;       // Global dynamic array used to keep track of number of palindromes.
unsigned long long CNTR = 1;

int main(int argc, char *argv[]){

    unsigned long long chosenThreadCnt = 0;
    unsigned long long chosenLimit = 0;

    if(!getArguments(argc, argv, chosenThreadCnt, chosenLimit))
        exit(1);

    // Allocate array size corresponding to the number of threads being used.
    // Each thread uses one index to keep track of number of palindromes.
    PAL_COUNT_ARRAY = new int[chosenThreadCnt];
    thread *palThread = new thread[chosenThreadCnt];

    while(CNTR < chosenLimit){
        
    cout << "Count: " << CNTR << endl;

    // Create a pool of threads.
    for(int i = 0; i < int(chosenThreadCnt); ++i)
        palThread[i] = thread(calcPalindrome, i);


    // Join all the threads once all the calculations are complete.
    for(int j = 0; j < int(chosenThreadCnt); ++j)
        palThread[j].join();

    

    }// end while()

    // Add up all the palindromes
    int sum = 0;
    for(int i = 0; i < int(chosenThreadCnt); ++i){
        sum += PAL_COUNT_ARRAY[i];
    }

    cout << "total # of palindrome: " << sum << endl;
    
    delete[] palThread;
    delete[] PAL_COUNT_ARRAY;

    return 0;
}

// Give out numbers to the threads to check for palindromes. Use mutex to lock.
unsigned long long getBlockNum(){

    // Prevent race condition
    lock_guard<mutex> guard(mutexVariable);
    
    return CNTR;
}

void calcPalindrome(int palCntArrIndex){

    unsigned long long blockNum = getBlockNum();        // A block of 10,000 incremented number is assigned to
                                                        // check for palindromes.

    CNTR += PAL_STEP;                               // Increment the count by 10,000. Each thread increments
                                                    // the counter after they receive their blockNum.


    unsigned long long num = 0, digit = 0, rev = 0;

    // Check for palindromes in the block of numbers.
    for(unsigned long long i = blockNum; i < blockNum + PAL_STEP; ++i){

        num = i;
        rev = 0;

        // Check for palindrome.
        do{
            digit = num % 10;
            rev = (rev * 10) + digit;
            num /= 10;

        }while(num != 0);

        // If the number is a palindrome, increment the index assigned to the thread.
        if(i == rev)
            PAL_COUNT_ARRAY[palCntArrIndex]++;
    }
}


// Looks at the main arugments passed in and check them for validity.
bool getArguments(int argc, char *argv[], unsigned long long &threadCnt, unsigned long long &limitVal){

    if(argc == 1){
        cout << "Usage: ./palNums -t <threadCount> -l <limitValue>" << endl;
        return false;
    }

    if(argc != 5){
        cout << "Invalid number of arguments" << endl;
        return false;
    }

    if(string(argv[1]) != "-t"){
        cout << "missing -t(thread) indicator" << endl;
        return false;
    }

    // Convert the string value into unsigned long long
    string::size_type tsz = 0;
    threadCnt = stoull(argv[2], &tsz, 10);

    if (threadCnt < MIN_THREAD_COUNT || threadCnt > MAX_THREAD_COUNT){
        cout << "invalid thread count (" << threadCnt << ")" << endl;
        return false;
    }

    if(string(argv[3]) != "-l"){
        cout << "missing -l(limit) indicator" << endl;
        return false;
    }

    // Convert the string value into unsinged long long
    string::size_type lsz = 0;
    limitVal = stoull(argv[4], &lsz, 10);

    if(limitVal  < MIN_LIMIT || limitVal > MAX_LIMIT){
        cout << "invalid number limit (" << limitVal << ")" << endl;
        return false;
    }

    return true;
}