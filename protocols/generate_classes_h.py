import sqlite3
import time

conn = sqlite3.connect('almond.db')
c = conn.cursor()
print "/**\n *\n * classes.h\n * Common classes definition \n * Generated at:",time.strftime("%d %b %Y %H:%M:%S"),"\n*/\n#ifndef __CLASSES_H__\n#define __CLASSES_H__\n"
print "\nenum class_id_nuts {\n"
c.execute('select * from device_classes')
for row in c:
	if row[1] == 0:
		print row[2].upper(), " = ", row[1],
	else:
		print ",\n",row[2].upper(), " = ", row[1],
print "\n};\n"
c.close
c = conn.cursor()
c.execute('select * from device_properties')
print "\nenum class_id_sensors {\n"
for row in c:
	if row[4] == 1:
		if row[1] == 0:
			print row[2].upper(), " = ", row[1],
		else:
			print ",\n",row[2].upper(), " = ", row[1],
	elif row[4] == 2:
		if row[1] == 128:
			print "\n};\n\nenum class_id_actuators {\n",row[2].upper(), " = ", row[1],
		else:
			print ",\n",row[2].upper(), " = ", row[1],
print "\n};\n"