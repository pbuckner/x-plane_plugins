#ifndef WIDGETS__H
#define WIDGETS__H
#ifdef __cplusplus
extern "C" {
#endif
  void resetWidgets(void);
  void logWidgets(PyObject *key, char *key_s, char *value_s);
#ifdef __cplusplus
}
#endif
#endif
