/*
 * Wio Terminal 超级玛丽游戏 - 改进版本
 * 基于 Arduino IDE 开发
 * 
 * 控制方式:
 * - A按钮: 跳跃 (短按普通跳跃，长按高跳跃)
 * - B按钮: 左右移动切换
 * - C按钮: 暂停/继续游戏
 */

#include <TFT_eSPI.h>

// 硬件定义
#define BUTTON_A WIO_KEY_A
#define BUTTON_B WIO_KEY_B
#define BUTTON_C WIO_KEY_C

// 屏幕尺寸
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// 游戏区域
#define GAME_WIDTH 320
#define GAME_HEIGHT 200
#define UI_HEIGHT 40

// 精灵尺寸
#define MARIO_WIDTH 16
#define MARIO_HEIGHT 16
#define ENEMY_SIZE 16
#define COIN_SIZE 8
#define BLOCK_SIZE 16
#define FLOWER_SIZE 12

// 颜色定义
#define MARIO_RED 0xF800
#define MARIO_BLUE 0x001F
#define MARIO_YELLOW 0xFFE0
#define MARIO_BROWN 0x8200
#define MARIO_SKIN 0xFD20
#define MARIO_BLACK 0x0000
#define MARIO_WHITE 0xFFFF

#define GOOMBA_BROWN 0x8200
#define GOOMBA_BLACK 0x0000
#define COIN_YELLOW 0xFFE0
#define BRICK_BROWN 0x8200
#define QUESTION_BLOCK_YELLOW 0xFFE0
#define GROUND_GREEN 0x07E0
#define SKY_BLUE 0x001F
#define FLOWER_RED 0xF800
#define FLOWER_GREEN 0x07E0

// 游戏配置 - 改进跳跃系统
#define GRAVITY 0.8
#define JUMP_VELOCITY -5       // 每次跳跃增加的速度（减少）
#define MAX_JUMP_VELOCITY -15  // 最大跳跃速度（减少）
#define MOVE_SPEED 3           // 移动速度加快
#define ENEMY_SPEED 1

// 全局变量
TFT_eSPI tft;

// 玛丽状态
struct Mario {
  float x, y;
  float vx, vy;
  int lives;
  int score;
  bool onGround;
  int direction; // -1 = left, 1 = right
  int jumpCount; // 跳跃次数计数器
} mario;

// 敌人状态
struct Enemy {
  float x, y;
  int direction;
  bool active;
} enemies[5];

// 道具状态
struct Item {
  float x, y;
  bool collected;
  int type; // 0=金币, 1=花朵
} items[30]; // 增加道具数量

// 方块状态
struct Block {
  float x, y;
  bool active;
  bool hasFlower;
  bool isQuestionBlock; // 是否是问号方块
} blocks[15]; // 增加方块数量

// 障碍物状态
struct Obstacle {
  float x, y;
  bool active;
} obstacles[5]; // 障碍物数组

// 游戏状态
enum GameScreen {
  MENU,
  PLAYING,
  PAUSED,
  GAME_OVER,
  SUCCESS
};

GameScreen currentScreen = MENU;
bool gameOver = false;
bool paused = false;
bool gameStarted = false;
bool buttonTestMode = false;
bool allCoinsCollected = false;
int enemyCount = 0;
int itemCount = 0;
int blockCount = 0;
int obstacleCount = 0;

// 音效系统
unsigned long lastBeepTime = 0;
int beepFrequency = 0;
int beepDuration = 0;
bool soundEnabled = true;

// 音效函数
void playSound(int frequency, int duration) {
  if (soundEnabled) {
    beepFrequency = frequency;
    beepDuration = duration;
    lastBeepTime = millis();
    // 使用蜂鸣器播放音效
    tone(WIO_BUZZER, frequency, duration);
  }
}

void playCoinSound() {
  playSound(800, 100); // 金币音效
}

void playJumpSound() {
  playSound(600, 80); // 跳跃音效
}

void playFlowerSound() {
  playSound(1000, 150); // 花朵音效
}

void playGameOverSound() {
  playSound(200, 500); // 游戏结束音效
}

void playSuccessSound() {
  playSound(1200, 200); // 成功音效
  delay(100);
  playSound(1400, 200);
  delay(100);
  playSound(1600, 300);
}

void showSuccessScreen() {
  tft.fillScreen(TFT_GREEN);
  
  // 绘制成功标题
  tft.setTextColor(TFT_WHITE, TFT_GREEN);
  tft.setTextSize(3);
  tft.setCursor(60, 60);
  tft.print("SUCCESS!");
  
  // 绘制分数
  tft.setTextColor(TFT_YELLOW, TFT_GREEN);
  tft.setTextSize(2);
  tft.setCursor(100, 100);
  tft.print("Score: ");
  tft.print(mario.score);
  
  // 绘制提示
  tft.setTextColor(TFT_WHITE, TFT_GREEN);
  tft.setTextSize(1);
  tft.setCursor(80, 140);
  tft.print("All coins collected!");
  tft.setCursor(80, 160);
  tft.print("Press C to restart");
  
  // 播放成功音效
  playSuccessSound();
}

void showStartScreen() {
  tft.fillScreen(TFT_BLACK);
  
  // 绘制标题
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(80, 40);
  tft.print("SUPER MARIO");
  
  // 绘制副标题
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(120, 70);
  tft.print("Wio Terminal");
  
  // 绘制控制说明
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(20, 100);
  tft.print("Controls:");
  tft.setCursor(20, 115);
  tft.print("A: Jump (Press multiple times)");
  tft.setCursor(20, 130);
  tft.print("B: Toggle Movement");
  tft.setCursor(20, 145);
  tft.print("C: Pause/Continue");
  tft.setCursor(20, 160);
  tft.print("Hold C: Restart Game");
  tft.setCursor(20, 175);
  tft.print("Yellow blocks (?): Hit from below!");
  tft.setCursor(20, 190);
  tft.print("Red obstacles (!): Game Over!");
  
  // 绘制开始提示
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(80, 210);
  tft.print("Press C to Start");
  
  // 添加按钮测试提示
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(80, 230);
  tft.print("Hold A for Button Test");
}

void setup() {
  // 初始化串口通信
  Serial.begin(115200);
  delay(1000);
  Serial.println("==================================");
  Serial.println("Wio Terminal 超级玛丽游戏启动中...");
  Serial.println("==================================");
  
  // 初始化输入引脚
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  
  Serial.println("按钮引脚初始化完成");
  Serial.print("BUTTON_A: "); Serial.println(BUTTON_A);
  Serial.print("BUTTON_B: "); Serial.println(BUTTON_B);
  Serial.print("BUTTON_C: "); Serial.println(BUTTON_C);
  
  // 初始化屏幕
  Serial.println("初始化屏幕...");
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  Serial.println("屏幕初始化完成");
  
  // 显示启动画面
  Serial.println("显示启动画面...");
  showStartScreen();
  
  // 初始化游戏
  Serial.println("初始化游戏数据...");
  initGame();
  
  Serial.println("=== 游戏初始化完成！等待按钮输入... ===");
}

void loop() {
  // 调试按钮状态
  static unsigned long lastDebugTime = 0;
  if (millis() - lastDebugTime > 500) {
    Serial.print("状态 - 游戏开始:"); Serial.print(gameStarted);
    Serial.print(" 暂停:"); Serial.print(paused);
    Serial.print(" 游戏结束:"); Serial.print(gameOver);
    Serial.print(" 按钮测试:"); Serial.print(buttonTestMode);
    Serial.print(" | 按钮 A:"); Serial.print(digitalRead(BUTTON_A));
    Serial.print(" B:"); Serial.print(digitalRead(BUTTON_B));
    Serial.print(" C:"); Serial.println(digitalRead(BUTTON_C));
    lastDebugTime = millis();
  }
  
  // 处理输入
  handleInput();
  
  // 更新游戏状态
  if (gameStarted && !gameOver && !paused) {
    updateGame();
  }
  
  // 绘制游戏画面
  drawGame();
  
  // 控制帧率
  delay(16);
}

void initGame() {
  // 初始化玛丽
  mario.x = 50;
  mario.y = 150;
  mario.vx = MOVE_SPEED;
  mario.vy = 0;
  mario.lives = 3;
  mario.score = 0;
  mario.onGround = false;
  mario.direction = 1;
  mario.jumpCount = 0;
  
  // 初始化敌人
  enemyCount = 3;
  enemies[0].x = 200;
  enemies[0].y = 150;
  enemies[0].direction = -1;
  enemies[0].active = true;
  
  enemies[1].x = 300;
  enemies[1].y = 150;
  enemies[1].direction = -1;
  enemies[1].active = true;
  
  enemies[2].x = 400;
  enemies[2].y = 150;
  enemies[2].direction = -1;
  enemies[2].active = true;
  
  // 初始化更多金币 - 分散布局
  itemCount = 12;
  // 金币位置 - 形成更自然的分布
  int coinPositions[12][2] = {
    {80, 100},   // 左上
    {120, 80},   // 左中上
    {160, 120},  // 左中
    {200, 60},   // 中上
    {240, 100},  // 中
    {280, 80},   // 中下
    {320, 120},  // 右中
    {360, 60},   // 右中上
    {400, 100},  // 右中
    {440, 80},   // 右中下
    {480, 120},  // 右下
    {520, 100}   // 最右
  };
  
  for (int i = 0; i < itemCount; i++) {
    items[i].x = coinPositions[i][0];
    items[i].y = coinPositions[i][1];
    items[i].collected = false;
    items[i].type = 0; // 金币
  }
  
  // 初始化更多方块
  blockCount = 10;
  
  // 普通方块 - 分散布局
  blocks[0].x = 100;
  blocks[0].y = 140;
  blocks[0].active = true;
  blocks[0].hasFlower = false;
  blocks[0].isQuestionBlock = false;
  
  blocks[1].x = 180;
  blocks[1].y = 120;
  blocks[1].active = true;
  blocks[1].hasFlower = false;
  blocks[1].isQuestionBlock = false;
  
  blocks[2].x = 260;
  blocks[2].y = 140;
  blocks[2].active = true;
  blocks[2].hasFlower = false;
  blocks[2].isQuestionBlock = false;
  
  // 问号方块 - 分散布局
  blocks[3].x = 140;
  blocks[3].y = 60;
  blocks[3].active = true;
  blocks[3].hasFlower = false;
  blocks[3].isQuestionBlock = true;
  
  blocks[4].x = 220;
  blocks[4].y = 80;
  blocks[4].active = true;
  blocks[4].hasFlower = false;
  blocks[4].isQuestionBlock = true;
  
  blocks[5].x = 300;
  blocks[5].y = 60;
  blocks[5].active = true;
  blocks[5].hasFlower = false;
  blocks[5].isQuestionBlock = true;
  
  blocks[6].x = 340;
  blocks[6].y = 120;
  blocks[6].active = true;
  blocks[6].hasFlower = false;
  blocks[6].isQuestionBlock = false;
  
  blocks[7].x = 420;
  blocks[7].y = 140;
  blocks[7].active = true;
  blocks[7].hasFlower = false;
  blocks[7].isQuestionBlock = false;
  
  // 额外的问号方块
  blocks[8].x = 380;
  blocks[8].y = 80;
  blocks[8].active = true;
  blocks[8].hasFlower = false;
  blocks[8].isQuestionBlock = true;
  
  blocks[9].x = 460;
  blocks[9].y = 60;
  blocks[9].active = true;
  blocks[9].hasFlower = false;
  blocks[9].isQuestionBlock = true;
  
  // 初始化障碍物 - 调整到屏幕内可见位置
  obstacleCount = 3;
  obstacles[0].x = 140;
  obstacles[0].y = 150;
  obstacles[0].active = true;
  
  obstacles[1].x = 240;
  obstacles[1].y = 150;
  obstacles[1].active = true;
  
  obstacles[2].x = 340;
  obstacles[2].y = 150;
  obstacles[2].active = true;
}

void resetGame() {
  gameOver = false;
  paused = false;
  gameStarted = false;
  buttonTestMode = false;
  allCoinsCollected = false;
  currentScreen = MENU;
  initGame();
}

void handleInput() {
  // A按钮处理 - 按次数累加跳跃高度
  static bool lastAState = HIGH;
  static unsigned long aPressTime = 0;
  bool currentAState = digitalRead(BUTTON_A);
  
  if (currentAState == LOW && lastAState == HIGH) {
    // A按钮按下
    aPressTime = millis();
    Serial.println("A按钮按下");
    
    if (!gameStarted) {
      // 检查是否长按进入按钮测试模式
      static unsigned long testModeTime = 0;
      testModeTime = millis();
      // 延迟检查长按
      while (digitalRead(BUTTON_A) == LOW) {
        if (millis() - testModeTime > 2000) {
          buttonTestMode = true;
          Serial.println("进入按钮测试模式");
          break;
        }
        delay(10);
      }
    } else if (!buttonTestMode) {
      // 游戏中的跳跃逻辑
      if (mario.onGround) {
        // 在地面上，重置跳跃计数并开始跳跃
        mario.jumpCount = 1;
        mario.vy = JUMP_VELOCITY;
        mario.onGround = false;
        playJumpSound();
        Serial.println("开始跳跃！跳跃次数: 1");
      } else {
        // 在空中，累加跳跃高度
        mario.jumpCount++;
        mario.vy += JUMP_VELOCITY;
        
        // 限制最大跳跃速度
        if (mario.vy < MAX_JUMP_VELOCITY) {
          mario.vy = MAX_JUMP_VELOCITY;
        }
        
        playJumpSound();
        Serial.print("空中跳跃！跳跃次数: ");
        Serial.print(mario.jumpCount);
        Serial.print(", 当前速度: ");
        Serial.println(mario.vy);
      }
    }
  }
  
  lastAState = currentAState;
  
  // B按钮处理 - 移动切换
  static bool lastBState = HIGH;
  static bool moving = false;
  bool currentBState = digitalRead(BUTTON_B);
  
  if (currentBState == LOW && lastBState == HIGH) {
    Serial.println("B按钮按下 - 切换移动状态");
    moving = !moving;
    if (moving) {
      mario.vx = mario.direction * MOVE_SPEED;
      Serial.println("开始移动");
    } else {
      mario.vx = 0;
      Serial.println("停止移动");
    }
    delay(100);
  }
  
  // 自动切换方向
  if (moving) {
    if (mario.x <= 0) {
      mario.direction = 1;
      mario.vx = MOVE_SPEED;
    } else if (mario.x >= GAME_WIDTH - MARIO_WIDTH) {
      mario.direction = -1;
      mario.vx = -MOVE_SPEED;
    }
  }
  
  lastBState = currentBState;
  
  // C按钮处理
  static bool lastCState = HIGH;
  static unsigned long cPressTime = 0;
  bool currentCState = digitalRead(BUTTON_C);
  
  if (currentCState == LOW && lastCState == HIGH) {
    Serial.println("C按钮按下");
    cPressTime = millis();
  } else if (currentCState == HIGH && lastCState == LOW) {
    unsigned long pressDuration = millis() - cPressTime;
    Serial.print("C按钮释放，持续时间: "); Serial.println(pressDuration);
    
    if (pressDuration < 1000) {
      Serial.println("C按钮短按");
      if (buttonTestMode) {
        Serial.println("退出按钮测试模式");
        buttonTestMode = false;
      } else if (currentScreen == SUCCESS) {
        Serial.println("从成功状态重新开始游戏");
        resetGame();
      } else if (gameOver) {
        Serial.println("重新开始游戏");
        resetGame();
      } else if (!gameStarted) {
        Serial.println("开始游戏");
        gameStarted = true;
        currentScreen = PLAYING;
      } else {
        Serial.println("切换暂停状态");
        paused = !paused;
        currentScreen = paused ? PAUSED : PLAYING;
      }
    } else {
      Serial.println("长按C，重新开始游戏");
      resetGame();
    }
    delay(200);
  }
  
  lastCState = currentCState;
}

void updateGame() {
  updateMario();
  updateEnemies();
  checkCollisions();
  checkObstacleCollisions();
  checkAllCoinsCollected();
  
  if (mario.lives <= 0) {
    gameOver = true;
    currentScreen = GAME_OVER;
  }
}

void updateMario() {
  // 应用重力
  if (!mario.onGround) {
    mario.vy += GRAVITY;
  }
  
  // 更新位置
  mario.x += mario.vx;
  mario.y += mario.vy;
  
  // 边界检查
  if (mario.x < 0) mario.x = 0;
  if (mario.x > GAME_WIDTH - MARIO_WIDTH) mario.x = GAME_WIDTH - MARIO_WIDTH;
  
  // 顶部边界检查 - 防止跳跃超出屏幕
  if (mario.y < 0) {
    mario.y = 0;
    mario.vy = 0; // 碰到顶部时停止上升
  }
  
  // 地面碰撞检测
  if (mario.y > GAME_HEIGHT - MARIO_HEIGHT - BLOCK_SIZE) {
    mario.y = GAME_HEIGHT - MARIO_HEIGHT - BLOCK_SIZE;
    mario.vy = 0;
    mario.onGround = true;
    mario.jumpCount = 0; // 着地时重置跳跃计数
  }
  
  // 方块碰撞检测
  mario.onGround = false;
  for (int i = 0; i < blockCount; i++) {
    if (blocks[i].active) {
      if (mario.x < blocks[i].x + BLOCK_SIZE &&
          mario.x + MARIO_WIDTH > blocks[i].x &&
          mario.y < blocks[i].y + BLOCK_SIZE &&
          mario.y + MARIO_HEIGHT > blocks[i].y) {
        
        if (mario.vy > 0 && mario.y < blocks[i].y) {
          // 从上方碰撞 - 站在方块上
          mario.y = blocks[i].y - MARIO_HEIGHT;
          mario.vy = 0;
          mario.onGround = true;
          mario.jumpCount = 0; // 站在方块上时重置跳跃计数
          
          // 只有问号方块才会生成花朵
          if (blocks[i].isQuestionBlock && !blocks[i].hasFlower) {
            blocks[i].hasFlower = true;
            // 添加花朵道具
            if (itemCount < 30) {
              items[itemCount].x = blocks[i].x;
              items[itemCount].y = blocks[i].y - FLOWER_SIZE;
              items[itemCount].collected = false;
              items[itemCount].type = 1; // 花朵
              itemCount++;
              Serial.println("问号方块生成花朵！");
            }
          }
        } else if (mario.vy < 0 && mario.y + MARIO_HEIGHT > blocks[i].y + BLOCK_SIZE) {
          // 从下方碰撞 - 顶方块
          mario.y = blocks[i].y + BLOCK_SIZE;
          mario.vy = 0;
          
          // 如果是问号方块且还没有生成花朵，则生成花朵
          if (blocks[i].isQuestionBlock && !blocks[i].hasFlower) {
            blocks[i].hasFlower = true;
            // 添加花朵道具
            if (itemCount < 30) {
              items[itemCount].x = blocks[i].x;
              items[itemCount].y = blocks[i].y - FLOWER_SIZE;
              items[itemCount].collected = false;
              items[itemCount].type = 1; // 花朵
              itemCount++;
              Serial.println("顶问号方块！生成花朵！");
            }
          }
        }
      }
    }
  }
}

void updateEnemies() {
  for (int i = 0; i < enemyCount; i++) {
    if (enemies[i].active) {
      enemies[i].x += enemies[i].direction * ENEMY_SPEED;
      
      if (enemies[i].x <= 0 || enemies[i].x >= GAME_WIDTH - ENEMY_SIZE) {
        enemies[i].direction *= -1;
      }
    }
  }
}

void checkCollisions() {
  // 检查玛丽与敌人的碰撞
  for (int i = 0; i < enemyCount; i++) {
    if (enemies[i].active) {
      if (mario.x < enemies[i].x + ENEMY_SIZE &&
          mario.x + MARIO_WIDTH > enemies[i].x &&
          mario.y < enemies[i].y + ENEMY_SIZE &&
          mario.y + MARIO_HEIGHT > enemies[i].y) {
        
        if (mario.vy > 0 && mario.y < enemies[i].y) {
          // 玛丽踩到敌人
          enemies[i].active = false;
          mario.vy = JUMP_VELOCITY / 2;
          mario.jumpCount = 0; // 踩敌人时重置跳跃计数
          mario.score += 200;
          Serial.println("踩到敌人！+200分");
        } else {
          // 玛丽被敌人伤害
          mario.lives--;
          Serial.println("被敌人伤害！生命-1");
        }
      }
    }
  }
  
  // 检查玛丽与道具的碰撞
  for (int i = 0; i < itemCount; i++) {
    if (!items[i].collected) {
      int itemSize = (items[i].type == 0) ? COIN_SIZE : FLOWER_SIZE;
      if (mario.x < items[i].x + itemSize &&
          mario.x + MARIO_WIDTH > items[i].x &&
          mario.y < items[i].y + itemSize &&
          mario.y + MARIO_HEIGHT > items[i].y) {
        
        items[i].collected = true;
        if (items[i].type == 0) {
          mario.score += 100;
          playCoinSound();
          Serial.println("收集金币！+100分");
        } else {
          mario.score += 200;
          playFlowerSound();
          Serial.println("收集花朵！+200分");
        }
      }
    }
  }
}

void checkObstacleCollisions() {
  // 检查玛丽与障碍物的碰撞
  for (int i = 0; i < obstacleCount; i++) {
    if (obstacles[i].active) {
      if (mario.x < obstacles[i].x + BLOCK_SIZE &&
          mario.x + MARIO_WIDTH > obstacles[i].x &&
          mario.y < obstacles[i].y + BLOCK_SIZE &&
          mario.y + MARIO_HEIGHT > obstacles[i].y) {
        
        // 碰到障碍物，游戏结束
        mario.lives = 0;
        Serial.println("碰到障碍物！游戏结束！");
        gameOver = true;
        currentScreen = GAME_OVER;
        playGameOverSound();
      }
    }
  }
}

void checkAllCoinsCollected() {
  int collectedCoins = 0;
  for (int i = 0; i < itemCount; i++) {
    if (items[i].type == 0 && items[i].collected) { // 只检查金币
      collectedCoins++;
    }
  }
  
  if (collectedCoins >= itemCount && !allCoinsCollected) {
    allCoinsCollected = true;
    currentScreen = SUCCESS;
    Serial.println("所有金币收集完成！游戏成功！");
  }
}

void drawGame() {
  if (!gameStarted) {
    showStartScreen();
    return;
  }
  
  if (buttonTestMode) {
    drawButtonTest();
    return;
  }
  
  // 根据游戏状态绘制不同屏幕
  switch (currentScreen) {
    case SUCCESS:
      showSuccessScreen();
      return;
    case GAME_OVER:
      drawGameOver();
      return;
    case PAUSED:
      drawPaused();
      return;
    default:
      break;
  }
  
  // 绘制背景
  tft.fillRect(0, 0, GAME_WIDTH, GAME_HEIGHT, SKY_BLUE);
  
  // 绘制地面
  tft.fillRect(0, GAME_HEIGHT - BLOCK_SIZE, GAME_WIDTH, BLOCK_SIZE, GROUND_GREEN);
  
  // 绘制方块
  drawBlocks();
  
  // 绘制玛丽
  drawMario();
  
  // 绘制敌人
  drawEnemies();
  
  // 绘制道具
  drawItems();
  
  // 绘制障碍物
  drawObstacles();
  
  // 绘制UI
  drawUI();
}

void drawMario() {
  int x = (int)mario.x;
  int y = (int)mario.y;
  
  // 绘制帽子
  tft.fillRect(x + 2, y, 12, 4, MARIO_RED);
  tft.fillRect(x + 4, y + 2, 8, 2, MARIO_SKIN);
  
  // 绘制面部
  tft.fillRect(x + 4, y + 4, 8, 6, MARIO_SKIN);
  
  // 绘制眼睛
  tft.fillRect(x + 5, y + 5, 2, 2, MARIO_BLACK);
  tft.fillRect(x + 9, y + 5, 2, 2, MARIO_BLACK);
  
  // 绘制鼻子
  tft.fillRect(x + 7, y + 7, 2, 2, MARIO_RED);
  
  // 绘制胡子
  tft.fillRect(x + 3, y + 9, 2, 1, MARIO_BLACK);
  tft.fillRect(x + 11, y + 9, 2, 1, MARIO_BLACK);
  
  // 绘制身体
  tft.fillRect(x + 4, y + 10, 8, 6, MARIO_RED);
  
  // 绘制背带
  tft.fillRect(x + 4, y + 10, 2, 6, MARIO_BLUE);
  tft.fillRect(x + 10, y + 10, 2, 6, MARIO_BLUE);
  
  // 绘制背带扣
  tft.fillRect(x + 6, y + 12, 4, 2, MARIO_YELLOW);
  
  // 绘制手臂
  tft.fillRect(x + 2, y + 12, 2, 4, MARIO_SKIN);
  tft.fillRect(x + 12, y + 12, 2, 4, MARIO_SKIN);
  
  // 绘制手套
  tft.fillRect(x + 1, y + 16, 3, 2, MARIO_WHITE);
  tft.fillRect(x + 12, y + 16, 3, 2, MARIO_WHITE);
  
  // 绘制腿部
  tft.fillRect(x + 4, y + 16, 3, 4, MARIO_BLUE);
  tft.fillRect(x + 9, y + 16, 3, 4, MARIO_BLUE);
  
  // 绘制鞋子
  tft.fillRect(x + 3, y + 20, 4, 2, MARIO_BROWN);
  tft.fillRect(x + 9, y + 20, 4, 2, MARIO_BROWN);
}

void drawEnemies() {
  for (int i = 0; i < enemyCount; i++) {
    if (enemies[i].active) {
      int x = (int)enemies[i].x;
      int y = (int)enemies[i].y;
      
      // 绘制敌人身体
      tft.fillRect(x + 2, y + 2, 12, 12, GOOMBA_BROWN);
      
      // 绘制眼睛
      tft.fillRect(x + 4, y + 4, 2, 2, GOOMBA_BLACK);
      tft.fillRect(x + 10, y + 4, 2, 2, GOOMBA_BLACK);
    }
  }
}

void drawItems() {
  for (int i = 0; i < itemCount; i++) {
    if (!items[i].collected) {
      int x = (int)items[i].x;
      int y = (int)items[i].y;
      
      if (items[i].type == 0) {
        // 绘制金币
        tft.fillCircle(x + COIN_SIZE/2, y + COIN_SIZE/2, COIN_SIZE/2, COIN_YELLOW);
      } else {
        // 绘制花朵
        tft.fillCircle(x + FLOWER_SIZE/2, y + FLOWER_SIZE/2, FLOWER_SIZE/2, FLOWER_RED);
        tft.fillCircle(x + FLOWER_SIZE/2, y + FLOWER_SIZE/2, FLOWER_SIZE/4, FLOWER_GREEN);
      }
    }
  }
}

void drawBlocks() {
  for (int i = 0; i < blockCount; i++) {
    if (blocks[i].active) {
      int x = (int)blocks[i].x;
      int y = (int)blocks[i].y;
      
      // 根据方块类型选择颜色
      uint16_t blockColor = blocks[i].isQuestionBlock ? QUESTION_BLOCK_YELLOW : BRICK_BROWN;
      
      // 绘制方块
      tft.fillRect(x, y, BLOCK_SIZE, BLOCK_SIZE, blockColor);
      tft.drawRect(x, y, BLOCK_SIZE, BLOCK_SIZE, MARIO_BLACK);
      
      // 如果是问号方块，绘制问号图案
      if (blocks[i].isQuestionBlock) {
        tft.setTextColor(MARIO_BLACK, QUESTION_BLOCK_YELLOW);
        tft.setTextSize(1);
        tft.setCursor(x + 5, y + 4);
        tft.print("?");
      }
    }
  }
}

void drawObstacles() {
  for (int i = 0; i < obstacleCount; i++) {
    if (obstacles[i].active) {
      int x = (int)obstacles[i].x;
      int y = (int)obstacles[i].y;
      
      // 绘制红色障碍物
      tft.fillRect(x, y, BLOCK_SIZE, BLOCK_SIZE, TFT_RED);
      tft.drawRect(x, y, BLOCK_SIZE, BLOCK_SIZE, MARIO_BLACK);
      
      // 绘制危险标志
      tft.setTextColor(MARIO_WHITE, TFT_RED);
      tft.setTextSize(1);
      tft.setCursor(x + 4, y + 4);
      tft.print("!");
    }
  }
}

void drawUI() {
  tft.fillRect(0, GAME_HEIGHT, GAME_WIDTH, UI_HEIGHT, TFT_BLACK);
  tft.drawLine(0, GAME_HEIGHT, GAME_WIDTH, GAME_HEIGHT, TFT_WHITE);
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, GAME_HEIGHT + 10);
  tft.print("Lives: ");
  tft.print(mario.lives);
  
  tft.setCursor(10, GAME_HEIGHT + 25);
  tft.print("Score: ");
  tft.print(mario.score);
  
  // 显示跳跃次数
  tft.setCursor(200, GAME_HEIGHT + 10);
  tft.print("Jumps: ");
  tft.print(mario.jumpCount);
}

void drawGameOver() {
  tft.fillRect(80, 80, 160, 80, TFT_BLACK);
  tft.drawRect(80, 80, 160, 80, TFT_WHITE);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(120, 100);
  tft.print("GAME OVER");
  tft.setTextSize(1);
  tft.setCursor(100, 130);
  tft.print("Score: ");
  tft.print(mario.score);
  tft.setCursor(90, 145);
  tft.print("Press C to restart");
}

void drawPaused() {
  tft.fillRect(80, 80, 160, 80, TFT_BLACK);
  tft.drawRect(80, 80, 160, 80, TFT_WHITE);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(130, 100);
  tft.print("PAUSED");
  tft.setTextSize(1);
  tft.setCursor(90, 130);
  tft.print("Press C to continue");
  tft.setCursor(90, 145);
  tft.print("Hold C to restart");
}

void drawButtonTest() {
  tft.fillScreen(TFT_BLACK);
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(50, 20);
  tft.println("Button Test Mode");
  
  int buttonA = digitalRead(BUTTON_A);
  int buttonB = digitalRead(BUTTON_B);
  int buttonC = digitalRead(BUTTON_C);
  
  tft.setTextSize(1);
  tft.setCursor(50, 80);
  tft.print("Button A: "); tft.print(buttonA);
  tft.setCursor(50, 100);
  tft.print("Button B: "); tft.print(buttonB);
  tft.setCursor(50, 120);
  tft.print("Button C: "); tft.print(buttonC);
  
  tft.setCursor(50, 150);
  if (buttonA == LOW) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.print("Button A Pressed!");
  } else if (buttonB == LOW) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.print("Button B Pressed!");
  } else if (buttonC == LOW) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.print("Button C Pressed!");
  } else {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print("No button pressed");
  }
  
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(50, 200);
  tft.print("Press C to return to game");
}