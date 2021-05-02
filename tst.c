#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define COMMENT_CHAR '#'

struct options {
  int verbose;
  int show_des;
  char *config_file;
  char *out_file;
  char *scope;
};

/* CLI helper functions */

void show_help(char *argv1){
  printf("Task Schduler: shows current task according to your schedule.\n");
  printf("USAGE: %s [options]\n\n", argv1);
  printf("OPTIONS:\n");
  printf("\t-h\tDisplay this message.\n");
  printf("\t-v\tVerbose Output.\n");
  printf("\t-d\tDisplay description of tasks.\n");
  printf("\t-c FILE\tProvide a task file. (Def: ~/.config/tst/schedule.sch)\n");
  printf("\t-s SCOPE\tProvide a scope. (Def: DEFAULT)\n");
  printf("\t-o FILE\tOutput file. (Def: stdout)\n");

  printf("\nFor more info visit https://github.com/Atreyagaurav/task-scheduler\n");
}

char* get_config_filename(){
  char *ch;
  ch = malloc(1024);
  strcpy(ch, getenv("HOME"));
  strcat(ch, "/.config/task/task.sch");
  return ch;
}

struct options read_opts(int argc, char *argv[]){
  struct options cli_op = {
    .config_file = get_config_filename(),
    .out_file = NULL,
    .scope = "DEFAULT",
    .show_des = 0,
    .verbose = 0
  };
  int opt;
  while((opt = getopt(argc,argv,":c:s:o:hvd")) != -1){
    switch (opt){
    case 'd':
      cli_op.show_des = 1;
      break;
    case 'v':
      cli_op.verbose = 1;
      break;
    case 'h':
      show_help(argv[0]);
      exit(0);
    case 's':
      cli_op.scope = optarg;
      break;
    case 'c':
      free(cli_op.config_file);
      cli_op.config_file = optarg;
      break;
    case 'o':
      cli_op.out_file = optarg;
      break;
    default:
      printf("Invalid option: -%c\n", opt);
      printf("Use -h flag to see help.\n");
      exit(1);
    }
  }

  /* check if config file can be successfully accessed. */
  if (access(cli_op.config_file, R_OK)){
    printf("Config file %s doesn't exist, or you don't"
	   "have the read permission.\n", cli_op.config_file);
    exit(1);
  }
  return cli_op;
}

/* FILE reading functions */

void skipThisLine(FILE* fp){
  while(fgetc(fp)!='\n');
}

void skipWhiteSpace(FILE* fp){
  char c = ' ';
  while(1){
    if(c==' '||c=='\t'){
      c=fgetc(fp);
    }else{
      break;
    }
  }
  ungetc(c, fp);
}

int nextLine(FILE* fp){
  char c;
  while (1){
    c=fgetc(fp);
    switch (c){
    case EOF:
      return 0;
    case '\n':
    case '\r':
    case ' ':
      continue;
    case COMMENT_CHAR:
      skipThisLine(fp);
      break;
    default:
      ungetc(c, fp);
      return 1;
    }
  }
}

int nextScope(FILE* fp){
  char c = ' ';
  while (c!='[') {
    if (nextLine(fp)){
      c = fgetc(fp);
    }else{
      return 0;
    }
  }
  return 1;
}

/* scope matching functions */

int match_scope(char *scope, char *target){
  int len = strlen(scope);
  char *ch = target;
  while ((ch = strstr(ch, scope)) != NULL) {
    char c = *(ch+len);
    if (c == ']' || c == ',' || c == ';' || c == '\0'){
      return 1;
    }
    ch += len;
  }
  return 0;
}

char* scan_all_scopes(FILE *fp, int *len, int *width){
  char c;
  *len = 0;
  *width = 0;
  int ln, wd;
  /* counting the total scopes in the file */
  fseek(fp, 0, SEEK_SET);
  while(nextScope(fp)){
    wd = 0;
    while((c = fgetc(fp)) != ']'){
      wd += 1;
    }
    *width = wd > *width ? wd : *width;
    *len += 1;
  }
  /* storing the scopes in a char buffer */
  char *scopes = malloc((*width+1) * *len * sizeof(char));
  fseek(fp, 0, SEEK_SET);
  ln = 0;
  while(nextScope(fp)){
    wd = 0;
    while((c = fgetc(fp)) != ']'){
      *(scopes + ln * *width + wd++) = c;
    }
    *(scopes + ln * *width + wd++) = '\0';
    ln += 1;
  }
  return scopes;
}

void show_matched_scope(char *filename, char *scope){
  FILE *fp;
  fp = fopen(filename, "r");
  int i, len, width;
  char *sc;
  char *scopes = scan_all_scopes(fp, &len, &width);
  for(i=0; i<len; i++){
    sc = (scopes + i * width);
    if (match_scope(scope, sc)){
      printf("%s\n", sc);
      break;
    }
  }
  free(scopes);
  fclose(fp);
}

/* MAIN function */

int main(int argc, char *argv[])
{
  struct options opt;
  opt = read_opts(argc, argv);
  show_matched_scope(opt.config_file, opt.scope);
  return 0;
}

