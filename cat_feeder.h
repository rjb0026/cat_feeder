
#ifndef _CAT_FEEDER_H_
#define _CAT_FEEDER_H_

#include <time.h>

/// @brief Set a generic daily feed time to the current date
///
/// Set a daily time like 7 a.m. (07:00:00) with arbitrary (default to 
/// zeros) month, day, year values to have the same date as the
/// current RTC time now
/// @param feed_time is a pointer to the desired feed time. ex: 07:00:00
/// @param now is a pointer to the full current RTC time. ex: 05:50:26 11/25/17
void cat_feeder_set_feed_time_today(struct tm *feed_time, 
    struct tm *now);

#endif
