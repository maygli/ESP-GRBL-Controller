#include "GrblCtrl_Board.h"
#include <Arduino.h>

GrblCtrl_Board Board;

void setup() {
  Board.init();
}



void loop(void) {
  Board.process();
}


