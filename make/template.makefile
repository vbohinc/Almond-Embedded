# MCU name
MCU = atmega8535
#MCU = atxmega128a1

# Processor frequency.
#     This will define a symbol, F_CPU, in all source code files equal to the 
#     processor frequency. You can then use this symbol in your source code to 
#     calculate timings. Do NOT tack on a 'UL' at the end, this will be done
#     automatically to create a 32-bit value in your source code.
#     Typical values are:
#         F_CPU =  1000000
#         F_CPU =  1843200
#         F_CPU =  2000000
#         F_CPU =  3686400
#         F_CPU =  4000000
#         F_CPU =  7372800
#         F_CPU =  8000000
#         F_CPU = 11059200
#         F_CPU = 14745600
#         F_CPU = 16000000
#         F_CPU = 18432000
#         F_CPU = 20000000
F_CPU = 1000000

TARGET = main

#path to root
BASE = ../.. 

# ok to simplyfy things we define libs, makes adding dependencies easier
# possible values:
# bluetooth
# twi
# spi
# and others
ALMONDLIBS = bluetooth 

SRC = #insert all extra src besides TARGET.c and those defined by the libs

EXTRAINCDIRS = ../sensors/

TARGETTYPE = NUT # or SQUIRREL 


