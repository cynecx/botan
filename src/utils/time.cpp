/**
* Time Functions
* (C) 1999-2009 Jack Lloyd
*
* Distributed under the terms of the Botan license
*/

#include <botan/time.h>
#include <botan/exceptn.h>
#include <ctime>

#if defined(BOTAN_TARGET_OS_HAS_WIN32_GET_SYSTEMTIME)
  #include <windows.h>
#endif

#if defined(BOTAN_TARGET_OS_HAS_GETTIMEOFDAY)
  #include <sys/time.h>
#endif

#if defined(BOTAN_TARGET_OS_HAS_CLOCK_GETTIME)

  #ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 199309
  #endif

  #include <time.h>

  #ifndef CLOCK_REALTIME
    #define CLOCK_REALTIME 0
  #endif

#endif

namespace Botan {

namespace {

/**
* Combine a two time values into a single one
*/
u64bit combine_timers(u32bit seconds, u32bit parts, u32bit parts_hz)
   {
   static const u64bit NANOSECONDS_UNITS = 1000000000;

   u64bit res = seconds * NANOSECONDS_UNITS;
   res += parts * (NANOSECONDS_UNITS / parts_hz);
   return res;
   }

std::tm do_gmtime(time_t time_val)
   {
   // Race condition: std::gmtime is not assured thread safe,
   // and C++ does not include gmtime_r. Use a mutex here?

   std::tm* tm_p = std::gmtime(&time_val);
   if (tm_p == 0)
      throw Encoding_Error("calendar_value could not convert with gmtime");
   return *tm_p;
   }

}

/*
* Convert a time_point to a calendar_point
*/
calendar_point calendar_value(
   const std::chrono::system_clock::time_point& time_point)
   {
   std::tm tm = do_gmtime(std::chrono::system_clock::to_time_t(time_point));

<<<<<<< variant A
   std::tm tm;

#if defined(BOTAN_TARGET_OS_HAS_GMTIME_S)
   gmtime_s(&tm, &time_val); // Windows
#elif defined(BOTAN_TARGET_OS_HAS_GMTIME_R)
   gmtime_r(&time_val, &tm); // Unix/SUSv2
#else
   std::tm* tm_p = std::gmtime(&time_val);
   if (tm_p == 0)
      throw Encoding_Error("time_t_to_tm could not convert");
   tm = *tm_p;
#endif

   return tm;
>>>>>>> variant B
   return calendar_point(tm.tm_year + 1900,
                         tm.tm_mon + 1,
                         tm.tm_mday,
                         tm.tm_hour,
                         tm.tm_min,
                         tm.tm_sec);
####### Ancestor
   std::tm* tm_p = std::gmtime(&time_val);
   if (tm_p == 0)
      throw Encoding_Error("time_t_to_tm could not convert");
   return (*tm_p);
======= end
   }

u64bit get_nanoseconds_clock()
   {
#if defined(BOTAN_TARGET_OS_HAS_CLOCK_GETTIME)
   struct ::timespec tv;
   ::clock_gettime(CLOCK_REALTIME, &tv);
   return combine_timers(tv.tv_sec, tv.tv_nsec, 1000000000);

#elif defined(BOTAN_TARGET_OS_HAS_GETTIMEOFDAY)
   struct ::timeval tv;
   ::gettimeofday(&tv, 0);
   return combine_timers(tv.tv_sec, tv.tv_usec, 1000000);

#elif defined(BOTAN_TARGET_OS_HAS_WIN32_GET_SYSTEMTIME)

   // Returns time since January 1, 1601 in 100-ns increments
   ::FILETIME tv;
   ::GetSystemTimeAsFileTime(&tv);
   u64bit tstamp = (static_cast<u64bit>(tv.dwHighDateTime) << 32) |
                   tv.dwLowDateTime;

   return (tstamp * 100); // Scale to 1 nanosecond units

#else
   return combine_timers(std::time(0), std::clock(), CLOCKS_PER_SEC);

#endif
   }

}
