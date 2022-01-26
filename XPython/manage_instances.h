extern PyObject *moduleDict;  /* info -> instance */
extern PyObject *pluginDict;  /* instance -> info */
extern PyObject *aircraftPlugins; /* [instance, instance, ] */
extern PyObject *sceneryPlugins; /* [instance, instance, ] */

#define PLUGIN_NAME 0
#define PLUGIN_SIGNATURE 1
#define PLUGIN_DESCRIPTION 2
#define PLUGIN_MODULE 3
#define PLUGIN_MODULE_NAME 4

extern void xpy_startInstances(int include_aircraft);
extern void xpy_startInternalInstances(void);
extern void xpy_stopInstances(void);
extern void xpy_enableInstances(void);
extern void xpy_disableInstances(void);

extern void xpy_startSceneryPlugins(void);
extern void xpy_enableSceneryPlugins(void);
extern void xpy_disableSceneryPlugins(void);
extern void xpy_stopSceneryPlugins(void);

extern void xpy_startAircraftPlugins(void);
extern void xpy_enableAircraftPlugins(void);
extern void xpy_disableAircraftPlugins(void);
extern void xpy_stopAircraftPlugins(void);
