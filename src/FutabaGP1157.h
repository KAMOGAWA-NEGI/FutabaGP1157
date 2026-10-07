#pragma once

#include <Arduino.h>
#include <Stream.h>

#define FUTABA_GP1157_VERSION_MAJOR 0
#define FUTABA_GP1157_VERSION_MINOR 1
#define FUTABA_GP1157_VERSION_PATCH 0

/**
 * @brief FUTABA GP1157A01 / GP1157A01B 系VFD用の非公式Arduinoライブラリ。
 *
 * 通信路は Stream で受け取るため、ライブラリ自体は特定のUART実装に依存しません。
 * UARTの初期化（38400bps / 8N1）とRS-232Cレベル変換は利用側で行ってください。
 */
class FutabaGP1157 {
public:
  static constexpr uint16_t ScreenWidth  = 256;
  static constexpr uint16_t ScreenHeight = 64;

  enum class Brightness : uint8_t {
    Off        = 0x10,
    Percent25  = 0x01,
    Percent50  = 0x02,
    Percent75  = 0x03,
    Percent100 = 0x04,
  };

  explicit FutabaGP1157(Stream& stream);

  /**
   * GP1157を既知のテキスト表示状態へ初期化します。
   * resetDisplay=true の場合は ESC @ を送信し、その後8x16/漢字ON/等倍/上書きモードへ設定します。
   * clearDisplay=true の場合は最後に画面をクリアします。
   */
  void begin(bool resetDisplay = true, bool clearDisplay = true);

  void reset();
  void clear();

  /** Xは1dot単位、Yは8dot単位へ切り捨てて送信します。 */
  void setCursor(uint16_t xDot, uint16_t yDot);

  void setFont8x16();
  void setKanjiMode(bool enabled);
  void setScale(uint8_t x, uint8_t y);
  void setBold(bool enabled);

  void setBrightness(Brightness brightness);
  void setBrightnessCode(uint8_t code);
  void setBrightnessPercent(uint8_t percent);

  void setOverwriteMode();
  void setHorizontalScrollMode();
  void setScrollSpeed(uint8_t speed);

  /** 16x16漢字表示向けの標準設定（8x16 + 漢字ON + 1倍）。 */
  void setText16Mode();

  /** 16x16漢字を2倍拡大して32x32表示する設定。 */
  void setText32Mode();

  /**
   * UTF-8文字列をShift-JISへ変換して表示します。
   * maxPixelsを超える手前で送信を止めます。
   * 戻り値は消費した横方向のdot数です。
   */
  uint16_t print(const char* utf8, uint16_t maxPixels = ScreenWidth);
  uint16_t print(const String& utf8, uint16_t maxPixels = ScreenWidth);

  /** 指定倍率を幅計算に使ってUTF-8文字列を表示します。setScale()自体は呼びません。 */
  uint16_t printScaled(const char* utf8, uint8_t scale, uint16_t maxPixels = ScreenWidth);
  uint16_t printScaled(const String& utf8, uint8_t scale, uint16_t maxPixels = ScreenWidth);

  /** 座標指定 + UTF-8表示。右端までを自動的に最大幅とします。 */
  uint16_t printAt(uint16_t xDot, uint16_t yDot, const char* utf8);
  uint16_t printAt(uint16_t xDot, uint16_t yDot, const String& utf8);

  /** 16x16モードへ切り替えた後、指定位置へ表示します。 */
  uint16_t printAt16(uint16_t xDot, uint16_t yDot, const char* utf8);
  uint16_t printAt16(uint16_t xDot, uint16_t yDot, const String& utf8);

  /** 32x32モードへ切り替えた後、指定位置へ表示します。 */
  uint16_t printAt32(uint16_t xDot, uint16_t yDot, const char* utf8);
  uint16_t printAt32(uint16_t xDot, uint16_t yDot, const String& utf8);

  /** UTF-8文字列の概算表示幅（dot）を返します。 */
  uint16_t measure(const char* utf8, uint8_t scale = 1) const;
  uint16_t measure(const String& utf8, uint8_t scale = 1) const;

  /** Shift-JISコードをそのまま1文字分送信します。 */
  size_t writeShiftJIS(uint16_t sjis);

  /** 生データ送信用。 */
  size_t writeRaw(const uint8_t* data, size_t length, bool flushAfter = true);
  size_t writeRaw(uint8_t data, bool flushAfter = true);

  /** BMP内Unicode 1文字をShift-JISへ変換。未対応時は0。 */
  static uint16_t unicodeToShiftJIS(uint16_t unicode);

  bool bold() const { return _bold; }
  uint8_t scaleX() const { return _scaleX; }
  uint8_t scaleY() const { return _scaleY; }
  uint8_t brightnessCode() const { return _brightnessCode; }

private:
  Stream& _stream;
  bool _bold = false;
  uint8_t _scaleX = 1;
  uint8_t _scaleY = 1;
  uint8_t _brightnessCode = 0x04;

  void sendCommand(const uint8_t* data, size_t length);
  uint16_t printInternal(const char* utf8, uint8_t scale, uint16_t maxPixels);
  uint16_t measureInternal(const char* utf8, uint8_t scale) const;

  static bool nextUtf8Codepoint(const char* text, size_t length, size_t& pos, uint32_t& codepoint);
  static uint16_t tableUnicode(size_t index);
  static uint16_t tableShiftJIS(size_t index);
};
