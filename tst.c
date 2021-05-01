#include <stdio.h>

void show_help(char *argv1){
  printf("Task Schduler: shows current task according to your schedule.\n");
  printf("USAGE: %s [options]\n\n", argv1);
  printf("OPTIONS:\n");
  printf("\t-h\tDisplay this message.\n");
  printf("\t-v\tVerbose Output.\n");
  printf("\t-d\tDisplay description of tasks.\n");
  printf("\t-c=\tProvide a task file. (Def: ~/.config/tst/schedule.sch)\n");
  printf("\t-s=\tProvide a scope. (Def: DEFAULT)\n");

  printf("\nFor more info visit https://github.com/Atreyagaurav/task-scheduler\n");
}


int main(int argc, char *argv[])
{
  show_help(argv[0]);
  return 0;
}

