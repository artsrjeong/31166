
/**********************************************************
 *  timer.h, to define the Timer class and prototype      *
 *  Written by Cheng-Shuo Wang 1/20/97                    *
 *  Revised to run in Windows Environment                 *
 **********************************************************/
/*
#include <limits.h>
#include <math.h>
#include <time.h>
*/
//#include "afxwin.h"
#include <windows.h>
#include <winbase.h>

#ifndef _TIMER
#define _TIMER

enum TimerStatus {stopped, running };

class Timer {
private:
  LARGE_INTEGER cstart;   //*** for using clock
  LARGE_INTEGER cfinish;
  LARGE_INTEGER lpFreq;
  TimerStatus status;
  double elapsed_time;
public:
  Timer() {
  QueryPerformanceFrequency(&lpFreq); 
  elapsed_time = 0;
  status = stopped;
  }
  // *** use default destructor ***
  void start();
  void stop();
  void clear();
  void restart();
  float read();
};

#endif   //** TIMER 
