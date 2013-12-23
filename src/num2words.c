#include "num2words.h"
#include "string.h"

static const char* const ONES[] = {
  "null",
  "eins",
  "zwei",
  "drei",
  "vier",
  "fünf",
  "sechs",
  "sieben",
  "acht",
  "neun"
};

static const char* const TEENS[] ={
  "",
  "elf",
  "zwölf",
  "dreizehn",
  "vierzehn",
  "fünfzehn",
  "sechzehn",
  "siebzehn",
  "achtzehn",
  "neunzehn"
};

static const char* const TENS[] = {
  "",
  "zehn",
  "zwanzig",
  "dreissig",
  "vierzig",
  "fünfzig",
  "sechzig",
  "siebzig",
  "achtzig",
  "neunzig"
};

static const char* STR_OH_CLOCK = "uhr";
static const char* STR_NOON = "zwölf";
static const char* STR_MIDNIGHT = "null";
static const char* STR_QUARTER = "viertel";
static const char* STR_TO = "vor";
static const char* STR_PAST = "nach";
static const char* STR_HALF = "halb";
static const char* STR_AFTER = "nach";
static const char* STR_AND = "und";
static const char* STR_TEST = "X";
static const char* STR_EIN = "ein";
static const char* STR_SPACE = "\n";


//this returns the correct pm/am hour,  instead of the %12 hour
int get_cor_hour (int inc_hour)
{
  if (inc_hour < 12)
  {
    return inc_hour;
  }
  else if (inc_hour == 24)
  {
    return 0;
  }
  else 
  {
    int val = (inc_hour %12);
    if (val == 0)
    {
      return 12;
    }
    else
    {
      return val;
    }
  }
}

static size_t append_number(char* words, int num)
{
  int tens_val = num / 10 % 10;
  int ones_val = num % 10;

  size_t len = 0;
  //if there is only a "ones number", print it
  if (((ones_val > 0)&&(tens_val<1)) || num == 0)
  {
    strcat(words, ONES[ones_val]);
    len += strlen(ONES[ones_val]);
  }
  //if number is bigger then ten
  else if (tens_val > 0)
  {
    //if number is between 11 and 19
    if (tens_val == 1 && num != 10)
    {
      strcat(words, TEENS[ones_val]);
      return strlen(TEENS[ones_val]);
    }
    //if number is a multiple of ten, print it
    else if (ones_val == 0)
    {
      strcat(words, TENS[tens_val]);
      len += strlen(TENS[tens_val]);
    }
    //if the number is between x1 and x9 and not below 20, print it
    else
    {
      //if the ones is a "eins" print a "ein"
      if (ones_val == 1)
      {
        strcat(words, STR_EIN);
        len += strlen(STR_EIN);
      }
      else
      {
        strcat(words, ONES[ones_val]);
        len += strlen(ONES[ones_val]);
      }
      strcat(words, STR_SPACE);
      len += strlen(STR_SPACE);
      strcat(words, STR_AND);
      len += strlen(STR_AND);
      strcat(words, STR_SPACE);
      len += strlen(STR_SPACE);
      strcat(words, TENS[tens_val]);
      len += strlen(TENS[tens_val]);
    }
  }
  //return string length
  return len;
}

static size_t append_string(char* buffer, const size_t length, const char* str) {
  strncat(buffer, str, length);

  size_t written = strlen(str);
  return (length > written) ? written : length;
}

void fuzzy_time_to_words(int hours, int minutes, char* words, size_t length)
{
  int fuzzy_hours = hours;
  //int fuzzy_minutes = ((minutes + 2) / 5) * 5;
  int fuzzy_minutes = minutes;

  /*// Handle hour & minute roll-over.
  if (fuzzy_minutes > 59) {
    fuzzy_minutes = 0;
    fuzzy_hours += 1;
    if (fuzzy_hours > 23) {
      fuzzy_hours = 0;
    }
  }*/

  size_t remaining = length;
  memset(words, 0, length);

  if (fuzzy_minutes != 0)
  {
    if (fuzzy_minutes < 15)
    {
      remaining -= append_number(words, fuzzy_minutes);
      remaining -= append_string(words, remaining, STR_SPACE);
      remaining -= append_string(words, remaining, STR_AFTER);
      remaining -= append_string(words, remaining, STR_SPACE);
      //remaining -= append_number(words, fuzzy_hours % 12);
      remaining -= append_number(words, get_cor_hour(fuzzy_hours));
    }
    //quarter past
    else if (fuzzy_minutes == 15)
     {
      remaining -= append_string(words, remaining, STR_QUARTER);
      remaining -= append_string(words, remaining, STR_SPACE);
      remaining -= append_string(words, remaining, STR_AFTER);
      remaining -= append_string(words, remaining, STR_SPACE);
      //remaining -= append_number(words, fuzzy_hours % 12);
      remaining -= append_number(words, get_cor_hour(fuzzy_hours));
     }
    //quarter to
    else if (fuzzy_minutes == 45)
    {
      remaining -= append_string(words, remaining, STR_QUARTER);
      remaining -= append_string(words, remaining, STR_SPACE);
      remaining -= append_string(words, remaining, STR_TO);
      remaining -= append_string(words, remaining, STR_SPACE);
      fuzzy_hours = (fuzzy_hours + 1) % 24;
      //remaining -= append_number(words, fuzzy_hours % 12);
      remaining -= append_number(words, get_cor_hour(fuzzy_hours));
    }
    //halb
    else if (fuzzy_minutes == 30)
    {
      remaining -= append_string(words, remaining, STR_HALF);
      remaining -= append_string(words, remaining, STR_SPACE);
      //remaining -= append_number(words, (fuzzy_hours + 1) % 12);
      remaining -= append_number(words, get_cor_hour(fuzzy_hours + 1));
      //remaining -= append_string(words, remaining, STR_PAST);
      //remaining -= append_string(words, remaining, " ");
    }

    //zero to 24
    else if (fuzzy_minutes < 25)
    {
      remaining -= append_number(words, fuzzy_minutes);
      remaining -= append_string(words, remaining, STR_SPACE);
      remaining -= append_string(words, remaining, STR_AFTER);
      remaining -= append_string(words, remaining, STR_SPACE);
      //remaining -= append_number(words, fuzzy_hours % 12);
      remaining -= append_number(words, get_cor_hour(fuzzy_hours));
    }
    // 25 to 29
    else if (fuzzy_minutes < 30)
    {
      remaining -= append_number(words, 30 - fuzzy_minutes);
      remaining -= append_string(words, remaining, STR_SPACE);
      remaining -= append_string(words, remaining, STR_TO);
      remaining -= append_string(words, remaining, STR_SPACE);
      remaining -= append_string(words, remaining, STR_HALF);
      remaining -= append_string(words, remaining, STR_SPACE);
      //remaining -= append_number(words, (fuzzy_hours + 1) % 12);
      remaining -= append_number(words, get_cor_hour(fuzzy_hours + 1));
    }
    //31 to 39
    //print example: "elf uhr drei und dreissig"
    else if ((fuzzy_minutes > 30)&&(fuzzy_minutes < 50))
    {
      //remaining -= append_number(words, fuzzy_hours % 12);
      remaining -= append_number(words, get_cor_hour(fuzzy_hours));
      remaining -= append_string(words, remaining, STR_SPACE);
      remaining -= append_string(words, remaining, STR_OH_CLOCK);
      remaining -= append_string(words, remaining, STR_SPACE);
      remaining -= append_number(words, fuzzy_minutes);
	}
    else
    {
      remaining -= append_number(words, 60 - fuzzy_minutes);
      remaining -= append_string(words, remaining, STR_SPACE);
      remaining -= append_string(words, remaining, STR_TO);
      remaining -= append_string(words, remaining, STR_SPACE);
      fuzzy_hours = (fuzzy_hours + 1) % 24;
      //remaining -= append_number(words, fuzzy_hours % 12);
      remaining -= append_number(words, get_cor_hour(fuzzy_hours));
    }
  }
  //midnight
  else if (fuzzy_hours == 0)
  {
    remaining -= append_string(words, remaining, STR_MIDNIGHT);
  }
  //noon
  else if (fuzzy_hours == 12)
  {
    remaining -= append_string(words, remaining, STR_NOON);
  }
  else
  {
    //remaining -= append_number(words, fuzzy_hours % 12);
    remaining -= append_number(words, get_cor_hour(fuzzy_hours));
  }
}
