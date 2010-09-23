MCU = atmega8535
F_CPU = 16000000
TARGET = main
BASE = ../../..
#ALMONDLIBS = twi error
ALMONDLIBS = twi error downlink
SRC = ../main.c ../../sensors/bmp085.c
EXTRAINCDIRS = ../../sensors/ ../ ../../
TARGETTYPE = NUT
ENABLE_DEBUG = 1
include $(BASE)/make/base.makefile
