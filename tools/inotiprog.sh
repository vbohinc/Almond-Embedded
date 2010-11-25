#!/bin/bash

# set the current path
cd ~/Dropbox/ALMOND/remoteprogrammer/
inotifywait -mr --timefmt '%d/%m/%y %H:%M' --format '%T %w %f' \
-e close ~/Dropbox/ALMOND/remoteprogrammer/ | while read date time dir file; do

		FILECHANGE=${dir}${file}
		# convert absolute path to relative
		#FILECHANGEREL=`echo "$FILECHANGE" | sed 's_'$CURPATH'/__'`
		echo $file

	  	if [ -f $file -a $file = "xmega.hex" ]
		then
			sudo avrdude -p atxmega128a1 -P usb:0000B0029073 -c stk500   -y -U flash:w:xmega.hex -U eeprom:w:xmega.eep
			rm xmega.hex
			rm xmega.eep
		fi

        if [ -f $file -a $file = "mega.hex" ]
		then
#            sudo avrdude -p atmega8535 -P usb:0000B0028758 -c stk500   -y -U flash:w:mega.hex -U eeprom:w:mega.eep
            sudo avrdude -p atmega8535 -P usb:0000B0028758 -c stk500   -y -U flash:w:mega.hex
			rm mega.hex
			rm mega.eep
        fi
	   
done

