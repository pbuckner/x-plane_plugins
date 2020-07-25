#!/bin/bash
if [ `uname -s` != "Linux" ]; then
    echo "Error: due to regex support, execute this on Linux"
    exit 1
fi

for file in XP*.py;
do
    file=`echo $file | sed s/\.py//`
    echo "import $file"
    grep '^def ' $file.py | sed -e 's/def \(XPU\|XPLM\|XP\)\(.\)\([^(]*\).*$/\l\2\3 = '$file'.\1\2\3/' | grep -v '_[tf] ='
    grep '^xp.*=' $file.py | sed -e 's/^\(xplm\|xp\)\(_*\)\(.\)\([^ =]*\).*$/\u\3\4 = '$file'.\1\2\3\4/' | grep -v '_[tf] ='
    grep '^XPLM_.*=' $file.py | sed -e 's/^\(XPLM\|xp\)\(_*\)\(.\)\([^ =]*\).*$/\u\3\4 = '$file'.\1\2\3\4/' | grep -v '_[tf] ='
    if [ $file == "XPLMPlugin" ]; then
        # add in simplified version for plugin messages, to match format of widget messages
        cat <<EOF
MsgPlaneCrashed = XPLMPlugin.XPLM_MSG_PLANE_CRASHED
MsgPlaneLoaded = XPLMPlugin.XPLM_MSG_PLANE_LOADED
MsgAirportLoaded = XPLMPlugin.XPLM_MSG_AIRPORT_LOADED
MsgSceneryLoaded = XPLMPlugin.XPLM_MSG_SCENERY_LOADED
MsgAirplaneCountChanged = XPLMPlugin.XPLM_MSG_AIRPLANE_COUNT_CHANGED
MsgPlaneUnloaded = XPLMPlugin.XPLM_MSG_PLANE_UNLOADED
MsgWillWritePrefs = XPLMPlugin.XPLM_MSG_WILL_WRITE_PREFS
MsgLiveryLoaded = XPLMPlugin.XPLM_MSG_LIVERY_LOADED
MsgEnteredVR = XPLMPlugin.XPLM_MSG_ENTERED_VR
MsgExitingVR = XPLMPlugin.XPLM_MSG_EXITING_VR
EOF
    fi
done

