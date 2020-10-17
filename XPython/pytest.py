
nextXPLMMenuIdx = 0
menuPluginIdxDict = {'a': [1,2, 3, 7],
                     'b': [4, 8, 9],
                     'c': [0, 5, 6, 10]}

def calcnext():
    return sum([len(x) for x in menuPluginIdxDict.values()])

def calsub(m, plugin, nextXPLMMenuIdx):
    l = m[plugin]
    l.reverse()
    for k, v in m.items():
        if k == plugin:
            m[k] = []
        else:
            for n in l:
                m[k] = [x if x < n else x - 1 for x in m[k]]
    return nextXPLMMenuIdx - len(l)
        
        

nextXPLMMenuIdx = calcnext()
print("next: {}".format(nextXPLMMenuIdx))

nextXPLMMenuIdx = calsub(menuPluginIdxDict, 'b', nextXPLMMenuIdx)
print(nextXPLMMenuIdx, menuPluginIdxDict)

