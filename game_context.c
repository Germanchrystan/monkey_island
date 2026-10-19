#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "game_context.h"

void initGameContext(GameContext *context)
{
  context->doors = NULL;
  context->doorCount = 0;
  context->dialogs = NULL;
  context->dialogCount = 0;
}

void setDoorRuntimeChangeState(GameContext *context, int levelIndex, int screenIndex, int doorId, bool unlocked)
{
  for (int i = 0; i < context->doorCount; i++)
  {
    if (context->doors[i].levelIndex == levelIndex &&
        context->doors[i].screenIndex == screenIndex &&
        context->doors[i].doorId == doorId)
    {
      context->doors[i].unlocked = unlocked;
      return;
    }
  }

  context->doorCount++;
  context->doors = realloc(context->doors, context->doorCount * sizeof(DoorRuntimeChangeState));
  if (context->doors == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for doors\n");
    exit(1);
  }
  context->doors[context->doorCount - 1].levelIndex = levelIndex;
  context->doors[context->doorCount - 1].screenIndex = screenIndex;
  context->doors[context->doorCount - 1].doorId = doorId;
  context->doors[context->doorCount - 1].unlocked = unlocked;
}

void getDoorRuntimeChangeState(GameContext *context, int levelIndex, int screenIndex, int doorId, bool *unlocked)
{
  for (int i = 0; i < context->doorCount; i++)
  {
    if (context->doors[i].levelIndex == levelIndex &&
        context->doors[i].screenIndex == screenIndex &&
        context->doors[i].doorId == doorId)
    {
      *unlocked = context->doors[i].unlocked;
      return;
    }
  }
  *unlocked = false;
}

void setDialogRuntimeChangeState(GameContext *context, int dialogId, const char *nodeId)
{
  for (int i = 0; i < context->dialogCount; i++)
  {
    if (context->dialogs[i].dialogId == dialogId)
    {
      context->dialogs[i].nodeId = nodeId;
      return;
    }
  }

  context->dialogCount++;
  context->dialogs = realloc(context->dialogs, context->dialogCount * sizeof(DialogRuntimeChangeState));
  if (context->dialogs == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for dialogs\n");
    exit(1);
  }
  context->dialogs[context->dialogCount - 1].dialogId = dialogId;
  context->dialogs[context->dialogCount - 1].nodeId = nodeId;
}

void getDialogRuntimeChangeState(GameContext *context, int dialogId, const char **nodeId)
{
  if (context == NULL || nodeId == NULL || context->dialogs == NULL)
  {
    *nodeId = NULL;
    return;
  }

  for (int i = 0; i < context->dialogCount; i++)
  {
    if (context->dialogs[i].dialogId == dialogId)
    {
      *nodeId = context->dialogs[i].nodeId;
      return;
    }
  }
  *nodeId = NULL;
}

void freeGameContext(GameContext *context)
{
  free(context->doors);
  context->doors = NULL;
  context->doorCount = 0;
  free(context->dialogs);
  context->dialogs = NULL;
  context->dialogCount = 0;
}

