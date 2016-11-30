#ifndef _TIMER_H
#define _TIMER_H

inline int timer_atomic_get_tod(void);
int timer_start();
int timer_delay(int interval);

#endif //_TIMER_H
