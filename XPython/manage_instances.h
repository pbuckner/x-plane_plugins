#include <Python.h>
extern PyObject *XPY3moduleDict;  /* moduleName -> instance */
extern PyObject *XPY3pluginDict;  /* instance -> info ... [name, signature, descrptions, module, module_name, disabled]*/
extern PyObject *XPY3aircraftPlugins; /* [instance, instance, ] */
extern PyObject *XPY3sceneryPlugins; /* [instance, instance, ] */

#define PLUGIN_NAME 0
#define PLUGIN_SIGNATURE 1
#define PLUGIN_DESCRIPTION 2
#define PLUGIN_MODULE 3
#define PLUGIN_MODULE_NAME 4
#define PLUGIN_DISABLED 5

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
