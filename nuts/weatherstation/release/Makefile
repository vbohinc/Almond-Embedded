MCU = atmega8535
F_CPU = 11059200
TARGET = main
BASE = ../../..
#ALMONDLIBS = twi error
ALMONDLIBS = twi downlink
SRC = ../main.c ../../sensors/bmp085.c
EXTRAINCDIRS = ../../sensors/ ../ ../../
TARGETTYPE = NUT
ENABLE_DEBUG = 0
include $(BASE)/make/base.makefile
