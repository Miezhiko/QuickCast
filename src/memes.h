#pragma once

#include "stdafx.h"
#include "static.h"
#include "utils.h"
#include "input.h"

static BOOL SILLY_WALK_LOCK = FALSE;

VOID sillyWalkLOL(VOID) {
  if (SILLY_WALK_LOCK) return;
  else SILLY_WALK_LOCK = TRUE;

  if (GetCursorPos(&CURSOR_POSITION)) {
    const LONG old_x = STORED_CURSOR_POSITION.x;
    const LONG old_y = STORED_CURSOR_POSITION.y;
    STORED_CURSOR_POSITION = CURSOR_POSITION;
    if (old_x == 0) {
      SILLY_WALK_LOCK = FALSE;
      return;
    }

    const LONG xDiff    = CURSOR_POSITION.x - old_x;
    const LONG yDiff    = CURSOR_POSITION.y - old_y;
    const FLOAT len     = sqrtf( pow(xDiff,2) + pow(yDiff,2) );
    const UINT32 steps  = (UINT32) floorf( len / SILLY_WALK_STEP );
    if (len == 0) {
      SILLY_WALK_LOCK = FALSE;
      return;
    }

    const FLOAT dx      = xDiff / len;
    const FLOAT dy      = yDiff / len;
    shift_down();
    for (INT i = 0; i < steps; ++i) {
      const FLOAT xi              = old_x + i * dx * SILLY_WALK_STEP;
      const FLOAT yi              = old_y + i * dy * SILLY_WALK_STEP;
      if (i % 2 == 0) {
        FLOAT x3i                 = xi - yDiff;
        FLOAT y3i                 = yi + xDiff;
        const FLOAT xCurrentDiff  = x3i - xi;
        const FLOAT yCurrentDiff  = y3i - yi;
        const FLOAT dist          = sqrtf( pow(xCurrentDiff,2) + pow(yCurrentDiff,2) );
        if (dist == 0) {
          SILLY_WALK_LOCK = FALSE;
          return;
        }

        const FLOAT dxCurrent = xCurrentDiff / dist;
        const FLOAT dyCurrent = yCurrentDiff / dist;
        x3i = xi + SILLY_WALK_STEP2 * dxCurrent;
        y3i = yi + SILLY_WALK_STEP2 * dyCurrent;
        SetCursorPos( (INT) floorf( x3i )
                    , (INT) floorf( y3i ) );
      } else
        SetCursorPos( (INT) floorf( xi )
                    , (INT) floorf( yi ) );
      sleep(-2000);
      mouseRightClick();
    }
    shift_up();
  }
  SILLY_WALK_LOCK = FALSE;
}

VOID backAndForwardVertical(VOID) {
  if (GetCursorPos(&CURSOR_POSITION)) {
    shift_down();
    for (INT YEE = 0; YEE < 100; ++YEE) {
      SetCursorPos(CURSOR_POSITION.x, CURSOR_POSITION.y - 25);
      mouseRightClick();
      SetCursorPos(CURSOR_POSITION.x, CURSOR_POSITION.y + 25);
      mouseRightClick();
    }
    shift_up();
  }
}

VOID backAndForwardHorizontal(VOID) {
  if (GetCursorPos(&CURSOR_POSITION)) {
    shift_down();
    for (INT YEE = 0; YEE < 100; ++YEE) {
      SetCursorPos(CURSOR_POSITION.x - 25, CURSOR_POSITION.y);
      mouseRightClick();
      SetCursorPos(CURSOR_POSITION.x + 25, CURSOR_POSITION.y);
      mouseRightClick();
    }
    shift_up();
  }
}
