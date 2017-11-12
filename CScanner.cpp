/******************************************************************************/
/*
/*  Implementation of CScanner class
/*
/******************************************************************************/

#include "CScanner.h"

// constructor
CScanner::CScanner( const char* p_processName, int valToScan ) :
    m_processName( p_processName )
{
    BOOL ret = false;

    // get handle to process
    ret = getProcessEntry( m_processName, &m_processEntry );

    // open the process to scan for its memory
    m_processHandle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, m_processEntry.th32ProcessID );

    /*****************************************************************
     *
     * Initial scan of range of pages
     *
     ****************************************************************/
    char* ptr;
    MEMORY_BASIC_INFORMATION mbi;
    SIZE_T bytesReturned;
    SIZE_T bytesRead;
    int intVal;
    sLoc location;

    // get information for a range of pages and store into structure mbi
    ptr = 0x0;
    bytesReturned = VirtualQueryEx( m_processHandle // handle of process whose memory is to be scanned
                                  , ptr             // pointer to base address of the region of pages
                                  , &mbi            // pointer to return structure
                                  , sizeof(mbi) );  // size of return structure

    do
    {
        // if state of memory in current region of memory is committed
        if( mbi.State == MEM_COMMIT )
        {
            // start loop to read all memory locations within this region
            char* addressToRead = (char*)mbi.BaseAddress;
            while( addressToRead <= (char*)mbi.BaseAddress + mbi.RegionSize )
            {
                // repeat if there are more address to read

                // read the value at address addressToRead
                ret = ReadProcessMemory( m_processHandle    // handle to process
                                       , addressToRead      // address whose value is to be read
                                       , &intVal            // buffer to return value to
                                       , sizeof( intVal )   // size of return buffer
                                       , &bytesRead );      // number of bytes transferred to buffer

                // store in the location structure
                location.ptr = addressToRead;
                location.val = intVal;

                // check for match
                if( intVal == valToScan )
                {
                    // add this location to a list
                    m_address.push_back( location );
                }

                // increment
                addressToRead = addressToRead + sizeof( intVal );
            }
        }

        // get information on the next range of pages
        ptr += mbi.RegionSize;
        bytesReturned = VirtualQueryEx( m_processHandle // handle of process whose memory is to be scanned
                                      , ptr             // pointer to base address of the region of pages
                                      , &mbi            // pointer to return structure
                                      , sizeof(mbi) );  // size of return structure

        // repeat until bytes returned is less than expected
    } while( bytesReturned == sizeof( mbi ) );

    printf( "Number of addresses in initial scan = %d\n", m_address.size() );
}

// destructor
CScanner::~CScanner()
{
    // close process handle
    CloseHandle( m_processHandle );
}

// recursive search
void CScanner::recursiveSearch()
{
    int valToScan;      // value to scan for in memory
    int intVal;         // buffer to store the value scanned from memory as int
    SIZE_T bytesRead;   // number of bytes read by ReadProcessMemory
    BOOL ret;           // return for ReadProcessMemory
    list<sLoc>::iterator lit;

    // create a copy of the initial scan results
    m_recursive = m_address;

    do
    {
        // request input from user
        printf( "Enter a number to scan for: " );
        scanf_s( "%d", &valToScan );

        // loop through the recursive vector
        lit = m_recursive.begin();
        do
        {
            // read the value at the addresses stored in the list as int
            ret = ReadProcessMemory( m_processHandle
                                   , lit->ptr
                                   , &intVal
                                   , sizeof( intVal )
                                   , &bytesRead );

            // if not a match
            if( intVal != valToScan )
            {
                // remove from list and increment iterator
                m_recursive.erase( lit++ );
            }
            else
            {
                // increment iterator
                lit++;
            }

        } while( lit != m_recursive.end() );

        printf( "------------------------------------------------------------\n" );
        printf( "Remaining matches: %d\n", m_recursive.size() );
        if( m_recursive.size() <= 20 )
        {
            lit = m_recursive.begin();

            // print remaining addresses
            do
            {
                printf( "%p\n", lit->ptr );
                lit++;
            } while( lit != m_recursive.end() );
        }

        // for human viewing of results
        getchar();

    } while( m_recursive.size() > 1 );
}

// get entry of process
BOOL CScanner::getProcessEntry ( const char* pName, PROCESSENTRY32* pProc)
{
    BOOL ret = false;
    HANDLE hSnap;
    PROCESSENTRY32 proc;

    // create a snapshot to include all processes in the system
    hSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

    // initialize the PROCESSENTRY32 structure
    proc.dwSize = sizeof(PROCESSENTRY32);

    // get the first process in the snapshot and print the name
    Process32First( hSnap, &proc );

    do
    {
        // check for a specific process by name
        if( strcmp( proc.szExeFile, pName ) == 0 )
        {
            // process was found, return this process handle
            *pProc = proc;
            ret = true;
            break;
        }

    // get next process in the snapshot
    } while( Process32Next(hSnap, &proc) );

    return ret;
}

// overload of getProcessEntry
BOOL CScanner::getProcessEntry ( string pName, PROCESSENTRY32* pProc)
{
    BOOL ret = false;
    HANDLE hSnap;
    PROCESSENTRY32 proc;

    // create a snapshot to include all processes in the system
    hSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

    // initialize the PROCESSENTRY32 structure
    proc.dwSize = sizeof(PROCESSENTRY32);

    // get the first process in the snapshot and print the name
    Process32First( hSnap, &proc );

    do
    {
        // check for a specific process by name
        if( strcmp( proc.szExeFile, pName.c_str() ) == 0 )
        {
            // process was found, return this process handle
            *pProc = proc;
            ret = true;
            break;
        }

    // get next process in the snapshot
    } while( Process32Next(hSnap, &proc) );

    return ret;
}
