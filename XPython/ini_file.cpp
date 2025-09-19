#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <ctype.h>
#include "ini_file.h"
#include "ini.h"

char xpy_ini_file[512] = "xppython3.ini";  /* by design, this will get changed to include full path in handleConfigFile() */

std::string xpy_config_get(std::string item) {
  /* kinda simple:
     "item" is simple Head.name -- a single period splits section head and value name

     In file:
     [Section Heads]   must match exactly, with no leading/trailing spaces
     names             must be single words, may have leading/trailing spaces
     values            must be single words(!), may have leading/trailing spaces
     Use '=' to separate name/value pairs. These are equivalent
        foo = bar
        foo = bar zoo
        foo=bar

     DO NOT put quotes around string values -- they're not required
     
     A name without separator or without a value results in ""
     A '#' will stop further processing:
        # foo         ==> ignored  (returning '')
        foo # bar     ==> foo:     (returning '')
        foo: bar #zoo ==> foo: bar (returning 'bar')
     Not found will return nullptr

     For now, we re-read the file on each "get"... we're assuming it's
     not a common operation and the file is small.
   */

  mINI::INIFile file(xpy_ini_file);
  mINI::INIStructure ini;
  file.read(ini);
  
  int location = item.find(".");
  if (location < 0) return "";

  std::string a = item.substr(0, location);
  std::string b = item.substr(location + 1, item.length() - (location + 1));

  if (ini.has(a) && ini[a].has(b))
    return ini[a][b];
  
  return "";
}

int xpy_config_get_int_default(std::string item, int if_not_found) {
  std::string value = xpy_config_get(item);
  if (value != "") {
    const std::vector<std::string> truevalues = {
      "ON", "On", "on",
      "True", "TRUE", "true",
      "T", "t",
      "YES", "Yes", "yes",
      "Y", "y"
    };
    
    // Check if value matches any true value
    for (const auto& truevalue : truevalues) {
      if (value == truevalue) {
        return 1;
      }
    }
    
    // If not a boolean true value, try to parse as integer
    return atoi(value.c_str());
  }
  return if_not_found;
}

int xpy_config_get_int(std::string item) {
  /* strings, nulls will return 0.
     Real numbers return only integer part.
     Negative numbers return negative  integers
  */
  return xpy_config_get_int_default(item, 0);
}

int main(int argc, char *argv[]){
  /*
    # comment
    [main] # Comment on section
    found = 99
    bar
    s = Hello # comment on value
    t = "Hello"
  */
  
  (void)argc;
  (void)argv;
  printf(">>> Main.found:        %d (99)\n", xpy_config_get_int(std::string("Main.found")));
  printf(">>> Main.not_found:    %d (0)\n", xpy_config_get_int(std::string("Main.not_found")));
  printf(">>> Main.not_found_47: %d (47)\n", xpy_config_get_int_default(std::string("Main.not_found"), 47));
  printf(">>> Other Section.foo: '%s' ('')\n", xpy_config_get(std::string("Other Section.foo")).c_str());
  printf(">>> Main.s:            '%s' ('Hello')\n", xpy_config_get(std::string("Main.s")).c_str());
  printf(">>> Main.t:            '%s' ('\"Hello\")'\n", xpy_config_get(std::string("Main.t")).c_str());
}
