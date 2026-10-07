#include <FutabaGP1157.h>

FutabaGP1157 vfd(Serial1);

void setup() {
  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(38400, SERIAL_8N1);

  vfd.begin();
  vfd.setBold(true);
  vfd.printAt32(0, 0,  "運転");
  vfd.printAt32(0, 32, "正常");
}

void loop() {}
