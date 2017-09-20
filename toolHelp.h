#ifndef _TOOLHELP_H
#define _TOOLHELP_H

#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

BOOL getProcessHandle ( const char* pName
                      , PROCESSENTRY32* pProc);
void listProcessNames();
void listThreadIds( const char* pName );
void listModuleNames( const char* pName );
void listHeapBlocks( const char* pName );

#endif
