#include <string>
extern char xpy_ini_file[];
extern std::string xpy_config_get(std::string item);
extern int xpy_config_get_int(std::string item);
extern int xpy_config_get_int_default(std::string item, int if_not_found);
