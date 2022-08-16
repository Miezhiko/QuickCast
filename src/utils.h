#pragma once

#include "stdafx.h"

// NT kernel sleep
VOID sleep(INT ticks) {
  static LARGE_INTEGER interval;
  interval.QuadPart = ticks;
  NtDelayExecution(FALSE, &interval);
}
