
adb push H:\users\hellokitty\workspace\elfAssistant\libs\armeabi-v7a\elfinject /data/local/
adb push H:\users\hellokitty\workspace\elfAssistant\libs\armeabi-v7a\libinject.so /data/local/
adb push H:\users\hellokitty\workspace\elfAssistant\libs\armeabi-v7a\libspeed.so /data/local/

adb push H:\users\hellokitty\workspace\elfAssistant\libs\armeabi-v7a\client /data/local/

adb shell chmod 777 /data/local/client
adb shell chmod 777 /data/local/elfinject
adb shell chmod 777 /data/local/libinject.so
adb shell chmod 777 /data/local/libspeed.so
adb shell chown system:system /data/local/libspeed.so


adb shell  /data/local/elfinject
pause