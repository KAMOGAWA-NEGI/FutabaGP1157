# FutabaGP1157

FUTABA **GP1157A01 / GP1157A01B 系VFDモジュール**をArduino環境から扱うための非公式ライブラリです。

このライブラリは、GP1157をRaspberry Pi Pico Wから制御する実機プロジェクトから切り出したものです。現在の `v0.1.0` では、**実機で確認できた機能だけ**をAPI化しています。

## できること

- RS-232C経由でGP1157を制御
- 256×64ドット画面の座標指定
- UTF-8文字列をShift-JISへ変換して日本語表示
- 16×16漢字表示
- 16×16漢字の2倍拡大による32×32表示
- ボールド表示
- 輝度 0 / 25 / 50 / 75 / 100 %
- 上書きモード
- 横スクロールモード
- スクロール速度設定
- 画面全体を消さず、必要な部分だけを書き換える部分更新

## 動作確認環境

現時点で実機確認している構成は次の通りです。

- Raspberry Pi Pico W
- Arduino-Pico 6.2.0
- FUTABA GP1157A01 系VFD
- UART: 38400 bps / 8N1
- Pico W とGP1157の間にRS-232Cレベル変換回路

他のArduino互換ボードでも `Stream` を使えるUARTであれば利用できる構造ですが、現時点では未確認です。

## 重要：GPIOへ直結しないでください

GP1157側のシリアルインターフェースは**RS-232Cレベル**です。Raspberry Pi Pico / ESP32などの3.3V UARTとは電圧レベルが異なります。

```text
Pico W UART TX ──> MAX3232等 ──> GP1157 RXD
Pico W UART RX <── MAX3232等 <── GP1157 TXD
GND            ──────────────── GND
```

PicoのGPIOをGP1157のRS-232C端子へ直接接続しないでください。

## インストール

### Arduino IDEへZIPから追加する場合

1. GitHubのReleasesからZIPを取得します。
2. Arduino IDEで `スケッチ → ライブラリをインクルード → .ZIP形式のライブラリをインストール` を選びます。
3. ZIPを指定します。

開発中は、このリポジトリの `FutabaGP1157` フォルダをArduinoの `libraries` フォルダへ置いても使えます。

## 最小コード

Raspberry Pi Pico WでGPIO0/1をUART0として使う例です。

```cpp
#include <FutabaGP1157.h>

FutabaGP1157 vfd(Serial1);

void setup() {
  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(38400, SERIAL_8N1);

  vfd.begin();
  vfd.printAt16(0, 0, "こんにちは");
  vfd.printAt16(0, 16, "日本語表示できました");
}

void loop() {}
```

Arduino IDEのソースコードはUTF-8で保存してください。ライブラリ内部でShift-JISへ変換してGP1157へ送ります。

## 32×32表示

GP1157の16×16漢字をX/Yとも2倍へ拡大します。

```cpp
vfd.setBold(true);
vfd.printAt32(0, 0,  "運転");
vfd.printAt32(0, 32, "正常");
```

## 部分更新

このライブラリは `print()` のたびに画面クリアを行いません。そのため、座標を指定して変更箇所だけを上書きできます。

例えば32×32モードで時刻 `03:19:42` を表示している場合、ASCII 1文字は16dot幅になるため、末尾の秒はX=96から始まります。

```cpp
vfd.setText32Mode();
vfd.setCursor(96, 24);
vfd.print("43", 32);
```

これにより、1秒ごとに画面全体を再描画する必要がなく、VFDのちらつきを大幅に減らせます。

`examples/PicoW_NTPClock` に、日付・時分・秒をそれぞれ必要な時だけ更新するNTP時計サンプルがあります。

## 輝度

```cpp
vfd.setBrightnessPercent(50);
```

または、列挙型を指定できます。

```cpp
vfd.setBrightness(FutabaGP1157::Brightness::Percent75);
```

GP1157側の設定に合わせて、利用できる値は0 / 25 / 50 / 75 / 100 %です。

## 横スクロール

```cpp
vfd.setText16Mode();
vfd.setHorizontalScrollMode();
vfd.setScrollSpeed(4);
vfd.setCursor(0, 24);
vfd.print("横スクロール表示テスト", 1024);
```

スクロールモードの細かな挙動はGP1157側の表示モードに依存します。

## 主なAPI

```cpp
vfd.begin();
vfd.reset();
vfd.clear();

vfd.setCursor(x, y);
vfd.setText16Mode();
vfd.setText32Mode();
vfd.setScale(x, y);
vfd.setBold(true);

vfd.setBrightnessPercent(75);
vfd.setOverwriteMode();
vfd.setHorizontalScrollMode();
vfd.setScrollSpeed(4);

vfd.print("日本語");
vfd.printAt16(0, 0, "16x16");
vfd.printAt32(0, 32, "大型");
```

## UTF-8 → Shift-JIS変換

ライブラリにはUnicode BMPから標準Shift-JISへ変換するテーブルを内蔵しています。

- ASCII: 1バイトで送信
- Shift-JIS 2バイト文字: 2バイトで送信
- 変換できない文字: `?` へ置換
- 絵文字などBMP外の文字: `?` へ置換

変換表のバイナリ容量はおよそ28KBです。そのため、フラッシュ容量の小さいAVR機では余裕が少なくなる可能性があります。

## サンプル

| サンプル | 内容 |
|---|---|
| `BasicText` | 日本語4行表示 |
| `LargeText` | 32×32大型文字 |
| `PartialUpdate` | 画面クリアなしの部分更新 |
| `HorizontalScroll` | 横スクロール |
| `PicoW_NTPClock` | Pico W + NTP時計、秒だけ部分更新 |

## ライブラリに含めない機能

Wi-Fi、Web UI、LittleFS、複数ページ管理、NTPそのものはGP1157固有の機能ではないため、ライブラリ本体には含めていません。

これらは上位アプリケーション側で実装し、表示処理だけを `FutabaGP1157` に任せる構成を想定しています。

## 現在のステータス

**v0.1.0**

まずは実機で確認済みのテキスト表示系機能を安定したAPIとして切り出した初版です。

未実装・今後検討する機能:

- GP1157のグラフィック描画機能
- ユーザーウィンドウ
- ダウンロード文字
- AND / OR / XOR合成描画
- その他の表示モード

未確認コマンドを推測で追加せず、実機確認できたものから順次追加する方針です。

## 資料

このプロジェクトではGP1157A01系のアプリケーションノート `AN-3225C` を参照しています。

メーカー資料そのものはこのリポジトリへ再配布しません。利用するモジュールの正式な資料を別途確認してください。

## ライセンス

MIT License

このライブラリはFUTABA / Noritake公式製品ではない、個人開発の非公式ライブラリです。
