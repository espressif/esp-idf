#include <string.h>
#include "apps/sntp_time.h"

s8_t sntp_time_timezone = 8;
u32_t sntp_time_realtime = 0;

int sntp_time_month;
int sntp_time_year;
char sntp_time_result[100];
sntp_tm_type sntp_time_rule[2];
sntp_tm sntp_time_result_buf;

static const int sntp_time_mon_lengths[2][12] = {
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
} ;

static const int sntp_time_year_lengths[2] = {
  365,
  366
};

sntp_tm *sntp_mktm_r(const sntp_time_t *tim_p, sntp_tm *res, int is_gmtime)
{
  long days, rem;
  sntp_time_t lcltime;
  int i;
  int y;
  int yleap;
  const int *ip;

  /* base decision about std/dst time on current time */
  lcltime = *tim_p;

  days = ((long)lcltime) / SECSPERDAY;
  rem = ((long)lcltime) % SECSPERDAY;
  while (rem < 0)
    {
      rem += SECSPERDAY;
      --days;
    }
  while (rem >= SECSPERDAY)
    {
      rem -= SECSPERDAY;
      ++days;
    }

  /* compute hour, min, and sec */
  res->tm_hour = (int) (rem / SECSPERHOUR);
  rem %= SECSPERHOUR;
  res->tm_min = (int) (rem / SECSPERMIN);
  res->tm_sec = (int) (rem % SECSPERMIN);

  /* compute day of week */
  if ((res->tm_wday = ((EPOCH_WDAY + days) % DAYSPERWEEK)) < 0)
    res->tm_wday += DAYSPERWEEK;

  /* compute year & day of year */
  y = EPOCH_YEAR;
  if (days >= 0)
    {
      for (;;)
	{
	  yleap = isleap(y);
	  if (days < sntp_time_year_lengths[yleap])
	    break;
	  y++;
	  days -= sntp_time_year_lengths[yleap];
	}
    }
  else
    {
      do
	{
	  --y;
	  yleap = isleap(y);
	  days += sntp_time_year_lengths[yleap];
	} while (days < 0);
    }

  res->tm_year = y - YEAR_BASE;
  res->tm_yday = days;
  ip = sntp_time_mon_lengths[yleap];
  for (res->tm_mon = 0; days >= ip[res->tm_mon]; ++res->tm_mon)
    days -= ip[res->tm_mon];
  res->tm_mday = days + 1;

  if (!is_gmtime)
    {
      int offset;
      int hours, mins, secs;

	  res->tm_isdst = 0;

      offset = (res->tm_isdst == 1 ? sntp_time_rule[1].offset : sntp_time_rule[0].offset);

      hours = offset / SECSPERHOUR;
      offset = offset % SECSPERHOUR;

      mins = offset / SECSPERMIN;
      secs = offset % SECSPERMIN;

      res->tm_sec -= secs;
      res->tm_min -= mins;
      res->tm_hour -= hours;

      if (res->tm_sec >= SECSPERMIN)
	{
	  res->tm_min += 1;
	  res->tm_sec -= SECSPERMIN;
	}
      else if (res->tm_sec < 0)
	{
	  res->tm_min -= 1;
	  res->tm_sec += SECSPERMIN;
	}
      if (res->tm_min >= MINSPERHOUR)
	{
	  res->tm_hour += 1;
	  res->tm_min -= MINSPERHOUR;
	}
      else if (res->tm_min < 0)
	{
	  res->tm_hour -= 1;
	  res->tm_min += MINSPERHOUR;
	}
      if (res->tm_hour >= HOURSPERDAY)
	{
	  ++res->tm_yday;
	  ++res->tm_wday;
	  if (res->tm_wday > 6)
	    res->tm_wday = 0;
	  ++res->tm_mday;
	  res->tm_hour -= HOURSPERDAY;
	  if (res->tm_mday > ip[res->tm_mon])
	    {
	      res->tm_mday -= ip[res->tm_mon];
	      res->tm_mon += 1;
	      if (res->tm_mon == 12)
		{
		  res->tm_mon = 0;
		  res->tm_year += 1;
		  res->tm_yday = 0;
		}
	    }
	}
       else if (res->tm_hour < 0)
	{
	  res->tm_yday -= 1;
	  res->tm_wday -= 1;
	  if (res->tm_wday < 0)
	    res->tm_wday = 6;
	  res->tm_mday -= 1;
	  res->tm_hour += 24;
	  if (res->tm_mday == 0)
	    {
	      res->tm_mon -= 1;
	      if (res->tm_mon < 0)
		{
		  res->tm_mon = 11;
		  res->tm_year -= 1;
		  res->tm_yday = 365 + isleap(res->tm_year);
		}
	      res->tm_mday = ip[res->tm_mon];
	    }
	}
    }
  else
    res->tm_isdst = 0;

  return (res);
}

sntp_tm *sntp_localtime_r(const sntp_time_t *tim_p, sntp_tm *res)
{
  return sntp_mktm_r (tim_p, res, 0);
}

sntp_tm *sntp_localtime(const sntp_time_t *tim_p)
{
  return sntp_localtime_r (tim_p, &sntp_time_result_buf);
}

int sntp_limitstime(int year)
{
  int days, year_days, years;
  int i, j;

  if (year < EPOCH_YEAR)
    return 0;

  sntp_time_year = year;

  years = (year - EPOCH_YEAR);

  year_days = years * 365 +
    (years - 1 + EPOCH_YEARS_SINCE_LEAP) / 4 - (years - 1 + EPOCH_YEARS_SINCE_CENTURY) / 100 +
    (years - 1 + EPOCH_YEARS_SINCE_LEAP_CENTURY) / 400;

  for (i = 0; i < 2; ++i)
    {
      if (sntp_time_rule[i].ch == 'J')
	days = year_days + sntp_time_rule[i].d + (isleap(year) && sntp_time_rule[i].d >= 60);
      else if (sntp_time_rule[i].ch == 'D')
	days = year_days + sntp_time_rule[i].d;
      else
	{
	  int yleap = isleap(year);
	  int m_day, m_wday, wday_diff;
	  const int *ip = sntp_time_mon_lengths[yleap];

	  days = year_days;

	  for (j = 1; j < sntp_time_rule[i].m; ++j)
	    days += ip[j-1];

	  m_wday = (EPOCH_WDAY + days) % DAYSPERWEEK;

	  wday_diff = sntp_time_rule[i].d - m_wday;
	  if (wday_diff < 0)
	    wday_diff += DAYSPERWEEK;
	  m_day = (sntp_time_rule[i].n - 1) * DAYSPERWEEK + wday_diff;

	  while (m_day >= ip[j-1])
	    m_day -= DAYSPERWEEK;

	  days += m_day;
	}

      /* store the change-over time in GMT form by adding offset */
      sntp_time_rule[i].change = days * SECSPERDAY + sntp_time_rule[i].s + sntp_time_rule[i].offset;
    }

  sntp_time_month = (sntp_time_rule[0].change < sntp_time_rule[1].change);

  return 1;
}

char *sntp_asctime_r(sntp_tm *tim_p ,char *result)
{
  static const char day_name[7][4] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };
  static const char mon_name[12][4] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  sprintf (result, "%s %s %02d %02d:%02d:%02d %02d\n",
	   day_name[tim_p->tm_wday],
	   mon_name[tim_p->tm_mon],
	   tim_p->tm_mday, tim_p->tm_hour, tim_p->tm_min,
	   tim_p->tm_sec, 1900 + tim_p->tm_year);
  return result;
}
char *sntp_asctime(sntp_tm *tim_p)
{
    return sntp_asctime_r (tim_p, sntp_time_result);
}

u32_t sntp_get_current_timestamp(void)
{
	if(sntp_time_realtime == 0){
		printf("please start sntp first !\n");
		return 0;
	} else {
		return sntp_time_realtime;
	}
}

char* sntp_get_real_time(sntp_time_t t)
{
	return sntp_asctime(sntp_localtime (&t));
}

/**
 * SNTP get sntp_time_timezone default GMT + 8
 */
s8_t sntp_get_timezone(void)
{
	return sntp_time_timezone;
}

/**
 * SNTP set sntp_time_timezone default GMT + 8
 */
bool sntp_set_timezone(s8_t timezone)
{
	if(timezone >= -11 && timezone <= 13) {
		sntp_time_timezone = timezone;
		return true;
	} else {
		return false;
	}

}
void sntp_inc_time(void)
{
	sntp_time_realtime ++;
}

void sntp_convert_time(sntp_time_t GMT_Time)
{
	sntp_time_t Local_Time = GMT_Time;
	Local_Time += sntp_time_timezone * 60 * 60;
	sntp_time_realtime = Local_Time;
	printf("%s\n",sntp_asctime(sntp_localtime (&Local_Time)));
}

