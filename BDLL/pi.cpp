// pi.cpp : DLL 응용 프로그램에 대한 진입점을 정의합니다.
//

#define hdw_ModuleDefinition
#define led_ModuleDefinition
#define svtool_ModuleDefinition

#include "stdafx.h"
#include "common.h"

#define EXTERN extern
#include "infbld.h"
#include "infini.h"
#include "infics.h"
#include "sio.h"
#include "assign.h"
#include "Infrng.h"
#include "Inferr.h"
#include "Flash.h"
#include "infmsg.h"
#include "infrng.h"
#undef EXTERN

#define EXTERN
#include "hdw.h"
#include "pi.h"
#include "led.h"
#include "Svtool.h"
#undef EXTERN


void  process_msg (BYTE_T *msg_ptr, BYTE_T ring_num);
extern int k;
BYTE_T* eeprom_offset;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    //hdw_Init();
    return TRUE;
}


extern "C" __declspec(dllexport) int Hello(void);
int Hello(void)
{
	return 3;
}

extern "C" __declspec(dllexport) int hello()
{
	return 2;
}


LONG_T splx(int param)
{
	return 0;
}

/*******************************************************************************
;  Name:  sio_TxPort - Transmit a Data Stream Out a Given Port
;
;  Description:
;  Transmit a data stream of bytes out a given port.  This operation is
;  performed in two steps.
;
;  The first step deals with putting the bytes in the transmit buffer.
;  If there is not enough buffer space available for all the bytes then
;  none of the bytes will be placed in the buffer.  When this happens
;  the SIO channel is reset.  This will clear up a possible hung up channel.
;
;  The second step checks the transmitter to see if it is turned on.  If
;  there are any bytes to be transmitted and the transmitter is off then
;  the transmitter is turned on to allow the transmit buffer to get sent
;  out.  The transmit buffer could be empty on this second step even
;  though bytes were put in the buffer on the first step.  This can happen
;  if the transmitter is already on when the bytes are put in and if
;  enough of a delay occurs between the time the bytes are put in the
;  buffer and the time the transmitter is checked.  This delay could occur
;  due to the processing delay or an interrupt.
;
;  Parameters:
;  Port_ID          (input)   port identification
;  Data_Ptr         (input)   pointer to the start of the data stream
;  Data_Size        (input)   size of the data stream in bytes
;  Valid_TX         (output)  TRUE = bytes transmitted
;                             FALSE = bytes not transmitted, channel reset
;******************************************************************************/
FLAG_T sio_TxPort(BYTE_T portNum, BYTE_T *Data_Ptr, LONG_T Data_Size)
{
	
  FLAG_T Valid_TX;
  FLAG_T Valid_Get;
  LONG_T  TX_Buf;    /* current TX buffer size */
  BYTE_T Data;
  LONG_T  imask;
  PORT_T  *port;
/*
  imask = splx(FULLMASK);
  port = &sio_Port[portNum];


  Valid_TX = FALSE;
  Valid_TX = Q_Put(&(port->TX_Q), Data_Ptr, Data_Size);
  TX_Buf = Q_Get_Count(&(port->TX_Q));

  if ( Valid_TX == TRUE )
  {
     if ( TX_Buf > 0 )
     {
        if (TX_Buf > (port->Max_Tx_Buf))
           port->Max_Tx_Buf = TX_Buf;

        sio_txtHandler(port);
        port->TX_Busy = TRUE;
     }
     else
        Valid_TX = FALSE;
  } 

  splx(imask);
  */
  Valid_TX=TRUE;
  return(Valid_TX);
} // sio_TxPort

void rtc_GetTime(BYTE_T* hour,BYTE_T *minute,BYTE_T *second)
{

}
void rtc_GetDate(BYTE_T* year,BYTE_T *month,BYTE_T *day,BYTE_T *week)
{

}

BOOL Exec_Enable_And_Run_Task(INT_T assign_new_tskID)
{
	assign_new_tsk();
	return TRUE;
}

extern "C" __declspec(dllexport) int qGetCount()
{
	int queue_count = Q_Get_Count(ica_call_q_ptr);
	return queue_count;
}

void hdw_WatchDog(void)
{
}

void flash_dumpBackupArea(BYTE_T* dest_adr,LONG_T len)
{

}

/*******************************************************************************
; Name : hdw_Init
;
; Decsriptions :
;    After startup routine, execute this routine.
;    Initialize MPU, FPU, peripheral device.
*******************************************************************************/
extern "C" __declspec(dllexport) void hdw_Init(void)
{
	fuzzy_logic_enabled=TRUE;
	ica_central_group_enabled_flag=TRUE;

  splx(FULLMASK);
  init_data_stores();
} /* hdw_Init */

/*******************************************************************************
; Name : led_Driver
; 
; Descriptions :
;    BIT STATE
;       0->LED ON    1->LED OFF
;    BIT ASSIGNMENT
;       LED   8    9    10   11   12   13   14   15
;       BIT  0/1  0/1   0/1  0/1  0/1  0/1  0/1  0/1
*******************************************************************************/
void led_Driver(void)
{

}

/**************************************************************************
;  Name: SV_Driver - Drives the Service Tool Software
;
;  Description:
;  Performs these functions:
;    o checks for a received byte and does some front-end checks on it
;    o checks for a service tool break
;    o calls the task that will handle the current service tool state
;    o transmits any new data to the service tool
;*************************************************************************/
void SV_Driver(void)
{
} /* SV_Driver */

void hdw_Monitor(void)
{}

void sio_Monitor(void)
{}

extern "C" __declspec(dllexport) void car_status00(BYTE_T car, BYTE_T load,BYTE_T pre_dir, BYTE_T opmode,BYTE_T mov_dir,BYTE_T r_door_state,BYTE_T f_door_state,BYTE_T NCF)
{
	BYTE_T msg_buffer[6];
    /* Define message header. */
    /* ---------------------- */
    msg_buffer[0] = car;       /* type 0, from OCSS */
    msg_buffer[1] = 0; // SubType 0
	msg_buffer[1] |=load<<2;
	msg_buffer[1] |=pre_dir;
	msg_buffer[2] =opmode;
	msg_buffer[3] =0;
	msg_buffer[3] |=mov_dir<<4;
	msg_buffer[3] |=r_door_state<<2;
	msg_buffer[3] |=f_door_state;
	msg_buffer[4]=NCF;
	msg_buffer[5] = M_checksum(msg_buffer,MSG_LEN);	
	process_msg(msg_buffer,1);
}

extern "C" __declspec(dllexport) void car_status01(BYTE_T car,BYTE_T car_in_ez_flag,BYTE_T motion_status,BYTE_T actual_pos)
{
	BYTE_T msg_buffer[6];
    /* Define message header. */
    /* ---------------------- */
    msg_buffer[0] = car;       /* type 0, from OCSS */
    msg_buffer[1] = 0x20; // SubType 0
	msg_buffer[1] |= motion_status;
	msg_buffer[3] =car_in_ez_flag<<7;
	msg_buffer[4]=actual_pos;
	msg_buffer[5] = M_checksum(msg_buffer,MSG_LEN);	
	process_msg(msg_buffer,1);
}

extern "C" __declspec(dllexport) void car_ic_param(BYTE_T car,BYTE_T target_pos)
{
	BYTE_T msg_buffer[6];
	msg_buffer[0]=0x50 | car;
	msg_buffer[1]=0x40;
	msg_buffer[1]|=ICSS2_ORIGIN;
	msg_buffer[2]=56;
	msg_buffer[3]=target_pos;
	msg_buffer[4]=0;
	msg_buffer[5]=M_checksum(msg_buffer,MSG_LEN);	
	process_msg(msg_buffer,1);
}