#include "CGame.h"
#include <cmath>
#include <algorithm>
#include <Windows.h>
#include <mmsystem.h>
#include <QDir>
#include <QDebug>

#pragma comment(lib, "winmm.lib")

CGame::CGame(QWidget *parent)
    : QMainWindow(parent)
    , gameTimer(nullptr)
    , gameState(STATE_START_SCREEN) 
    , gameOver(false)
    , playerCount(2)
    , winner(0)
    , score(0)
    , playerLevel(1)
    , exp(0)
    , expMax(100)
    , selectedCharacterIndex(0)
    , selectedCharacterIndexP2(1)
    , selectedCharacterIndexP3(2)
    , selectedCharacterIndexP4(3)
    , characterCount(0)
    , playerWidth(32)
    , playerHeight(32)
    , playerVelocityX(0)
    , playerVelocityY(0)
    , isLeftPressed(false)
    , isRightPressed(false)
    , isUpPressed(false)
    , isDownPressed(false)
    , isAttackPressed(false)
    , playerFrame(0)
    , facingRight(true)
    , playerLives(100)
    , playerMaxLives(100)
    , attackCooldown(0)
    , damage(10)
    , speed(8.0f)
    , reduce(0.0f)
    , isJumping(false)
    , jumpCount(0)
    , powerUpDamage(0)
    , powerUpSpeed(0.0f)
    , powerUpDuration(0)
    , hasPowerUp(false)
    , selectedSkill(SkillType::SKILL_MISSILE_RAIN)
    , isSkillPressed(false)
    , attackSpeed(6.0f)
    , enemyDeathCnt(0)
    , expBallCnt(0)
{
    ui.setupUi(this);

    // 设置窗口大小
    setFixedSize(800, 600);
    setWindowTitle("幸存者大作战");

    // 加载资源
    loadResources();

    // 初始化游戏
    initGame();

    // 创建游戏定时器
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &CGame::updateGame);
    gameTimer->start(16); // 60 FPS

    // 初始化时间步长控制
    elapsedTimer.start();
    accumulator = 0.0f;
}

CGame::~CGame()
{
    delete gameTimer;
    // 停止音频
    PlaySound(NULL, NULL, 0);
}

void CGame::initGame()
{
    // 初始化游戏状态
    gameState = STATE_START_SCREEN;
    gameOver = false;
    playerCount = 2; // 默认双人游戏
    winner = 0;
    score = 0;
    playerLevel = 1;
    exp = 0;
    expMax = 100;
    enemyDeathCnt = 0;

    // 初始化角色选择
    selectedCharacterIndex = 0; // P1角色
    selectedCharacterIndexP2 = 1; // P2角色
    selectedCharacterIndexP3 = 2; // P3角色
    selectedCharacterIndexP4 = 3; // P4角色
    characterCount = characterImages.size();

    // 初始化玩家1
    playerPos = QPointF(400, 300);
    playerVelocityX = 0;
    playerVelocityY = 0;
    isLeftPressed = false;
    isRightPressed = false;
    isUpPressed = false;
    isDownPressed = false;
    isAttackPressed = false;
    playerFrame = 0;
    facingRight = true;
    playerLives = 100; // 生命值
    playerMaxLives = 100;
    attackCooldown = 0; // 射击冷却时间
    damage = 10; // 攻击力
    speed = 8.0f; // 移动速度
    reduce = 0.0f; // 伤害减免
    isJumping = false; // 初始化跳跃状态
    jumpCount = 0; // 初始化跳跃次数

    // 初始化道具效果
    powerUpDamage = 0;
    powerUpSpeed = 0.0f;
    powerUpDuration = 0;
    hasPowerUp = false;

    // 初始化技能系统
    selectedSkill = SkillType::SKILL_MISSILE_RAIN;

    // 技能1：导弹雨
    skillCooldown[SkillType::SKILL_MISSILE_RAIN] = 0;
    skillCooldownMax[SkillType::SKILL_MISSILE_RAIN] = 480; // 8秒（480帧）
    isSkillAvailable[SkillType::SKILL_MISSILE_RAIN] = true;
    skillDamage[SkillType::SKILL_MISSILE_RAIN] = 150; // 初始技能伤害（加强三倍后）
    
    // 技能2：能量护盾
    skillCooldown[SkillType::SKILL_ENERGY_SHIELD] = 0;
    skillCooldownMax[SkillType::SKILL_ENERGY_SHIELD] = 600; // 10秒（600帧）
    isSkillAvailable[SkillType::SKILL_ENERGY_SHIELD] = true;
    skillDamage[SkillType::SKILL_ENERGY_SHIELD] = 0; // 护盾不造成伤害
    
    // 技能3：闪电链
    skillCooldown[SkillType::SKILL_LIGHTNING_CHAIN] = 0;
    skillCooldownMax[SkillType::SKILL_LIGHTNING_CHAIN] = 360; // 6秒（360帧）
    isSkillAvailable[SkillType::SKILL_LIGHTNING_CHAIN] = true;
    skillDamage[SkillType::SKILL_LIGHTNING_CHAIN] = 100; // 初始技能伤害
    
    isSkillPressed = false;
    attackSpeed = 1.0f; // 初始攻击速度
    
    // 初始化玩家2
    playerPosP2 = QPointF(600, 400);
    playerVelocityXP2 = 0;
    playerVelocityYP2 = 0;
    isLeftPressedP2 = false;
    isRightPressedP2 = false;
    isAttackPressedP2 = false;
    playerFrameP2 = 0;
    facingRightP2 = false;
    playerLivesP2 = 3; // 3条生命值
    attackCooldownP2 = 0; // 射击冷却时间
    isJumpingP2 = false; // 初始化跳跃状态
    jumpCountP2 = 0; // 初始化跳跃次数
    
    // 初始化玩家3
    playerPosP3 = QPointF(300, 400);
    playerVelocityXP3 = 0;
    playerVelocityYP3 = 0;
    isLeftPressedP3 = false;
    isRightPressedP3 = false;
    isAttackPressedP3 = false;
    playerFrameP3 = 0;
    facingRightP3 = true;
    playerLivesP3 = 3; // 3条生命值
    attackCooldownP3 = 0; // 射击冷却时间
    isJumpingP3 = false; // 初始化跳跃状态
    jumpCountP3 = 0; // 初始化跳跃次数
    
    // 初始化玩家4
    playerPosP4 = QPointF(500, 400);
    playerVelocityXP4 = 0;
    playerVelocityYP4 = 0;
    isLeftPressedP4 = false;
    isRightPressedP4 = false;
    isAttackPressedP4 = false;
    playerFrameP4 = 0;
    facingRightP4 = false;
    playerLivesP4 = 3; // 3条生命值
    attackCooldownP4 = 0; // 射击冷却时间
    isJumpingP4 = false; // 初始化跳跃状态
    jumpCountP4 = 0; // 初始化跳跃次数
    
    // 清空子弹
    bullets.clear();
    
    // 初始化敌人
    enemies.clear();
    
    // 初始化经验球
    expBalls.assign(EBALL_MAX, {QPointF(0, 0), 0, false, 0});
    expBallCnt = 0;
    
    // 初始化悬浮球
    floatingOrbs.clear();
    
    // 初始化道具
    powerUps.clear();
    
    // 初始化光环效果
    auras.clear();
    
    // 加载游戏场景
    loadTiles();
    loadItems();
    loadEnemies();
    loadDecorations();
    
    // 初始化升级面板
    // initUpgradePanel();
    
    // 播放背景音乐
    QString musicPath = QDir::currentPath() + "/Tiles/bg.wav";
    qDebug() << "Audio path:" << musicPath;
    
    // 检查文件是否存在
    QFileInfo fileInfo(musicPath);
    qDebug() << "File exists:" << fileInfo.exists();
    qDebug() << "File size:" << fileInfo.size() << "bytes";
    
    if (fileInfo.exists()) {
        // 使用PlaySound播放音频
        std::wstring wPath = musicPath.toStdWString();
        BOOL result = PlaySound(wPath.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        qDebug() << "PlaySound result:" << result;
        if (!result) {
            qDebug() << "PlaySound error:" << GetLastError();
        }
    } else {
        qDebug() << "Audio file not found!";
    }

}

void CGame::loadResources()
{
    // 加载图片资源
    groundTile.load("Tiles/tile_0000.png");
    platformTile.load("Tiles/tile_0001.png");
    wallTile.load("Tiles/tile_0002.png");
    coinImage.load("Tiles/tile_0011.png"); // 使用0011.png作为金币图片
    playerImage.load("Tiles/Characters/tile_0001.png");
    playerImageP2.load("Tiles/Characters/tile_0003.png");
    playerImageP3.load("Tiles/Characters/tile_0005.png");
    playerImageP4.load("Tiles/Characters/tile_0007.png");
    enemyImage.load("Tiles/Characters/tile_0012.png");
    backgroundImage.load("Tiles/1.png"); // 使用1.png作为背景
    singlePlayerBackgroundImage.load("Tiles/121.png"); // 使用121.png作为单人模式背景

    // 加载子弹和生命值图片
    bulletImage.load("Tiles/tile_0091.png"); // 子弹图片改为0091
    lifeImage.load("Tiles/tile_0044.png"); // 生命值图片
    lifeEmptyImage.load("Tiles/tile_0046.png"); // 空生命值图片
    expBallImage.load("Tiles/tile_0011.png"); // 使用金币图片作为经验球图片
    powerUpImage.load("Tiles/tile_0044.png"); // 道具图片
    bossImage.load("Tiles/tile_0012.png"); // BOSS图片
    missileImage.load("Tiles/tile_0091.png"); // 导弹图片
    
    // 加载装饰元素
    treeImage.load("Tiles/tile_0030.png");
    cloudImage.load("Tiles/tile_0040.png");
    ladderImage.load("Tiles/tile_0050.png");
    flagImage.load("Tiles/tile_0060.png");
    bushImage.load("Tiles/tile_0070.png");
    flowerImage.load("Tiles/tile_0080.png");
    mushroomImage.load("Tiles/tile_0090.png");
    crateImage.load("Tiles/tile_0100.png");
    gemImage.load("Tiles/tile_0110.png");
    spikeImage.load("Tiles/tile_0120.png");
    
    // 加载角色图片
    loadCharacters();
}

void CGame::loadCharacters()
{
    characterImages.clear();
    characterJumpImages.clear();

    // 只保留4个不同颜色的角色，删除灰色角色
    // 保留的角色：
    // tile_0000.png (绿色)
    // tile_0002.png (蓝色) 
    // tile_0004.png (粉色)
    // tile_0006.png (黄色)
    // 对应的跳跃动画：
    // tile_0001.png (绿色跳跃)
    // tile_0003.png (蓝色跳跃) 
    // tile_0005.png (粉色跳跃)
    // tile_0007.png (黄色跳跃)

    int characterIndices[] = {0, 2, 4, 6};
    int jumpIndices[] = {1, 3, 5, 7};

    for (int i = 0; i < 4; ++i)
    {
        // 加载站立角色
        int index = characterIndices[i];
        QString filename = QString("Tiles/Characters/tile_%1.png").arg(index, 4, 10, QChar('0'));
        QImage characterImg;
        if (characterImg.load(filename))
        {
            characterImages.push_back(characterImg);
        }
        
        // 加载跳跃角色
        int jumpIndex = jumpIndices[i];
        QString jumpFilename = QString("Tiles/Characters/tile_%1.png").arg(jumpIndex, 4, 10, QChar('0'));
        QImage jumpImg;
        if (jumpImg.load(jumpFilename))
        {
            characterJumpImages.push_back(jumpImg);
        }
    }
    
    characterCount = characterImages.size();
    if (characterCount > 0)
    {
        playerImage = characterImages[0];
    }
}

void CGame::loadTiles()
{
    // 清空瓦片
    tiles.clear();
    
    // 密集的草坪地面 - 不留空隙
    for (int x = 0; x < 800; x += 16)  // 密集排列，无空隙
    {
        Tile tile;
        tile.pos = QPoint(x, 536);
        tile.width = 32;
        tile.height = 32;
        tile.isSolid = true;
        tiles.push_back(tile);
    }
    
    // 添加红线标记位置的草坪平台
    
    // 左侧平台（红线1）- 左侧中间位置
    for (int x = 0; x < 200; x += 16)
    {
        Tile tile;
        tile.pos = QPoint(x, 416);
        tile.width = 32;
        tile.height = 32;
        tile.isSolid = true;
        tiles.push_back(tile);
    }
    
    // 中间平台（红线2，树下方）- 树正下方
    for (int x = 450; x < 550; x += 16)
    {
        Tile tile;
        tile.pos = QPoint(x, 480);
        tile.width = 32;
        tile.height = 32;
        tile.isSolid = true;
        tiles.push_back(tile);
    }
    
    // 右侧平台（红线3）- 右侧较高位置
    for (int x = 700; x < 800; x += 16)
    {
        Tile tile;
        tile.pos = QPoint(x, 352);
        tile.width = 32;
        tile.height = 32;
        tile.isSolid = true;
        tiles.push_back(tile);
    }
}

void CGame::loadItems()
{
    // 清空物品
    items.clear();
    
    // 只在一人模式下生成金币
    if (playerCount == 1)
    {
        // 随机生成金币
        int coinCount = 10; // 生成10个金币
        for (int i = 0; i < coinCount; ++i)
        {
            Item item;
            // 随机位置，避开边缘
            item.pos = QPoint(rand() % (800 - 32), rand() % (600 - 32));
            item.width = 32;
            item.height = 32;
            item.collected = false;
            items.push_back(item);
        }
    }
}

void CGame::loadEnemies()
{
    // 清空敌人
    enemies.clear();
    
    // 单人模式下初始化敌人
    if (playerCount == 1) {
        // 初始生成一些敌人
        for (int i = 0; i < 5; i++) {
            spawnEnemy();
        }
    }
}

void CGame::loadDecorations()
{
    // 清空装饰 - 不加载任何装饰
    decorations.clear();
}

void CGame::updateGame()
{
    // 计算时间步长
    static const float TIME_STEP = 1.0f / 60.0f; // 60 FPS
    float deltaTime = elapsedTimer.restart() / 1000.0f;
    
    // 限制最大时间步长，防止游戏在卡顿后速度过快
    if (deltaTime > 0.1f) {
        deltaTime = 0.1f;
    }
    
    // 累积时间
    accumulator += deltaTime;
    
    // 使用固定时间步长更新游戏逻辑
    while (accumulator >= TIME_STEP) {
        // 根据游戏状态更新
        switch (gameState)
        {
        case STATE_START_SCREEN:
            // 开始界面不需要更新逻辑
            break;
            
        case STATE_CHARACTER_SELECT:
            // 角色选择界面不需要更新逻辑
            break;
            
        case STATE_PLAYING:
            if (gameOver)
                return;
            
            // 更新玩家1
            updatePlayer();
            
            // 单人模式下的特殊逻辑
            if (playerCount == 1)
            {
                // 定期生成敌人
                static int enemySpawnTimer = 0;
                if (enemySpawnTimer++ >= 60) { // 每60帧生成一个敌人
                    enemySpawnTimer = 0;
                    spawnEnemy();
                }
                
                // 更新敌人
                updateEnemies();
                
                // 更新经验球
                updateExpBalls();
                
                // 更新悬浮球
                updateFloatingOrbs();
                
                // 更新道具
            updatePowerUps();
            
            // 更新光环效果
            updateAuras();
                
                // 检查悬浮球与敌人的碰撞
                checkOrbEnemyCollisions();
                
                // 收集经验球
                collectExpBalls();
                
                // 检查道具碰撞
                checkPowerUpCollisions();
                
                // 检查升级
                if (exp >= expMax) {
                    levelUp();
                }
                
                // 技能冷却时间更新
            for (int i = 0; i < 3; i++) {
                if (skillCooldown[i] > 0) {
                    skillCooldown[i]--;
                    if (skillCooldown[i] == 0) {
                        isSkillAvailable[i] = true;
                    }
                }
            }
                
                // 技能触发逻辑
                if (isSkillPressed && isSkillAvailable[selectedSkill] && playerCount == 1) {
                    isSkillAvailable[selectedSkill] = false;
                    skillCooldown[selectedSkill] = skillCooldownMax[selectedSkill];
                    
                    switch (selectedSkill) {
                    case SKILL_MISSILE_RAIN:
                        // 连续发射10枚导弹
                        for (int i = 0; i < 10; i++) {
                            // 创建导弹
                            Bullet bullet;
                            bullet.pos = QPointF(playerPos.x() + playerWidth / 2 - 4, playerPos.y() + playerHeight / 2 - 4);
                            bullet.damage = skillDamage[SKILL_MISSILE_RAIN]; // 导弹伤害
                            bullet.active = true;
                            bullet.owner = 1;
                            bullet.isTracking = true;
                            
                            // 寻找最近的敌人作为目标
                            Enemy* closestEnemy = nullptr;
                            float closestDistance = FLT_MAX;
                            
                            for (auto& enemy : enemies) {
                                if (enemy.active) {
                                    float distance = QVector2D(enemy.pos - playerPos).length();
                                    if (distance < closestDistance) {
                                        closestDistance = distance;
                                        closestEnemy = &enemy;
                                    }
                                }
                            }
                            
                            if (closestEnemy) {
                                bullet.target = closestEnemy;
                                QVector2D direction = QVector2D(closestEnemy->pos - bullet.pos);
                                direction.normalize();
                                // 为每枚导弹添加微小的角度偏差，形成扇形发射
                                float angleOffset = (i - 4.5f) * 0.1f; // -0.45到0.45弧度的偏差
                                float cosOffset = cos(angleOffset);
                                float sinOffset = sin(angleOffset);
                                bullet.velocityX = (direction.x() * cosOffset - direction.y() * sinOffset) * 10.0f;
                                bullet.velocityY = (direction.x() * sinOffset + direction.y() * cosOffset) * 10.0f;
                            } else {
                                // 没有敌人时，导弹向四周扩散
                                float angle = 2.0f * M_PI * i / 10.0f;
                                bullet.velocityX = cos(angle) * 10.0f;
                                bullet.velocityY = sin(angle) * 10.0f;
                                bullet.isTracking = false;
                            }
                            
                            bullets.push_back(bullet);
                        }
                        break;
                        
                    case SKILL_ENERGY_SHIELD:
                        // 能量护盾：临时增加生命值
                        playerLives += 50;
                        if (playerLives > playerMaxLives) {
                            playerLives = playerMaxLives;
                        }
                        break;
                        
                    case SKILL_LIGHTNING_CHAIN:
                        // 闪电链：攻击多个敌人
                        if (!enemies.empty()) {
                            // 找到第一个敌人作为起点
                            Enemy* firstEnemy = nullptr;
                            for (auto& enemy : enemies) {
                                if (enemy.active) {
                                    firstEnemy = &enemy;
                                    break;
                                }
                            }
                            
                            if (firstEnemy) {
                                // 对第一个敌人造成伤害
                                firstEnemy->hp -= skillDamage[SKILL_LIGHTNING_CHAIN];
                                
                                // 使敌人眩晕1秒（60帧）
                                firstEnemy->isStunned = true;
                                firstEnemy->stunTimer = 60;
                                
                                // 在敌人位置上产生蓝色光环
                                Aura aura;
                                aura.pos = firstEnemy->pos;
                                aura.radius = 30;
                                aura.timer = 60; // 1秒
                                aura.active = true;
                                auras.push_back(aura);
                                
                                // 寻找其他敌人进行链式攻击
                                std::vector<Enemy*> hitEnemies;
                                hitEnemies.push_back(firstEnemy);
                                
                                // 最多链3个敌人
                                for (int chain = 0; chain < 2; chain++) {
                                    Enemy* nextEnemy = nullptr;
                                    float closestDistance = FLT_MAX;
                                    
                                    for (auto& enemy : enemies) {
                                        if (enemy.active) {
                                            // 检查是否已经被击中
                                            bool alreadyHit = false;
                                            for (auto* hitEnemy : hitEnemies) {
                                                if (&enemy == hitEnemy) {
                                                    alreadyHit = true;
                                                    break;
                                                }
                                            }
                                            
                                            if (!alreadyHit) {
                                                float distance = QVector2D(enemy.pos - hitEnemies.back()->pos).length();
                                                if (distance < closestDistance && distance < 200) { // 200像素范围内
                                                    closestDistance = distance;
                                                    nextEnemy = &enemy;
                                                }
                                            }
                                        }
                                    }
                                    
                                    if (nextEnemy) {
                                        nextEnemy->hp -= skillDamage[SKILL_LIGHTNING_CHAIN];
                                        
                                        // 使敌人眩晕1秒（60帧）
                                        nextEnemy->isStunned = true;
                                        nextEnemy->stunTimer = 60;
                                        
                                        // 在敌人位置上产生蓝色光环
                                        Aura aura;
                                        aura.pos = nextEnemy->pos;
                                        aura.radius = 30;
                                        aura.timer = 60; // 1秒
                                        aura.active = true;
                                        auras.push_back(aura);
                                        
                                        hitEnemies.push_back(nextEnemy);
                                    } else {
                                        break;
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            }
            else
            {
                // 多人模式逻辑
                // 多人模式下不生成敌人
                
                // 更新玩家2
                if (playerCount >= 2)
                    updatePlayerP2();
                
                // 更新玩家3
                if (playerCount >= 3)
                    updatePlayerP3();
                
                // 更新玩家4
                if (playerCount >= 4)
                    updatePlayerP4();
            }
            
            // 更新子弹
            updateBullets();
            
            // 检查碰撞
            checkCollisions();
            
            // 检查子弹碰撞
            checkBulletCollisions();
            
            // 收集物品
            collectItems();
            
            // 检查游戏结束
            checkGameOver();
            break;
            
        case STATE_GAME_OVER:
            // 游戏结束界面不需要更新逻辑
            break;
            
        case STATE_UPGRADE:
            // 升级界面不需要更新逻辑
            break;
        case STATE_DATA_PANEL:
            // 数据面板界面不需要更新逻辑（游戏暂停）
            break;
        }
        
        // 减少累积时间
        accumulator -= TIME_STEP;
    }
    
    // 重绘
    repaint();
}

void CGame::updatePlayer()
{
    if (playerCount == 1)
    {
        // 1玩家模式下的ASDW操控
        // 重置速度
        playerVelocityX = 0;
        playerVelocityY = 0;
        
        // 左右移动逻辑
        if (isLeftPressed && !isRightPressed)
        {
            playerVelocityX = -speed; // A键向左
            facingRight = false;
        }
        else if (isRightPressed && !isLeftPressed)
        {
            playerVelocityX = speed; // D键向右
            facingRight = true;
        }
        
        // 上下移动逻辑
        if (isUpPressed && !isDownPressed)
        {
            playerVelocityY = -speed; // W键向上
        }
        else if (isDownPressed && !isUpPressed)
        {
            playerVelocityY = speed; // S键向下
        }
        
        // 直接更新位置
        playerPos.setX(playerPos.x() + playerVelocityX);
        playerPos.setY(playerPos.y() + playerVelocityY);
        
        // 边界检查
        if (playerPos.x() < 0)
            playerPos.setX(0);
        if (playerPos.x() > 800 - playerWidth)
            playerPos.setX(800 - playerWidth);
        if (playerPos.y() < 0)
            playerPos.setY(0);
        if (playerPos.y() > 600 - playerHeight)
            playerPos.setY(600 - playerHeight);
    }
    else
    {
        // 多玩家模式下保持原有移动逻辑
        // 极简移动代码 - 确保左右移动流畅
        
        // 水平移动 - 完全直接控制
        const float MOVE_SPEED = 6.0f * 0.7f; // 原来的0.7
        
        // 重置速度
        playerVelocityX = 0;
        
        // 左右移动逻辑 - 确保处理逻辑完全一致
        if (isLeftPressed && !isRightPressed)
        {
            playerVelocityX = -MOVE_SPEED; // A键向左
            facingRight = false;
        }
        else if (isRightPressed && !isLeftPressed)
        {
            playerVelocityX = MOVE_SPEED; // D键向右
            facingRight = true;
        }
        
        // 垂直移动
        playerVelocityY += GRAVITY;
        if (playerVelocityY > 18.0f) // 12.0f * 1.5
        {
            playerVelocityY = 18.0f;
        }
        
        // 直接更新位置
        playerPos.setX(playerPos.x() + playerVelocityX);
        playerPos.setY(playerPos.y() + playerVelocityY);
    }
    
    // 动画
    if (std::abs(playerVelocityX) > 0.1f || std::abs(playerVelocityY) > 0.1f)
    {
        playerFrame = (playerFrame + 1) % 4;
    }
    
    // 攻击冷却时间
    if (attackCooldown > 0)
        attackCooldown--;
    
    // 攻击逻辑
    if (isAttackPressed && attackCooldown == 0)
    {
        // 创建子弹
        Bullet bullet;
        bullet.pos = QPointF(playerPos.x() + playerWidth / 2 - 4, playerPos.y() + playerHeight / 2 - 4);
        bullet.damage = damage;
        bullet.active = true;
        bullet.owner = 1;
        
        if (playerCount == 1)
        {
            // 单人模式：子弹自动追踪最近的敌人
            Enemy* closestEnemy = nullptr;
            float closestDistance = FLT_MAX;
            
            for (auto& enemy : enemies) {
                if (enemy.active) {
                    float distance = QVector2D(enemy.pos - playerPos).length();
                    if (distance < closestDistance) {
                        closestDistance = distance;
                        closestEnemy = &enemy;
                    }
                }
            }
            
            if (closestEnemy) {
                // 追踪子弹
                bullet.isTracking = true;
                bullet.target = closestEnemy;
                QVector2D direction = QVector2D(closestEnemy->pos - bullet.pos);
                direction.normalize();
                bullet.velocityX = direction.x() * 8.0f;
                bullet.velocityY = direction.y() * 8.0f;
            } else {
                // 没有敌人时，子弹直线飞行
                bullet.isTracking = false;
                bullet.velocityX = facingRight ? 8.0f : -8.0f;
                bullet.velocityY = 0;
            }
        }
        else
        {
            // 多人模式：子弹直线飞行
            bullet.isTracking = false;
            bullet.velocityX = facingRight ? 8.0f : -8.0f;
            bullet.velocityY = 0;
        }
        
        bullets.push_back(bullet);
        
        // 设置冷却时间（降低射击频率）
        if (playerCount == 1) {
            // 单人模式：冷却时间受攻击速度影响
            attackCooldown = static_cast<int>(10 / attackSpeed); // 10帧冷却，受攻击速度影响
        } else {
            // 多人模式：固定0.8秒冷却
            attackCooldown = 48; // 0.8秒 = 48帧
        }
    }
}

void CGame::updatePlayerP2()
{
    // 极简移动代码 - 确保左右移动流畅
    
    // 水平移动 - 完全直接控制
    const float MOVE_SPEED = 6.0f * 0.7f; // 原来的0.7
    
    // 重置速度
    playerVelocityXP2 = 0;
    
    // 左右移动逻辑 - 确保处理逻辑完全一致
    if (isLeftPressedP2 && !isRightPressedP2)
    {
        playerVelocityXP2 = -MOVE_SPEED; // 左方向键向左
        facingRightP2 = false;
    }
    else if (isRightPressedP2 && !isLeftPressedP2)
    {
        playerVelocityXP2 = MOVE_SPEED; // 右方向键向右
        facingRightP2 = true;
    }
    
    // 垂直移动
    playerVelocityYP2 += GRAVITY;
    if (playerVelocityYP2 > 18.0f) // 12.0f * 1.5
    {
        playerVelocityYP2 = 18.0f;
    }
    
    // 直接更新位置
    playerPosP2.setX(playerPosP2.x() + playerVelocityXP2);
    playerPosP2.setY(playerPosP2.y() + playerVelocityYP2);
    
    // 动画
    if (std::abs(playerVelocityXP2) > 0.1f)
    {
        playerFrameP2 = (playerFrameP2 + 1) % 4;
    }
    
    // 攻击冷却时间
    if (attackCooldownP2 > 0)
        attackCooldownP2--;
    
    // 攻击逻辑
    if (isAttackPressedP2 && attackCooldownP2 == 0)
    {
        // 创建子弹
        Bullet bullet;
        bullet.pos = QPointF(playerPosP2.x() + playerWidth / 2 - 4, playerPosP2.y() + playerHeight / 2 - 4);
        bullet.velocityX = facingRightP2 ? 8.0f : -8.0f;
        bullet.velocityY = 0;
        bullet.owner = 2;
        bullet.active = true;
        bullets.push_back(bullet);
        
        // 设置冷却时间（降低射击频率）
        attackCooldownP2 = 48; // 0.8秒 = 48帧
    }
}

void CGame::updatePlayerP3()
{
    // 极简移动代码 - 确保左右移动流畅
    
    // 水平移动 - 完全直接控制
    const float MOVE_SPEED = 6.0f * 0.7f; // 原来的0.7
    
    // 重置速度
    playerVelocityXP3 = 0;
    
    // 左右移动逻辑 - P3使用JKLI按键
    if (isLeftPressedP3 && !isRightPressedP3)
    {
        playerVelocityXP3 = -MOVE_SPEED; // L键向左
        facingRightP3 = false;
    }
    else if (isRightPressedP3 && !isLeftPressedP3)
    {
        playerVelocityXP3 = MOVE_SPEED; // I键向右
        facingRightP3 = true;
    }
    
    // 垂直移动
    playerVelocityYP3 += GRAVITY;
    if (playerVelocityYP3 > 18.0f) // 12.0f * 1.5
    {
        playerVelocityYP3 = 18.0f;
    }
    
    // 直接更新位置
    playerPosP3.setX(playerPosP3.x() + playerVelocityXP3);
    playerPosP3.setY(playerPosP3.y() + playerVelocityYP3);
    
    // 动画
    if (std::abs(playerVelocityXP3) > 0.1f)
    {
        playerFrameP3 = (playerFrameP3 + 1) % 4;
    }
    
    // 攻击冷却时间
    if (attackCooldownP3 > 0)
        attackCooldownP3--;
    
    // 攻击逻辑
    if (isAttackPressedP3 && attackCooldownP3 == 0)
    {
        // 创建子弹
        Bullet bullet;
        bullet.pos = QPointF(playerPosP3.x() + playerWidth / 2 - 4, playerPosP3.y() + playerHeight / 2 - 4);
        bullet.velocityX = facingRightP3 ? 8.0f : -8.0f;
        bullet.velocityY = 0;
        bullet.owner = 3;
        bullet.active = true;
        bullets.push_back(bullet);
        
        // 设置冷却时间（降低射击频率）
        attackCooldownP3 = 48; // 0.8秒 = 48帧
    }
}

void CGame::updatePlayerP4()
{
    // 极简移动代码 - 确保左右移动流畅
    
    // 水平移动 - 完全直接控制
    const float MOVE_SPEED = 6.0f * 0.7f; // 原来的0.7
    
    // 重置速度
    playerVelocityXP4 = 0;
    
    // 左右移动逻辑 - P4使用1352按键
    if (isLeftPressedP4 && !isRightPressedP4)
    {
        playerVelocityXP4 = -MOVE_SPEED; // 1键向左
        facingRightP4 = false;
    }
    else if (isRightPressedP4 && !isLeftPressedP4)
    {
        playerVelocityXP4 = MOVE_SPEED; // 3键向右
        facingRightP4 = true;
    }
    
    // 垂直移动
    playerVelocityYP4 += GRAVITY;
    if (playerVelocityYP4 > 18.0f) // 12.0f * 1.5
    {
        playerVelocityYP4 = 18.0f;
    }
    
    // 直接更新位置
    playerPosP4.setX(playerPosP4.x() + playerVelocityXP4);
    playerPosP4.setY(playerPosP4.y() + playerVelocityYP4);
    
    // 动画
    if (std::abs(playerVelocityXP4) > 0.1f)
    {
        playerFrameP4 = (playerFrameP4 + 1) % 4;
    }
    
    // 攻击冷却时间
    if (attackCooldownP4 > 0)
        attackCooldownP4--;
    
    // 攻击逻辑
    if (isAttackPressedP4 && attackCooldownP4 == 0)
    {
        // 创建子弹
        Bullet bullet;
        bullet.pos = QPointF(playerPosP4.x() + playerWidth / 2 - 4, playerPosP4.y() + playerHeight / 2 - 4);
        bullet.velocityX = facingRightP4 ? 8.0f : -8.0f;
        bullet.velocityY = 0;
        bullet.owner = 4;
        bullet.active = true;
        bullets.push_back(bullet);
        
        // 设置冷却时间（降低射击频率）
        attackCooldownP4 = 48; // 0.8秒 = 48帧
    }
}

void CGame::updateBullets()
{
    for (auto &bullet : bullets)
    {
        if (bullet.active)
        {
            // 更新子弹位置
            bullet.pos.setX(bullet.pos.x() + bullet.velocityX);
            bullet.pos.setY(bullet.pos.y() + bullet.velocityY);
            
            // 检查子弹是否超出屏幕
            if (bullet.pos.x() < 0 || bullet.pos.x() > 800 || bullet.pos.y() < 0 || bullet.pos.y() > 600)
            {
                bullet.active = false;
            }
        }
    }
    
    // 清理非活跃子弹
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet &b) { return !b.active; }), bullets.end());
}

void CGame::checkCollisions()
{
    // 单人模式下跳过瓦片碰撞检测，实现最简单的上下左右移动
    if (playerCount != 1)
    {
        // 只保留垂直碰撞检测，让角色能站在草坪上
        // 水平碰撞检测暂时注释，避免影响移动
        
        // 玩家1垂直碰撞检测
        QRect playerRectV(playerPos.x(), playerPos.y(), playerWidth, playerHeight);
        
        for (const auto &tile : tiles)
        {
            if (tile.isSolid)
            {
                QRect tileRect(tile.pos.x(), tile.pos.y(), tile.width, tile.height);
                
                if (playerRectV.intersects(tileRect))
                {
                    // 垂直碰撞
                    if (playerVelocityY > 0) // 下落
                    {
                        playerPos.setY(tileRect.top() - playerHeight);
                        playerVelocityY = 0;
                        isJumping = false; // 重置跳跃状态
                        jumpCount = 0; // 重置跳跃次数
                    }
                    else if (playerVelocityY < 0) // 上升
                    {
                        playerPos.setY(tileRect.bottom());
                        playerVelocityY = 0;
                    }
                }
            }
        }
        
        // 玩家2垂直碰撞检测
        QRect playerRectVP2(playerPosP2.x(), playerPosP2.y(), playerWidth, playerHeight);
        
        for (const auto &tile : tiles)
        {
            if (tile.isSolid)
            {
                QRect tileRect(tile.pos.x(), tile.pos.y(), tile.width, tile.height);
                
                if (playerRectVP2.intersects(tileRect))
                {
                    // 垂直碰撞
                    if (playerVelocityYP2 > 0) // 下落
                    {
                        playerPosP2.setY(tileRect.top() - playerHeight);
                        playerVelocityYP2 = 0;
                        isJumpingP2 = false; // 重置跳跃状态
                        jumpCountP2 = 0; // 重置跳跃次数
                    }
                    else if (playerVelocityYP2 < 0) // 上升
                    {
                        playerPosP2.setY(tileRect.bottom());
                        playerVelocityYP2 = 0;
                    }
                }
            }
        }
        
        // 玩家3垂直碰撞检测
        if (playerCount >= 3)
        {
            QRect playerRectVP3(playerPosP3.x(), playerPosP3.y(), playerWidth, playerHeight);
            
            for (const auto &tile : tiles)
            {
                if (tile.isSolid)
                {
                    QRect tileRect(tile.pos.x(), tile.pos.y(), tile.width, tile.height);
                    
                    if (playerRectVP3.intersects(tileRect))
                    {
                        // 垂直碰撞
                        if (playerVelocityYP3 > 0) // 下落
                        {
                            playerPosP3.setY(tileRect.top() - playerHeight);
                            playerVelocityYP3 = 0;
                            isJumpingP3 = false; // 重置跳跃状态
                            jumpCountP3 = 0; // 重置跳跃次数
                        }
                        else if (playerVelocityYP3 < 0) // 上升
                        {
                            playerPosP3.setY(tileRect.bottom());
                            playerVelocityYP3 = 0;
                        }
                    }
                }
            }
        }
        
        // 玩家4垂直碰撞检测
        if (playerCount >= 4)
        {
            QRect playerRectVP4(playerPosP4.x(), playerPosP4.y(), playerWidth, playerHeight);
            
            for (const auto &tile : tiles)
            {
                if (tile.isSolid)
                {
                    QRect tileRect(tile.pos.x(), tile.pos.y(), tile.width, tile.height);
                    
                    if (playerRectVP4.intersects(tileRect))
                    {
                        // 垂直碰撞
                        if (playerVelocityYP4 > 0) // 下落
                        {
                            playerPosP4.setY(tileRect.top() - playerHeight);
                            playerVelocityYP4 = 0;
                            isJumpingP4 = false; // 重置跳跃状态
                            jumpCountP4 = 0; // 重置跳跃次数
                        }
                        else if (playerVelocityYP4 < 0) // 上升
                        {
                            playerPosP4.setY(tileRect.bottom());
                            playerVelocityYP4 = 0;
                        }
                    }
                }
            }
        }
    }
    
    // 单人模式下，敌人与玩家的碰撞检测
    if (playerCount == 1)
    {
        QRect playerRect(playerPos.x(), playerPos.y(), playerWidth, playerHeight);
        
        for (auto& enemy : enemies) {
            if (enemy.active) {
                QRect enemyRect(enemy.pos.x(), enemy.pos.y(), enemy.width, enemy.height);
                
                if (playerRect.intersects(enemyRect)) {
                    // 玩家受到伤害
                    int damageTaken = enemy.damage - static_cast<int>(enemy.damage * reduce);
                    if (damageTaken < 1) damageTaken = 1;
                    playerLives -= damageTaken;
                    
                    // 敌人死亡
                    enemy.active = false;
                    enemyDeathCnt++;
                    addExpBall(enemy.pos.x(), enemy.pos.y(), enemy.expValue);
                }
            }
        }
    }
}

void CGame::checkBulletCollisions()
{
    // 玩家1碰撞检测
    QRect playerRect(playerPos.x(), playerPos.y(), playerWidth, playerHeight);
    
    // 玩家2碰撞检测
    QRect playerRectP2(playerPosP2.x(), playerPosP2.y(), playerWidth, playerHeight);
    
    // 玩家3碰撞检测
    QRect playerRectP3(0, 0, 0, 0);
    if (playerCount >= 3)
        playerRectP3 = QRect(playerPosP3.x(), playerPosP3.y(), playerWidth, playerHeight);
    
    // 玩家4碰撞检测
    QRect playerRectP4(0, 0, 0, 0);
    if (playerCount >= 4)
        playerRectP4 = QRect(playerPosP4.x(), playerPosP4.y(), playerWidth, playerHeight);
    
    for (auto &bullet : bullets)
    {
        if (bullet.active)
        {
            QRect bulletRect(bullet.pos.x(), bullet.pos.y(), 8, 8);
            
            // 检查子弹是否命中玩家1
            if (bullet.owner != 1 && bulletRect.intersects(playerRect))
            {
                playerLives--;
                bullet.active = false;
            }
            
            // 检查子弹是否命中玩家2
            if (bullet.owner != 2 && bulletRect.intersects(playerRectP2))
            {
                playerLivesP2--;
                bullet.active = false;
            }
            
            // 检查子弹是否命中玩家3
            if (playerCount >= 3 && bullet.owner != 3 && bulletRect.intersects(playerRectP3))
            {
                playerLivesP3--;
                bullet.active = false;
            }
            
            // 检查子弹是否命中玩家4
            if (playerCount >= 4 && bullet.owner != 4 && bulletRect.intersects(playerRectP4))
            {
                playerLivesP4--;
                bullet.active = false;
            }
            
            // 检查子弹是否命中敌方
            if (bullet.owner > 0) // 确保是玩家的子弹
            {
                for (auto& enemy : enemies) {
                    if (enemy.active) {
                        QRect enemyRect(enemy.pos.x(), enemy.pos.y(), enemy.width, enemy.height);
                        if (bulletRect.intersects(enemyRect)) {
                            // 敌人受到伤害
                            enemy.hp -= bullet.damage;
                            if (enemy.hp <= 0) {
                                // 敌人死亡
                                enemy.active = false;
                                enemyDeathCnt++;
                                if (playerCount == 1) {
                                    // 单人模式下生成经验球
                                    addExpBall(enemy.pos.x(), enemy.pos.y(), enemy.expValue);
                                }
                            }
                            bullet.active = false;
                            break;
                        }
                    }
                }
            }
            
            // 检查敌人是否与玩家碰撞
            if (playerCount == 1) {
                for (auto& enemy : enemies) {
                    if (enemy.active) {
                        QRect enemyRect(enemy.pos.x(), enemy.pos.y(), enemy.width, enemy.height);
                        
                        // 检查敌人是否与玩家1碰撞
                        if (enemyRect.intersects(playerRect)) {
                            playerLives--;
                            enemy.active = false;
                            break;
                        }
                    }
                }
            }
            
            // 更新追踪子弹的方向
            if (bullet.isTracking && bullet.target) {
                // 确保target指针有效
                try {
                    // 额外的安全检查：确保指针不是无效值
                    uintptr_t targetAddr = reinterpret_cast<uintptr_t>(bullet.target);
                    if (targetAddr < 0x10000 || targetAddr > 0x7FFFFFFFFFFFFFFF) {
                        // 指针值太小或太大，可能是无效的
                        bullet.isTracking = false;
                        bullet.target = nullptr;
                    } else {
                        // 尝试访问target的active属性
                        if (bullet.target->active) {
                            QVector2D direction = QVector2D(bullet.target->pos - bullet.pos);
                            direction.normalize();
                            bullet.velocityX = direction.x() * 8.0f;
                            bullet.velocityY = direction.y() * 8.0f;
                        } else {
                            // 目标已死亡，停止追踪
                            bullet.isTracking = false;
                            bullet.target = nullptr;
                        }
                    }
                } catch (...) {
                    // 指针无效，停止追踪
                    bullet.isTracking = false;
                    bullet.target = nullptr;
                }
            }
        }
    }
}

void CGame::collectItems()
{
    // 移除金币系统
}

void CGame::updateEnemies()
{
    // 更新所有敌人
    for (auto& enemy : enemies) {
        if (enemy.active) {
            // 处理眩晕状态
            if (enemy.isStunned) {
                // 眩晕时停止移动
                enemy.velocityX = 0;
                enemy.velocityY = 0;
                
                // 眩晕计时器递减
                if (enemy.stunTimer > 0) {
                    enemy.stunTimer--;
                } else {
                    // 眩晕结束
                    enemy.isStunned = false;
                }
            } else {
                // 敌人向玩家移动
                QVector2D direction = QVector2D(playerPos - enemy.pos);
                direction.normalize();
                enemy.velocityX = direction.x() * 2.0f;
                enemy.velocityY = direction.y() * 2.0f;
                
                // 更新敌人位置
                enemy.pos.setX(enemy.pos.x() + enemy.velocityX);
                enemy.pos.setY(enemy.pos.y() + enemy.velocityY);
            }
        }
    }
}

void CGame::updateExpBalls()
{
    // 更新所有经验球
    for (auto& expBall : expBalls) {
        if (expBall.active) {
            // 经验球向玩家移动
            QVector2D direction = QVector2D(playerPos - expBall.pos);
            float distance = direction.length();
            if (distance < 200) { // 当经验球靠近玩家时，开始向玩家移动
                direction.normalize();
                expBall.pos.setX(expBall.pos.x() + direction.x() * 3.0f);
                expBall.pos.setY(expBall.pos.y() + direction.y() * 3.0f);
            }
            
            // 经验球计时器
            if (expBall.timer++ >= 600) { // 10秒后消失
                expBall.active = false;
                expBallCnt--;
            }
        }
    }
}

void CGame::updateFloatingOrbs()
{
    // 更新所有悬浮球
    for (int i = 0; i < floatingOrbs.size(); i++) {
        auto& orb = floatingOrbs[i];
        if (orb.active) {
            // 悬浮球围绕玩家旋转
            orb.angle += 0.05f;
            orb.pos.setX(playerPos.x() + playerWidth / 2 + cos(orb.angle + i * 2.0f * M_PI / floatingOrbs.size()) * orb.radius);
            orb.pos.setY(playerPos.y() + playerHeight / 2 + sin(orb.angle + i * 2.0f * M_PI / floatingOrbs.size()) * orb.radius);
        }
    }
}

void CGame::updatePowerUps()
{
    // 更新所有道具
    for (auto& powerUp : powerUps) {
        if (powerUp.active) {
            // 道具持续时间减少
            if (powerUp.duration > 0) {
                powerUp.duration--;
            } else {
                // 道具持续时间结束
                powerUp.active = false;
            }
        }
    }
    
    // 更新道具效果
    if (hasPowerUp) {
        if (powerUpDuration > 0) {
            powerUpDuration--;
        } else {
            // 道具效果结束
            damage -= powerUpDamage;
            speed -= powerUpSpeed;
            powerUpDamage = 0;
            powerUpSpeed = 0.0f;
            hasPowerUp = false;
        }
    }
}

void CGame::updateAuras()
{
    // 更新所有光环效果
    for (auto& aura : auras) {
        if (aura.active) {
            // 光环持续时间减少
            if (aura.timer > 0) {
                aura.timer--;
            } else {
                // 光环持续时间结束
                aura.active = false;
            }
        }
    }
    
    // 移除不活跃的光环
    auras.erase(std::remove_if(auras.begin(), auras.end(), [](const Aura& aura) {
        return !aura.active;
    }), auras.end());
}

void CGame::checkPowerUpCollisions()
{
    // 检查玩家与道具的碰撞
    QRect playerRect(playerPos.x(), playerPos.y(), playerWidth, playerHeight);
    
    for (auto& powerUp : powerUps) {
        if (powerUp.active) {
            QRect powerUpRect(powerUp.pos.x(), powerUp.pos.y(), 32, 32);
            if (playerRect.intersects(powerUpRect)) {
                // 激活道具效果
                // 道具效果为回复10点生命
                playerLives += 10;
                if (playerLives > playerMaxLives) {
                    playerLives = playerMaxLives;
                }
                
                // 道具被收集，不再显示
                powerUp.active = false;
            }
        }
    }
}

void CGame::checkOrbEnemyCollisions()
{
    // 检查悬浮球与敌人的碰撞
    for (auto& orb : floatingOrbs) {
        if (orb.active) {
            for (auto& enemy : enemies) {
                if (enemy.active) {
                    QRect orbRect(orb.pos.x(), orb.pos.y(), orb.width, orb.height);
                    QRect enemyRect(enemy.pos.x(), enemy.pos.y(), enemy.width, enemy.height);
                    
                    if (orbRect.intersects(enemyRect)) {
                        // 敌人受到伤害
                        enemy.hp -= orb.damage;
                        if (enemy.hp <= 0) {
                            // 敌人死亡，生成经验球
                            enemy.active = false;
                            enemyDeathCnt++;
                            addExpBall(enemy.pos.x(), enemy.pos.y(), enemy.expValue);
                        }
                    }
                }
            }
        }
    }
}

void CGame::collectExpBalls()
{
    // 收集经验球
    QRect playerRect(playerPos.x(), playerPos.y(), playerWidth, playerHeight);
    
    for (auto& expBall : expBalls) {
        if (expBall.active) {
            QRect expBallRect(expBall.pos.x(), expBall.pos.y(), 32, 32);
            if (playerRect.intersects(expBallRect)) {
                // 收集经验球
                exp += expBall.value;
                expBall.active = false;
                expBallCnt--;
            }
        }
    }
}

void CGame::spawnEnemy()
{
    // 在屏幕边缘生成敌人
    Enemy enemy;
    enemy.width = 32;
    enemy.height = 32;
    enemy.hp = 50 + playerLevel * 10; // 敌人生命值随玩家等级增加
    enemy.maxHp = enemy.hp;
    enemy.damage = 10 + playerLevel * 2; // 敌人伤害随玩家等级增加
    enemy.expValue = 20 + playerLevel * 5; // 敌人经验值随玩家等级增加
    enemy.active = true;
    
    // 随机选择敌人类型
    enemy.type = rand() % 3;
    
    // 随机选择生成位置（屏幕边缘）
    int side = rand() % 4;
    switch (side) {
    case 0: // 顶部
        enemy.pos = QPointF(rand() % 800, -50);
        break;
    case 1: // 右侧
        enemy.pos = QPointF(850, rand() % 600);
        break;
    case 2: // 底部
        enemy.pos = QPointF(rand() % 800, 650);
        break;
    case 3: // 左侧
        enemy.pos = QPointF(-50, rand() % 600);
        break;
    }
    
    enemy.velocityX = 0;
    enemy.velocityY = 0;
    enemy.isStunned = false;
    enemy.stunTimer = 0;
    
    enemies.push_back(enemy);
}

void CGame::levelUp()
{
    // 升级
    playerLevel++;
    exp -= expMax;
    expMax = 100 + playerLevel * 50; // 升级所需经验随等级增加
    
    // 生成随机的升级选项
    std::vector<int> availableChoices = {0, 1, 2, 3, 4, 5};
    selectedUpgradeChoices.clear();
    
    for (int i = 0; i < 3; i++) {
        int randomIndex = rand() % availableChoices.size();
        selectedUpgradeChoices.push_back(availableChoices[randomIndex]);
        availableChoices.erase(availableChoices.begin() + randomIndex);
    }
    
    // 显示升级面板
    gameState = STATE_UPGRADE;
}



void CGame::handleUpgradeChoice(int choice)
{
    // 处理升级选择
    if (choice >= 0 && choice < selectedUpgradeChoices.size()) {
        int actualChoice = selectedUpgradeChoices[choice];
        switch (actualChoice) {
        case 0: // 攻击力 +5
            damage += 5;
            break;
        case 1: // 速度 +1
            speed += 1.0f;
            break;
        case 2: // 生命值 +20
            playerLives += 20;
            playerMaxLives += 20;
            break;
        case 3: // 攻击速度 +5%
            attackSpeed *= 1.05f;
            break;
        case 4: // 技能伤害 +50
            skillDamage[selectedSkill] += 50;
            break;
        case 5: // 技能冷却 -10%
            skillCooldownMax[selectedSkill] = static_cast<int>(skillCooldownMax[selectedSkill] * 0.9f);
            if (skillCooldownMax[selectedSkill] < 60) { // 最低1秒
                skillCooldownMax[selectedSkill] = 60;
            }
            break;
        }
    }
    
    // 回到游戏
    gameState = STATE_PLAYING;
    
    // 每次升级时，在屏幕中随机生成一个道具
    PowerUp powerUp;
    powerUp.pos = QPointF(rand() % (800 - 32), rand() % (600 - 32));
    powerUp.type = rand() % 3; // 0=攻击力提升, 1=速度提升, 2=生命值恢复
    powerUp.duration = 180; // 3秒（180帧）
    powerUp.active = true;
    powerUps.push_back(powerUp);
    
    // 每生三级会出现BOSS增加难度
    if (playerLevel % 3 == 0 && playerLevel > 0) {
        // 生成BOSS
        Enemy boss;
        boss.width = 64;
        boss.height = 64;
        boss.hp = (500 + playerLevel * 50) / 5; // BOSS生命值为原来的1/5
        boss.maxHp = boss.hp;
        boss.damage = 20 + playerLevel * 5; // BOSS伤害随玩家等级增加
        boss.expValue = 500 + playerLevel * 100; // BOSS经验值随玩家等级增加
        boss.active = true;
        boss.type = 99; // BOSS类型
        
        // 在屏幕边缘生成BOSS
        int side = rand() % 4;
        switch (side) {
        case 0: // 顶部
            boss.pos = QPointF(rand() % 800, -100);
            break;
        case 1: // 右侧
            boss.pos = QPointF(900, rand() % 600);
            break;
        case 2: // 底部
            boss.pos = QPointF(rand() % 800, 700);
            break;
        case 3: // 左侧
            boss.pos = QPointF(-100, rand() % 600);
            break;
        }
        
        boss.velocityX = 0;
        boss.velocityY = 0;
        
        enemies.push_back(boss);
    }
}

void CGame::addExpBall(double x, double y, int value)
{
    // 添加经验球
    if (expBallCnt >= EBALL_MAX) {
        return;
    }
    
    for (auto& expBall : expBalls) {
        if (!expBall.active) {
            expBall.pos = QPointF(x, y);
            expBall.value = value;
            expBall.active = true;
            expBall.timer = 0;
            expBallCnt++;
            return;
        }
    }
}

void CGame::drawUpgradePanel(QPainter &painter)
{
    // 绘制半透明背景
    painter.fillRect(0, 0, 800, 600, QColor(0, 0, 0, 180));
    
    // 绘制升级面板
    painter.setPen(Qt::white);
    painter.setFont(QFont("微软雅黑", 24, QFont::Bold));
    painter.drawText(350, 150, "选择升级");
    
    // 绘制升级选项
    painter.setFont(QFont("微软雅黑", 16, QFont::Medium));
    
    // 选项1
    painter.drawRect(250, 200, 300, 60);
    switch (selectedUpgradeChoices[0]) {
    case 0:
        painter.drawText(300, 240, "攻击力 +5");
        break;
    case 1:
        painter.drawText(300, 240, "速度 +1");
        break;
    case 2:
        painter.drawText(300, 240, "生命值 +20");
        break;
    case 3:
        painter.drawText(300, 240, "攻击速度 +5%");
        break;
    case 4:
        painter.drawText(300, 240, "技能伤害 +50");
        break;
    case 5:
        painter.drawText(300, 240, "技能冷却 -10%");
        break;
    }
    
    // 选项2
    painter.drawRect(250, 280, 300, 60);
    switch (selectedUpgradeChoices[1]) {
    case 0:
        painter.drawText(300, 320, "攻击力 +5");
        break;
    case 1:
        painter.drawText(300, 320, "速度 +1");
        break;
    case 2:
        painter.drawText(300, 320, "生命值 +20");
        break;
    case 3:
        painter.drawText(300, 320, "攻击速度 +5%");
        break;
    case 4:
        painter.drawText(300, 320, "技能伤害 +50");
        break;
    case 5:
        painter.drawText(300, 320, "技能冷却 -10%");
        break;
    }
    
    // 选项3
    painter.drawRect(250, 360, 300, 60);
    switch (selectedUpgradeChoices[2]) {
    case 0:
        painter.drawText(300, 400, "攻击力 +5");
        break;
    case 1:
        painter.drawText(300, 400, "速度 +1");
        break;
    case 2:
        painter.drawText(300, 400, "生命值 +20");
        break;
    case 3:
        painter.drawText(300, 400, "攻击速度 +5%");
        break;
    case 4:
        painter.drawText(300, 400, "技能伤害 +50");
        break;
    case 5:
        painter.drawText(300, 400, "技能冷却 -10%");
        break;
    }
    
    // 绘制提示
    painter.setFont(QFont("微软雅黑", 14, QFont::Medium));
    painter.drawText(300, 460, "按 1、2、3 选择升级选项");
}

void CGame::checkGameOver()
{
    // 检查是否掉落
    if (playerPos.y() > 600 || (playerCount >= 2 && playerPosP2.y() > 600) || (playerCount >= 3 && playerPosP3.y() > 600) || (playerCount >= 4 && playerPosP4.y() > 600))
    {
        gameOver = true;
        gameState = STATE_GAME_OVER;
    }
    
    // 检查生命值
    int alivePlayers = 0;
    int lastAlivePlayer = 0;
    
    if (playerLives > 0)
    {
        alivePlayers++;
        lastAlivePlayer = 1;
    }
    if (playerCount >= 2 && playerLivesP2 > 0)
    {
        alivePlayers++;
        lastAlivePlayer = 2;
    }
    if (playerCount >= 3 && playerLivesP3 > 0)
    {
        alivePlayers++;
        lastAlivePlayer = 3;
    }
    if (playerCount >= 4 && playerLivesP4 > 0)
    {
        alivePlayers++;
        lastAlivePlayer = 4;
    }
    
    // 只有在多人模式下才根据存活玩家数量判定游戏结束
    if (playerCount > 1)
    {
        if (alivePlayers == 1)
        {
            gameOver = true;
            gameState = STATE_GAME_OVER;
            winner = lastAlivePlayer;
        }
        else if (alivePlayers == 0)
        {
            gameOver = true;
            gameState = STATE_GAME_OVER;
            winner = 0; // 平局
        }
    }
    else
    {
        // 单人模式下，只有当玩家生命值为0时才结束游戏
        if (alivePlayers == 0)
        {
            gameOver = true;
            gameState = STATE_GAME_OVER;
            winner = 0; // 游戏结束
        }
    }
}

void CGame::restartGame()
{
    initGame();
}

void CGame::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    
    // 根据游戏状态绘制不同界面
    switch (gameState)
    {
    case STATE_START_SCREEN:
        drawStartScreen(painter);
        break;
        
    case STATE_PLAYER_SELECT:
        drawPlayerSelect(painter);
        break;
        
    case STATE_CHARACTER_SELECT:
        drawCharacterSelect(painter);
        break;
        
    case STATE_PLAYING:
        drawGame(painter);
        break;
        
    case STATE_GAME_OVER:
        drawGameOver(painter);
        break;
        
    case STATE_HELP:
        drawHelp(painter);
        break;
        
    case STATE_UPGRADE:
        drawGame(painter);
        break;
    case STATE_DATA_PANEL:
        drawDataPanel(painter);
        break;
    }
}

void CGame::drawStartScreen(QPainter &painter)
{
    // 绘制渐变背景
    QLinearGradient backgroundGradient(0, 0, 0, 600);
    backgroundGradient.setColorAt(0, QColor(40, 40, 80));
    backgroundGradient.setColorAt(1, QColor(20, 20, 50));
    painter.fillRect(0, 0, 800, 600, backgroundGradient);
    
    // 绘制装饰元素
    // 绘制星星背景
    painter.setPen(QColor(200, 200, 255, 100));
    for (int i = 0; i < 50; i++) {
        int x = rand() % 800;
        int y = rand() % 400;
        painter.drawEllipse(x, y, 2, 2);
    }
    
    // 绘制装饰图片
    if (!cloudImage.isNull()) {
        painter.drawImage(80, 80, cloudImage.scaled(120, 60));
        painter.drawImage(600, 120, cloudImage.scaled(140, 70));
        painter.drawImage(300, 100, cloudImage.scaled(100, 50));
    }
    
    // 绘制标题
    painter.setPen(QColor(255, 255, 255));
    painter.setFont(QFont("微软雅黑", 64, QFont::Bold));
    // 添加文字阴影效果
    painter.setPen(QColor(0, 0, 0, 100));
    painter.drawText(182, 202, "幸存者大作战");
    painter.setPen(QColor(255, 255, 255));
    painter.drawText(180, 200, "幸存者大作战");
    
    // 绘制副标题
    painter.setFont(QFont("微软雅黑", 20, QFont::Medium));
    painter.setPen(QColor(200, 200, 255));
    painter.drawText(300, 260, "生存与竞技的终极挑战");
    
    // 绘制开始提示
    painter.setFont(QFont("微软雅黑", 32, QFont::Medium));
    painter.setPen(QColor(255, 255, 200));
    painter.drawText(240, 380, "按空格键开始游戏");
    
    // 绘制游戏特色
    painter.setFont(QFont("微软雅黑", 16));
    painter.setPen(QColor(180, 180, 255));
    painter.drawText(220, 450, "• 单人幸存者模式 - 击败敌人升级变强");
    painter.drawText(220, 480, "• 多人对战模式 - 成为最后存活的幸存者");
    painter.drawText(220, 510, "• 多种技能和升级系统");
    
    // 绘制版本信息
    painter.setFont(QFont("微软雅黑", 12));
    painter.setPen(QColor(150, 150, 200));
    painter.drawText(650, 590, "版本 1.0.0");
}

void CGame::drawPlayerSelect(QPainter &painter)
{
    // 绘制背景
    painter.fillRect(0, 0, 800, 600, QColor(30, 30, 60));
    
    // 绘制装饰图片
    if (!cloudImage.isNull()) {
        painter.drawImage(100, 80, cloudImage.scaled(100, 50));
        painter.drawImage(600, 120, cloudImage.scaled(120, 60));
    }
    
    // 绘制标题
    painter.setPen(Qt::white);
    painter.setFont(QFont("微软雅黑", 48, QFont::Bold));
    painter.drawText(220, 120, "选择玩家数量");
    
    // 绘制选项
    int optionWidth = 400;
    int optionHeight = 50;
    int startY = 160;
    int spacing = 60;
    
    // 1人选项
    QRect option1(200, startY, optionWidth, optionHeight);
    if (playerCount == 1) {
        painter.setPen(QPen(Qt::green, 4));
        painter.setBrush(QColor(0, 100, 0, 50));
    } else {
        painter.setPen(QPen(Qt::white, 2));
        painter.setBrush(Qt::NoBrush);
    }
    painter.drawRect(option1);
    painter.setFont(QFont("微软雅黑", 22, QFont::Bold));
    painter.drawText(option1, Qt::AlignCenter, "1人游戏");
    
    // 2人选项
    QRect option2(200, startY + spacing, optionWidth, optionHeight);
    if (playerCount == 2) {
        painter.setPen(QPen(Qt::green, 4));
        painter.setBrush(QColor(0, 100, 0, 50));
    } else {
        painter.setPen(QPen(Qt::white, 2));
        painter.setBrush(Qt::NoBrush);
    }
    painter.drawRect(option2);
    painter.setFont(QFont("微软雅黑", 22, QFont::Bold));
    painter.drawText(option2, Qt::AlignCenter, "2人游戏");
    
    // 3人选项
    QRect option3(200, startY + spacing * 2, optionWidth, optionHeight);
    if (playerCount == 3) {
        painter.setPen(QPen(Qt::green, 4));
        painter.setBrush(QColor(0, 100, 0, 50));
    } else {
        painter.setPen(QPen(Qt::white, 2));
        painter.setBrush(Qt::NoBrush);
    }
    painter.drawRect(option3);
    painter.setFont(QFont("微软雅黑", 20, QFont::Bold));
    painter.drawText(option3, Qt::AlignCenter, "3人游戏 (JKLI控制)");
    
    // 4人选项
    QRect option4(200, startY + spacing * 3, optionWidth, optionHeight);
    if (playerCount == 4) {
        painter.setPen(QPen(Qt::green, 4));
        painter.setBrush(QColor(0, 100, 0, 50));
    } else {
        painter.setPen(QPen(Qt::white, 2));
        painter.setBrush(Qt::NoBrush);
    }
    painter.drawRect(option4);
    painter.setFont(QFont("微软雅黑", 20, QFont::Bold));
    painter.drawText(option4, Qt::AlignCenter, "4人游戏 (1235控制)");
    
    // 帮助选项
    QRect optionHelp(200, startY + spacing * 4, optionWidth, optionHeight);
    if (playerCount == 0) {
        painter.setPen(QPen(Qt::cyan, 4));
        painter.setBrush(QColor(0, 100, 100, 50));
    } else {
        painter.setPen(QPen(Qt::cyan, 2));
        painter.setBrush(Qt::NoBrush);
    }
    painter.drawRect(optionHelp);
    painter.setFont(QFont("微软雅黑", 22, QFont::Bold));
    painter.drawText(optionHelp, Qt::AlignCenter, "帮助");
    
    // 绘制操作提示
    painter.setPen(Qt::white);
    painter.setFont(QFont("微软雅黑", 18, QFont::Medium));
    painter.drawText(220, 480, "上下方向键选择，空格键确认");
    painter.drawText(240, 510, "选择帮助查看游戏教程");
    
    // 绘制底部装饰
    if (!groundTile.isNull()) {
        for (int x = 0; x < 800; x += 32) {
            painter.drawImage(x, 568, groundTile);
        }
    }
}

void CGame::drawHelp(QPainter &painter)
{
    // 绘制背景
    painter.fillRect(0, 0, 800, 600, QColor(20, 20, 40));
    
    // 绘制标题
    painter.setPen(Qt::white);
    painter.setFont(QFont("微软雅黑", 40, QFont::Bold));
    painter.drawText(300, 80, "游戏帮助");
    
    // 绘制教程内容
    painter.setFont(QFont("微软雅黑", 16, QFont::Medium));
    
    // 游戏模式说明
    painter.setPen(Qt::cyan);
    painter.drawText(50, 130, "游戏模式说明:");
    painter.setPen(Qt::white);
    painter.drawText(80, 160, "1人游戏: 单人幸存者模式，击败敌人获取经验升级");
    painter.drawText(80, 190, "2人游戏: 双人对战模式，击败对手成为最后幸存者");
    painter.drawText(80, 220, "3人游戏: 三人对战模式，使用JKLI控制");
    painter.drawText(80, 250, "4人游戏: 四人对战模式，使用1235控制");
    
    // 按键说明
    painter.setPen(Qt::cyan);
    painter.drawText(50, 300, "按键说明:");
    painter.setPen(Qt::white);
    painter.drawText(80, 330, "P1 (1人游戏): WASD移动，空格键攻击，F键释放技能");
    painter.drawText(80, 360, "P2 (2人游戏): 方向键移动，空格键攻击");
    painter.drawText(80, 390, "P3 (3人游戏): J/L左右移动，I/K上下移动，空格键攻击");
    painter.drawText(80, 420, "P4 (4人游戏): 1/3左右移动，5/2上下移动，空格键攻击");
    
    // 游戏目标
    painter.setPen(Qt::cyan);
    painter.drawText(50, 470, "游戏目标:");
    painter.setPen(Qt::white);
    painter.drawText(80, 500, "- 单人模式: 尽可能长时间生存，击败敌人升级变强");
    painter.drawText(80, 530, "- 多人模式: 击败所有对手，成为最后存活的幸存者");
    
    // 返回提示
    painter.setPen(Qt::yellow);
    painter.setFont(QFont("微软雅黑", 18, QFont::Bold));
    painter.drawText(250, 570, "按空格键返回");
}

void CGame::drawDataPanel(QPainter &painter)
{
    // 绘制半透明背景
    painter.fillRect(0, 0, 800, 600, QColor(0, 0, 0, 180));
    
    // 绘制数据面板
    painter.setPen(Qt::white);
    painter.setFont(QFont("微软雅黑", 24, QFont::Bold));
    painter.drawText(350, 100, "数据面板");
    
    // 绘制数据
    painter.setFont(QFont("微软雅黑", 16, QFont::Medium));
    painter.drawText(300, 150, QString("攻击力: %1").arg(damage));
    painter.drawText(300, 180, QString("攻击速度: %1次/秒").arg(60.0f / (10.0f / attackSpeed), 0, 'f', 2)); // 基于10帧冷却，60帧/秒
    
    // 显示当前选中的技能
    QString skillName;
    switch (selectedSkill) {
    case SKILL_MISSILE_RAIN:
        skillName = "导弹雨";
        break;
    case SKILL_ENERGY_SHIELD:
        skillName = "能量护盾";
        break;
    case SKILL_LIGHTNING_CHAIN:
        skillName = "闪电链";
        break;
    }
    painter.drawText(300, 210, QString("当前技能: %1").arg(skillName));
    painter.drawText(300, 240, QString("技能伤害: %1").arg(skillDamage[selectedSkill]));
    painter.drawText(300, 270, QString("技能冷却: %1秒").arg(skillCooldownMax[selectedSkill] / 60));
    painter.drawText(300, 300, QString("生命值: %1/%2").arg(playerLives).arg(playerMaxLives));
    painter.drawText(300, 330, QString("速度: %1m/s").arg(speed * 0.5, 0, 'f', 1)); // 基础速度8.0f显示为4.0m/s
    painter.drawText(300, 360, QString("等级: %1").arg(playerLevel));
    painter.drawText(300, 390, QString("经验: %1/%2").arg(exp).arg(expMax));
    painter.drawText(300, 420, QString("击杀: %1").arg(enemyDeathCnt));
    
    // 绘制提示
    painter.setPen(Qt::yellow);
    painter.setFont(QFont("微软雅黑", 18, QFont::Bold));
    painter.drawText(250, 500, "按P键返回游戏");
}

void CGame::drawCharacterSelect(QPainter &painter)
{
    // 绘制背景
    painter.fillRect(0, 0, 800, 600, QColor(40, 40, 70));
    
    // 绘制装饰图片
    if (!cloudImage.isNull()) {
        painter.drawImage(50, 50, cloudImage.scaled(80, 40));
        painter.drawImage(650, 80, cloudImage.scaled(100, 50));
    }
    
    // 绘制角色选项
    int iconSize = 70;
    int spacing = 100;
    int cols = 2;
    
    // P1角色选择
    painter.setPen(Qt::white);
    painter.setFont(QFont("微软雅黑", 18, QFont::Bold));
    painter.drawText(100, 100, "P1");
    
    // P1角色选择区域
    int startX1 = 50;
    int startY1 = 130;
    for (int i = 0; i < characterCount && i < 4; ++i)
    {
        int row = i / cols;
        int col = i % cols;
        int x = startX1 + col * spacing;
        int y = startY1 + row * 100;
        
        // 绘制选中框
        if (i == selectedCharacterIndex)
        {
            painter.setPen(QPen(Qt::blue, 4));
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(x - 4, y - 4, iconSize + 8, iconSize + 8);
        }
        
        // 绘制角色图片
        if (i < characterImages.size())
        {
            painter.drawImage(x, y, characterImages[i].scaled(iconSize, iconSize, Qt::KeepAspectRatio));
        }
    }
    
    // 当玩家数量为1时，只显示P1角色选择
    if (playerCount == 1)
    {
        // 技能选择
        painter.setPen(Qt::white);
        painter.setFont(QFont("微软雅黑", 18, QFont::Bold));
        painter.drawText(450, 100, "技能选择");
        
        // 技能选项
        int skillSpacing = 120;
        int startX = 400;
        int startY = 150;
        
        // 技能1：导弹雨
        if (selectedSkill == SKILL_MISSILE_RAIN) {
            painter.setPen(QPen(Qt::blue, 4));
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(startX - 4, startY - 4, 100, 40);
        }
        painter.setPen(Qt::white);
        painter.setFont(QFont("微软雅黑", 14, QFont::Medium));
        painter.drawText(startX, startY + 25, "1. 导弹雨");
        
        // 技能2：能量护盾
        if (selectedSkill == SKILL_ENERGY_SHIELD) {
            painter.setPen(QPen(Qt::blue, 4));
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(startX - 4, startY + 50 - 4, 100, 40);
        }
        painter.setPen(Qt::white);
        painter.drawText(startX, startY + 50 + 25, "2. 能量护盾");
        
        // 技能3：闪电链
        if (selectedSkill == SKILL_LIGHTNING_CHAIN) {
            painter.setPen(QPen(Qt::blue, 4));
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(startX - 4, startY + 100 - 4, 100, 40);
        }
        painter.setPen(Qt::white);
        painter.drawText(startX, startY + 100 + 25, "3. 闪电链");
        
        // 技能描述
        painter.setPen(Qt::yellow);
        painter.setFont(QFont("微软雅黑", 12, QFont::Medium));
        painter.drawText(400, 300, "使用数字键1-3选择技能");
        
        switch (selectedSkill) {
        case SKILL_MISSILE_RAIN:
            painter.drawText(400, 320, "发射10枚追踪导弹");
            painter.drawText(400, 340, "冷却时间: 8秒");
            painter.drawText(400, 360, "初始伤害: 150");
            break;
        case SKILL_ENERGY_SHIELD:
            painter.drawText(400, 320, "恢复50点生命值");
            painter.drawText(400, 340, "冷却时间: 10秒");
            painter.drawText(400, 360, "无伤害");
            break;
        case SKILL_LIGHTNING_CHAIN:
            painter.drawText(400, 320, "攻击多个敌人，最多链3个");
            painter.drawText(400, 340, "冷却时间: 6秒");
            painter.drawText(400, 360, "初始伤害: 100");
            break;
        }
    }
    else
    {
        // P2角色选择
        painter.setPen(Qt::white);
        painter.drawText(450, 100, "P2");
        
        // P2角色选择区域
        int startX2 = 400;
        int startY2 = 130;
        for (int i = 0; i < characterCount && i < 4; ++i)
        {
            int row = i / cols;
            int col = i % cols;
            int x = startX2 + col * spacing;
            int y = startY2 + row * 100;
            
            // 绘制选中框
            if (i == selectedCharacterIndexP2)
            {
                painter.setPen(QPen(Qt::red, 4));
                painter.setBrush(Qt::NoBrush);
                painter.drawRect(x - 4, y - 4, iconSize + 8, iconSize + 8);
            }
            
            // 绘制角色图片
            if (i < characterImages.size())
            {
                painter.drawImage(x, y, characterImages[i].scaled(iconSize, iconSize, Qt::KeepAspectRatio));
            }
        }
        
        // P3角色选择
        if (playerCount >= 3)
        {
            painter.setPen(Qt::white);
            painter.drawText(100, 300, "P3");
            
            // P3角色选择区域
            int startX3 = 50;
            int startY3 = 330;
            for (int i = 0; i < characterCount && i < 4; ++i)
            {
                int row = i / cols;
                int col = i % cols;
                int x = startX3 + col * spacing;
                int y = startY3 + row * 100;
                
                // 绘制选中框
                if (i == selectedCharacterIndexP3)
                {
                    painter.setPen(QPen(Qt::green, 4));
                    painter.setBrush(Qt::NoBrush);
                    painter.drawRect(x - 4, y - 4, iconSize + 8, iconSize + 8);
                }
                
                // 绘制角色图片
                if (i < characterImages.size())
                {
                    painter.drawImage(x, y, characterImages[i].scaled(iconSize, iconSize, Qt::KeepAspectRatio));
                }
            }
        }
        
        // P4角色选择
        if (playerCount >= 4)
        {
            painter.setPen(Qt::white);
            painter.drawText(450, 300, "P4");
            
            // P4角色选择区域
            int startX4 = 400;
            int startY4 = 330;
            for (int i = 0; i < characterCount && i < 4; ++i)
            {
                int row = i / cols;
                int col = i % cols;
                int x = startX4 + col * spacing;
                int y = startY4 + row * 100;
                
                // 绘制选中框
                if (i == selectedCharacterIndexP4)
                {
                    painter.setPen(QPen(Qt::yellow, 4));
                    painter.setBrush(Qt::NoBrush);
                    painter.drawRect(x - 4, y - 4, iconSize + 8, iconSize + 8);
                }
                
                // 绘制角色图片
                if (i < characterImages.size())
                {
                    painter.drawImage(x, y, characterImages[i].scaled(iconSize, iconSize, Qt::KeepAspectRatio));
                }
            }
        }
    }
    
    // 绘制底部装饰
    if (!groundTile.isNull()) {
        for (int x = 0; x < 800; x += 32) {
            painter.drawImage(x, 568, groundTile);
        }
    }
}

void CGame::drawGame(QPainter &painter)
{
    // 当玩家数量为1时，使用pic.png作为背景
    if (playerCount == 1)
    {
        if (!singlePlayerBackgroundImage.isNull())
        {
            painter.drawImage(0, 0, singlePlayerBackgroundImage.scaled(800, 600, Qt::KeepAspectRatioByExpanding));
        }
        else
        {
            painter.fillRect(0, 0, 800, 600, QColor(0, 255, 0)); // 纯绿色背景作为备用
        }
        
        // 移除金币绘制
        
        // 绘制经验球
        for (const auto &expBall : expBalls) {
            if (expBall.active) {
                painter.drawImage(expBall.pos, expBallImage);
            }
        }
        
        // 绘制敌人
        for (const auto &enemy : enemies) {
            if (enemy.active) {
                if (enemy.type == 99) { // BOSS
                    painter.drawImage(enemy.pos, bossImage.scaled(enemy.width, enemy.height));
                } else { // 普通敌人
                    painter.drawImage(enemy.pos, enemyImage);
                }
                
                // 绘制敌人生命值条
                int hpBarWidth = enemy.width * enemy.hp / enemy.maxHp;
                painter.setPen(Qt::red);
                painter.setBrush(Qt::red);
                painter.drawRect(enemy.pos.x(), enemy.pos.y() - 5, hpBarWidth, 3);
            }
        }
        
        // 绘制悬浮球
        for (const auto &orb : floatingOrbs) {
            if (orb.active) {
                painter.setPen(Qt::blue);
                painter.setBrush(Qt::blue);
                painter.drawEllipse(orb.pos.x(), orb.pos.y(), orb.width, orb.height);
            }
        }
        
        // 绘制道具
        for (const auto &powerUp : powerUps) {
            if (powerUp.active) {
                painter.drawImage(powerUp.pos, powerUpImage.scaled(32, 32));
            }
        }
        
        // 绘制光环效果
        for (const auto &aura : auras) {
            if (aura.active) {
                // 绘制蓝色光环
                painter.setPen(Qt::blue);
                painter.setBrush(QColor(0, 0, 255, 100)); // 半透明蓝色
                painter.drawEllipse(aura.pos.x() - aura.radius / 2, aura.pos.y() - aura.radius / 2, aura.radius, aura.radius);
            }
        }
    }
    else
    {
        // 绘制背景图片
        painter.drawImage(0, 0, backgroundImage.scaled(800, 600, Qt::KeepAspectRatioByExpanding));
        
        // 绘制云朵
        for (const auto &dec : decorations)
        {
            if (dec.type == 1) // 云
            {
                painter.drawImage(dec.pos, dec.image);
            }
        }
        
        // 绘制瓦片
        for (const auto &tile : tiles)
        {
            if (tile.pos.y() >= 500) // 地面
                painter.drawImage(tile.pos, groundTile);
            else if (tile.pos.x() == 0 || tile.pos.x() == 768) // 墙壁
                painter.drawImage(tile.pos, wallTile);
            else // 平台
                painter.drawImage(tile.pos, platformTile);
        }
        
        // 绘制装饰元素（在物品和敌人下面）
        for (const auto &dec : decorations)
        {
            if (dec.type != 1) // 除了云
            {
                painter.drawImage(dec.pos, dec.image);
            }
        }
        
        // 绘制物品
        for (const auto &item : items)
        {
            if (!item.collected)
            {
                painter.drawImage(item.pos, coinImage);
            }
        }
        
        // 多人模式下不绘制敌人
        // 敌人只在单人模式下出现
    }
    
    // 绘制玩家1 - 根据朝向翻转
    if (playerLives > 0) {
        QImage currentPlayerImage = playerImage;
        QImage flippedPlayer;
        if (facingRight) {
            flippedPlayer = currentPlayerImage;
        } else {
            flippedPlayer = currentPlayerImage.copy();
            // 水平翻转图片
            int width = flippedPlayer.width();
            int height = flippedPlayer.height();
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width / 2; ++x) {
                    QRgb leftPixel = flippedPlayer.pixel(x, y);
                    QRgb rightPixel = flippedPlayer.pixel(width - 1 - x, y);
                    flippedPlayer.setPixel(x, y, rightPixel);
                    flippedPlayer.setPixel(width - 1 - x, y, leftPixel);
                }
            }
        }
        painter.drawImage(static_cast<int>(playerPos.x()), static_cast<int>(playerPos.y()), flippedPlayer);
    }
    
    // 只在多人模式下绘制玩家2
    if (playerCount >= 2 && playerLivesP2 > 0)
    {
        // 绘制玩家2 - 根据朝向翻转
        QImage currentPlayerImageP2 = playerImageP2;
        QImage flippedPlayerP2;
        if (facingRightP2) {
            flippedPlayerP2 = currentPlayerImageP2;
        } else {
            flippedPlayerP2 = currentPlayerImageP2.copy();
            // 水平翻转图片
            int width = flippedPlayerP2.width();
            int height = flippedPlayerP2.height();
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width / 2; ++x) {
                    QRgb leftPixel = flippedPlayerP2.pixel(x, y);
                    QRgb rightPixel = flippedPlayerP2.pixel(width - 1 - x, y);
                    flippedPlayerP2.setPixel(x, y, rightPixel);
                    flippedPlayerP2.setPixel(width - 1 - x, y, leftPixel);
                }
            }
        }
        painter.drawImage(static_cast<int>(playerPosP2.x()), static_cast<int>(playerPosP2.y()), flippedPlayerP2);
    }
    
    // 绘制玩家3 - 根据朝向翻转
    if (playerCount >= 3 && playerLivesP3 > 0)
    {
        QImage currentPlayerImageP3 = playerImageP3;
        QImage flippedPlayerP3;
        if (facingRightP3) {
            flippedPlayerP3 = currentPlayerImageP3;
        } else {
            flippedPlayerP3 = currentPlayerImageP3.copy();
            // 水平翻转图片
            int width = flippedPlayerP3.width();
            int height = flippedPlayerP3.height();
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width / 2; ++x) {
                    QRgb leftPixel = flippedPlayerP3.pixel(x, y);
                    QRgb rightPixel = flippedPlayerP3.pixel(width - 1 - x, y);
                    flippedPlayerP3.setPixel(x, y, rightPixel);
                    flippedPlayerP3.setPixel(width - 1 - x, y, leftPixel);
                }
            }
        }
        painter.drawImage(static_cast<int>(playerPosP3.x()), static_cast<int>(playerPosP3.y()), flippedPlayerP3);
    }
    
    // 绘制玩家4 - 根据朝向翻转
    if (playerCount >= 4 && playerLivesP4 > 0)
    {
        QImage currentPlayerImageP4 = playerImageP4;
        QImage flippedPlayerP4;
        if (facingRightP4) {
            flippedPlayerP4 = currentPlayerImageP4;
        } else {
            flippedPlayerP4 = currentPlayerImageP4.copy();
            // 水平翻转图片
            int width = flippedPlayerP4.width();
            int height = flippedPlayerP4.height();
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width / 2; ++x) {
                    QRgb leftPixel = flippedPlayerP4.pixel(x, y);
                    QRgb rightPixel = flippedPlayerP4.pixel(width - 1 - x, y);
                    flippedPlayerP4.setPixel(x, y, rightPixel);
                    flippedPlayerP4.setPixel(width - 1 - x, y, leftPixel);
                }
            }
        }
        painter.drawImage(static_cast<int>(playerPosP4.x()), static_cast<int>(playerPosP4.y()), flippedPlayerP4);
    }
    
    // 绘制子弹
    for (const auto &bullet : bullets)
    {
        if (bullet.active)
        {
            // 检查是否是导弹（通过伤害值判断，导弹伤害为skillDamage[SKILL_MISSILE_RAIN]）
            if (bullet.damage == skillDamage[SKILL_MISSILE_RAIN]) {
                // 绘制导弹
                painter.drawImage(static_cast<int>(bullet.pos.x()), static_cast<int>(bullet.pos.y()), missileImage.scaled(16, 16));
            } else {
                // 绘制普通子弹
                painter.drawImage(static_cast<int>(bullet.pos.x()), static_cast<int>(bullet.pos.y()), bulletImage.scaled(8, 8));
            }
        }
    }
    
    // 绘制等级和经验
    if (playerCount == 1)
    {
        painter.setPen(Qt::black);
        painter.setFont(QFont("微软雅黑", 18, QFont::Medium));
        painter.drawText(10, 30, QString("等级: %1").arg(playerLevel));
        painter.drawText(10, 60, QString("经验: %1/%2").arg(exp).arg(expMax));
        painter.drawText(10, 90, QString("击杀: %1").arg(enemyDeathCnt));
        
        // 绘制生命值条
        int hpBarWidth = 200 * playerLives / playerMaxLives;
        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
        painter.drawRect(10, 140, hpBarWidth, 20);
        painter.setPen(Qt::black);
        painter.drawText(10, 155, QString("生命值: %1/%2").arg(playerLives).arg(playerMaxLives));
        
        // 绘制技能冷却时间
        painter.setPen(Qt::black);
        painter.setFont(QFont("微软雅黑", 14, QFont::Medium));
        painter.drawText(10, 185, "技能: F");
        
        // 绘制当前技能名称
        QString skillName;
        switch (selectedSkill) {
        case SKILL_MISSILE_RAIN:
            skillName = "导弹雨";
            break;
        case SKILL_ENERGY_SHIELD:
            skillName = "能量护盾";
            break;
        case SKILL_LIGHTNING_CHAIN:
            skillName = "闪电链";
            break;
        }
        painter.drawText(100, 185, QString("当前技能: %1").arg(skillName));
        
        // 技能冷却条
        int cooldownBarWidth = 200 * skillCooldown[selectedSkill] / skillCooldownMax[selectedSkill];
        if (skillCooldown[selectedSkill] > 0) {
            painter.setPen(Qt::gray);
            painter.setBrush(Qt::gray);
            painter.drawRect(10, 195, 200, 10);
            painter.setPen(Qt::blue);
            painter.setBrush(Qt::blue);
            painter.drawRect(10, 195, 200 - cooldownBarWidth, 10);
        } else {
            painter.setPen(Qt::green);
            painter.setBrush(Qt::green);
            painter.drawRect(10, 195, 200, 10);
        }
        
        // 显示冷却时间（秒）
        int cooldownSeconds = skillCooldown[selectedSkill] / 60;
        painter.setPen(Qt::black);
        painter.drawText(220, 203, QString("%1s").arg(cooldownSeconds));
        

    }
    else
    {
        // 绘制生命值
        painter.setPen(Qt::black);
        painter.setFont(QFont("微软雅黑", 18, QFont::Medium));
        painter.drawText(10, 55, "P1 生命值:");
    }
    
    // 多人模式下的生命值显示
    if (playerCount > 1) {
        for (int i = 0; i < 3; ++i)
        {
            if (i < playerLives)
                painter.drawImage(120 + i * 20, 50, lifeImage.scaled(20, 20));
            else
                painter.drawImage(120 + i * 20, 50, lifeEmptyImage.scaled(20, 20));
        }
        
        // 只在多人模式下显示P2的生命值
        if (playerCount >= 2)
        {
            painter.drawText(600, 55, "P2 生命值:");
            for (int i = 0; i < 3; ++i)
            {
                if (i < playerLivesP2)
                    painter.drawImage(710 + i * 20, 50, lifeImage.scaled(20, 20));
                else
                    painter.drawImage(710 + i * 20, 50, lifeEmptyImage.scaled(20, 20));
            }
        }
        
        // P3生命值
        if (playerCount >= 3)
        {
            painter.drawText(10, 85, "P3 生命值:");
            for (int i = 0; i < 3; ++i)
            {
                if (i < playerLivesP3)
                    painter.drawImage(120 + i * 20, 80, lifeImage.scaled(20, 20));
                else
                    painter.drawImage(120 + i * 20, 80, lifeEmptyImage.scaled(20, 20));
            }
        }
        
        // P4生命值
        if (playerCount >= 4)
        {
            painter.drawText(600, 85, "P4 生命值:");
            for (int i = 0; i < 3; ++i)
            {
                if (i < playerLivesP4)
                    painter.drawImage(710 + i * 20, 80, lifeImage.scaled(20, 20));
                else
                    painter.drawImage(710 + i * 20, 80, lifeEmptyImage.scaled(20, 20));
            }
        }
    }
    
    // 绘制升级面板
    if (gameState == STATE_UPGRADE) {
        drawUpgradePanel(painter);
    }
}

void CGame::drawGameOver(QPainter &painter)
{
    // 先绘制游戏画面
    drawGame(painter);
    
    // 绘制半透明遮罩
    painter.fillRect(0, 0, 800, 600, QColor(0, 0, 0, 150));
    
    // 绘制游戏结束文字
    painter.setPen(Qt::red);
    painter.setFont(QFont("微软雅黑", 56, QFont::Bold));
    painter.drawText(260, 280, "游戏结束!");
    
    // 绘制获胜者
    painter.setPen(Qt::white);
    painter.setFont(QFont("微软雅黑", 36, QFont::Bold));
    if (winner == 1)
        painter.drawText(280, 340, "P1 获胜!");
    else if (winner == 2)
        painter.drawText(280, 340, "P2 获胜!");
    else if (winner == 3)
        painter.drawText(280, 340, "P3 获胜!");
    else if (winner == 4)
        painter.drawText(280, 340, "P4 获胜!");
    
    // 绘制重新开始提示
    painter.setFont(QFont("微软雅黑", 24));
    painter.drawText(260, 400, "按R键重新开始");
    painter.drawText(260, 430, "按ESC返回主菜单");
}

void CGame::keyPressEvent(QKeyEvent *event)
{
    // 根据游戏状态处理按键
    switch (gameState)
    {
    case STATE_START_SCREEN:
        if (event->key() == Qt::Key_Space)
        {
            gameState = STATE_PLAYER_SELECT;
        }
        break;
        
    case STATE_PLAYER_SELECT:
        if (event->key() == Qt::Key_Up)
        {
            if (playerCount == 0) // 帮助选项
                playerCount = 4;
            else if (playerCount == 1)
                playerCount = 0; // 切换到帮助
            else
                playerCount--;
        }
        else if (event->key() == Qt::Key_Down)
        {
            if (playerCount == 0) // 帮助选项
                playerCount = 1;
            else if (playerCount == 4)
                playerCount = 0; // 切换到帮助
            else
                playerCount++;
        }
        else if (event->key() == Qt::Key_Space)
        {
            if (playerCount == 0)
                gameState = STATE_HELP;
            else
                gameState = STATE_CHARACTER_SELECT;
        }
        break;
        
    case STATE_CHARACTER_SELECT:
        // 单人模式下的技能选择
        if (playerCount == 1) {
            if (event->key() == Qt::Key_1) {
                selectedSkill = SKILL_MISSILE_RAIN;
            } else if (event->key() == Qt::Key_2) {
                selectedSkill = SKILL_ENERGY_SHIELD;
            } else if (event->key() == Qt::Key_3) {
                selectedSkill = SKILL_LIGHTNING_CHAIN;
            }
        }
        
        // P1控制 (WASD)
        if (event->key() == Qt::Key_A)
        {
            selectedCharacterIndex = (selectedCharacterIndex - 1 + characterCount) % characterCount;
        }
        else if (event->key() == Qt::Key_D)
        {
            selectedCharacterIndex = (selectedCharacterIndex + 1) % characterCount;
        }
        else if (event->key() == Qt::Key_W) // P1上键
        {
            selectedCharacterIndex = (selectedCharacterIndex - 1 + characterCount) % characterCount;
        }
        else if (event->key() == Qt::Key_S) // P1下键
        {
            selectedCharacterIndex = (selectedCharacterIndex + 1) % characterCount;
        }
        // P2控制 (方向键)
        else if (event->key() == Qt::Key_Left)
        {
            selectedCharacterIndexP2 = (selectedCharacterIndexP2 - 1 + characterCount) % characterCount;
        }
        else if (event->key() == Qt::Key_Right)
        {
            selectedCharacterIndexP2 = (selectedCharacterIndexP2 + 1) % characterCount;
        }
        else if (event->key() == Qt::Key_Up) // P2上键
        {
            selectedCharacterIndexP2 = (selectedCharacterIndexP2 - 1 + characterCount) % characterCount;
        }
        else if (event->key() == Qt::Key_Down) // P2下键
        {
            selectedCharacterIndexP2 = (selectedCharacterIndexP2 + 1) % characterCount;
        }
        // P3控制 (JKLI)
        if (playerCount >= 3)
        {
            if (event->key() == Qt::Key_J)
            {
                selectedCharacterIndexP3 = (selectedCharacterIndexP3 - 1 + characterCount) % characterCount;
            }
            else if (event->key() == Qt::Key_L)
            {
                selectedCharacterIndexP3 = (selectedCharacterIndexP3 + 1) % characterCount;
            }
            else if (event->key() == Qt::Key_I) // P3上键
            {
                selectedCharacterIndexP3 = (selectedCharacterIndexP3 - 1 + characterCount) % characterCount;
            }
            else if (event->key() == Qt::Key_K) // P3下键
            {
                selectedCharacterIndexP3 = (selectedCharacterIndexP3 + 1) % characterCount;
            }
        }
        // P4控制 (1352)
        if (playerCount >= 4)
        {
            if (event->key() == Qt::Key_1)
            {
                selectedCharacterIndexP4 = (selectedCharacterIndexP4 - 1 + characterCount) % characterCount;
            }
            else if (event->key() == Qt::Key_3)
            {
                selectedCharacterIndexP4 = (selectedCharacterIndexP4 + 1) % characterCount;
            }
            else if (event->key() == Qt::Key_5) // P4上键
            {
                selectedCharacterIndexP4 = (selectedCharacterIndexP4 - 1 + characterCount) % characterCount;
            }
            else if (event->key() == Qt::Key_2) // P4下键
            {
                selectedCharacterIndexP4 = (selectedCharacterIndexP4 + 1) % characterCount;
            }
        }
        // 确认选择 - 只使用空格键
        if (event->key() == Qt::Key_Space)
        {
            // 确认选择，开始游戏
            if (selectedCharacterIndex < characterImages.size())
            {
                playerImage = characterImages[selectedCharacterIndex];
            }
            if (selectedCharacterIndexP2 < characterImages.size())
            {
                playerImageP2 = characterImages[selectedCharacterIndexP2];
            }
            if (playerCount >= 3 && selectedCharacterIndexP3 < characterImages.size())
            {
                playerImageP3 = characterImages[selectedCharacterIndexP3];
            }
            if (playerCount >= 4 && selectedCharacterIndexP4 < characterImages.size())
            {
                playerImageP4 = characterImages[selectedCharacterIndexP4];
            }
            gameState = STATE_PLAYING;
            gameOver = false;
            playerPos = QPointF(200, 400);
            playerVelocityX = 0;
            playerVelocityY = 0;
            isJumping = false;
            jumpCount = 0; // 重置二段跳计数器
            facingRight = true;
            playerPosP2 = QPointF(600, 400);
            playerVelocityXP2 = 0;
            playerVelocityYP2 = 0;
            isJumpingP2 = false;
            jumpCountP2 = 0;
            facingRightP2 = false;
            
            // 重新加载物品，确保一人模式下生成金币
            loadItems();
            if (playerCount >= 3)
            {
                playerPosP3 = QPointF(300, 400);
                playerVelocityXP3 = 0;
                playerVelocityYP3 = 0;
                isJumpingP3 = false;
                jumpCountP3 = 0;
                facingRightP3 = true;
            }
            if (playerCount >= 4)
            {
                playerPosP4 = QPointF(500, 400);
                playerVelocityXP4 = 0;
                playerVelocityYP4 = 0;
                isJumpingP4 = false;
                jumpCountP4 = 0;
                facingRightP4 = false;
            }
        }
        break;
        
    case STATE_PLAYING:
        if (gameOver)
        {
            if (event->key() == Qt::Key_R)
            {
                restartGame();
            }
            else if (event->key() == Qt::Key_Escape)
            {
                gameState = STATE_START_SCREEN;
                initGame();
            }
            return;
        }
        
        // 单人模式独立按键处理
        if (playerCount == 1)
        {
            switch (event->key())
            {
            // 单人模式下的ASDW控制
            case Qt::Key_A:
                isLeftPressed = true; // A键向左移动
                break;
            case Qt::Key_D:
                isRightPressed = true; // D键向右移动
                break;
            case Qt::Key_W:
                isUpPressed = true; // W键向上移动
                break;
            case Qt::Key_S:
                isDownPressed = true; // S键向下移动
                break;
            case Qt::Key_Space:
                isAttackPressed = true; // 单人模式下空格键攻击
                break;
            case Qt::Key_F:
                isSkillPressed = true; // F键释放技能
                break;
            case Qt::Key_P:
                // 切换到数据面板
                gameState = STATE_DATA_PANEL;
                break;
            default:
                break;
            }
        }
        else
        {
            // 多人模式按键处理
            switch (event->key())
            {
            // P1控制 (WASD)
            case Qt::Key_A:
                isLeftPressed = true; // A键向左移动
                break;
            case Qt::Key_D:
                isRightPressed = true; // D键向右移动
                break;
            case Qt::Key_W:
                // 多玩家模式下W键跳跃
                if (!isJumping) // 地面跳跃
                {
                    playerVelocityY = JUMP_FORCE;
                    isJumping = true;
                    jumpCount = 1; // 第一次跳跃
                }
                else if (jumpCount < 2) // 二段跳
                {
                    playerVelocityY = JUMP_FORCE * 0.8f; // 二段跳力度稍小
                    jumpCount = 2; // 第二次跳跃
                }
                break;
            case Qt::Key_Space:
                // 空格键始终用于跳跃
                if (!isJumping) // 地面跳跃
                {
                    playerVelocityY = JUMP_FORCE;
                    isJumping = true;
                    jumpCount = 1; // 第一次跳跃
                }
                else if (jumpCount < 2) // 二段跳
                {
                    playerVelocityY = JUMP_FORCE * 0.8f; // 二段跳力度稍小
                    jumpCount = 2; // 第二次跳跃
                }
                break;
            case Qt::Key_S:
                isAttackPressed = true; // 多玩家模式下S键攻击
                break;
            
            // P2控制 (方向键)
            case Qt::Key_Left:
                isLeftPressedP2 = true; // 方向键Left向左移动
                break;
            case Qt::Key_Right:
                isRightPressedP2 = true; // 方向键Right向右移动
                break;
            case Qt::Key_Up:
                if (!isJumpingP2) // 地面跳跃
                {
                    playerVelocityYP2 = JUMP_FORCE;
                    isJumpingP2 = true;
                    jumpCountP2 = 1; // 第一次跳跃
                }
                else if (jumpCountP2 < 2) // 二段跳
                {
                    playerVelocityYP2 = JUMP_FORCE * 0.8f; // 二段跳力度稍小
                    jumpCountP2 = 2; // 第二次跳跃
                }
                break;
            case Qt::Key_Down:
                isAttackPressedP2 = true; // 方向键Down攻击
                break;
            
            // P3控制 (JKLI) - 与P1的ASDW对应
            case Qt::Key_J:
                if (playerCount >= 3)
                    isLeftPressedP3 = true; // J键向左移动 (对应P1的A)
                break;
            case Qt::Key_L:
                if (playerCount >= 3)
                    isRightPressedP3 = true; // L键向右移动 (对应P1的D)
                break;
            case Qt::Key_I:
                if (playerCount >= 3 && !isJumpingP3) // 地面跳跃 (对应P1的W)
                {
                    playerVelocityYP3 = JUMP_FORCE;
                    isJumpingP3 = true;
                    jumpCountP3 = 1; // 第一次跳跃
            }
            else if (playerCount >= 3 && jumpCountP3 < 2) // 二段跳
            {
                    playerVelocityYP3 = JUMP_FORCE * 0.8f; // 二段跳力度稍小
                    jumpCountP3 = 2; // 第二次跳跃
                }
                break;
            case Qt::Key_K:
                if (playerCount >= 3)
                    isAttackPressedP3 = true; // K键攻击 (对应P1的S)
                break;
            
            // P4控制 (1352)
            case Qt::Key_1:
                if (playerCount >= 4)
                    isLeftPressedP4 = true; // 1键向左移动
                break;
            case Qt::Key_3:
                if (playerCount >= 4)
                    isRightPressedP4 = true; // 3键向右移动
                break;
            case Qt::Key_5:
                if (playerCount >= 4 && !isJumpingP4) // 地面跳跃
                {
                    playerVelocityYP4 = JUMP_FORCE;
                    isJumpingP4 = true;
                    jumpCountP4 = 1; // 第一次跳跃
                }
                else if (playerCount >= 4 && jumpCountP4 < 2) // 二段跳
                {
                    playerVelocityYP4 = JUMP_FORCE * 0.8f; // 二段跳力度稍小
                    jumpCountP4 = 2; // 第二次跳跃
                }
                break;
            case Qt::Key_2:
                if (playerCount >= 4)
                    isAttackPressedP4 = true; // 2键攻击
                break;
            default:
                break;
            }
            break;
        }
    case STATE_GAME_OVER:
        if (event->key() == Qt::Key_R)
        {
            restartGame();
        }
        else if (event->key() == Qt::Key_Escape)
        {
            gameState = STATE_START_SCREEN;
            initGame();
        }
        break;
        
    case STATE_HELP:
        if (event->key() == Qt::Key_Space)
        {
            gameState = STATE_PLAYER_SELECT;
        }
        break;
        
    case STATE_DATA_PANEL:
        if (event->key() == Qt::Key_P)
        {
            gameState = STATE_PLAYING;
        }
        break;
        
    case STATE_UPGRADE:
        // 升级面板按键处理
        if (event->key() == Qt::Key_1) {
            handleUpgradeChoice(0);
        } else if (event->key() == Qt::Key_2) {
            handleUpgradeChoice(1);
        } else if (event->key() == Qt::Key_3) {
            handleUpgradeChoice(2);
        }
        break;
    }
}

void CGame::mouseMoveEvent(QMouseEvent *event)
{
    // 鼠标移动事件处理
}

void CGame::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    // P1控制 (WASD)
    case Qt::Key_A:
        isLeftPressed = false; // A键释放
        break;
    case Qt::Key_D:
        isRightPressed = false; // D键释放
        break;
    case Qt::Key_W:
        isUpPressed = false; // W键释放，无论是否是单人模式
        break;
    case Qt::Key_S:
        if (playerCount == 1)
        {
            isDownPressed = false; // 1玩家模式下S键释放
        }
        else
        {
            isAttackPressed = false; // 多玩家模式下S键释放
        }
        break;
    case Qt::Key_Space:
        if (playerCount == 1)
        {
            isAttackPressed = false; // 单人模式下空格键释放
        }
        break;
        
    // P2控制 (方向键)
    case Qt::Key_Left:
        isLeftPressedP2 = false;
        break;
    case Qt::Key_Right:
        isRightPressedP2 = false;
        break;
    case Qt::Key_Down:
        isAttackPressedP2 = false; // 方向键Down释放
        break;
        
    // P3控制 (JKLI) - 与P1的ASDW对应
    case Qt::Key_J:
        isLeftPressedP3 = false; // J键释放 (对应P1的A)
        break;
    case Qt::Key_L:
        isRightPressedP3 = false; // L键释放 (对应P1的D)
        break;
    case Qt::Key_K:
        isAttackPressedP3 = false; // K键释放 (对应P1的S)
        break;
    case Qt::Key_I:
        // I键是跳跃，不需要释放处理 (对应P1的W)
        break;
        
    // P4控制 (1352)
    case Qt::Key_1:
        isLeftPressedP4 = false; // 1键释放
        break;
    case Qt::Key_3:
        isRightPressedP4 = false; // 3键释放
        break;
    case Qt::Key_2:
        isAttackPressedP4 = false; // 2键释放
        break;
    case Qt::Key_F:
        isSkillPressed = false; // F键释放
        break;
    default:
        break;
    }
}

