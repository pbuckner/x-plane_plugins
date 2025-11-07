#pragma once
#include <Python.h>
#include <unordered_map>
#include <string>

enum PluginType {
  PLUGIN_GLOBAL,
  PLUGIN_AIRCRAFT,
  PLUGIN_SCENERY,
  PLUGIN_INTERNAL
};

struct PluginInfo {
  std::string name;
  std::string signature;
  std::string description;
  const char* module_name;  // Points into moduleNamePool (interned string)
  bool disabled;
  PluginType plugin_type;
};

extern std::unordered_map<std::string, PyObject*> XPY3moduleInfoDict;  /* moduleName -> instance */
extern std::unordered_map<PyObject *, PluginInfo> XPY3pluginInfoDict;  /* instance -> info struct */
extern PyObject *XPY3aircraftPlugins; /* [instance, instance, ] */
extern PyObject *XPY3sceneryPlugins; /* [instance, instance, ] */

// Legacy defines kept for compatibility with any remaining code
#define PLUGIN_NAME 0
#define PLUGIN_SIGNATURE 1
#define PLUGIN_DESCRIPTION 2
#define PLUGIN_MODULE 3
#define PLUGIN_MODULE_NAME 4
#define PLUGIN_DISABLED 5

extern void xpy_startInstances(int include_aircraft);
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

