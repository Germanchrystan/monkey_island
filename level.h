#pragma once

#include <stdbool.h>
#include "grid.h"
#include "event_registry.h"

typedef struct Game Game;

typedef enum 
{
  FACING_RIGHT,
  FACING_LEFT,
  FACING_UP,
  FACING_DOWN,
} FacingDirection;

typedef struct {
  int spawnIndex;
  GridPosition position;
  FacingDirection facingDirection;
} SpawnPoint;

typedef struct 
{
  EventType type;
  EventPayloadType payloadType;
  EventPayload payload;
} EventData;

typedef struct
{
  int eventDataQ;
  EventData *eventData;
} EventDataList;

typedef struct 
{
  bool active;
  bool oneShot;
  EventDataList eventDataList;
  GridPosition position;
  GridPosition targetPosition;
} EventObject;

typedef struct 
{
  Vector2 backgroundSize;
  Vector2 foregroundSize;
  Texture2D *background;
  Texture2D *foreground;
} ScreenTextures;

typedef struct 
{
  int spawnPointQ;
  int eventObjQ;
  Grid grid;
  SpawnPoint *spawnPoints; // TODO: SpawnPoint should be an struct with GridPosition and facing direction
  EventObject *eventObjects;
  ScreenTextures screenTextures;
} Screen;

typedef struct Level
{
  int screensQ;
  int currentScreen;
  Screen *screens;
  Player player;
} Level;

typedef void (*LevelEvent)(Level *level);

void saveLevel(Level *level, const char *filename);
void loadLevel(Level *level, const char *filename);
void updateLevel(Game *game, Level *level, float dt);
void transitionScreen(Level *level, int screenIndex, int spawnIndex);
void drawLevel(Level *level, int drawMode);
void drawDoors(Screen *screen);
void drawEventObjects(Screen *screen);
void freeLevel(Level *level);
void checkForPendingEventObjectTrigger(Game *game, Level *level);