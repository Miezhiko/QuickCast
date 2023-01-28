#pragma once

#include "stdafx.h"

#define WC3_SLEEP sleep(WC3_SLEEP_TIMEOUT);

// NT kernel sleep
VOID sleep(INT ticks) {
  static LARGE_INTEGER interval;
  interval.QuadPart = ticks;
  NtDelayExecution(FALSE, &interval);
}
