#include "ntp.h"
#include <time.h>

// NTP Servers: pool.ntp.org is a good global default
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec =
    -10800; // GMT-3 (Brazil default from request context? User looks like
            // Saldañha/Brazil IP 192.168.100.x isn't help but User mentioned
            // BRL. I'll stick to UTC or use standard offset)
// Actually user's logs showed timestamp 02:20:58-03:00 which is GMT-3. So I
// will use -3 * 3600. Ideally this should be configurable, but for now I'll
// hardcode based on user context.
const int daylightOffset_sec = 0;

void init_ntp() { configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); }

bool is_time_synced() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return false;
  }
  return timeinfo.tm_year > (2016 - 1900);
}

String get_current_time() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "--:--";
  }
  char timeStringBuff[10];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M", &timeinfo);
  return String(timeStringBuff);
}

String get_current_date() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "--/--/--";
  }
  char timeStringBuff[20];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%d/%m/%Y", &timeinfo);
  return String(timeStringBuff);
}
