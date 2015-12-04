
adb push H:\users\hellokitty\workspace\elfAssistant\libs\x86\elfinject /data/local/
adb push H:\users\hellokitty\workspace\elfAssistant\libs\x86\libinject.so /data/local/
adb push H:\users\hellokitty\workspace\elfAssistant\libs\x86\libspeed.so /data/local/
adb push H:\users\hellokitty\workspace\elfAssistant\libs\x86\client /data/local/

adb shell chmod 777 /data/local/elfinject
adb shell chmod 777 /data/local/client

adb shell chmod 777 /data/local/libinject.so
adb shell chown system:system /data/local/libinject.so
adb shell chmod 777 /data/local/libspeed.so
adb shell chown system:system /data/local/libspeed.so

adb shell  /data/local/elfinject
pause