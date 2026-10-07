#include <FutabaGP1157.h>

FutabaGP1157 vfd(Serial1);

void setup() {
  // Raspberry Pi Pico / Pico W の例
  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(38400, SERIAL_8N1);

  vfd.begin();
  vfd.printAt16(0, 0,  "こんにちは");
  vfd.printAt16(0, 16, "GP1157 日本語表示");
  vfd.printAt16(0, 32, "UTF-8 -> Shift-JIS");
  vfd.printAt16(0, 48, "表示成功！");
}

void loop() {}
