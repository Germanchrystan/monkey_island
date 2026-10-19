#pragma once

#include "level.h"
#include "event_queue.h"
#include "player.h"
#include "inventory.h"
#include "ui.h"
#include "game_context.h"

#define GAME_STATUS_PLAYING 0
#define GAME_STATUS_PAUSED 1
#define GAME_STATUS_UI_ACTIVE 2
#define GAME_STATUS_FADE_OUT 3
#define GAME_STATUS_TRANSITION 4
#define GAME_STATUS_TALK_TREE 5

typedef struct TalkTree TalkTree;

typedef enum
{
  PLAYING = GAME_STATUS_PLAYING,
  PAUSE = GAME_STATUS_PAUSED,
  UI_INTERRUPTION = GAME_STATUS_UI_ACTIVE,
  FADE_OUT = GAME_STATUS_FADE_OUT,
  TRANSITION = GAME_STATUS_TRANSITION,
  TALK_TREE = GAME_STATUS_TALK_TREE,
  STATE_COUNT
} GameState;

typedef struct PendingEvent
{
  bool hasPendingEventObject;
  int pendingEventScreenIndex;
  int pendingEventObjectIndex;
  GridPosition pendingEventTargetPosition;
} PendingEvent;

typedef struct PendingTransition
{
  bool hasPendingTransition;
  int targetScreenIndex;
  int targetSpawnIndex;
  int targetLevelIndex;
} PendingTransition;

typedef struct Game 
{
  GameState state;
  int levelIndex; // Expand on this when we have multiple levels
  Level level;
  Inventory inventory;
  EventQueue eventQueue;
  UI ui;
  PendingEvent pendingEvent;
  PendingTransition pendingTransition;
  Camera2D camera;
  GameContext context;
  TalkTree *currentTalkTree;
} Game;

void initGame(Game *game, const char *levelFilename);
void updateGame(Game *game, float dt);
void loadGameLevel(Game *game, const char *filename);
void drawGame(Game *game, int drawMode);
void freeGame(Game *game);
void loadEventQueue(Game *game, EventDataList *eventDataList);
void notifyItemPickup(Game *game, const char *itemName);
void notifyDoorUnlocked(Game *game, const char *itemName);
void notifyDoorLocked(Game *game);
void changeGameState(Game *game, GameState newStatus);
void setPendingTransition(Game *game, int targetScreenIndex, int targetSpawnIndex, int targetLevelIndex);
void updatePendingTransition(Game *game);