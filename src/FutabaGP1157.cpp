#include "FutabaGP1157.h"
#include "GP1157_SJIS_Table.h"

#include <string.h>

FutabaGP1157::FutabaGP1157(Stream& stream)
  : _stream(stream) {}

void FutabaGP1157::begin(bool resetDisplay, bool clearDisplay) {
  if (resetDisplay) {
    reset();
  }

  setOverwriteMode();
  setFont8x16();
  setKanjiMode(true);
  setScale(1, 1);
  setBold(false);
  setBrightnessCode(_brightnessCode);

  if (clearDisplay) {
    clear();
  }
}

void FutabaGP1157::reset() {
  const uint8_t cmd[] = {0x1B, 0x40}; // ESC @
  sendCommand(cmd, sizeof(cmd));
  delay(100);
}

void FutabaGP1157::clear() {
  _stream.write((uint8_t)0x0C);
  _stream.flush();
  delay(15);
}

void FutabaGP1157::setCursor(uint16_t xDot, uint16_t yDot) {
  // GP1157側のY指定は8dot単位。
  const uint16_t y8 = yDot / 8;
  const uint8_t cmd[] = {
    0x1F, 0x24,
    (uint8_t)(xDot & 0xFF),
    (uint8_t)((xDot >> 8) & 0xFF),
    (uint8_t)(y8 & 0xFF),
    (uint8_t)((y8 >> 8) & 0xFF)
  };
  sendCommand(cmd, sizeof(cmd));
}

void FutabaGP1157::setFont8x16() {
  const uint8_t cmd[] = {0x1F, 0x28, 0x67, 0x01, 0x02};
  sendCommand(cmd, sizeof(cmd));
}

void FutabaGP1157::setKanjiMode(bool enabled) {
  const uint8_t cmd[] = {0x1F, 0x28, 0x67, 0x02, (uint8_t)(enabled ? 0x01 : 0x00)};
  sendCommand(cmd, sizeof(cmd));
}

void FutabaGP1157::setScale(uint8_t x, uint8_t y) {
  if (x < 1) x = 1;
  if (x > 4) x = 4;
  if (y < 1) y = 1;
  if (y > 4) y = 4;
  const uint8_t cmd[] = {0x1F, 0x28, 0x67, 0x40, x, y};
  sendCommand(cmd, sizeof(cmd));
  _scaleX = x;
  _scaleY = y;
}

void FutabaGP1157::setBold(bool enabled) {
  const uint8_t cmd[] = {0x1F, 0x28, 0x67, 0x41, (uint8_t)(enabled ? 0x01 : 0x00)};
  sendCommand(cmd, sizeof(cmd));
  _bold = enabled;
}

void FutabaGP1157::setBrightness(Brightness brightness) {
  setBrightnessCode((uint8_t)brightness);
}

void FutabaGP1157::setBrightnessCode(uint8_t code) {
  if (!(code == 0x10 || (code >= 0x01 && code <= 0x04))) {
    code = 0x04;
  }
  const uint8_t cmd[] = {0x1F, 0x58, code};
  sendCommand(cmd, sizeof(cmd));
  _brightnessCode = code;
}

void FutabaGP1157::setBrightnessPercent(uint8_t percent) {
  if (percent == 0) {
    setBrightness(Brightness::Off);
  } else if (percent <= 25) {
    setBrightness(Brightness::Percent25);
  } else if (percent <= 50) {
    setBrightness(Brightness::Percent50);
  } else if (percent <= 75) {
    setBrightness(Brightness::Percent75);
  } else {
    setBrightness(Brightness::Percent100);
  }
}

void FutabaGP1157::setOverwriteMode() {
  const uint8_t cmd[] = {0x1F, 0x01};
  sendCommand(cmd, sizeof(cmd));
}

void FutabaGP1157::setHorizontalScrollMode() {
  const uint8_t cmd[] = {0x1F, 0x03};
  sendCommand(cmd, sizeof(cmd));
}

void FutabaGP1157::setScrollSpeed(uint8_t speed) {
  if (speed > 0x1F) speed = 0x1F;
  const uint8_t cmd[] = {0x1F, 0x73, speed};
  sendCommand(cmd, sizeof(cmd));
}

void FutabaGP1157::setText16Mode() {
  setFont8x16();
  setKanjiMode(true);
  setScale(1, 1);
  setBold(_bold);
}

void FutabaGP1157::setText32Mode() {
  setFont8x16();
  setKanjiMode(true);
  setScale(2, 2);
  setBold(_bold);
}

uint16_t FutabaGP1157::print(const char* utf8, uint16_t maxPixels) {
  return printInternal(utf8, _scaleX, maxPixels);
}

uint16_t FutabaGP1157::print(const String& utf8, uint16_t maxPixels) {
  return print(utf8.c_str(), maxPixels);
}

uint16_t FutabaGP1157::printScaled(const char* utf8, uint8_t scale, uint16_t maxPixels) {
  if (scale < 1) scale = 1;
  if (scale > 4) scale = 4;
  return printInternal(utf8, scale, maxPixels);
}

uint16_t FutabaGP1157::printScaled(const String& utf8, uint8_t scale, uint16_t maxPixels) {
  return printScaled(utf8.c_str(), scale, maxPixels);
}

uint16_t FutabaGP1157::printAt(uint16_t xDot, uint16_t yDot, const char* utf8) {
  if (xDot >= ScreenWidth || yDot >= ScreenHeight) return 0;
  setCursor(xDot, yDot);
  return print(utf8, ScreenWidth - xDot);
}

uint16_t FutabaGP1157::printAt(uint16_t xDot, uint16_t yDot, const String& utf8) {
  return printAt(xDot, yDot, utf8.c_str());
}

uint16_t FutabaGP1157::printAt16(uint16_t xDot, uint16_t yDot, const char* utf8) {
  setText16Mode();
  return printAt(xDot, yDot, utf8);
}

uint16_t FutabaGP1157::printAt16(uint16_t xDot, uint16_t yDot, const String& utf8) {
  return printAt16(xDot, yDot, utf8.c_str());
}

uint16_t FutabaGP1157::printAt32(uint16_t xDot, uint16_t yDot, const char* utf8) {
  setText32Mode();
  return printAt(xDot, yDot, utf8);
}

uint16_t FutabaGP1157::printAt32(uint16_t xDot, uint16_t yDot, const String& utf8) {
  return printAt32(xDot, yDot, utf8.c_str());
}

uint16_t FutabaGP1157::measure(const char* utf8, uint8_t scale) const {
  if (scale < 1) scale = 1;
  if (scale > 4) scale = 4;
  return measureInternal(utf8, scale);
}

uint16_t FutabaGP1157::measure(const String& utf8, uint8_t scale) const {
  return measure(utf8.c_str(), scale);
}

size_t FutabaGP1157::writeShiftJIS(uint16_t sjis) {
  size_t written = 0;
  if (sjis <= 0xFF) {
    written = _stream.write((uint8_t)sjis);
  } else {
    written += _stream.write((uint8_t)(sjis >> 8));
    written += _stream.write((uint8_t)(sjis & 0xFF));
  }
  _stream.flush();
  return written;
}

size_t FutabaGP1157::writeRaw(const uint8_t* data, size_t length, bool flushAfter) {
  const size_t written = _stream.write(data, length);
  if (flushAfter) _stream.flush();
  return written;
}

size_t FutabaGP1157::writeRaw(uint8_t data, bool flushAfter) {
  const size_t written = _stream.write(data);
  if (flushAfter) _stream.flush();
  return written;
}

void FutabaGP1157::sendCommand(const uint8_t* data, size_t length) {
  _stream.write(data, length);
  _stream.flush();
}

uint16_t FutabaGP1157::printInternal(const char* utf8, uint8_t scale, uint16_t maxPixels) {
  if (!utf8) return 0;

  const size_t length = strlen(utf8);
  size_t pos = 0;
  uint16_t used = 0;

  while (pos < length) {
    uint32_t cp = 0;

    if (!nextUtf8Codepoint(utf8, length, pos, cp)) {
      const uint16_t width = 8U * scale;
      if ((uint32_t)used + width > maxPixels) break;
      _stream.write((uint8_t)'?');
      used += width;
      continue;
    }

    if (cp == '\r' || cp == '\n') break;

    const uint16_t sjis = (cp <= 0xFFFF) ? unicodeToShiftJIS((uint16_t)cp) : 0;
    const uint16_t width = (sjis > 0xFF ? 16U : 8U) * scale;

    if ((uint32_t)used + width > maxPixels) break;

    if (sjis == 0) {
      _stream.write((uint8_t)'?');
    } else if (sjis <= 0xFF) {
      _stream.write((uint8_t)sjis);
    } else {
      _stream.write((uint8_t)(sjis >> 8));
      _stream.write((uint8_t)(sjis & 0xFF));
    }

    used += width;
  }

  _stream.flush();
  return used;
}

uint16_t FutabaGP1157::measureInternal(const char* utf8, uint8_t scale) const {
  if (!utf8) return 0;

  const size_t length = strlen(utf8);
  size_t pos = 0;
  uint32_t width = 0;

  while (pos < length) {
    uint32_t cp = 0;
    if (!nextUtf8Codepoint(utf8, length, pos, cp)) {
      width += 8U * scale;
      continue;
    }

    if (cp == '\r' || cp == '\n') break;

    const uint16_t sjis = (cp <= 0xFFFF) ? unicodeToShiftJIS((uint16_t)cp) : 0;
    width += (sjis > 0xFF ? 16U : 8U) * scale;

    if (width > 0xFFFF) return 0xFFFF;
  }

  return (uint16_t)width;
}

bool FutabaGP1157::nextUtf8Codepoint(const char* text, size_t length, size_t& pos, uint32_t& cp) {
  if (!text || pos >= length) return false;

  const uint8_t* data = reinterpret_cast<const uint8_t*>(text);
  const uint8_t c = data[pos++];

  if (c < 0x80) {
    cp = c;
    return true;
  }

  if ((c & 0xE0) == 0xC0) {
    if (pos >= length) return false;
    const uint8_t b = data[pos++];
    if ((b & 0xC0) != 0x80) return false;
    cp = ((uint32_t)(c & 0x1F) << 6) | (b & 0x3F);
    return true;
  }

  if ((c & 0xF0) == 0xE0) {
    if (pos + 1 >= length) return false;
    const uint8_t b = data[pos++];
    const uint8_t d = data[pos++];
    if ((b & 0xC0) != 0x80 || (d & 0xC0) != 0x80) return false;
    cp = ((uint32_t)(c & 0x0F) << 12) |
         ((uint32_t)(b & 0x3F) << 6) |
         (uint32_t)(d & 0x3F);
    return true;
  }

  if ((c & 0xF8) == 0xF0) {
    if (pos + 2 >= length) return false;
    pos += 3;
    cp = 0xFFFFFFFF;
    return true;
  }

  return false;
}

uint16_t FutabaGP1157::tableUnicode(size_t index) {
#if defined(ARDUINO_ARCH_AVR)
  return pgm_read_word(&GP1157_SJIS_MAP[index].unicode);
#else
  return GP1157_SJIS_MAP[index].unicode;
#endif
}

uint16_t FutabaGP1157::tableShiftJIS(size_t index) {
#if defined(ARDUINO_ARCH_AVR)
  return pgm_read_word(&GP1157_SJIS_MAP[index].sjis);
#else
  return GP1157_SJIS_MAP[index].sjis;
#endif
}

uint16_t FutabaGP1157::unicodeToShiftJIS(uint16_t unicode) {
  if (unicode <= 0x7F) return unicode;

  size_t lo = 0;
  size_t hi = GP1157_SJIS_MAP_COUNT;

  while (lo < hi) {
    const size_t mid = lo + (hi - lo) / 2;
    const uint16_t mappedUnicode = tableUnicode(mid);
    if (mappedUnicode < unicode) {
      lo = mid + 1;
    } else {
      hi = mid;
    }
  }

  if (lo < GP1157_SJIS_MAP_COUNT && tableUnicode(lo) == unicode) {
    return tableShiftJIS(lo);
  }

  return 0;
}
