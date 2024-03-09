make -j -C build subsdk9_meta
# make -C send
cp ./build/main.npdm /mnt/c/Users/Binston/AppData/Roaming/Ryujinx/sdcard/atmosphere/contents/0100000000010000/exefs
cp ./build/subsdk9 /mnt/c/Users/Binston/AppData/Roaming/Ryujinx/sdcard/atmosphere/contents/0100000000010000/exefs
cp -r ./romfs /mnt/c/Users/Binston/AppData/Roaming/Ryujinx/sdcard/atmosphere/contents/0100000000010000