#pragma once

#include "common.h"
#include "states/state.h"

#include "game/ship.h"
#include "game/soundEffect.h"

typedef struct {
    bool sonar;
    int x;
    int y;
} PositionPacket;

typedef struct {
    SonarType result[3][3];
} SonarResultPacket;

typedef struct {
    bool miss;
    bool sunk;

    ShipTypes type;
    PointingNose pointingTo;
    int x;
    int y;
} ShipSunkPacket;

#define DECOYS_AMOUNT 5
#define SONARS_AMOUNT 5

typedef enum {
    PLAYING_PLACING_SHIPS,
    PLAYING_WAITING_FOR_ENEMY_PLACING,

    PLAYING_YOUR_TURN,
    PLAYING_WAITING_FOR_ENEMY_MOVE,

    PLAYING_YOU_WON,
    PLAYING_ENEMY_WON,

    PLAYING_ENEMY_LEFT,
    PLAYING_DONE,
} PlayingStateInternalState;

class PlayingState : public State
{
    public:
        PlayingState(bool firstPlayer);
        ~PlayingState();

        void update();
        void drawTop();
        void drawBottom();

    private:
        void drawButtons();
        void drawGrid(gfxScreen_t screen);
        void drawEnemyGrid(gfxScreen_t screen);

        bool allShipsReady();
        void startYourTurn();

        bool canDecoy();
        bool canSonar();
        bool canShoot();

        bool won();
        bool lost();
        bool checkEnemyLeft();

        void startNewSoundEffect(SoundEffectID id, u64 delay);

        std::array<FriendlyShip, SHIPS_AMOUNT> ships;
        std::array<EnemyShip, SHIPS_AMOUNT> enemyShips;

        PlayingStateInternalState state;
        bool firstPlayer;
        bool enemyReady;
        bool enemyJoined;

        ShipTypes toHover;
        ShipTypes hoveringShip;
        PointingNose hoverRotation;
        int squareX, squareY;
        int lastHitX, lastHitY;
        int remainingDecoys, remainingSonars;
        bool usedSpecial;
        bool swapGrids;

        std::array<std::unique_ptr<SoundEffect>, SFX_AMOUNT> soundEffects;
};
