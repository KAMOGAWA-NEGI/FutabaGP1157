#include <FutabaGP1157.h>

FutabaGP1157 vfd(Serial1);

void setup() {
  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(38400, SERIAL_8N1);

  vfd.begin();
  vfd.setText16Mode();
  vfd.setHorizontalScrollMode();
  vfd.setScrollSpeed(4);
  vfd.setCursor(0, 24);
  vfd.print("GP1157 横スクロール表示テスト　こんにちは！", 1024);
}

void loop() {}
