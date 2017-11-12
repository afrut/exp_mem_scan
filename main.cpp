#include "toolHelp.h"
#include "CScanner.h"
#include <list>

#define STRLEN 100
#define ADDRESS_CHECK 0x0086B138

using namespace std;

// TODO: create mode: recursive
// TODO: create mode: has value changed
// TODO: create mode: has value not changed
// TODO: test how to tell if two floats are equal

int main()
{
    int valToScan;

    // ask user what value to scan
    printf( "Enter a number to scan for: " );
    scanf_s( "%d", &valToScan );
    
    // create scanner object
    CScanner scanner = CScanner( "Diablo III64.exe", valToScan );

    // start recursive searching
    scanner.recursiveSearch();

    return 0;
}
