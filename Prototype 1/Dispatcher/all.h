/*-------------------------------------------------------------- %BMH% --
| File Name: all.h - Common definintion file, not application specific
|
| Description:
|  This file contains information and defintions that are common to all
|  modules in the system.  These items are NOT application specific.
|  They are primitive level items that would be generic to any application.
|
|     SCN       Date    Who      Description
| -----------  ------  -----     -------------------------------------------
| AAA30xxxAAA  960220  BA/SH     Initial Creation.
|
| AAA30xxxAAA  960931  BA/SH/MR  Made modifications for use with HPLIM,
|                                Titan OCSS, and Titan GCSS.
|
|  Copyright (C) 1996 Otis Elevator. All Rights Reserved.
|
|  AAB-30301-1AA    Toshimitsu Mori                  3-Feb-2000
|  - CHG 30301-103  Replace "INT_T" to "LONG_T" and replace "INT_S" to "LONG_S"
|                   to standardize MSC and MT68K.
---------------------------------------------------------------- %EMH% --*/
#ifndef all_DEFINED             // for multiple includes
/*-----------------------------------------------------------------------*/
#include "switch.h"             // project specific switches

/*-----------------------------------------------------------------------
|  Standard Boolean Type (use for new code)
-------------------------------------------------------------------------*/
#ifdef GCS
typedef unsigned char   BOOL;
#define TRUE            0xff    // true MUST be FF so NOT TRUE is equal to FALSE
#define FALSE		    0

#define ON              TRUE
#define OFF             FALSE
#endif

/*-----------------------------------------------------------------------
|  Standard Type Definitions (use for new code)
-------------------------------------------------------------------------*/
typedef unsigned char   BYTE;           // 1 byte
typedef unsigned short  WORD;           // 2 bytes
typedef unsigned long   DWORD;          // 4 bytes

#ifdef GCS
typedef signed   char   CHAR;           // 1 byte
#endif

typedef unsigned char   UCHAR;          // 1 byte

typedef signed   short  SHORT;          // 2 bytes
typedef unsigned short  USHORT;         // 2 bytes

typedef signed   long   LONG;           // 4 bytes
typedef unsigned long   ULONG;          // 4 bytes

typedef float           FLOAT;          // 4 bytes

typedef double          DOUBLE;         // 8 bytes

/*-----------------------------------------------------------------------
|  Operator Definitions
-------------------------------------------------------------------------*/
#define BITAND      &
#define BITOR       |
#define BITEXOR     ^
#define AND         &&
#define OR          ||
#define INV         ~
#define NOT         !
#define EQ          ==

/*-----------------------------------------------------------------------
|  Boolean Type - Backward compatibility (do not use for new code)
-------------------------------------------------------------------------*/
#define FLAG_T          BOOL
#define all_FlagT       BOOL
#define all_BooleanT    BOOL
#define all_True        TRUE
#define all_False       FALSE
#define all_On          TRUE
#define all_Off         FALSE
#define YES             TRUE
#define NO              FALSE


/*-----------------------------------------------------------------------
|  Type definitions - Backward compatibility (do not use for new code)
-------------------------------------------------------------------------*/
#define all_ByteT       BYTE
#define all_WordT       WORD
#define INT             SHORT
typedef unsigned int    UINT;

#define BYTE_T          BYTE            // 1 byte  unsigned
#define BYTE_S          CHAR            // 1 byte  unsigned
#define WORD_T          WORD            // 2 bytes unsigned
typedef short           WORD_S;         // 2 bytes signed
#define REG_LONG_T       short           // 2 bytes signed
#define FLOAT_T         FLOAT           // 4 bytes
#define DOUBLE_T        DOUBLE          // 8 bytes

#ifdef  MSC
  typedef unsigned long   LONG_T;     // 4 (motorola) bytes unsigned
  typedef          long   LONG_S;     // 4 (motorola) bytes unsigned
#else
 #ifdef MT68K_C
   typedef unsigned long   LONG_T;     // 4 (motorola) bytes unsigned
   typedef long            LONG_S;     // 4 bytes signed
 #else
   #ifdef DCC68K
     typedef unsigned long   LONG_T;     // 4 (motorola) bytes unsigned
     typedef long            LONG_S;     // 4 bytes signed
   #else
     typedef unsigned long   LONG_T;      // 4 bytes unsigned
     typedef long            LONG_S;      // 4 bytes signed
     typedef unsigned int    LONG_T;     // 2 (intel) 4 (motorola) bytes unsigned
     typedef          int    LONG_S;     // 2 (intel) 4 (motorola) bytes unsigned
   #endif //DCC68K
 #endif //MT68K
#endif //MSC
//=======================================================================
// COMPILER DEPENDENT SECTION
//=======================================================================

/*-----------------------------------------------------------------------
|  Define a ROM Constant
|  ex.  unsigned all_RomT romStr = "I'm stored in ROM!";
-------------------------------------------------------------------------*/
#ifdef MSC
  #define ROM  far  /* far data is located in ROM space */
#endif

/*-----------------------------------------------------------------------
|  Inline Assembly
|
|  Define the way in-line assembly is defined.
|  This is used when porting from one compiler to another so this
|  none ANSI definition only has to be changed in one place.
|  Compiler Definitions
-------------------------------------------------------------------------*/
#ifdef MSC
  #define ASM_BEG  _asm {     /* start in-line assembly */
  #define ASM_END  }          /* end in-line assembly */
#endif

/*-----------------------------------------------------------------------
|  Inline Assembly - Backward compatibility (do not use for new code)
-------------------------------------------------------------------------*/
#ifdef MSC
  #define all_AsmS  _asm {     /* start in-line assembly */
  #define all_AsmE  }          /* end in-line assembly */
#endif

/*-----------------------------------------------------------------------
|  Compiler Memory Reference
|
|  Define the way the compiler references memory types.
|  This is used when porting from one compiler to another so these none
|  ANSI definitions only have to be changed in one place.
-------------------------------------------------------------------------*/
#ifdef MSC
  #define NEAR near
  #define FAR far
  #define INTERRUPT interrupt far
  #define NULL_INTERRUPT  ((void (INTERRUPT *) (void)) 0)
#endif
#ifdef MT68K_C
  #define NEAR_T
  #define FAR_T
  #define INTERRUPT_T
  #define INTERRUPT_KEY
#endif

#ifdef DCC68K
  #ifndef NEAR_T
    #define NEAR_T
  #endif
  #ifndef FAR_T
    #define FAR_T
  #endif
  #ifndef INTERRUPT_T
    #define INTERRUPT_T
  #endif
  #ifndef INTERRUPT_KEY
    #define INTERRUPT_KEY
  #endif
#endif

/*-----------------------------------------------------------------------
|  Compiler Memory Reference -
|  Backward compatibility (do not use for new code)
-------------------------------------------------------------------------*/
#ifdef MSC
  #define NEAR_T near
  #define FAR_T far
  #define INTERRUPT_T interrupt far
  #define INTERRUPT_KEY

  #define all_NearT near
  #define all_FarT far
  #define all_IntT _cdecl interrupt far
  #define all_NullInt  ((void (all_IntT *) (void)) 0)
#endif

/*-----------------------------------------------------------------------
|  Generic Macros
-------------------------------------------------------------------------*/
#ifdef MSC
  //#define LOBYTE(w)     ((BYTE)(w))
  //#define HIBYTE(w)           ((BYTE)(((UINT)(w) >> 8) & 0xFF))
  //#define LOWORD(l)           ((WORD)(DWORD)(l))
  //#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))
  //#define MAKELONG(low, high) ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))
#endif

/*---------------------------------------------------------------------------%BSH%----
| Compiler Macros
| min - Returns the smaller of the two evaluated parameters.
| max - Returns the larger of the two evaluated parameters.
|----------------------------------------------------------------------------%ESH%---*/
#ifdef MT68K_C
  #define min(a,b) ( (a) < (b) ? (a) : (b) )
  #define max(a,b) ( (a) < (b) ? (b) : (a) )
#endif

#ifdef MSC
  #ifndef min
    #define min(a,b) ( (a) < (b) ? (a) : (b) )
  #endif
  #ifndef max
    #define max(a,b) ( (a) < (b) ? (b) : (a) )
  #endif
#endif

#ifdef DCC68K
  #ifndef min
    #define min(a,b) ( (a) < (b) ? (a) : (b) )
  #endif
  #ifndef max
    #define max(a,b) ( (a) < (b) ? (b) : (a) )
  #endif
#endif

/*-----------------------------------------------------------------------
|  Memory Model
|
|  Define the current memory model being used.
|  This is used to accomadate any memory model dependent code.
|  Compiler Definitions
-------------------------------------------------------------------------*/
#ifdef all_MSC
  // #define all_SmMemory
  #define all_MedMemory
#endif

/*-----------------------------------------------------------------------*/
#define all_DEFINED
#endif
