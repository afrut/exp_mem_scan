/******************************************************************************/
/*
/*  This class performs memory scanning of a target process
/*
/******************************************************************************/

using namespace std;

#ifndef __CSCANNER
#define __SCANNER

#include <stdio.h>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <list>

// location structure
struct sLoc
{
    char* ptr; // address
    int   val; // value at address
};

class CScanner
{
    // constructors
    public:
        CScanner( const char* name, int valToScan );// process name

    // destructor
    public:
        virtual ~CScanner();
    
    // members
    private:
        string m_processName;           // name of the process
        PROCESSENTRY32 m_processEntry;  // information structure about the process
        HANDLE m_processHandle;         // handle to the process
		list<sLoc> m_address;           // initial scan of memory addresses that are committed
        list<sLoc> m_recursive;         // vector used in recursive searching

    // scanning modes
    public:
        void recursiveSearch();

    // internal functions
    private:
        BOOL getProcessEntry( const char* pName, PROCESSENTRY32* pProc );
        BOOL getProcessEntry( string pName, PROCESSENTRY32* pProc );
};

#endif
