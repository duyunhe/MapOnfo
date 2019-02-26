#pragma once

#include "windows.h"
#include <stdint.h>

int CodeConvert(uint32_t uDestCodePage, void* pDestChar, uint32_t uSrcCodePage, void* pSrcChar);

