#include <stdio.h>
#include <stdlib.h>
#include "level.h"
#include "grid.h"
#include "game.h"

void updateLevel(Game *game, Level *level, float dt)
{
  Screen *screen = &level->screens[level->currentScreen];
  Grid *grid = &screen->grid;
  GridPosition lastTargetPos;
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
  {
    game->pendingEvent.hasPendingEventObject = false; // Clear pending event object on new click
    Vector2 mousePos = GetMousePosition();
    Vector2 outVector;
    GridPosition outGridPos;
    bool isValidPosition = translateToGridPosition(grid, mousePos, &game->camera, &outVector, &outGridPos);
    // Event Object Detection
    EventObject *clickedObj = NULL;
    int clickedObjIndex = -1;
    for (int i = 0; i < screen->eventObjQ; i++)
    {
      EventObject *obj = &screen->eventObjects[i];
      if (!obj->active)
        continue;
      if (obj->position.col == outGridPos.col &&
          obj->position.row == outGridPos.row)
      {
        clickedObj = obj;
        clickedObjIndex = i;
        break;
      }
    }

    if (clickedObj != NULL)
    {
      game->pendingEvent.hasPendingEventObject = true;
      game->pendingEvent.pendingEventScreenIndex = level->currentScreen;
      game->pendingEvent.pendingEventObjectIndex = clickedObjIndex;
      game->pendingEvent.pendingEventTargetPosition = clickedObj->targetPosition;

      setPlayerPath(&level->player, search(grid, &level->player, &clickedObj->targetPosition));

      return;
    }
    if (!isValidPosition) return;
    lastTargetPos = outGridPos;
    GridPosition newTargetPos = defineNewTargetPosition(grid, &lastTargetPos, &level->player);
    lastTargetPos = newTargetPos;
    setPlayerPath(&level->player, search(grid, &level->player, &lastTargetPos));
  }
  updatePlayer(&level->player, dt);
  checkForPendingEventObjectTrigger(game, level);
}

void transitionScreen(Level *level, int screenIndex, int spawnIndex)
{
  freePlayerPath(&level->player);
  int newCurrentScreenIndex = 0;
  if (screenIndex >= level->screensQ || screenIndex < 0)
  {
    printf("Wrong screen index\n");
  }
  else
  {
    newCurrentScreenIndex = screenIndex;
  }
  Screen *newScreen = &level->screens[newCurrentScreenIndex];
  level->currentScreen = newCurrentScreenIndex;
  GridPosition *spawnPoint = &(GridPosition){0, 0};
  
  bool spawnPointFound = false;
  for (int i = 0; i < newScreen->spawnPointQ; i++)
  {
    if (newScreen->spawnPoints[i].spawnIndex == spawnIndex)
    {
      spawnPointFound = true;
      level->player.x = newScreen->spawnPoints[i].position.col;
      level->player.y = newScreen->spawnPoints[i].position.row;
      break;
    }
  }
  
  if (!spawnPointFound)
  {
    printf("Spawn point with index %d not found in screen %d\n", spawnIndex, newCurrentScreenIndex);
    GridPosition defaultSpawnPoint = {0, 0};
    Grid levelGrid = newScreen->grid;
    for (int row = 0; row < levelGrid.rows; row++)
    {
      bool found = false;
      for (int col = 0; col < levelGrid.cols; col++)
      {
        if (!levelGrid.cell[row][col].solid)
        {
          defaultSpawnPoint.col = col;
          defaultSpawnPoint.row = row;
          found = true;
          break;
        }
      }
      if (found) break;
    }
    level->player.x = defaultSpawnPoint.col;
    level->player.y = defaultSpawnPoint.row;
  }
}

void drawLevel(Level *level, int drawMode)
{
  float transparency = drawMode == 0 ? 1.0f : 0.5f;
  drawGrid(&level->screens[level->currentScreen].grid);
  drawEventObjects(&level->screens[level->currentScreen]);
  DrawTexture(*level->screens[level->currentScreen].screenTextures.background, 0, 0, (Color){255, 255, 255, (unsigned char)(255 * transparency)});
  drawPlayer(&level->player, level->screens[level->currentScreen].grid.cellSize, drawMode);
  DrawTexture(*level->screens[level->currentScreen].screenTextures.foreground, 0, 0, (Color){255, 255, 255, (unsigned char)(255 * transparency)});
}

void drawEventObjects(Screen *screen)
{
  // TODO: Extend this to draw different types of event objects differently, maybe with sprites
  // Only draw active event objects
  for (int i = 0; i < screen->eventObjQ; ++i)
  {
    EventObject *eventObject = &screen->eventObjects[i];
    if (!eventObject->active)
      continue;
    DrawRectangle(
        eventObject->position.col * screen->grid.cellSize + 1,
        eventObject->position.row * screen->grid.cellSize + 1,
        screen->grid.cellSize - 1,
        screen->grid.cellSize - 1,
        GOLD);
  }
}

void freeLevel(Level *level)
{
  for (int i = 0; i < level->screensQ; ++i)
  {
    free(level->screens[i].spawnPoints);
    for (int j = 0; j < level->screens[i].grid.rows; ++j)
    {
      free(level->screens[i].grid.cell[j]);
    }
    free(level->screens[i].grid.cell);
    for (int j = 0; j < level->screens[i].eventObjQ; j++)
    {
      EventDataList eventDataList = level->screens[i].eventObjects[j].eventDataList;
      for (int k = 0; k < eventDataList.eventDataQ; k++)
      {
        free(eventDataList.eventData + k); // TODO: Check this
      }
    }
    free(level->screens[i].eventObjects);
  }
  free(level->screens);
}

void checkForPendingEventObjectTrigger(Game *game, Level *level)
{
  if (!game->pendingEvent.hasPendingEventObject)
    return;

  if ((int)level->player.x == game->pendingEvent.pendingEventTargetPosition.col &&
      (int)level->player.y == game->pendingEvent.pendingEventTargetPosition.row)
  {
    EventObject *eventObject = &level->screens[game->pendingEvent.pendingEventScreenIndex].eventObjects[game->pendingEvent.pendingEventObjectIndex];
    loadEventQueue(game, &eventObject->eventDataList);

    if (eventObject->oneShot)
    {
      // TODO: Add GameContext functionality to mark one-shot event objects as completed, instead of just deactivating them. 
      eventObject->active = false;
    }
    game->pendingEvent.hasPendingEventObject = false;
  }
}