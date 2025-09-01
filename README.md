# Wio Terminal 超级玛丽游戏

## 项目概述

这是一个为 Seeed Wio Terminal 开发的完整超级玛丽游戏项目，采用面向对象的设计模式和模块化架构，实现了经典平台游戏的核心功能。游戏具有完整的平台跳跃玩法，包括角色控制、敌人AI、道具收集、音效系统、物理引擎和成功条件。

### 项目特色
- 🎮 **经典游戏体验**：完整的超级玛丽式平台跳跃玩法
- 🏗️ **模块化架构**：清晰的代码结构，易于扩展和维护
- 🎯 **智能AI系统**：敌人自动寻路和智能行为
- 🔊 **丰富音效**：完整的音效系统，增强游戏体验
- 📱 **触摸友好**：针对Wio Terminal触摸屏优化
- 🔧 **易于配置**：详细的配置说明和故障排除指南

## 🎥 游戏演示

### 视频展示
[![Wio Terminal 超级玛丽游戏演示](https://img.shields.io/badge/观看完整演示视频-点击这里-red?style=for-the-badge&logo=youtube)](https://github.com/你的用户名/wio_terminal_project/issues/1)

### 游戏截图
| 游戏开始界面 | 游戏进行中 | 成功界面 |
|-------------|-----------|----------|
| ![开始界面](https://via.placeholder.com/300x200/4CAF50/FFFFFF?text=游戏开始界面) | ![游戏进行](https://via.placeholder.com/300x200/2196F3/FFFFFF?text=游戏进行中) | ![成功界面](https://via.placeholder.com/300x200/FF9800/FFFFFF?text=成功界面) |

### 功能演示
- 🎮 **跳跃机制演示**：展示连续跳跃和高度控制
- 🪙 **金币收集**：收集金币的音效和分数增加
- 🌸 **道具生成**：顶撞问号方块生成花朵
- 👾 **敌人互动**：踩踏敌人的机制演示
- 🏆 **成功庆祝**：完成游戏的成功界面和音效

### 快速预览
```bash
# 克隆项目
git clone https://github.com/你的用户名/wio_terminal_project.git

# 查看演示视频
# 点击上方"观看完整演示视频"按钮
```

## 硬件要求

### 必需硬件
- **Seeed Wio Terminal** - 主开发板
- **USB Type-C 数据线** - 用于程序上传和供电
- **电脑** - Windows/macOS/Linux 系统

### 软件要求
- **Arduino IDE** (推荐 2.x 版本，支持更好的库管理)
- **TFT_eSPI 库** (需要特殊配置以适配Wio Terminal)
- **Arduino SAMD 核心** (用于SAMD21处理器支持)

### 系统要求
- **操作系统**：Windows 10+, macOS 10.14+, Ubuntu 18.04+
- **内存**：至少 4GB RAM
- **存储空间**：至少 2GB 可用空间

## 游戏特性

### 核心玩法
- **平台跳跃**：经典的超级玛丽式跳跃机制
- **道具收集**：收集金币和花朵获得分数
- **敌人互动**：踩踏敌人获得额外分数
- **障碍物挑战**：避开红色障碍物，触碰即游戏结束
- **方块互动**：顶问号方块生成花朵

### 角色控制
- **A按钮**：跳跃控制
  - 短按：普通跳跃
  - 连续按：在空中累加跳跃高度（有最大高度限制）
  - 长按（2秒）：进入按钮测试模式
- **B按钮**：移动控制
  - 切换自动移动状态
  - 到达屏幕边缘自动转向
- **C按钮**：游戏控制
  - 短按：开始游戏/暂停/继续/重新开始
  - 长按：重新开始游戏

### 游戏元素

#### 角色
- **玛丽**：主角，具有详细的像素艺术外观
  - 红色帽子、蓝色背带、黄色背带扣
  - 白色手套、棕色鞋子
  - 支持跳跃计数显示

#### 道具
- **金币**：黄色圆形，收集获得100分
- **花朵**：红色花朵，收集获得200分
- **问号方块**：黄色方块，顶撞生成花朵

#### 方块
- **普通方块**：棕色方块，提供跳跃平台
- **问号方块**：黄色方块，顶撞生成花朵
- **障碍物**：红色方块，触碰游戏结束

#### 敌人
- **Goomba**：棕色敌人，左右移动
- **踩踏机制**：从上方踩踏消灭敌人，获得200分

### 音效系统
- **跳跃音效**：600Hz，80ms
- **金币音效**：800Hz，100ms
- **花朵音效**：1000Hz，150ms
- **游戏结束音效**：200Hz，500ms
- **成功音效**：三音阶上升音效（1200Hz→1400Hz→1600Hz）

### 游戏状态
- **开始菜单**：显示游戏标题和控制说明
- **游戏进行**：正常游戏状态
- **暂停状态**：游戏暂停，显示暂停界面
- **游戏结束**：生命值为0或触碰障碍物
- **成功状态**：收集所有金币后显示成功界面

### 成功条件
- **收集所有金币**：12个金币全部收集完成
- **成功界面**：绿色背景，显示"SUCCESS!"和最终分数
- **音效庆祝**：播放三音阶成功音效

## 技术实现

### 游戏架构
- **状态机**：使用枚举管理游戏状态
- **物理引擎**：重力、跳跃、碰撞检测
- **音效系统**：基于蜂鸣器的音效播放
- **图形渲染**：TFT_eSPI库的像素级绘制

### 核心算法
- **跳跃机制**：累加式跳跃高度计算，有最大高度限制
- **碰撞检测**：AABB碰撞检测算法
- **自动移动**：边界检测和方向切换
- **道具生成**：问号方块的动态道具生成
- **边界控制**：防止角色超出屏幕范围

### 数据结构
```cpp
struct Mario {
  float x, y;           // 位置
  float vx, vy;         // 速度
  int lives;            // 生命值
  int score;            // 分数
  bool onGround;        // 着地状态
  int direction;        // 朝向
  int jumpCount;        // 跳跃次数
};
```

## 安装和配置

### 1. 硬件连接
确保 Wio Terminal 正确连接到电脑，并安装相应的驱动程序。

### 2. Arduino IDE 设置
1. 打开 Arduino IDE
2. 选择开发板：`Tools > Board > Seeeduino SAMD > Seeed Wio Terminal`
3. 选择端口：`Tools > Port > [你的设备端口]`

### 3. TFT_eSPI 库配置
**重要**：需要特殊配置 TFT_eSPI 库以适配 Wio Terminal。

创建或修改 `TFT_eSPI_User_Setup.h` 文件：
```cpp
#define WIO_TERMINAL
#define TFT_WIDTH 320
#define TFT_HEIGHT 240
#define TFT_MISO -1
#define TFT_MOSI 10
#define TFT_SCLK 8
#define TFT_CS   7
#define TFT_DC   6
#define TFT_RST  5
#define TFT_BL   4
#define TFT_BACKLIGHT_ON HIGH
```

### 4. 编译和上传
1. 打开 `super_mario.ino` 文件
2. 点击编译按钮检查代码
3. 点击上传按钮将程序上传到设备

## 游戏操作指南

### 开始游戏
1. 设备启动后显示开始菜单
2. 按 C 按钮开始游戏
3. 使用 A 按钮跳跃，B 按钮控制移动

### 游戏目标
- 收集所有12个金币
- 避开红色障碍物
- 顶问号方块获得花朵
- 踩踏敌人获得额外分数

### 高级技巧
- **连续跳跃**：在空中多次按 A 按钮增加跳跃高度（有最大限制）
- **自动移动**：使用 B 按钮开启自动移动，专注于跳跃时机
- **按钮测试**：长按 A 按钮2秒进入按钮测试模式
- **边界控制**：跳跃不会超出屏幕顶部

### 常见问题
1. **编译错误**：确保 TFT_eSPI 库配置正确
2. **显示异常**：检查屏幕连接和库配置
3. **按钮无响应**：确认按钮引脚定义正确
4. **音效无声**：检查蜂鸣器连接
5. **角色超出屏幕**：已修复边界检测问题

### 调试信息
游戏通过串口输出详细的调试信息：
- 按钮状态变化
- 游戏状态切换
- 碰撞检测结果
- 分数变化

## 开发说明

### 代码结构
- **主程序**：`super_mario.ino` - 包含所有游戏逻辑
- **配置文档**：`TFT_eSPI_Setup.md` - 库配置说明
- **项目总结**：`PROJECT_SUMMARY.md` - 详细技术文档

### 扩展建议
- 添加更多关卡
- 实现存档功能
- 增加更多敌人类型
- 添加背景音乐
- 实现多人模式

## 许可证

本项目采用 MIT 许可证，详见 LICENSE 文件。

## 贡献

欢迎提交 Issue 和 Pull Request 来改进这个项目！

---

**享受游戏！** 🎮 

---

# Wio Terminal Super Mario Game

## Project Overview

This is a complete Super Mario game project developed for the Seeed Wio Terminal, featuring object-oriented design patterns and modular architecture that implements the core functionality of classic platform games. The game includes complete platform jumping gameplay with character control, enemy AI, item collection, sound effects, physics engine, and success conditions.

### Project Features
- 🎮 **Classic Gaming Experience**: Complete Super Mario-style platform jumping gameplay
- 🏗️ **Modular Architecture**: Clear code structure, easy to extend and maintain
- 🎯 **Intelligent AI System**: Enemy auto-pathfinding and smart behavior
- 🔊 **Rich Sound Effects**: Complete sound system to enhance gaming experience
- 📱 **Touch-Friendly**: Optimized for Wio Terminal touch screen
- 🔧 **Easy Configuration**: Detailed configuration instructions and troubleshooting guide

## 🎥 Game Demo

### Video Showcase
[![Wio Terminal Super Mario Game Demo](https://img.shields.io/badge/Watch_Full_Demo_Video-Click_Here-red?style=for-the-badge&logo=youtube)](https://github.com/your-username/wio_terminal_project/issues/1)

### Game Screenshots
| Start Menu | Gameplay | Success Screen |
|------------|----------|----------------|
| ![Start Menu](https://via.placeholder.com/300x200/4CAF50/FFFFFF?text=Start+Menu) | ![Gameplay](https://via.placeholder.com/300x200/2196F3/FFFFFF?text=Gameplay) | ![Success Screen](https://via.placeholder.com/300x200/FF9800/FFFFFF?text=Success+Screen) |

### Feature Demonstrations
- 🎮 **Jump Mechanism Demo**: Showcase continuous jumping and height control
- 🪙 **Coin Collection**: Coin collection sound effects and score increase
- 🌸 **Item Generation**: Hitting question blocks to generate flowers
- 👾 **Enemy Interaction**: Stomping enemy mechanism demonstration
- 🏆 **Success Celebration**: Game completion success screen and sound effects

### Quick Preview
```bash
# Clone the project
git clone https://github.com/your-username/wio_terminal_project.git

# Watch demo video
# Click the "Watch Full Demo Video" button above
```

## Hardware Requirements

### Required Hardware
- **Seeed Wio Terminal** - Main development board
- **USB Type-C Cable** - For program upload and power supply
- **Computer** - Windows/macOS/Linux system

### Software Requirements
- **Arduino IDE** (Recommended 2.x version with better library management)
- **TFT_eSPI Library** (Requires special configuration for Wio Terminal)
- **Arduino SAMD Core** (For SAMD21 processor support)

### System Requirements
- **Operating System**: Windows 10+, macOS 10.14+, Ubuntu 18.04+
- **Memory**: At least 4GB RAM
- **Storage**: At least 2GB available space

## Game Features

### Core Gameplay
- **Platform Jumping**: Classic Super Mario-style jumping mechanics
- **Item Collection**: Collect coins and flowers to earn points
- **Enemy Interaction**: Stomp enemies to earn extra points
- **Obstacle Challenge**: Avoid red obstacles, touching them ends the game
- **Block Interaction**: Hit question blocks to generate flowers

### Character Controls
- **A Button**: Jump control
  - Short press: Normal jump
  - Continuous press: Accumulate jump height in air (with maximum height limit)
  - Long press (2 seconds): Enter button test mode
- **B Button**: Movement control
  - Toggle auto-movement state
  - Auto-turn when reaching screen edges
- **C Button**: Game control
  - Short press: Start game/Pause/Continue/Restart
  - Long press: Restart game

### Game Elements

#### Character
- **Mario**: Main character with detailed pixel art appearance
  - Red hat, blue overalls, yellow overall buttons
  - White gloves, brown shoes
  - Jump count display support

#### Items
- **Coins**: Yellow circles, collect for 100 points
- **Flowers**: Red flowers, collect for 200 points
- **Question Blocks**: Yellow blocks, hit to generate flowers

#### Blocks
- **Regular Blocks**: Brown blocks providing jump platforms
- **Question Blocks**: Yellow blocks, hit to generate flowers
- **Obstacles**: Red blocks, touching ends the game

#### Enemies
- **Goomba**: Brown enemies moving left and right
- **Stomp Mechanism**: Stomp enemies from above to defeat them, earn 200 points

### Sound System
- **Jump Sound**: 600Hz, 80ms
- **Coin Sound**: 800Hz, 100ms
- **Flower Sound**: 1000Hz, 150ms
- **Game Over Sound**: 200Hz, 500ms
- **Success Sound**: Three-tone ascending sound (1200Hz→1400Hz→1600Hz)

### Game States
- **Start Menu**: Display game title and control instructions
- **Playing**: Normal game state
- **Paused**: Game paused, display pause interface
- **Game Over**: Lives = 0 or touching obstacles
- **Success**: Display success interface after collecting all coins

### Success Conditions
- **Collect All Coins**: Complete collection of all 12 coins
- **Success Interface**: Green background, display "SUCCESS!" and final score
- **Sound Celebration**: Play three-tone success sound

## Technical Implementation

### Game Architecture
- **State Machine**: Use enumeration to manage game states
- **Physics Engine**: Gravity, jumping, collision detection
- **Sound System**: Buzzer-based sound effect playback
- **Graphics Rendering**: TFT_eSPI library pixel-level drawing

### Core Algorithms
- **Jump Mechanism**: Accumulative jump height calculation with maximum height limit
- **Collision Detection**: AABB collision detection algorithm
- **Auto Movement**: Boundary detection and direction switching
- **Item Generation**: Dynamic item generation from question blocks
- **Boundary Control**: Prevent characters from going beyond screen boundaries

### Data Structures
```cpp
struct Mario {
  float x, y;           // Position
  float vx, vy;         // Velocity
  int lives;            // Lives
  int score;            // Score
  bool onGround;        // Ground state
  int direction;        // Direction
  int jumpCount;        // Jump count
};
```

## Installation and Configuration

### 1. Hardware Connection
Ensure Wio Terminal is properly connected to your computer and install appropriate drivers.

### 2. Arduino IDE Setup
1. Open Arduino IDE
2. Select board: `Tools > Board > Seeeduino SAMD > Seeed Wio Terminal`
3. Select port: `Tools > Port > [Your device port]`

### 3. TFT_eSPI Library Configuration
**Important**: Special configuration of TFT_eSPI library is required for Wio Terminal.

Create or modify `TFT_eSPI_User_Setup.h` file:
```cpp
#define WIO_TERMINAL
#define TFT_WIDTH 320
#define TFT_HEIGHT 240
#define TFT_MISO -1
#define TFT_MOSI 10
#define TFT_SCLK 8
#define TFT_CS   7
#define TFT_DC   6
#define TFT_RST  5
#define TFT_BL   4
#define TFT_BACKLIGHT_ON HIGH
```

### 4. Compile and Upload
1. Open `super_mario.ino` file
2. Click compile button to check code
3. Click upload button to upload program to device

## Game Operation Guide

### Starting the Game
1. Device displays start menu after boot
2. Press C button to start game
3. Use A button to jump, B button to control movement

### Game Objectives
- Collect all 12 coins
- Avoid red obstacles
- Hit question blocks to get flowers
- Stomp enemies for extra points

### Advanced Techniques
- **Continuous Jumping**: Press A button multiple times in air to increase jump height (with maximum limit)
- **Auto Movement**: Use B button to enable auto-movement, focus on jump timing
- **Button Test**: Long press A button for 2 seconds to enter button test mode
- **Boundary Control**: Jumping won't exceed screen top

## Troubleshooting

### Common Issues
1. **Compilation Errors**: Ensure TFT_eSPI library is configured correctly
2. **Display Issues**: Check screen connection and library configuration
3. **Button No Response**: Confirm button pin definitions are correct
4. **No Sound**: Check buzzer connection
5. **Character Out of Screen**: Boundary detection issues have been fixed

### Debug Information
Game outputs detailed debug information through serial port:
- Button state changes
- Game state transitions
- Collision detection results
- Score changes

## Development Notes

### Code Structure
- **Main Program**: `super_mario.ino` - Contains all game logic
- **Configuration Docs**: `TFT_eSPI_Setup.md` - Library configuration instructions
- **Project Summary**: `PROJECT_SUMMARY.md` - Detailed technical documentation

### Extension Suggestions
- Add more levels
- Implement save functionality
- Add more enemy types
- Add background music
- Implement multiplayer mode

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Contributing

Welcome to submit Issues and Pull Requests to improve this project!

---

**Enjoy the Game!** 🎮

