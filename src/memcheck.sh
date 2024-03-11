#!/bin/bash
if test -f /tmp/.cMem;then
cat /proc/meminfo | grep MemFree | awk '{print $2}' > /tmp/.cMem/.meminfo.log
fi

