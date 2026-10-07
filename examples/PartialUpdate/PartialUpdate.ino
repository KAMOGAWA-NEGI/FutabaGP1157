#include <FutabaGP1157.h>

FutabaGP1157 vfd(Serial1);
uint8_t secondsValue = 0;
uint32_t lastUpdate = 0;

void setup() {
  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(38400, SERIAL_8N1);

  vfd.begin();

  // 画面全体は一度だけ描画。
  vfd.printAt16(0, 0, "部分更新テスト");
  vfd.printAt16(0, 16, "秒だけ書き換えます");

  vfd.setText32Mode();
  vfd.setCursor(0, 32);
  vfd.print("SEC:");
}

void loop() {
  if (millis() - lastUpdate >= 1000) {
    lastUpdate += 1000;

    char buf[3];
    snprintf(buf, sizeof(buf), "%02u", secondsValue);

    // 32x32モードのASCIIは1文字16dot幅。
    // "SEC:" = 4文字なので、秒2桁はX=64から上書きする。
    vfd.setCursor(64, 32);
    vfd.print(buf, 32);

    secondsValue = (secondsValue + 1) % 60;
  }
}
