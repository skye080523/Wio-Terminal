#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "game_assets.h"

// 游戏对象基类
class GameObject {
public:
  float x, y;
  float vx, vy;
  int width, height;
  bool active;
  
  GameObject(float x, float y, int w, int h) : x(x), y(y), width(w), height(h), vx(0), vy(0), active(true) {}
  virtual ~GameObject() {}
  
  virtual void update() = 0;
  virtual void draw(TFT_eSPI& tft) = 0;
  virtual bool checkCollision(GameObject* other);
};

// 玛丽角色类
class Mario : public GameObject {
public:
  MarioState state;
  Direction direction;
  int lives;
  int score;
  bool onGround;
  int animationFrame;
  unsigned long lastFrameTime;
  
  Mario() : GameObject(50, 150, MARIO_WIDTH, MARIO_HEIGHT), 
            state(STANDING), direction(RIGHT), lives(3), score(0), 
            onGround(false), animationFrame(0), lastFrameTime(0) {}
  
  void update() override;
  void draw(TFT_eSPI& tft) override;
  void handleInput();
  void jump();
  void move(Direction dir);
  void crouch();
  void takeDamage();
  void collectCoin();
  void drawSprite(TFT_eSPI& tft, int x, int y);
};

// 敌人类
class Enemy : public GameObject {
public:
  ObjectType type;
  Direction direction;
  int animationFrame;
  
  Enemy(float x, float y, ObjectType t) : GameObject(x, y, ENEMY_SIZE, ENEMY_SIZE), 
                                         type(t), direction(LEFT), animationFrame(0) {}
  
  void update() override;
  void draw(TFT_eSPI& tft) override;
  void move();
  void reverseDirection();
};

// 道具类
class Item : public GameObject {
public:
  ObjectType type;
  bool collected;
  
  Item(float x, float y, ObjectType t) : GameObject(x, y, COIN_SIZE, COIN_SIZE), 
                                        type(t), collected(false) {}
  
  void update() override;
  void draw(TFT_eSPI& tft) override;
  void collect();
};

// 游戏世界类
class GameWorld {
public:
  uint8_t level[12][20];  // 12行20列的游戏世界
  int cameraX;
  
  GameWorld() : cameraX(0) {
    loadLevel();
  }
  
  void loadLevel();
  void update();
  void draw(TFT_eSPI& tft);
  ObjectType getTile(int x, int y);
  void setTile(int x, int y, ObjectType type);
  bool isSolid(int x, int y);
  void drawTile(TFT_eSPI& tft, int x, int y, ObjectType type);
};

// 游戏管理器类
class GameManager {
public:
  Mario mario;
  GameWorld world;
  Enemy* enemies[10];
  Item* items[20];
  int enemyCount;
  int itemCount;
  bool gameOver;
  bool paused;
  
  GameManager() : enemyCount(0), itemCount(0), gameOver(false), paused(false) {
    initGame();
  }
  
  ~GameManager() {
    cleanup();
  }
  
  void initGame();
  void cleanup();
  void update();
  void draw(TFT_eSPI& tft);
  void handleInput();
  void spawnEnemy(float x, float y, ObjectType type);
  void spawnItem(float x, float y, ObjectType type);
  void checkCollisions();
  void gameOverScreen(TFT_eSPI& tft);
  void pauseScreen(TFT_eSPI& tft);
  void drawUI(TFT_eSPI& tft);
  void resetGame();
};

// 实现方法

bool GameObject::checkCollision(GameObject* other) {
  return (x < other->x + other->width &&
          x + width > other->x &&
          y < other->y + other->height &&
          y + height > other->y);
}

void Mario::update() {
  // 应用重力
  if (!onGround) {
    vy += GRAVITY;
  }
  
  // 更新位置
  x += vx;
  y += vy;
  
  // 边界检查
  if (x < 0) x = 0;
  if (x > GAME_WIDTH - width) x = GAME_WIDTH - width;
  if (y > GAME_HEIGHT - height) {
    y = GAME_HEIGHT - height;
    vy = 0;
    onGround = true;
  }
  
  // 动画更新
  if (millis() - lastFrameTime > 100) {
    animationFrame = (animationFrame + 1) % 4;
    lastFrameTime = millis();
  }
  
  // 状态更新
  if (onGround) {
    if (vx == 0) {
      state = STANDING;
    } else {
      state = WALKING;
    }
  } else {
    state = JUMPING;
  }
}

void Mario::draw(TFT_eSPI& tft) {
  drawSprite(tft, (int)x, (int)y);
}

void Mario::handleInput() {
  // 输入处理在GameManager中实现
}

void Mario::jump() {
  if (onGround) {
    vy = JUMP_VELOCITY;
    onGround = false;
  }
}

void Mario::move(Direction dir) {
  direction = dir;
  vx = dir * MOVE_SPEED;
}

void Mario::crouch() {
  if (onGround) {
    state = CROUCHING;
    vx = 0;
  }
}

void Mario::takeDamage() {
  lives--;
  if (lives <= 0) {
    state = DEAD;
  }
}

void Mario::collectCoin() {
  score += 100;
}

void Mario::drawSprite(TFT_eSPI& tft, int x, int y) {
  // 简单的玛丽精灵绘制
  uint16_t color = MARIO_RED;
  
  // 绘制玛丽的基本形状
  tft.fillRect(x + 4, y + 2, 8, 4, MARIO_SKIN);    // 脸
  tft.fillRect(x + 2, y + 6, 12, 8, color);        // 身体
  tft.fillRect(x + 4, y + 14, 4, 2, MARIO_BROWN);  // 左腿
  tft.fillRect(x + 8, y + 14, 4, 2, MARIO_BROWN);  // 右腿
  
  // 帽子
  tft.fillRect(x + 2, y, 12, 4, color);
  tft.fillRect(x + 4, y + 2, 8, 2, MARIO_SKIN);
}

void Enemy::update() {
  move();
  
  // 动画更新
  if (millis() % 500 < 250) {
    animationFrame = 0;
  } else {
    animationFrame = 1;
  }
}

void Enemy::draw(TFT_eSPI& tft) {
  if (type == GOOMBA) {
    // 绘制蘑菇怪
    tft.fillRect((int)x + 2, (int)y + 2, 12, 12, GOOMBA_BROWN);
    tft.fillRect((int)x + 4, (int)y + 4, 8, 8, GOOMBA_BLACK);
  }
}

void Enemy::move() {
  x += direction * ENEMY_SPEED;
  
  // 简单的AI：碰到边界就转向
  if (x <= 0 || x >= GAME_WIDTH - width) {
    reverseDirection();
  }
}

void Enemy::reverseDirection() {
  direction = (direction == LEFT) ? RIGHT : LEFT;
}

void Item::update() {
  // 道具的简单动画
  if (!collected) {
    y += sin(millis() * 0.01) * 0.5;
  }
}

void Item::draw(TFT_eSPI& tft) {
  if (!collected) {
    if (type == COIN) {
      tft.fillCircle((int)x + COIN_SIZE/2, (int)y + COIN_SIZE/2, COIN_SIZE/2, COIN_YELLOW);
    }
  }
}

void Item::collect() {
  collected = true;
  active = false;
}

void GameWorld::loadLevel() {
  // 加载关卡数据
  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 20; j++) {
      level[i][j] = LEVEL_1[i * 20 + j];
    }
  }
}

void GameWorld::update() {
  // 相机跟随玛丽
  // 这里简化处理
}

void GameWorld::draw(TFT_eSPI& tft) {
  // 绘制背景
  tft.fillRect(0, 0, GAME_WIDTH, GAME_HEIGHT, SKY_BLUE);
  
  // 绘制地形
  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 20; j++) {
      int screenX = j * BLOCK_SIZE - cameraX;
      if (screenX >= -BLOCK_SIZE && screenX < GAME_WIDTH) {
        drawTile(tft, screenX, i * BLOCK_SIZE, (ObjectType)level[i][j]);
      }
    }
  }
}

ObjectType GameWorld::getTile(int x, int y) {
  int gridX = x / BLOCK_SIZE;
  int gridY = y / BLOCK_SIZE;
  
  if (gridX >= 0 && gridX < 20 && gridY >= 0 && gridY < 12) {
    return (ObjectType)level[gridY][gridX];
  }
  return EMPTY;
}

void GameWorld::setTile(int x, int y, ObjectType type) {
  int gridX = x / BLOCK_SIZE;
  int gridY = y / BLOCK_SIZE;
  
  if (gridX >= 0 && gridX < 20 && gridY >= 0 && gridY < 12) {
    level[gridY][gridX] = type;
  }
}

bool GameWorld::isSolid(int x, int y) {
  ObjectType tile = getTile(x, y);
  return (tile == GROUND || tile == BRICK || tile == PIPE);
}

void GameWorld::drawTile(TFT_eSPI& tft, int x, int y, ObjectType type) {
  switch (type) {
    case GROUND:
      tft.fillRect(x, y, BLOCK_SIZE, BLOCK_SIZE, GROUND_GREEN);
      break;
    case BRICK:
      tft.fillRect(x, y, BLOCK_SIZE, BLOCK_SIZE, BRICK_BROWN);
      break;
    case PIPE:
      tft.fillRect(x, y, BLOCK_SIZE, BLOCK_SIZE, GROUND_GREEN);
      tft.fillRect(x + 2, y + 2, BLOCK_SIZE - 4, BLOCK_SIZE - 4, MARIO_BROWN);
      break;
    default:
      break;
  }
}

void GameManager::initGame() {
  // 初始化游戏
  mario = Mario();
  world = GameWorld();
  
  // 生成一些敌人和道具
  spawnEnemy(200, 150, GOOMBA);
  spawnEnemy(300, 150, GOOMBA);
  spawnItem(150, 100, COIN);
  spawnItem(250, 100, COIN);
}

void GameManager::cleanup() {
  // 清理内存
  for (int i = 0; i < enemyCount; i++) {
    delete enemies[i];
  }
  for (int i = 0; i < itemCount; i++) {
    delete items[i];
  }
}

void GameManager::update() {
  if (gameOver || paused) return;
  
  mario.update();
  world.update();
  
  // 更新敌人
  for (int i = 0; i < enemyCount; i++) {
    if (enemies[i]->active) {
      enemies[i]->update();
    }
  }
  
  // 更新道具
  for (int i = 0; i < itemCount; i++) {
    if (items[i]->active) {
      items[i]->update();
    }
  }
  
  checkCollisions();
  
  // 检查游戏结束条件
  if (mario.lives <= 0) {
    gameOver = true;
  }
}

void GameManager::draw(TFT_eSPI& tft) {
  world.draw(tft);
  
  // 绘制道具
  for (int i = 0; i < itemCount; i++) {
    if (items[i]->active) {
      items[i]->draw(tft);
    }
  }
  
  // 绘制敌人
  for (int i = 0; i < enemyCount; i++) {
    if (enemies[i]->active) {
      enemies[i]->draw(tft);
    }
  }
  
  // 绘制玛丽
  mario.draw(tft);
  
  // 绘制UI
  drawUI(tft);
  
  if (gameOver) {
    gameOverScreen(tft);
  }
  
  if (paused) {
    pauseScreen(tft);
  }
}

void GameManager::handleInput() {
  // 处理输入
  if (digitalRead(BUTTON_A) == LOW) {
    mario.jump();
  }
  
  if (digitalRead(DPAD_LEFT) == LOW) {
    mario.move(LEFT);
  } else if (digitalRead(DPAD_RIGHT) == LOW) {
    mario.move(RIGHT);
  } else {
    mario.vx = 0;
  }
  
  if (digitalRead(DPAD_PRESS) == LOW) {
    mario.crouch();
  }
  
  if (digitalRead(BUTTON_C) == LOW) {
    paused = !paused;
    delay(200); // 防抖
  }
}

void GameManager::spawnEnemy(float x, float y, ObjectType type) {
  if (enemyCount < 10) {
    enemies[enemyCount++] = new Enemy(x, y, type);
  }
}

void GameManager::spawnItem(float x, float y, ObjectType type) {
  if (itemCount < 20) {
    items[itemCount++] = new Item(x, y, type);
  }
}

void GameManager::checkCollisions() {
  // 检查玛丽与敌人的碰撞
  for (int i = 0; i < enemyCount; i++) {
    if (enemies[i]->active && mario.checkCollision(enemies[i])) {
      if (mario.vy > 0 && mario.y < enemies[i]->y) {
        // 玛丽踩到敌人
        enemies[i]->active = false;
        mario.vy = JUMP_VELOCITY / 2;
        mario.score += 200;
      } else {
        // 玛丽被敌人伤害
        mario.takeDamage();
      }
    }
  }
  
  // 检查玛丽与道具的碰撞
  for (int i = 0; i < itemCount; i++) {
    if (items[i]->active && mario.checkCollision(items[i])) {
      items[i]->collect();
      mario.collectCoin();
    }
  }
}

void GameManager::gameOverScreen(TFT_eSPI& tft) {
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
}

void GameManager::pauseScreen(TFT_eSPI& tft) {
  tft.fillRect(80, 80, 160, 80, TFT_BLACK);
  tft.drawRect(80, 80, 160, 80, TFT_WHITE);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(130, 100);
  tft.print("PAUSED");
  tft.setTextSize(1);
  tft.setCursor(110, 130);
  tft.print("Press C to continue");
}

void GameManager::drawUI(TFT_eSPI& tft) {
  // 绘制UI区域
  tft.fillRect(0, GAME_HEIGHT, GAME_WIDTH, UI_HEIGHT, TFT_BLACK);
  tft.drawLine(0, GAME_HEIGHT, GAME_WIDTH, GAME_HEIGHT, TFT_WHITE);
  
  // 显示生命值
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, GAME_HEIGHT + 10);
  tft.print("Lives: ");
  tft.print(mario.lives);
  
  // 显示分数
  tft.setCursor(10, GAME_HEIGHT + 25);
  tft.print("Score: ");
  tft.print(mario.score);
}

void GameManager::resetGame() {
  cleanup();
  initGame();
  gameOver = false;
  paused = false;
}

#endif 