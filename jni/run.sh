#!/bin/bash
adb push /Volumes/macos/Users/damotou/elfupspeed/libs/armeabi/elfinject /data/local/
adb push /Volumes/macos/Users/damotou/elfupspeed/libs/armeabi/libspeed.so  /data/local/
adb push /Volumes/macos/Users/damotou/elfupspeed/libs/armeabi/testclient /data/local/
adb push /Volumes/macos/Users/damotou/elfupspeed/libs/armeabi/testserver /data/local/

adb shell chmod 777 /data/local/testserver
adb shell chmod 777 /data/local/testclient
adb shell chmod 777 /data/local/elfinject
adb shell chmod 777 /data/local/libspeed.so

adb shell /data/local/elfinject  sh.lilith.dgame.mi 5 /data/local/libspeed.so /data/local/libsubstrate.so

#adb shell /data/local/testserver