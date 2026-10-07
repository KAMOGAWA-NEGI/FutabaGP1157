#include <WiFi.h>
#include <time.h>
#include <FutabaGP1157.h>

const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASS = "YOUR_PASSWORD";
const char* TZ_INFO = "JST-9";

FutabaGP1157 vfd(Serial1);

int lastYear = -1;
int lastYDay = -1;
int lastHour = -1;
int lastMinute = -1;
int lastSecond = -1;

String weekdayJa(int wday) {
  static const char* d[] = {"日", "月", "火", "水", "木", "金", "土"};
  if (wday < 0 || wday > 6) return "?";
  return d[wday];
}

void drawFullClock(const struct tm& t) {
  char dateBuf[24];
  char timeBuf[16];

  snprintf(dateBuf, sizeof(dateBuf), "%04d/%02d/%02d",
           t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
  snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d",
           t.tm_hour, t.tm_min, t.tm_sec);

  vfd.clear();
  vfd.setText16Mode();
  vfd.setCursor(0, 0);
  vfd.print(String(dateBuf) + " " + weekdayJa(t.tm_wday) + "曜日");

  vfd.setText32Mode();
  vfd.setCursor(0, 24);
  vfd.print(timeBuf);
}

void updateClockPartial(const struct tm& t) {
  // 日付が変わった時だけ日付行を書き換える。
  if (t.tm_year != lastYear || t.tm_yday != lastYDay) {
    char dateBuf[24];
    snprintf(dateBuf, sizeof(dateBuf), "%04d/%02d/%02d",
             t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);

    vfd.setText16Mode();
    vfd.setCursor(0, 0);
    vfd.print(String(dateBuf) + " " + weekdayJa(t.tm_wday) + "曜日", 256);
  }

  // 時または分が変わった時だけ HH:MM: を更新。
  if (t.tm_hour != lastHour || t.tm_min != lastMinute) {
    char hmBuf[8];
    snprintf(hmBuf, sizeof(hmBuf), "%02d:%02d:", t.tm_hour, t.tm_min);

    vfd.setText32Mode();
    vfd.setCursor(0, 24);
    vfd.print(hmBuf, 96);
  }

  // 毎秒は末尾2文字だけ上書き。
  if (t.tm_sec != lastSecond) {
    char secBuf[3];
    snprintf(secBuf, sizeof(secBuf), "%02d", t.tm_sec);

    vfd.setText32Mode();
    vfd.setCursor(96, 24);
    vfd.print(secBuf, 32);
  }

  lastYear = t.tm_year;
  lastYDay = t.tm_yday;
  lastHour = t.tm_hour;
  lastMinute = t.tm_min;
  lastSecond = t.tm_sec;
}

void setup() {
  Serial.begin(115200);

  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(38400, SERIAL_8N1);
  vfd.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
  }

  setenv("TZ", TZ_INFO, 1);
  tzset();
  NTP.begin("ntp.nict.jp", "pool.ntp.org");

  while (time(nullptr) < 1700000000) {
    delay(250);
  }

  time_t now = time(nullptr);
  struct tm t;
  localtime_r(&now, &t);
  drawFullClock(t);

  lastYear = t.tm_year;
  lastYDay = t.tm_yday;
  lastHour = t.tm_hour;
  lastMinute = t.tm_min;
  lastSecond = t.tm_sec;
}

void loop() {
  static uint32_t lastTick = 0;
  if (millis() - lastTick < 100) return;
  lastTick = millis();

  time_t now = time(nullptr);
  if (now < 1700000000) return;

  struct tm t;
  localtime_r(&now, &t);
  updateClockPartial(t);
}
