import glob
import os
import sys
packages = []
os.chdir('/Users/pbuck/xp')
scenery_root = os.path.join('/Users/pbuck/xp/Custom Scenery')
if scenery_root not in sys.path:
    sys.path.append(scenery_root)

for x in glob.glob(os.path.join(scenery_root, '*', 'plugins/PythonPlugins')):
    path_rel = os.path.relpath(x, start=scenery_root)
    if x not in sys.path:
        sys.path.append(x)
    package = (path_rel + '/plugins/PythonPlugins').replace('/', '.').replace('\\\\', '.')
    if (x, package) not in packages:
        packages.append((x, package))

print("packages:")
for x in packages:
    print(x)

print("path: {}".format(sys.path[-1]))
