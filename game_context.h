#pragma once

typedef struct DoorRuntimeChangeState
{
  int levelIndex;
  int screenIndex;
  int doorId;
  bool unlocked;
} DoorRuntimeChangeState;

typedef struct DialogRuntimeChangeState
{
  int dialogId;
  const char *nodeId;
} DialogRuntimeChangeState;

typedef struct GameContext
{
  DoorRuntimeChangeState *doors;
  int doorCount;
  DialogRuntimeChangeState *dialogs;
  int dialogCount;
} GameContext;

void initGameContext(GameContext *context);
void setDoorRuntimeChangeState(GameContext *context, int levelIndex, int screenIndex, int doorId, bool unlocked);
void getDoorRuntimeChangeState(GameContext *context, int levelIndex, int screenIndex, int doorId, bool *unlocked);
void setDialogRuntimeChangeState(GameContext *context, int dialogId, const char *nodeId);
void getDialogRuntimeChangeState(GameContext *context, int dialogId, const char **nodeId);
void freeGameContext(GameContext *context);