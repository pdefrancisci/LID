#!/bin/bash
counter=1
for file in $(ls -tr ./dith); do
    if [[ -f "./dith/$file" ]]; then
        mv -- "./dith/$file" "./dith/image$counter.bmp"
        ((counter++))
    fi
done
cp ./dith/* /media/pjd/CDROM/ 
