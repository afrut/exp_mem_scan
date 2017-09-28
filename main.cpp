#include "toolHelp.h"
#include <list>

#define STRLEN 100
#define ADDRESS_CHECK 0x0086B138

// MEMORY_BASIC_INFORMATION.AllocationProtect constants
#define PAGE_EXECUTE 0x10
#define PAGE_EXECUTE_READ 0x20
#define PAGE_EXECUTE_READWRITE 0x40
#define PAGE_EXECUTE_WRITECOPY 0x80
#define PAGE_NOACCESS 0x01
#define PAGE_READONLY 0x02
#define PAGE_READWRITE 0x04
#define PAGE_WRITECOPY 0x08
#define PAGE_TARGETS_INVALID 0x40000000
#define PAGE_GUARD 0x100
#define PAGE_NOCACHE 0x200
#define PAGE_WRITECOMBINE 0x400

// MEMORY_BASIC_INFORMATION.State constants
#define MEM_COMMIT 0x1000
#define MEM_FREE 0x10000
#define MEM_RESERVE 0x2000

// MEMORY_BASIC_INFORMATION.Type constants
#define MEM_IMAGE 0x1000000
#define MEM_FREE 0x40000
#define MEM_RESERVE 0x20000

using namespace std;

int main()
{
    PROCESSENTRY32 proc;
    HANDLE hProc;
    BOOL ret = false;
    SIZE_T size;
    MEMORY_BASIC_INFORMATION mbi;
    char* ptr = NULL;
    int cnt = 0;
    char strAllocationProtect[STRLEN];
    char strState[STRLEN];
    char strType[STRLEN];
    int memVal;
    SIZE_T bytesRead;
    char* addressToRead;
    int valToScan;
    std::list<char*> ls;
    list<char*>::iterator lsIter;
    char* addressToCheck = (char*)ADDRESS_CHECK;

    // prompt the user for a value to scan
    printf("Enter an integer to scan for: ");
    scanf_s("%d",&valToScan);

    // get the process ID
    ret = getProcessHandle( "whileLoop.exe", &proc );

    // get the process handle from process ID
    hProc = OpenProcess( PROCESS_ALL_ACCESS, FALSE, proc.th32ProcessID );

    // get information for a range of pages and store into mbi
    size = VirtualQueryEx( hProc, ptr, &mbi, sizeof(mbi) );

    // initial scan of the range of pages
    do
    {
        switch(mbi.AllocationProtect)
        {
            case PAGE_EXECUTE: strcpy_s( strAllocationProtect, STRLEN, "PAGE_EXECUTE" );
                break;
            case PAGE_EXECUTE_READ: strcpy_s( strAllocationProtect, STRLEN, "PAGE_EXECUTE_READ" );
                break;
            case PAGE_EXECUTE_READWRITE: strcpy_s( strAllocationProtect, STRLEN, "PAGE_EXECUTE_READWRITE" );
                break;
            case PAGE_EXECUTE_WRITECOPY: strcpy_s( strAllocationProtect, STRLEN, "PAGE_EXECUTE_WRITECOPY" );
                break;
            case PAGE_NOACCESS: strcpy_s( strAllocationProtect, STRLEN, "PAGE_NOACCESS" );
                break;
            case PAGE_READONLY: strcpy_s( strAllocationProtect, STRLEN, "PAGE_READONLY" );
                break;
            case PAGE_READWRITE: strcpy_s( strAllocationProtect, STRLEN, "PAGE_READWRITE" );
                break;
            case PAGE_WRITECOPY: strcpy_s( strAllocationProtect, STRLEN, "PAGE_WRITECOPY" );
                break;
            case PAGE_TARGETS_INVALID: strcpy_s( strAllocationProtect, STRLEN, "PAGE_TARGETS_INVALID/PAGE_TARGETS_NO_UPDATE" );
                break;
            case PAGE_GUARD: strcpy_s( strAllocationProtect, STRLEN, "PAGE_GUARD" );
                break;
            case PAGE_NOCACHE: strcpy_s( strAllocationProtect, STRLEN, "PAGE_NOCACHE" );
                break;
            case PAGE_WRITECOMBINE: strcpy_s( strAllocationProtect, STRLEN, "PAGE_WRITECOMBINE" );
                break;
            default: strcpy_s( strAllocationProtect, STRLEN, "DEFAULT" );
                break;
        }

        switch( mbi.State )
        {
            case MEM_COMMIT: strcpy_s( strState, "MEM_COMMIT" );
                break;
            case MEM_FREE: strcpy_s( strState, "MEM_FREE" );
                break;
            case MEM_RESERVE: strcpy_s( strState, "MEM_RESERVE" );
                break;
            default: strcpy_s( strState, "DEFAULT" );
                break;
        }

        switch( mbi.Type )
        {
            case MEM_IMAGE: strcpy_s( strType, "MEM_IMAGE" );
                break;
            case MEM_MAPPED: strcpy_s( strType, "MEM_MAPPED" );
                break;
            case MEM_PRIVATE: strcpy_s( strType, "MEM_PRIVATE" );
                break;
            default: strcpy_s( strType, "DEFAULT" );
                break;
        }

        if( mbi.State == MEM_COMMIT )
        {
            // start loop to read all memory locations within this page
            addressToRead = (char*)mbi.BaseAddress;
            while( addressToRead <= (char*)mbi.BaseAddress + mbi.RegionSize )
            {
                ret = ReadProcessMemory( hProc
                                       , addressToRead
                                       , &memVal
                                       , sizeof( memVal )
                                       , &bytesRead );

                // store the address read if it matches the value to scan
                if( memVal == valToScan )
                {
                    ls.push_back( addressToRead );
                    cnt++;
                }

                // increment the address to read
                addressToRead = addressToRead + sizeof( memVal );
            }
        }

        // increment the pointer
        ptr += mbi.RegionSize;
        size = VirtualQueryEx( hProc, ptr, &mbi, sizeof(mbi) );

    // loop if the call to VirtualQueryEx succeeds
    } while( size == sizeof(mbi) );

    printf( "Addresses returned: %d\n", cnt );

    // successive scans of addresses matched at least once
    do
    {
        printf( "------------------------------------------\n" );
        // prompt the user for a value to scan again
        printf("Enter an integer to scan for: ");
        scanf_s("%d",&valToScan);


        // loop through remaining addresses in the list
        cnt = 0;
        lsIter = ls.begin();
        do
        {
            // read the value at the addresses stored in the list
            ret = ReadProcessMemory( hProc
                                   , *lsIter
                                   , &memVal
                                   , sizeof( memVal )
                                   , &bytesRead );

            if( memVal == valToScan )
            {
                // value matches the value to scan for
                cnt++;
                lsIter++;
            }
            else
            {
                // remove the element since value does not match
                lsIter = ls.erase( lsIter );
            }
        } while( lsIter != ls.end() );

        // print results and pause
        printf( "Addresses returned: %d\n", ls.size() );
        getchar();

    } while( ls.size() > 1 );

    ret = CloseHandle( hProc );

    getchar();
}
