extern char xpy_ini_file[];
#ifdef __cplusplus
extern "C" {
#endif
extern char*xpy_config_get(char *item);
extern int xpy_config_get_int(char *item);
extern int xpy_config_get_int_default(char *item, int if_not_found);
#ifdef __cplusplus
}
#endif
