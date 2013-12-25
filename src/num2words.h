#pragma once

#include "string.h"
/*
typedef enum
{
  FALSE,
  TRUE
} boolean;
*/

/* the letter type can be defined with the following #defines:
 * #define CAPITAL: all letters are in CAPITAL
 * #define HALF_CAPITAL: only the first letter of a word is in Capital
 * "nothing defines": all letters are small                                  */
//#define CAPITAL
#define HALF_CAPITAL

void fuzzy_time_to_words(int hours, int minutes, char* words, size_t length);
