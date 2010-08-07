/*
*/

#define LED1_DDR DDRD
#define LED1_PORT PORTD
#define LED1_PIN 7

#include <bluetooth/bluetooth.h>
#include <common/timer.h>
#include <avr/sleep.h>

void init_timer()
{
  ASSR |= (1<<AS2);
  TCCR2 |= (1<<CS02) | (1<<CS00);
  TCCR2 &= ~(1<<CS01);
  TIMSK |= OCIE2;
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);
}

void start_sleep(uint16_t sec)
{
  if (sec == 0)
    return;
  uint32_t totalinc = (sec*32000)/1024;
  uint8_t remainwait = totalinc%256;
  OCR2 = 255;
  TCNT2 = 0;
  while((ASSR&(1<<OCR2UB)) != 0);
  while((ASSR&(1<<TCR2UB)) != 0);
  debug_pgm(PSTR("3"));
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
  sleep_disable();
  
}
