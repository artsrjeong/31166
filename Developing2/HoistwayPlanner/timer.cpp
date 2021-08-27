
//#include <iostream.h>
#include "stdafx.h"
#include "timer.h"



void Timer::start()
{
  QueryPerformanceCounter(&cstart);
  status = running;
  elapsed_time = 0;
}

void Timer::stop()
{
  if (status == stopped) 
    return;
  QueryPerformanceCounter(&cfinish);
  status = stopped;

  elapsed_time += (cfinish.QuadPart - cstart.QuadPart)/(double)lpFreq.QuadPart;
}

float Timer::read()
{
  
  if (status == stopped) 
    return((float) elapsed_time);	//CSW051019 :cast the type to float to eliminate warning message

  QueryPerformanceCounter(&cfinish);
  elapsed_time += (cfinish.QuadPart - cstart.QuadPart)/(double)lpFreq.QuadPart;
  return((float)elapsed_time);	//CSW051019 :cast the type to float to eliminate warning message

}




/**** stop and clear the timer ******/
void Timer::clear() 
{
  status = stopped;
  elapsed_time = 0;
}

/*** clear and restart the timer *****/

void Timer::restart()
{
  clear();
  start();
}
