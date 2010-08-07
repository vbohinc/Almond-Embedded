/*
*/

#define LED1_DDR DDRD
#define LED1_PORT PORTD
#define LED1_PIN 7

#include <bluetooth/bluetooth.h>
#include <common/timer.h>
#include <avr/sleep.h>
#include <util/delay.h>




void init_timer()
{
    /*ASSR  = (1<< AS2);              // Timer2 asynchron takten
    _delay_ms(1000);               // Einschwingzeit des 32kHz Quarzes
    TCCR2  = 6;                     // Vorteiler 256 -> 2s Überlaufperiode
    while((ASSR & (1<< TCR2UB)));   // Warte auf das Ende des Zugriffs
    TIFR   = (1<<TOV2);             // Interrupts löschen (*)
    TIMSK |= (1<<TOIE2);            // Timer overflow Interrupt freischalten*/


/*
  ASSR |= (1<<AS2);
  _delay_ms(1000);
  TCCR2 = (1<<CS02) | (1<<CS00);

  while((ASSR & (1<< TCR2UB)));   // Warte auf das Ende des Zugriffs
  TIFR   = (1<<TOV2);             // Interrupts löschen (*)
  TIMSK |= (1<<TOIE2);            // Timer overflow Interrupt freischalten*/

}


void sleep_sec()
{
	_delay_ms(1000);
	/*OCR2 = 0;                       // Dummyzugriff
	while((ASSR & (1<< OCR2UB)));   // Warte auf das Ende des Zugriffs

	set_sleep_mode(SLEEP_MODE_PWR_SAVE);

	sleep_mode();                   // in den Schlafmodus wechseln*/

}

void start_sleep(uint16_t sec)
{

  for (uint16_t i=0; i<sec; i++)
  {

      LED1_PORT &= ~(1<<LED1_PIN);
	  sleep_sec();
	  LED1_PORT |= (1<<LED1_PIN);

  }

  // WICHTIG!
  // Wenn der Timer2 im asynchronen Modus periodisch zum Wecken aus dem Sleep Mode
  // genutzt wird, dann muss vor dem Wiedereintritt mindestens
  // 1 Oszillatortakt des Timers abgewartet werden (~30us), um die Interruptlogik
  // wieder zu aktivieren, anderenfalls wacht der AVR nicht mehr auf.
  // Die folgenden zwei Zeilen tun dies.
  // Nur wenn sichergestellt ist, dass der Interrupt + Hauptschleife länger als 30us dauern,
  // kann man den Test weglassen



 /* uint32_t totalinc = (sec*32000)/1024;
  uint8_t remainwait = totalinc%256;
  OCR2 = 255;
  TCNT2 = 0;
  while((ASSR&(1<<OCR2UB)) != 0);
  while((ASSR&(1<<TCR2UB)) != 0);
  debug_pgm(PSTR("3"));
  if (totalinc==58)
	  debug_pgm(PSTR("a"));
  else
	  debug_pgm(PSTR("b"));
  for(uint16_t tics = totalinc/256; tics > 0; tics--)
  {
    debug_pgm(PSTR("4"));
    cli();
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
    debug_pgm(PSTR("5")); 
  }
  debug_pgm(PSTR("6")); 
  cli();
  OCR2 = remainwait;
  debug_pgm(PSTR("7"));
  while((ASSR&(1<<OCR2UB)) != 0);
  debug_pgm(PSTR("8"));
  sleep_enable();
  debug_pgm(PSTR("9"));
  sei();
  debug_pgm(PSTR("1"));
  sleep_cpu();
  debug_pgm(PSTR("2"));
  sleep_disable();*/
  
}
