MCU = atxmega128a1
#F_CPU = 11059200
#F_CPU = 22118400
F_CPU = 27648000
#F_CPU = 2000000
TARGET = squirrel
SRC =  squirrel.c util.c ../test/pong.c ../drivers/platform/buttons.c 
BASE = ..
ALMONDLIBS = gui downlink uplink storage rtc
EXTRAINCDIRS = ../drivers/ ../protocols/ ../shared/
TARGETTYPE = SQUIRREL
ENABLE_DEBUG = 1
include ../make/base.makefile 

