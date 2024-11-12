#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


char xpy_ini_file[512] = "xppython3.ini";  /* by design, this will get changed to include full path in handleConfigFile() */

char *xpy_config_get(char *item)
{
  /* kinda simple:
     [Section Heads]   must match exactly, with no leading/trailing spaces
     names             must be single words, may have leading/trailing spaces
     values            must be single words(!), may have leading/trailing spaces
     Use ':' or '=' or ' ' to separate name/value pairs. These are equivalent, returning 'bar':
        foo: bar
        foo bar
        foo = bar
        foo = bar zoo
     A name without separator or without a value results in ""
     A '#' will stop further processing:
        # foo         ==> ignored  (returning NULL)
        foo # bar     ==> foo:     (returning '')
        foo: bar #zoo ==> foo: bar (returning 'bar')
     Not found will return NULL
   */
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
        if (tok && tok[0] == '#') {
          //printf("found '%s', breaking loop, getting next line\n", tok);
          break;
        }
        int found_name=0;
        if (!found_name && 0 == strcmp(tok, name)) {
          /* If name is found, but value not, return '' instead of NULL */
          char empty_str[] = "";
          char *v = strtok(NULL, " :=");
          if (v && v[0] == '#') {
            v = NULL;
          }
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

int xpy_config_get_int_default(char *item, int if_not_found) {
  char *foo = xpy_config_get(item);
  char *truevalues[] = {"ON", "On", "on", "True", "TRUE", "true", "T", "t", "YES", "Yes", "yes", "Y", "y", ""};
  char **v;
  v = truevalues;
  if (foo) {
    while (**v != 0) {
      if (0 == strcmp(foo, *v++)) {
        return 1;
      }
    }
    return atoi(foo);
  }
  return if_not_found;
}  

int xpy_config_get_int(char *item) {
  /* strings, nulls will return 0.
     Real numbers return only integer part.
     Negative numbers return negative  integers
  */
  return xpy_config_get_int_default(item, 0);
}


/* int main(int argc, char *argv[]){ */
/*   (void)argc; */
/*   (void)argv; */
/*   printf(">>> [Main].foo: %d\n", xpy_config_get_int("[Main].foo")); */
/*   printf(">>> [Main].cdr: %d\n", xpy_config_get_int("[Main].cdr")); */
/*   printf(">>> [Other Section].foo: %s\n", xpy_config_get("[Other Section].foo")); */
/*   printf(">>> [Other Section].cdr: %s\n", xpy_config_get("[Other Section].cdr")); */
/* } */
