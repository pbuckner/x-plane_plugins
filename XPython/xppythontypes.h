extern PyObject *PyHotKeyInfo_New(int virtualKey, int flags, char* description, int plugin);
extern PyObject *PyTrackMetrics_New(int isVertical, int downBtnSize, int downPageSize, int thumbSize, int upPageSize, int upBtnSize);
extern PyObject *PyNavAidInfo_New(int type, float latitude, float longitude, float height, int frequency, float heading, char* navAidID, char *name, int reg);
extern PyObject *PyFMSEntryInfo_New(int type, char *navAidID, int ref, int altitude, float lat, float lon);
extern PyObject *PyPluginInfo_New(char *name, char *filePath, char *signature, char *description);
extern PyObject *PyProbeInfo_New(int result, float locationX, float locationY, float locationZ, float normalX, float normalY, float normalZ, float velocityX, float velocityY, float velocityZ, int is_wet);
