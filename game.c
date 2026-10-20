#include "game.h"
#include "camera.h"
#include "talk_tree.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

void initGame(Game *game, const char *levelFilename)
{
  game->state = GAME_STATUS_PLAYING;
  initEventQueue(&game->eventQueue);
  loadLevel(&game->level, levelFilename);
  game->ui.textBoxUI.active = false;
  game->ui.textBoxUI.text = NULL;
  game->ui.textBoxUI.timer = 0;
  game->pendingEvent.hasPendingEventObject = false;
  game->pendingEvent.pendingEventScreenIndex = -1;
  game->pendingEvent.pendingEventObjectIndex = -1;
  game->pendingTransition.hasPendingTransition = false;
  game->pendingTransition.targetLevelIndex = -1;
  game->pendingTransition.targetScreenIndex = -1;
  game->pendingTransition.targetSpawnIndex = -1;
  initGameContext(&game->context);
  initCamera(game);
  initUI(&game->ui);
}

void loadGameLevel(Game *game, const char *filename)
{
  if (filename == NULL)
  {
    fprintf(stderr, "Level filename is NULL\n");
    return;
  }
  if (game == NULL)
  {
    fprintf(stderr, "Game pointer is NULL\n");
    return;
  }
  loadLevel(&game->level, filename);
}

void updateGame(Game *game, float dt)
{
  updateCamera(game);
  switch (game->state)
  {
  case PLAYING:
  {
    updateTextBox(&game->ui.textBoxUI, dt);
    updateLevel(game, &game->level, dt);
    updateEventQueue(&game->eventQueue, dt, game);
    break;
  }
  case UI_INTERRUPTION:
  {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
      // TODO: Code a more clear transition
      if (game->ui.talkTreeUI.active)
      {
        changeGameState(game, TALK_TREE);
      }
      else
      {
        changeGameState(game, PLAYING);
      }
      clearUI(&game->ui);
    }
    bool textBoxFinished = updateTextBox(&game->ui.textBoxUI, dt);
    if (textBoxFinished)
      if (game->pendingTransition.hasPendingTransition)
      {
        changeGameState(game, FADE_OUT);
      }
      else
      {
        if (game->ui.talkTreeUI.active)
        {
          changeGameState(game, TALK_TREE);
        }
        else
        {
          changeGameState(game, PLAYING);
        }
      }
    break;
  }
  case FADE_OUT:
  {
    updateBlackScreen(&game->ui.blackScreen, dt);
    if (game->ui.blackScreen.transparency >= 1.0f)
    {
      changeGameState(game, TRANSITION);
    }
    break;
  }
  case TRANSITION:
  {
    updatePendingTransition(game);
    if (game->pendingTransition.hasPendingTransition)
      break; // SAFEGUARD: Wait for transition to complete before changing state
    game->ui.blackScreen.transparency = 0;
    changeGameState(game, PLAYING);
    game->level.player.animator->currentAnimation = 0;
    break;
  }
  case TALK_TREE:
  {
    updateEventQueue(&game->eventQueue, dt, game);
    updateTalkTreeUI(game, dt);
    break;
  }
  default:
    break;
  }
}

void drawGame(Game *game, int drawMode)
{
  BeginMode2D(game->camera);
  drawLevel(&game->level, drawMode);
  EndMode2D();

  drawBlackScreen(&game->ui.blackScreen);
  drawTalkTreeUI(game);
  drawTextBox(&game->ui.textBoxUI);
}

void freeGame(Game *game)
{
  freeLevel(&game->level);
  freeEventQueue(&game->eventQueue);
  if (game->ui.textBoxUI.active)
  {
    free(game->ui.textBoxUI.text);
    game->ui.textBoxUI.text = NULL;
  }
  freeGameContext(&game->context);
}

void loadEventQueue(Game *game, EventDataList *eventDataList)
{
  for (int i = 0; i < eventDataList->eventDataQ; i++)
  {
    Event event;
    EventData *eventData = &eventDataList->eventData[i];
    event.finished = false;
    event.timer = 0;
    event.payload = eventData->payload;
    event.payloadType = eventData->payloadType;
    event.type = eventData->type;
    // event.targetPosition = event->targetPosition;
    enqueueEvent(&game->eventQueue, event);
  }
}

void changeGameState(Game *game, GameState newGameState)
{
  game->state = newGameState;
}

void notifyItemPickup(Game *game, const char *itemName)
{
  char text[TEXT_BOX_MAX_TEXT_LENGTH];
  snprintf(text, sizeof(text), "Recibiste: %s", itemName);
  addTextToTextBox(&game->ui.textBoxUI, text);
  changeGameState(game, UI_INTERRUPTION);
}

void notifyDoorLocked(Game *game)
{
  char text[TEXT_BOX_MAX_TEXT_LENGTH];
  snprintf(text, sizeof(text), "Door is locked\n");
  addTextToTextBox(&game->ui.textBoxUI, text);
  changeGameState(game, UI_INTERRUPTION);
}

void notifyDoorUnlocked(Game *game, const char *itemName)
{
  char text[TEXT_BOX_MAX_TEXT_LENGTH];
  snprintf(text, sizeof(text), "Door unlocked with %s\n", itemName);
  addTextToTextBox(&game->ui.textBoxUI, text);
  changeGameState(game, UI_INTERRUPTION);
}

void setPendingTransition(Game *game, int targetScreenIndex, int targetSpawnIndex, int targetLevelIndex)
{
  if (game->pendingTransition.hasPendingTransition)
  {
    printf("WARNING: There was a previous transition.\n");
  }
  game->pendingTransition.hasPendingTransition = true;
  game->pendingTransition.targetScreenIndex = targetScreenIndex;
  game->pendingTransition.targetSpawnIndex = targetSpawnIndex;
  game->pendingTransition.targetLevelIndex = targetLevelIndex;
  changeGameState(game, FADE_OUT);
}

void updatePendingTransition(Game *game)
{
  if (!game->pendingTransition.hasPendingTransition)
    return;
  if (game->eventQueue.count > 0)
    return;

  // TODO: Add functionality for level transition
  transitionScreen(&game->level, game->pendingTransition.targetScreenIndex, game->pendingTransition.targetSpawnIndex);

  game->pendingTransition.hasPendingTransition = false;
  game->pendingTransition.targetLevelIndex = -1;
  game->pendingTransition.targetScreenIndex = -1;
  game->pendingTransition.targetSpawnIndex = -1;
}