MCU = atxmega128a1
F_CPU = 32000000
TARGET = menu_test.c
SRC =  menu_test.c ../../../squirrel/squirrel.c ../../../squirrel/util.c
BASE = ../../..
ALMONDLIBS = display gui downlink uplink storage rtc
EXTRAINCDIRS = ../../../drivers/ ../../../protocols/ ../../../shared/ $(BASE)/squirrel/
TARGETTYPE = SQUIRREL
ENABLE_DEBUG = 1
include ../../../make/base.makefile 
