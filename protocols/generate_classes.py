#!/usr/bin/python

# generate_classes.py: classes.h generator
#
# TODO: Fix DB Layout
#

import sqlite3
import time

class Generator(object):

    def __init__(self):
        self.__header = ''
        self.h('/**')
        self.h(' *')
        self.h(' * classes.h')
        self.h(' *')
        self.h(' * Common classes definition')
        self.h(' * Generated at: ' + time.strftime("%d %b %Y %H:%M:%S")) 
        self.h(' */')
        self.h('')
        self.h('#ifndef __CLASSES_H__')
        self.h('#define __CLASSES_H__')
        self.h('')
        
    def h(self, s):
        if isinstance(s, unicode):
            s = s.encode('utf-8')
        self.__header += s
        self.__header += '\n'
    
    def hh(self, s):
        if isinstance(s, unicode):
            s = s.encode('utf-8')
        self.__header += s
        #self.__header += '\n'


    def parse_class_id_nuts(self):
        self.h('enum class_id_nuts {')
        
        conn = sqlite3.connect('almond.db')
        c = conn.cursor()
        c.execute('select * from device_classes')
        for row in c:
        	if row[1] == 0:
		        self.hh("\t" + str(row[2].upper()) + " = " + hex(row[1]))
	        else:
		        self.hh(",\n\t" + str(row[2].upper()) + " = " + hex(row[1]))
        c.close
    
        self.h('\n};')
        self.h('')
		
    def parse_class_id_extensions(self):
        self.h('enum class_id_sensors {')
        
        conn = sqlite3.connect('almond.db')
        c = conn.cursor()
        c.execute('select * from device_properties where property_type is 1') 
        # FIXME SENSORS
        for row in c:
        	if row[1] == 0:
		        self.hh("\t" + str(row[2].upper()) + " = " + hex(row[1]))
	        else:
		        self.hh(",\n\t" + str(row[2].upper()) + " = " + hex(row[1]))
        c.close

        self.h('\n};')
        self.h('')
        
        self.h('enum class_id_actuators {')
        c = conn.cursor()
        c.execute('select * from device_properties where property_type is 2') 
        # FIXME ACTUATORS
	first = True
        for row in c:
        	if first:
		        self.hh("\t" + str(row[2].upper()) + " = " + hex(row[1]))
		        first = False
	        else:
		        self.hh(",\n\t" + str(row[2].upper()) + " = " + hex(row[1]))
        c.close

        self.h('\n};')
        self.h('')

    def print_header(self):
        print self.__header
		

if __name__ == '__main__':

    generator = Generator()
    generator.parse_class_id_nuts()
    generator.parse_class_id_extensions()
    generator.print_header()
