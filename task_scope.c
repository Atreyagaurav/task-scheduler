/* If you are editing this code be careful that the token max length
   is hardcoded. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int add_day_token(int day, char *tk);
int add_month_token(int month, char *tk);
int add_date_token(int year, int month, int day, char *tk);

int add_day_token(int day, char *tk) {
  switch (day) {
  case 0:
    strcpy(tk, "SUN");
    break;
  case 1:
    strcpy(tk, "MON");
    break;
  case 2:
    strcpy(tk, "TUE");
    break;
  case 3:
    strcpy(tk, "WED");
    break;
  case 4:
    strcpy(tk, "THU");
    break;
  case 5:
    strcpy(tk, "FRI");
    break;
  case 6:
    strcpy(tk, "SAT");
    break;
  }
  return 3;
}
int add_month_token(int month, char *tk) {
  switch (month) {
  case 0:
    strcpy(tk, "JAN");
    break;
  case 1:
    strcpy(tk, "FEB");
    break;
  case 2:
    strcpy(tk, "MAR");
    break;
  case 3:
    strcpy(tk, "APR");
    break;
  case 4:
    strcpy(tk, "MAY");
    break;
  case 5:
    strcpy(tk, "JUN");
    break;
  case 6:
    strcpy(tk, "JUL");
    break;
  case 7:
    strcpy(tk, "AUG");
    break;
  case 8:
    strcpy(tk, "SEP");
    break;
  case 9:
    strcpy(tk, "OCT");
    break;
  case 10:
    strcpy(tk, "NOV");
    break;
  case 11:
    strcpy(tk, "DEC");
    break;
  }
  return 3;
}

int add_date_token(int year, int month, int day, char *tk) {
  char date[32];
  char mon[5];
  add_month_token(month, mon);
  sprintf(date, "%4d-%s-%02d", year, mon, day);
  strcpy(tk, date);
  return strlen(date);
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    printf("No commandline arguments supported for now.\n");
    exit(1);
  }
  char output_token[64]; /* 32 is enough as of now but just in case. */
  char *tk = output_token;
  time_t now;
  struct tm *timeinfo;

  time(&now);
  timeinfo = localtime(&now);

  tk += add_date_token(timeinfo->tm_year + 1900, timeinfo->tm_mon,
                       timeinfo->tm_mday, tk);
  *tk++ = ',';
  tk += add_day_token(timeinfo->tm_wday, tk);
  *tk++ = ',';
  tk += add_month_token(timeinfo->tm_mon, tk);
  printf("%s\n", output_token);
  return 0;
}
