#!/bin/bash
for file in XP*.py;
do
    file=`echo $file | sed s/\.py//`
    echo "import $file"
    grep '^def ' $file.py | sed -e 's/def \(XPU\|XPLM\|XP\)\(.\)\([^(]*\).*$/\l\2\3 = '$file'.\1\2\3/' | grep -v '_[tf] ='
    grep '^xp.*=' $file.py | sed -e 's/^\(xplm\|xp\)\(_*\)\(.\)\([^ =]*\).*$/\u\3\4 = '$file'.\1\2\3\4/' | grep -v '_[tf] ='
    grep '^XPLM_.*=' $file.py | sed -e 's/^\(XPLM\|xp\)\(_*\)\(.\)\([^ =]*\).*$/\u\3\4 = '$file'.\1\2\3\4/' | grep -v '_[tf] ='
done    

