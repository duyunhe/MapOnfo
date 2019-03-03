#pragma once

#include "windows.h"
#include <stdint.h>
#include <string>

int CodeConvert(uint32_t uDestCodePage, void* pDestChar, uint32_t uSrcCodePage, void* pSrcChar);
unsigned int ELFhash(char *str);
std::string double2String(double num);