 # summiere und zähle die Werte der Spalte 2 derjenigen Zeilen einer Textdatei,
  # die mit dem String "daten" beginnen oder dem String "wert" enden
  
  /^.text|^.data|^.bss|^.eeprom/ {
	if (NR == 3)
		rom = $2
	if (NR == 4)
	{
		data = $2
	}
	if (NR == 5)
		ram = $2
	if (NR == 6)
		eeprom = $2
  }
  
  # Summe und Durchschnitt formatiert ausgeben
  END {
	rom += data;
	ram += data;
	printf("%d %d %d %d\n", rom, ram, data, eeprom);
  
  }

