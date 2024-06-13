from XPPython3 import xp


# pluginDict = {<instance>: (PLUGIN_NAME, PLUGIN_SIGNATURE, PLUGIN_DESCRIPTION,
#                            PLUGIN_MODULE, PLUGIN_MODULE_NAME, PLUGIN_DISABLED})
# moduleDict = {PLUGIN_MODULE_NAME: <instance>}
#
# accessorDict = {accessorDictKey : (MODULE_NAME, drefName, drefType, writable, readint, writeint,
#                                    readfloat, writefloat, readdouble, writedouble, readintarray, writeintarray,
#                                    readfloatarray, writefloatarray, readdata, writedata, readRefcon, writerefcon)}
# drefs = {<drefCapsule> : accessorDictKey}
#
# flDict = {<flDictKey> : callback, refcon, module_name, type}
# flIDDict = {<flDictKey> : <flightLoopCapsule>}
# 
# commandCallbackDict = {<cmdCallbackKey>: CAPSULE, CALLBACK, BEFORE, REFCON, MODULE_NAME}
#
# MODULE_NAME like 'XPPython3.I_PI_FirstTime' 'PythonPlugins.PI_Other', 'Laminar Research.Cessna 172SP.plugins.PythonPlugins.PI_electrical'
# dict.items() returns ((key, value), (key, value))

def getModuleName(instance):
    return [x for x in xp.pythonGetDicts()['plugins'].items() if x[0] == instance][0][1][4]


def getAccessors(instance):
    moduleName = getModuleName(instance)
    # print(moduleName)
    accessorDictKeys = [x[0] for x in xp.pythonGetDicts()['accessors'].items() if x[1][0] == moduleName]
    # print(accessorDictKeys)
    drefCapsules = [x[0] for x in xp.pythonGetDicts()['drefs'].items() if x[1] in accessorDictKeys]
    return drefCapsules


def getFlightLoops(instance):
    moduleName = getModuleName(instance)
    flKeys = [x[0] for x in xp.pythonGetDicts()['flightLoops'].items() if x[1][2] == moduleName and x[1][3] == 1]  # get new style only
    # print(f"{flKeys=}")
    flCapsules = [x[1] for x in xp.pythonGetDicts()['flightLoopIDs'].items() if x[0] in flKeys]
    # print(f"{flCapsules=}")
    return flCapsules

    
def getCommands(instance):
    moduleName = getModuleName(instance)
    commands = [(x[1][0], x[1][1], x[1][2], x[1][3]) for x in xp.pythonGetDicts()['commandCallbacks'].items() if x[1][4] == moduleName]
    # print(commands)
    return commands
