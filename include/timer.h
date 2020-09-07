#ifndef __TIMER_H__
#define __TIMER_H__

#define TIME_SCALE 1 // vitrual_time = SCALE * real_time

void SetTimer(
	int intval_sec,
	int interval_usec = 0,
	int delay_sec = 0,
	int delay_usec = 1);

void SigHandle(int sig);

int GetTime();

void StartTimer();
void EndTimer();

void ClearLog();

#endif