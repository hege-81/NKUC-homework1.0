#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <QPoint>
#include <QRect>
#include <QImage>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QElapsedTimer>
#include <QMouseEvent>

#include <vector>
#include "ui_CGame.h"

class CGame : public QMainWindow
{
    Q_OBJECT

public:
    CGame(QWidget *parent = nullptr);
    ~CGame();

private slots:
    void updateGame();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::CGameClass ui;
    QTimer *gameTimer;
    QElapsedTimer elapsedTimer;
    float accumulator;
    QWidget *gameWidget;

    // 游戏状态
    enum GameState {
        STATE_START_SCREEN,
        STATE_PLAYER_SELECT,
        STATE_CHARACTER_SELECT,
        STATE_PLAYING,
        STATE_GAME_OVER,
        STATE_HELP,
        STATE_UPGRADE,
        STATE_DATA_PANEL
    };
    GameState gameState;

    bool gameOver;
    int score;
    int playerLevel;
    int exp;
    int expMax;

    // 游戏模式
    int playerCount;
    int winner;

    // 角色选择
    std::vector<QImage> characterImages;
    int selectedCharacterIndex;
    int selectedCharacterIndexP2;
    int selectedCharacterIndexP3;
    int selectedCharacterIndexP4;
    int characterCount;

    // 玩家1属性
    QPointF playerPos;
    int playerWidth;
    int playerHeight;
    float playerVelocityX;
    float playerVelocityY;
    bool isLeftPressed;
    bool isRightPressed;
    bool isUpPressed;
    bool isDownPressed;
    bool isAttackPressed;
    int playerFrame;
    bool facingRight;
    int playerLives;
    int playerMaxLives;
    int attackCooldown;
    int damage;
    float speed;
    float reduce;
    bool isJumping;
    int jumpCount;

    // 道具效果
    int powerUpDamage;
    float powerUpSpeed;
    int powerUpDuration;
    bool hasPowerUp;

    // 技能系统
    QImage missileImage;
    // 技能系统
    enum SkillType {
        SKILL_MISSILE_RAIN,
        SKILL_ENERGY_SHIELD,
        SKILL_LIGHTNING_CHAIN
    };
    
    int selectedSkill;
    int skillCooldown[3];
    int skillCooldownMax[3];
    bool isSkillAvailable[3];
    bool isSkillPressed;
    int skillDamage[3];
    float attackSpeed;

    // 升级系统
    std::vector<int> selectedUpgradeChoices;

    // 玩家2属性
    QPointF playerPosP2;
    float playerVelocityXP2;
    float playerVelocityYP2;
    bool isLeftPressedP2;
    bool isRightPressedP2;
    bool isAttackPressedP2;
    int playerFrameP2;
    bool facingRightP2;
    int playerLivesP2;
    int attackCooldownP2;
    bool isJumpingP2;
    int jumpCountP2;

    // 玩家3属性
    QPointF playerPosP3;
    float playerVelocityXP3;
    float playerVelocityYP3;
    bool isLeftPressedP3;
    bool isRightPressedP3;
    bool isAttackPressedP3;
    int playerFrameP3;
    bool facingRightP3;
    int playerLivesP3;
    int attackCooldownP3;
    bool isJumpingP3;
    int jumpCountP3;

    // 玩家4属性
    QPointF playerPosP4;
    float playerVelocityXP4;
    float playerVelocityYP4;
    bool isLeftPressedP4;
    bool isRightPressedP4;
    bool isAttackPressedP4;
    int playerFrameP4;
    bool facingRightP4;
    int playerLivesP4;
    int attackCooldownP4;
    bool isJumpingP4;
    int jumpCountP4;

    // 跳跃动画
    std::vector<QImage> characterJumpImages;
    bool isShowingJumpAnimation;

    // 物理常量
    const float GRAVITY = 0.9f;
    const float JUMP_FORCE = -18.0f;
    const float MOVE_ACCELERATION = 1.2f;
    const float MAX_SPEED = 9.0f;
    const float FRICTION = 0.85f;
    const float AIR_RESISTANCE = 0.95f;

    // 图片资源
    QImage groundTile;
    QImage platformTile;
    QImage wallTile;
    QImage coinImage;
    QImage playerImage;
    QImage playerImageP2;
    QImage playerImageP3;
    QImage playerImageP4;
    QImage enemyImage;
    QImage backgroundImage;
    QImage singlePlayerBackgroundImage;
    QImage bulletImage;
    QImage lifeImage;
    QImage lifeEmptyImage;
    QImage expBallImage;
    QImage powerUpImage;
    QImage bossImage;

    // 敌人类
    struct Enemy {
        QPointF pos;
        int width;
        int height;
        float velocityX;
        float velocityY;
        int hp;
        int maxHp;
        int damage;
        int expValue;
        bool active;
        int type;
        bool isStunned;
        int stunTimer;
    };

    std::vector<Enemy> enemies;
    int enemyDeathCnt;

    // 经验球结构
    struct ExpBall {
        QPointF pos;
        int value;
        bool active;
        int timer;
    };
    std::vector<ExpBall> expBalls;
    const int EBALL_MAX = 50;
    int expBallCnt;

    // 悬浮球结构
    struct FloatingOrb {
        QPointF pos;
        int width;
        int height;
        float angle;
        float radius;
        int damage;
        bool active;
    };
    std::vector<FloatingOrb> floatingOrbs;
    
    // 光环效果结构
    struct Aura {
        QPointF pos;
        int radius;
        int timer;
        bool active;
    };
    std::vector<Aura> auras;

    // 道具结构
    struct PowerUp {
        QPointF pos;
        int type;
        int duration;
        bool active;
    };
    std::vector<PowerUp> powerUps;

    // 装饰元素
    QImage treeImage;
    QImage cloudImage;
    QImage ladderImage;
    QImage flagImage;
    QImage bushImage;
    QImage flowerImage;
    QImage mushroomImage;
    QImage crateImage;
    QImage gemImage;
    QImage spikeImage;

    // 游戏场景
    struct Tile {
        QPoint pos;
        int width;
        int height;
        bool isSolid;
    };

    struct Item {
        QPoint pos;
        int width;
        int height;
        bool collected;
    };

    struct Decoration {
        QPoint pos;
        QImage image;
        int type;
    };

    // 子弹结构
    struct Bullet {
        QPointF pos;
        float velocityX;
        float velocityY;
        int owner;
        int damage;
        bool active;
        bool isTracking;
        Enemy* target;
    };

    std::vector<Tile> tiles;
    std::vector<Item> items;
    std::vector<Decoration> decorations;
    std::vector<Bullet> bullets;

    // 游戏方法
    void initGame();
    void loadResources();
    void loadCharacters();
    void loadTiles();
    void loadItems();
    void loadEnemies();
    void loadDecorations();
    void updatePlayer();
    void updatePlayerP2();
    void updatePlayerP3();
    void updatePlayerP4();
    void updateBullets();
    void updateEnemies();
    void updateExpBalls();
    void updateFloatingOrbs();
    void updatePowerUps();
    void updateAuras();
    void checkCollisions();
    void checkBulletCollisions();
    void checkOrbEnemyCollisions();
    void checkPowerUpCollisions();
    void collectItems();
    void collectExpBalls();
    void checkGameOver();
    void restartGame();
    void drawStartScreen(QPainter &painter);
    void drawCharacterSelect(QPainter &painter);
    void drawPlayerSelect(QPainter &painter);
    void drawGame(QPainter &painter);
    void drawGameOver(QPainter &painter);
    void drawHelp(QPainter &painter);
    void drawDataPanel(QPainter &painter);
    void drawUpgradePanel(QPainter &painter);
    void initUpgradePanel();
    void showUpgradePanel();
    void hideUpgradePanel();
    void handleUpgradeChoice(int choice);
    void addExpBall(double x, double y, int value);
    void spawnEnemy();
    void levelUp();
};