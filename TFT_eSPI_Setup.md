# TFT_eSPI 库配置说明

## 配置步骤

### 1. 找到 TFT_eSPI 库文件夹
通常在以下位置：
- Windows: `C:\Users\[用户名]\Documents\Arduino\libraries\TFT_eSPI`
- macOS: `~/Documents/Arduino/libraries/TFT_eSPI`
- Linux: `~/Arduino/libraries/TFT_eSPI`

### 2. 编辑 User_Setup.h 文件
打开 `User_Setup.h` 文件，找到并修改以下配置：

```cpp
// 注释掉其他开发板配置，只保留 Wio Terminal
//#define ST7735_DRIVER
#define ILI9341_DRIVER

// 屏幕分辨率
#define TFT_WIDTH  320
#define TFT_HEIGHT 240

// 颜色深度
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel high font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// 启用 SPI 优化
#define SPI_FREQUENCY  40000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000

// 启用 DMA（如果支持）
#define DMA_ENABLE

// 启用字体支持
#define SMOOTH_FONT
```

### 3. 检查 User_Setup_Select.h 文件
确保 `User_Setup_Select.h` 文件中包含：

```cpp
#include <User_Setup.h>
```

### 4. 验证配置
编译项目时，如果看到类似以下输出，说明配置正确：
```
Sketch uses 123456 bytes (45%) of program storage space. Maximum is 262144 bytes.
Global variables use 12345 bytes (15%) of dynamic memory, leaving 67890 bytes for local variables. Maximum is 81920 bytes.
```

## 常见问题

### 编译错误：找不到 TFT_eSPI
1. 确保库已正确安装
2. 重启 Arduino IDE
3. 检查库文件夹路径

### 显示异常或黑屏
1. 检查屏幕分辨率设置
2. 确认驱动类型（ILI9341）
3. 检查 SPI 频率设置

### 性能问题
1. 降低 SPI 频率
2. 禁用不必要的字体
3. 减少同时显示的图形元素

## 测试代码

上传以下测试代码验证配置：

```cpp
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(50, 100);
  tft.println("TFT_eSPI Test OK!");
}

void loop() {
  // 空循环
}
```

如果能看到 "TFT_eSPI Test OK!" 文字，说明配置成功。 