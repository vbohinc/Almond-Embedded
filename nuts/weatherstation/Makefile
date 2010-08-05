MCU = atmega8535
F_CPU = 16000000
TARGET = main
BASE = ../..
#ALMONDLIBS = twi error
ALMONDLIBS = downlink twi
SRC = ../sensors/bmp085.c ../common/timer.c
EXTRAINCDIRS = ../sensors/ ../
TARGETTYPE = NUT
ENABLE_DEBUG = 1
include ../../make/base.makefile
