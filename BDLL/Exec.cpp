#define exec_ModuleDefinition
#include "stdafx.h"
#include "common.h"

#define EXTERN
#include "exec.h"
#undef EXTERN

#include "exec.t"

/**************************************************************************
;  Name: Exec_Enable_Task - Enable Scheduling for a Task 
; 
;  Description:
;  Enables a task to get scheduled on the next scheduling cycle.  Periodic
;  tasks will be enabled and will be scheduled at the periodic rate.  Event
;  tasks will be scheduled once and then disabled.
; 
;  Parameters:
;  Task ID         (input)   task identification tag
;  Success Flag    (output)  operation successfull 
;*************************************************************************/
FLAG_T Exec_Enable_Task(TASK_ID_T Task_ID) {

  FLAG_T Success;
  TASK_T *Task_Ptr;
/* jsr exec_TaskEnableFail 을 받아들이지 못함
  Task_ID--;  // bring into 0 .. Max Tasks-1 range 
  if (Task_ID >= exec_MaxTasks) {
    exec_TaskEnableFail++;
    Success = FALSE;
  }
  else {
    Task_Ptr = &Task_Table[Task_ID];
    if (Task_Ptr->Schedule_On == FALSE) {
      Task_Ptr->Schedule_On = TRUE;
      Task_Ptr->Timer = Task_Ptr->Init_Delay;
    }
    Success = TRUE;
  }
  */
  return(Success);
} /* Exec_Enable_Task */
