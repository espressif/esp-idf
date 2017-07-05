#include "bt_utils.h"


/*****************************************************************************
**
** Function        raise_priority_a2dp
**
** Description     Raise task priority for A2DP streaming
**
** Returns         void
**
*******************************************************************************/
void raise_priority_a2dp(tHIGH_PRIORITY_TASK high_task)
{
    (void) high_task;
    return;
}

/*****************************************************************************
**
** Function        adjust_priority_a2dp
**
** Description     increase the a2dp consumer task priority temporarily when start
**                 audio playing, to avoid overflow the audio packet queue, restore
**                 the a2dp consumer task priority when stop audio playing.
**
** Returns         void
**
*******************************************************************************/
void adjust_priority_a2dp(int start)
{
    (void) start;
    return;
}
