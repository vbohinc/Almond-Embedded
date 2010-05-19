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


    def parse_class_id_nuts(self):
        self.h('enum class_id_nuts {')
        
        conn = sqlite3.connect('almond.db')
        c = conn.cursor()
        c.execute('select * from device_classes')
        for row in c:
        	if row[1] == 0:
		        self.h("\t" + str(row[2].upper()) + " = " + str(row[1]) + ",")
	        else:
		        self.h("\t" + str(row[2].upper()) + " = " + str(row[1]))
        c.close
    
        self.h('};')
        self.h('')
		
    def parse_class_id_extensions(self):
        self.h('enum class_id_extensions {')
        
        conn = sqlite3.connect('almond.db')
        c = conn.cursor()
        c.execute('select * from device_properties') 
        # FIXME
        for row in c:
        	if row[1] == 0:
		        self.h("\t" + str(row[2].upper()) + " = " + str(row[1]) + ",")
	        else:
		        self.h("\t" + str(row[2].upper()) + " = " + str(row[1]))
        c.close

        self.h('};')
        self.h('')

    def print_header(self):
        print self.__header
		

if __name__ == '__main__':
    generator = Generator()
    generator.parse_class_id_nuts()
    generator.parse_class_id_extensions()
    generator.print_header()
