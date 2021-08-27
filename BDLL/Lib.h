/* U10 Project: @(#)lib.h	1.1	3/23/95	14:22:55 */
/**************************************************************************
;  File Name: LIB.H - Public Interface for LIB.C
; 
;  File Description:
;  This file describes the LIB.C public definitions.  This file is to be
;  included in any file that uses these services.   See the LIB.C file
;  for detailed usage information.
; 
; 
;  This work and the information it contains are the confidential
;  property of Otis Elevator Company ("Otis").  It is delivered on
;  the express condition that it will be used only for, or on
;  behalf of, Otis; that neither it nor the information it contains
;  will be reproduced, distributed or disclosed, in whole or in part,
;  without the written consent of Otis; and that on demand it and any
;  copies will be promptly returned.
;
;  Unpublished work - Copyright 1995 Otis Elevator Company
;  All rights reserved.
;
;
;  Revision History:
;
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Created (renamed) module from "lib_p.h".
;
;*************************************************************************/
#ifndef LIB_INCLUDED  /* protects from multiple includes */
#define LIB_INCLUDED

/**************************************************************************
;  Constants
;*************************************************************************/
/* None */


/**************************************************************************
;  Data Types
;*************************************************************************/
/* None */


/**************************************************************************
;  Variables
;*************************************************************************/
/* None */


/**************************************************************************
;  Function Prototypes
;*************************************************************************/
void Lib_Memcpy(BYTE_T *, BYTE_T *, LONG_T);
void Lib_SRbyte(BYTE_T *, INT_T);
void Lib_Memset(BYTE_T *, BYTE_T, INT_T);
BYTE_T *Lib_Memsrch(BYTE_T *, BYTE_T, INT_T);
BYTE_T *Lib_Memrsrch(BYTE_T *, BYTE_T, INT_T);
#ifndef U10
void Lib_Outp(BYTE_T, BYTE_T);
BYTE_T Lib_Inp(BYTE_T);
void Lib_Int_Vector(INT_T, void (INTERRUPT_T *) (void));
#endif 

/**************************************************************************
;  Macro Functions  (In-Line Code)
;*************************************************************************/

/**************************************************************************
;  Name: Lib_BP_Parm1_IL - Adjust Base Pointer (BP) to First Parameter
; 
;  Description:
;  Use at the begining of a C routine that uses in-line assembly to access
;  the parameters passed into the routine.  Once this is done the
;  parameters can ONLY be accesed by assembly.
; 
;  The BP Restore function MUST be used once the references to the
;  BP register have been completed.
;*************************************************************************/
#ifndef U10
#ifdef SMALL_CODE_MODEL
  #define Lib_BP_Parm1_IL  						 \
     asm("   ADD    BP,4   ");						 \
    /* Lib_BP_Parm1_IL */
#else  /* large code model */
  #define Lib_BP_Parm1_IL  						 \
     asm("   ADD    BP,6   ");						 \
    /* Lib_BP_Parm1_IL */
#endif
#endif

/**************************************************************************
;  Name: Lib_BP_Restore_IL - Restore BP After Reference to First Parameter
; 
;  Description:
;  Used in conjuntion with the BP Parm1 function.  Restores the BP
;  register to its original value.
;*************************************************************************/
#ifndef U10
#ifdef SMALL_CODE_MODEL
  #define Lib_BP_Restore_IL  						 \
     asm("   SUB    BP,4   ");						 \
    /* Lib_BP_Restore_IL */
#else  /* large code model */
  #define Lib_BP_Restore_IL  						 \
     asm("   SUB    BP,6   ");						 \
    /* Lib_BP_Restore_IL */
#endif
#endif

/**************************************************************************
;  Name: Lib_Prologue_BP_IL - Save BP and set BP = SP.
; 
;  Description:
;  Used in conjuntion with the Lib_Epilogue_BP_IL to restore BP.
;*************************************************************************/
#ifndef U10
#define Lib_Prologue_BP_IL  						 \
     asm("   PUSH   BP     ");						 \
     asm("   MOV    BP,SP  ");						 
#endif

/**************************************************************************
;  Name: Lib_Epilogue_BP_IL - Restore SP, POP BP.
; 
;  Description:
;  Used in conjuntion with the Lib_Prologue_BP_IL to maintain BP.
;*************************************************************************/
#ifndef U10
#define Lib_Epilogue_BP_IL  						 \
     asm("   NOP           ");						 \
     asm("   POP    BP     ");						 
#endif

/**************************************************************************
;  Name: Lib_Prologue_DI_IL - Save DI
; 
;  Description:
;  Used in conjuntion with the Lib_Epilogue_DI_IL to maintain DI.
;*************************************************************************/
#ifndef U10
#define Lib_Prologue_DI_IL  						 \
     asm("   PUSH    DI     ");						 \
     asm("   PUSH    SI     ");
#endif

/**************************************************************************
;  Name: Lib_Epilogue_DI_IL - Save DI
; 
;  Description:
;  Used in conjuntion with the Lib_Prologue_DI_IL to maintain DI.
;*************************************************************************/
#ifndef U10
#define Lib_Epilogue_DI_IL  						 \
     asm("   POP    SI     ");						 \
     asm("   POP    DI     ");
#endif

#endif  /* end of include file definition */

