#include "toolHelp.h"

/******************************************************************************
 *                                                                             
 * Return a process handle by specifying a name
 *
 ******************************************************************************/
BOOL getProcessHandle ( const char* pName, PROCESSENTRY32* pProc)
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

/******************************************************************************
 *                                                                             
 * List all process names to console output
 *
 ******************************************************************************/
void listProcessNames() 
{
    BOOL ret = false;
    HANDLE hSnap;
    PROCESSENTRY32 proc;

    // create a snapshot to include all processes in the system
    hSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

    // initialize the PROCESSENTRY32 structure
    proc.dwSize = sizeof(PROCESSENTRY32);

    // get the first process in the snapshot and print the name
    ret = Process32First( hSnap, &proc );

    do
    {
        printf( "%s\n", proc.szExeFile );

    // get next process in the snapshot
    } while( Process32Next(hSnap, &proc) );
}

/******************************************************************************
 *                                                                             
 * List all the thread id's created by the specified process
 *
 ******************************************************************************/
void listThreadIds( const char* pName )
{
    BOOL ret = false;
    HANDLE hSnap;
    PROCESSENTRY32 proc;
    THREADENTRY32 thd;
    int cnt;

    BOOL processFound = getProcessHandle( pName, &proc );

    // initialize the THREADENTRY32 structure
    thd.dwSize = sizeof(THREADENTRY32);

    // create a snapshot to include all processes in the system
    hSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, proc.th32ProcessID );

    // get the first thread
    ret = Thread32First( hSnap, &thd );

    cnt = 0;
    do
    {
        if( thd.th32OwnerProcessID == proc.th32ProcessID )
        {
            cnt++;
            printf( "\t%s:%d\n"
                  , proc.szExeFile
                  , thd.th32ThreadID );
            if( cnt == proc.cntThreads )
            {
                printf( "\tNumber of threads: %d\n", proc.cntThreads );
                break;
            }
        }

    // get next thread in the snapshot
    } while( Thread32Next(hSnap, &thd) );
}

/******************************************************************************
 *                                                                             
 * List all modules of a process specified by name
 *
 ******************************************************************************/
void listModuleNames( const char* pName )
{
    BOOL ret = false;
    HANDLE hSnap;
    PROCESSENTRY32 proc;
    MODULEENTRY32 mdl;
    int cnt;

    BOOL processFound = getProcessHandle( pName, &proc );

    // initialize the MODULEENTRY32 structure
    mdl.dwSize = sizeof(MODULEENTRY32);

    // create a snapshot to include all processes in the system
    hSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, proc.th32ProcessID );

    // get the first module
    ret = Module32First( hSnap, &mdl );

    cnt = 0;
    do
    {
        cnt++;
        printf( "\t%s:%s\n"
              , proc.szExeFile
              , mdl.szModule );

    // get next module in the snapshot
    } while( Module32Next(hSnap, &mdl) );

    printf( "\tNumber of modules: %d\n", cnt );
}

/******************************************************************************
 *                                                                             
 * List all heaps of a process specified by name
 *
 ******************************************************************************/
void listHeapBlocks( const char* pName )
{
    BOOL ret = false;
    HANDLE hSnap;
    PROCESSENTRY32 proc;
    HEAPLIST32 hplist;
    HEAPENTRY32 hp;
    int cnt;
    int cntHpBlk;

    BOOL processFound = getProcessHandle( pName, &proc );

    // initialize the HEAPLIST32 structure
    hplist.dwSize = sizeof(HEAPLIST32);
    hp.dwSize = sizeof(HEAPENTRY32);

    // create a snapshot to include all processes in the system
    hSnap = CreateToolhelp32Snapshot( TH32CS_SNAPHEAPLIST, proc.th32ProcessID );

    // get the first thread
    ret = Heap32ListFirst( hSnap, &hplist );

    cnt = 0;
    do
    {
        cnt++;
        printf( "\t%s:%d\n"
              , proc.szExeFile
              , hplist.th32HeapID );

        ret = Heap32First( &hp, proc.th32ProcessID, hplist.th32HeapID );
        cntHpBlk = 0;
        do
        {
            cntHpBlk++;
            printf( "\t\t%s:%d:0x%16x\n"
                  , proc.szExeFile
                  , hplist.th32HeapID
                  , hp.dwAddress );
        } while( Heap32Next(&hp) );

        printf("\t\tNumber of heap blocks: %d\n", cntHpBlk );


    // get next module in the snapshot
    } while( Heap32ListNext(hSnap, &hplist) );

    printf( "\tNumber of heaps: %d\n", cnt );
}
