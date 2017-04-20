#pragma once

#include "stringUtils.h"

namespace str
{

// returns an escape code that can be placed in a C++ string literal and will evaluate to the given char
string charToCppStringLiteralEscaped(unsigned char c);

// returns a base number with 0-9 being 0-9, 10-35 being a-z and 36-61 being A-Z
// if maxDigits is >=0 will return only the leftmost maxDigits
string intToBase62(unsigned int in, int maxDigits=-1);

// uses intToBase62 to convert ptrIn to a string that can be used in a C++ id
string ptrToUniqueStr(void* ptrIn, int digits=4);

}

