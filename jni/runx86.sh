#!/bin/bash
adb push /Volumes/macos/Users/damotou/elfupspeed/libs/x86/elfinject /data/local/
adb push /Volumes/macos/Users/damotou/elfupspeed/libs/x86/libspeed.so  /data/local/
adb push /Volumes/macos/Users/damotou/elfupspeed/libs/x86/testclient /data/local/

adb shell chmod 777 /data/local/testclient
adb shell chmod 777 /data/local/elfinject
adb shell chmod 777 /data/local/libspeed.so

#adb shell /data/local/elfinject sh.lilith.dgame.uc 5 /data/local/libspeed.so /data/local/libsubstrate.so

adb shell /data/local/elfinject  com.hcg.cok.ewan.kaopu 5 /data/local/libspeed.so /data/local/libsubstrate.so