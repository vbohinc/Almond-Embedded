#MCU = atmega8535
MCU = atxmega128a1
F_CPU = 32000000
TARGET = spi_test
BASE = ..
ALMONDLIBS = uart button error gui
SRC = spi_test.c ../drivers/spi/spi.c ../drivers/storage/sd.c #../drivers/display/display.c
EXTRAINCDIRS =
TARGETTYPE = SQUIRREL
ENABLE_DEBUG = 1
include ../make/base.makefile 
