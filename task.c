#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define COMMENT_CHAR '#'

struct options {
  int verbose;
  int show_des;
  char *config_file;
  char *out_file;
  char *scope;
};

struct task {
  int st_hour;
  int st_min;
  int en_hour;
  int en_min;
  char summary[20];
  char *des;
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
  
  if (access(ch, R_OK)){
    /* runs when access fails.
     * this means it'll priotize user config over system one. */
    strcpy(ch, "/etc/task/task.sch");
  }
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

/* time helper functions */

void get_current_hm(int *h, int *m){
  time_t now;
  struct tm *timeinfo;
  time(&now);
  timeinfo = localtime(&now);
  *h = timeinfo->tm_hour;
  *m = timeinfo->tm_min;
}

int get_file_hm(FILE *fp, int *h, int *m){
  char c;
  *h=0, *m=0;
  int flag=0;
  while(flag<2){
    c=fgetc(fp);
    switch(c){
    case ' ':
    case '\t':
      break;
    case '0'...'9':
      if (flag){
	*m = *m * 10 + c - '0';
      }else{
	*h = *h * 10 + c - '0';
      }
      break;
    case ':':
    case '-':
      if (flag)
        return 1;
      flag += 1;
      break;
    default:
      return flag ? 1 : 0;
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
  *width += 1;
  /* storing the scopes in a char buffer */
  char *scopes = malloc(*width * *len * sizeof(char));
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

char* matched_scope(FILE *fp, char *scope, int *max_width){
  int i, j, len, width, slen;
  char *sc, *res;
  char *scopes = scan_all_scopes(fp, &len, &width);
  *max_width = width;
  res = malloc(width*sizeof(char));
  
  slen = strlen(scope);
  char *in_sco = malloc((slen+1)*sizeof(char));
  int pos;
  /* <=len to match the last null character of the token. */
  for(i=0, pos=0; i <= slen; i++){
    switch(*(scope+i)){
    case ';':
    case ',':
    case '\0':
      *(in_sco + pos) = '\0';
      /* looping through the scopes in the file to get the correct scope. */
      for (j = 0; j < len; j++) {
        sc = (scopes + j * width);
        if (match_scope(in_sco, sc)) {
          strcpy(res, sc);
	  free(scopes);
	  return res;
        }
      }
      pos = 0;
      break;
    default:
      *(in_sco + pos++) = *(scope + i);
    }
  }
  free(scopes);
  strcpy(res, "DEFAULT");
  return res;
}

struct task current_task(char *filename, char *scope){
  struct task t1 = {};
  FILE *fp;
  int max_width;
  fp = fopen(filename, "r");
  scope = matched_scope(fp, scope, &max_width);
  int h1, m1, h2, m2;
  get_current_hm(&h1, &m1);
  /* printf("TTIME: %d:%d\n", h1, m1); */

  /* going through the scopes again */
  fseek(fp, 0, SEEK_SET);
  char *curr_scope = malloc(max_width * sizeof(char));
  int i=0, flag=1;
  char c;
  char summar[25];
  while(flag && nextScope(fp)){
    while((c = fgetc(fp)) != ']'){
      *(curr_scope + i++) = c;
    }
    *(curr_scope + i++) = '\0';
    if(match_scope(scope, curr_scope)){
      skipThisLine(fp);
      while(nextLine(fp)){
	if(!get_file_hm(fp, &h2, &m2)){
	  t1.en_hour = 24;
	  t1.en_min = 0;
	  break;
	}

	/* printf(">%d:%d\n", h2, m2); */

	if ((h2*60+m2)>(h1*60+m1)){
	  /* means we went overbord */
	  t1.en_hour = h2;
	  t1.en_min = m2;
	  break;
	}
	
	/* copy the summary text to buffer. */
        i = 0;
	skipWhiteSpace(fp);
        while ((c = fgetc(fp)) != '\n' && i < 24) {
          summar[i++] = c;
        }
        summar[i] = '\0';
        if (i > 18) {
          summar[16] = '.';
          summar[17] = '.';
          summar[18] = '.';
          summar[19] = '\0';
        }
        strcpy(t1.summary, summar);
	t1.st_hour = h2;
	t1.st_min = m2;
      }
    }
    i = 0;
  }
  free(curr_scope);
  fclose(fp);
  return t1;
}


/* MAIN function */

int main(int argc, char *argv[])
{
  struct options opt;
  struct task tk;
  opt = read_opts(argc, argv);
  tk = current_task(opt.config_file, opt.scope);

  FILE *fout;
  if (opt.out_file){
    fout = fopen(opt.out_file, "w");
  }else{
    fout = stdout;
  }
  fprintf(fout, "%s (%02d:%02d-%02d:%02d)\n",
	 tk.summary,
	 tk.st_hour, tk.st_min,
	 tk.en_hour, tk.en_min);
  if (opt.out_file){
    fclose(fout);
  }
  return 0;
}

