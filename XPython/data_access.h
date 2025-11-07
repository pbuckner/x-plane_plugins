#ifndef DATA_ACCESS__H
#define DATA_ACCESS__H
void resetDataRefs(void);
PyObject *buildDataRefCallbackDict(void);
PyObject *buildSharedDataRefCallbackDict(void);
#endif
