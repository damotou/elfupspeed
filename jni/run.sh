#!/bin/bash
adb push /Users/damotou/elfAssistant/obj/local/armeabi/elfinject /data/local/
adb push /Users/damotou/elfAssistant/obj/local/armeabi/libspeed.so  /data/local/
adb push /Users/damotou/elfAssistant/obj/local/armeabi/testclient /data/local/
adb push /Users/damotou/elfAssistant/obj/local/armeabi/testserver /data/local/

adb shell chmod 777 /data/local/testserver
adb shell chmod 777 /data/local/testclient
adb shell chmod 777 /data/local/elfinject
adb shell chmod 777 /data/local/libspeed.so

adb shell /data/local/elfinject com.hcg.cok.wdj 5 /data/local/libspeed.so /data/local/libsubstrate.so

#adb shell /data/local/testserver