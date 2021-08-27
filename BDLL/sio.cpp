#include "stdafx.h"
#include "common.h"
#define sio_ModuleDefinition

#define EXTERN extern
#include "infrng.h"
#include "infocs.h"
#include "inferr.h"
#undef EXTERN

#define EXTERN
#include "sio.h"
#undef EXTERN



/**************************************************************************
;  Name:  SIO_TX_RING - Transmit a Message On Ring
; 
;  Description:
;  Transmit a message on the appropriate ring.
;
;  If ring 1 is okay, the system transmits the message on ring 1.
;  If ring 2 is okay and ring 1 is not okay, the system transmits
;  the message on ring 2.  If neither ring is okay, the message is
;  transmitted on both rings.
;
;  Parameters:
;  Data_Ptr         (input)   pointer to the start of the data stream  
;  Data_Size        (input)   size of the data stream in bytes
;  Ring_Type        (input)   0:Ring1  1:Ring2   2:Both-Ring
;*************************************************************************/
//void SIO_TX_RING(BYTE_T *Data_Ptr, INT_T Data_Size)
void sio_TxRing(BYTE_T *Data_Ptr, INT_T Data_Size, BYTE_T ringType)
{
  INT_T  imask, i;
  BYTE_T cksm;

   //  imask=splx(FULLMASK); 
   /* If ring 1 is okay, transmit message on that ring */

   if ((ring_info[RING1].type3_tout != 0) && (ringType == RING1))
   {
      if (!(sio_TxPort(RING_1_PORT, Data_Ptr, Data_Size)))
			{
			 E_log(RING1_TX_ERROR);
			}
  }
   /* ring 1 is bad, is ring 2 okay? */
   else if ((ring_info[RING2].type3_tout != 0) && (ringType == RING2))
   {
      if (!(sio_TxPort(RING_2_PORT, Data_Ptr, Data_Size)))
			{
			 E_log(RING2_TX_ERROR);
			}
   }
   else /*  Transmit on both "ringType==3:"RING_1_2"  */
   {
      if (!(sio_TxPort(RING_1_PORT, Data_Ptr, Data_Size)))
			{
			 E_log(RING1_TX_ERROR);
			}
      if (!(sio_TxPort(RING_2_PORT, Data_Ptr, Data_Size)))
			{
			 E_log(RING2_TX_ERROR);
			}
   }
 // splx(imask); 
} // sio_TxRing

FLAG_T sio_RxPort(BYTE_T portNum, BYTE_T *data, LONG_T size)
{
	return TRUE;

}