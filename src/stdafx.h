#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h>

extern INT WINAPI ZwSetTimerResolution(
  IN ULONG RequestedResolution,
  IN BOOLEAN Set,
  OUT PULONG ActualResolution
);
