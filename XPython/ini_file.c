#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char xpy_ini_file[512] = "xppython3.ini";  /* by design, this will get changed to include full path in handleConfigFile() */

char *xpy_config_get(char *item)
{
  char *dup = strdup(item);
  char *name = strchr(dup, '.');
  *name = '\0';
  name ++;
  char *section = dup;

  FILE *fp = fopen(xpy_ini_file, "r");

  if (!fp) {
    return NULL;
  }

  char line[1024];
  int found_section = 0;
  while(fgets(line, 1024, fp)) {
    if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
    if (! found_section && 0 == strcmp(line, section)) {
      found_section = 1;
      continue;
    }
    if (found_section) {
      char *tok = strtok(line, " =:");
      if (tok && tok[0] == '[') {
        /* start of new section, bail */
        found_section = 0;
        continue;
      }
      while(tok) {
        int found_name=0;
        if (!found_name && 0 == strcmp(tok, name)) {
          /* If name is found, but value not, return '' instead of NULL */
          char empty_str[] = "";
          char *v = strtok(NULL, " :=");
          fclose(fp);
          return strdup(v ? v : empty_str);
        }
        tok = strtok(NULL, " =:");
      }
    }
  }
  fclose(fp);
  return NULL;
}

int xpy_config_get_int(char *item) {
  char *foo = xpy_config_get(item);
  if (foo) return atoi(foo);
  return 0;
}

/*
int main(int argc, char *argv[]){
  (void)argc;
  (void)argv;
  printf(">>> [Main].foo: %s\n", config_get("[Main].foo"));
  printf(">>> [Main].cdr: %s\n", config_get("[Main].cdr"));
  printf(">>> [Other Section].foo: %s\n", config_get("[Other Section].foo"));
  printf(">>> [Other Section].cdr: %s\n", config_get("[Other Section].cdr"));
}
*/
