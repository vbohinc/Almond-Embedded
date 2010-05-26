#include <common/timer.h>

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
  uint32_t totalinc = (sec*32000)/1024;
  uint8_t remainwait = totalinc%256;
  OCR2 = 255;
  TCNT2 = 0;
  while(ASSR&(1<<OCR2UB) != 0);
  while(ASSR&(1<<TCR2UB) != 0);
  for(uint16_t tics = totalinc/256; tics > 0; tics--)
  {
    cli();
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
  }
  cli();
  OCR2 = remainwait;
  while(ASSR&(1<<OCR2UB) != 0);
  sleep_enable();
  sei();
  sleep_cpu();
  sleep_disable();
}
