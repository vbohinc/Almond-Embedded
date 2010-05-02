#include <../sensors/twi.h>
#include <util/twi.h>

void twi_start() {
  TWSR = 0;
  TWBR = 10;
}

void twi_stop() {

}
