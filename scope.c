#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/* This program should output the token for scopes from the
   current system date. */

void add_day_token(int day, char *tk);
void add_month_token(int month, char *tk);
void add_date_token(time_t timestamp, char *tk);

/* add logics to these functions here. so that they
   concat the tokens to the tk parameter. */

int main(int argc, char *argv[])
{
  if (argc>1){
    printf("No commandline arguments supported for now.\n");
    exit(1);
  }
  char output_token[64];
  /* call 3 functions here with suitable arguments, tk=output_token*/
  printf("%s", output_token);
  return 0;
}
