#define FULLMASK 0


LONG_T splx(int param);
void rtc_GetTime(BYTE_T* hour,BYTE_T* minute,BYTE_T* second);
void rtc_GetDate(BYTE_T* year,BYTE_T* month,BYTE_T* day,BYTE_T *week);
BOOL Exec_Enable_And_Run_Task(INT_T tskID);