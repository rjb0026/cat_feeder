
#include "cat_feeder.h"

void cat_feeder_set_feed_time_today(struct tm *feed_time, 
    struct tm *now)
{
  feed_time->tm_mday = now->tm_mday;
  feed_time->tm_wday = now->tm_wday;
  feed_time->tm_year = now->tm_year;
  feed_time->tm_mon = now->tm_mon;
  // year day and daylight savings time not important
}
