#include <stdio.h>
#include <stdlib.h>
#include "../level.h"
#include "../grid.h"
#include "../game.h"
#include "../event_registry.h"

static void loadEventDataFromType(FILE *file, int eventType, EventData *eventData)
{
  eventData->type = eventType;
  switch (eventType)
  {
  case EVENT_TYPE_ITEM_PICKUP:
    int dummySourceScreenIndex, dummySourceEventObjectIndex; // TODO: DELETE DUMMY DATA
    eventData->payloadType = EVENT_PAYLOAD_ITEM_PICKUP;
    fscanf(file, "%d %d %d", &dummySourceScreenIndex, &dummySourceEventObjectIndex, &eventData->payload.itemPickup.itemIndex);
    break;
  case EVENT_TYPE_DOOR_INTERACTION:
    eventData->payloadType = EVENT_PAYLOAD_DOOR_INTERACTION;
    {
      int defaultLocked = 0;
      fscanf(file, "%d %d %d %d %d %d",
             &eventData->payload.doorInteraction.sourceScreenIndex,
             &eventData->payload.doorInteraction.sourceDoorId,
             &eventData->payload.doorInteraction.destinationScreenIndex,
             &eventData->payload.doorInteraction.destinationSpawnIndex,
             &eventData->payload.doorInteraction.sourceRequiredItemId,
             &defaultLocked);
      eventData->payload.doorInteraction.defaultLocked = defaultLocked != 0;
    }
    break;
  case EVENT_TYPE_DIALOG_TRIGGER:
    eventData->payloadType = EVENT_PAYLOAD_DIALOG_TRIGGER;
    fscanf(file, "%d", &eventData->payload.dialogTrigger.dialogId);
    break;
  case EVENT_SET_DIALOG_RUNTIME_START:
    eventData->payloadType = EVENT_PAYLOAD_SET_DIALOG_RUNTIME_START;
    fscanf(file, "%d", &eventData->payload.dialogRuntimeStart.dialogId);
    eventData->payload.dialogRuntimeStart.startNode = ""; // TODO: ADD THIS DATA TO LEVEL
    break;
  case EVENT_TYPE_KINEMATIC_MOVEMENT:
    eventData->payloadType = EVENT_PAYLOAD_KINEMATIC_MOVEMENT;
    // TODO: This is just a placeholder, we need to decide how to serialize kinematic movement data
    fscanf(file, "%d %d", &eventData->payload.kinematicMovement.targetPosition.col, &eventData->payload.kinematicMovement.targetPosition.row);
    break;
  default:
    printf("Unknown event type: %d\n", eventType);
    break;
  }
}

static void saveEventDataToType(FILE *file, EventData *eventData)
{
  fprintf(file, "%d\n", eventData->type);
  switch (eventData->type)
  {
  case EVENT_TYPE_ITEM_PICKUP:
    // TODO: DELETE DUMMY DATA
    fprintf(file, "%d %d %d\n", 0, 0, eventData->payload.itemPickup.itemIndex);
    break;
  case EVENT_TYPE_DOOR_INTERACTION:
    fprintf(file, "%d %d %d %d %d %d\n",
            eventData->payload.doorInteraction.sourceScreenIndex,
            eventData->payload.doorInteraction.sourceDoorId,
            eventData->payload.doorInteraction.destinationScreenIndex,
            eventData->payload.doorInteraction.destinationSpawnIndex,
            eventData->payload.doorInteraction.sourceRequiredItemId,
            eventData->payload.doorInteraction.defaultLocked ? 1 : 0);
    break;
  case EVENT_TYPE_DIALOG_TRIGGER:
    fprintf(file, "%d\n", eventData->payload.dialogTrigger.dialogId);
    break;
  case EVENT_SET_DIALOG_RUNTIME_START:
    fprintf(file, "%d\n", eventData->payload.dialogRuntimeStart.dialogId);
    eventData->payload.dialogRuntimeStart.startNode = ""; // TODO: ADD THIS DATA TO LEVEL
    break;
  case EVENT_TYPE_KINEMATIC_MOVEMENT:
    fprintf(file, "%d %d\n", eventData->payload.kinematicMovement.targetPosition.col, eventData->payload.kinematicMovement.targetPosition.row);
    break;
  default:
    printf("Unknown event type: %d\n", eventData->type);
    break;
  }
}

void saveLevel(Level *level, const char *filename)
{
  FILE *file = fopen(filename, "w");
  if (file == NULL)
  {
    fprintf(stderr, "Error opening level file for writing: %s\n", filename);
    return;
  }

  fprintf(file, "%d\n", 1); // Version number
  fprintf(file, "%d\n", level->screensQ);
  for (int i = 0; i < level->screensQ; ++i)
  {
    Screen *screen = &level->screens[i];
    Grid *grid = &screen->grid;
    fprintf(file, "%d %d %d\n", grid->rows, grid->cols, grid->cellSize);

    int wallCount = 0;
    for (int row = 0; row < grid->rows; ++row)
    {
      for (int col = 0; col < grid->cols; ++col)
      {
        if (grid->cell[row][col].solid)
        {
          wallCount++;
        }
      }
    }
    fprintf(file, "%d\n", wallCount);
    for (int row = 0; row < grid->rows; ++row)
    {
      for (int col = 0; col < grid->cols; ++col)
      {
        if (grid->cell[row][col].solid)
        {
          fprintf(file, "%d %d\n", col, row);
        }
      }
    }

    fprintf(file, "%d\n", screen->spawnPointQ);
    for (int j = 0; j < screen->spawnPointQ; ++j)
    {
      SpawnPoint pos = screen->spawnPoints[j];
      fprintf(file, "%d %d %d %d\n", pos.spawnIndex, pos.position.col, pos.position.row, pos.facingDirection);
    }

    fprintf(file, "%d\n", screen->eventObjQ);
    for (int j = 0; j < screen->eventObjQ; ++j)
    {
      EventObject eventObject = screen->eventObjects[j];
      fprintf(file, "%d %d %d %d %d %d %d\n",
              eventObject.active ? 1 : 0,
              eventObject.oneShot ? 1 : 0,
              eventObject.position.col,
              eventObject.position.row,
              eventObject.targetPosition.col,
              eventObject.targetPosition.row,
              eventObject.eventDataList.eventDataQ);
      for (int k = 0; k < eventObject.eventDataList.eventDataQ; ++k)
      {
        saveEventDataToType(file, &eventObject.eventDataList.eventData[k]);
      }
    }
  }

  fclose(file);
}

void loadLevel(Level *level, const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    fprintf(stderr, "Error opening level file: %s\n", filename);
    return;
  }

  int version;
  fscanf(file, "%d", &version);
  int screenCount;
  fscanf(file, "%d", &screenCount);
  level->screensQ = screenCount;
  level->screens = (Screen *)malloc(screenCount * sizeof(Screen));
  if (level->screens == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for screens\n");
    fclose(file);
    level = NULL;
    return;
  }
  for (int i = 0; i < screenCount; ++i)
  {
    Screen *screen = &level->screens[i];
    Grid grid;
    int rows, cols, cellSize;
    fscanf(file, "%d %d %d", &rows, &cols, &cellSize);
    initializeGrid(&grid, rows, cols, cellSize);
    int textureId, backgroundWidth, backgroundHeight, foregroundWidth, foregroundHeight;
    ScreenTextures screenTextures;
    fscanf(file, "%d %d %d %d %d", &textureId, &backgroundWidth, &backgroundHeight, &foregroundWidth, &foregroundHeight);
    screenTextures.backgroundSize = (Vector2){backgroundWidth, backgroundHeight};
    screenTextures.foregroundSize = (Vector2){foregroundWidth, foregroundHeight};
    const char bkName[20];
    const char fgName[20];
    sprintf(bkName, "assets/bg_%d.png", textureId);
    sprintf(fgName, "assets/fg_%d.png", textureId);
    screenTextures.background = malloc(sizeof(*screenTextures.background));
    screenTextures.foreground = malloc(sizeof(*screenTextures.foreground));
    if (screenTextures.background != NULL)
      *screenTextures.background = LoadTexture(bkName);
    if (screenTextures.foreground != NULL)
      *screenTextures.foreground = LoadTexture(fgName);
    screen->screenTextures = screenTextures;
    screen->grid = grid;
    int wallCount, spawnCount, doorCount, eventObjCount;
    fscanf(file, "%d", &wallCount);

    for (int j = 0; j < wallCount; ++j)
    {
      GridPosition pos;
      fscanf(file, "%d %d", &pos.col, &pos.row);
      screen->grid.cell[pos.row][pos.col].solid = true;
    }
    fscanf(file, "%d", &spawnCount);
    screen->spawnPointQ = spawnCount;
    screen->spawnPoints = (SpawnPoint *)malloc(spawnCount * sizeof(SpawnPoint));
    if (screen->spawnPoints == NULL)
    {
      fprintf(stderr, "Failed to allocate memory for spawn points\n");
      fclose(file);
      freeLevel(level);
      level = NULL;
      return;
    }
    for (int j = 0; j < spawnCount; ++j)
    {
      SpawnPoint spawnPoint;
      fscanf(file, "%d %d %d %d", &spawnPoint.spawnIndex, &spawnPoint.position.col, &spawnPoint.position.row, &spawnPoint.facingDirection);
      screen->spawnPoints[j] = spawnPoint;
    }

    fscanf(file, "%d", &eventObjCount);
    screen->eventObjQ = eventObjCount;
    screen->eventObjects = (EventObject *)malloc(eventObjCount * sizeof(EventObject));
    if (screen->eventObjects == NULL)
    {
      fprintf(stderr, "Failed to allocate memory for event objects\n");
      fclose(file);
      freeLevel(level);
      level = NULL;
      return;
    }
    for (int j = 0; j < eventObjCount; j++)
    {
      EventObject eventObject;
      int active = 1;
      int oneShot = 0;
      fscanf(file, "%d %d %d %d %d %d %d",
             &active,
             &oneShot,
             &eventObject.position.col,
             &eventObject.position.row,
             &eventObject.targetPosition.col,
             &eventObject.targetPosition.row,
             &eventObject.eventDataList.eventDataQ);
      eventObject.active = active != 0;
      eventObject.oneShot = oneShot != 0;
      eventObject.eventDataList.eventData = (EventData *)malloc(eventObject.eventDataList.eventDataQ * sizeof(EventData));
      if (eventObject.eventDataList.eventData == NULL)
      {
        fprintf(stderr, "Failed to allocate memory for event data\n");
        fclose(file);
        freeLevel(level);
        level = NULL;
        return;
      }
      for (int k = 0; k < eventObject.eventDataList.eventDataQ; k++)
      {
        int eventType;
        fscanf(file, "%d", &eventType);
        loadEventDataFromType(file, eventType, eventObject.eventDataList.eventData + k);
      }
      screen->eventObjects[j] = eventObject;
    }
  }
  //
  initPlayer(&level->player, level->screens[0].spawnPoints[0].position.col, level->screens[0].spawnPoints[0].position.row);
  level->currentScreen = 0;
  fclose(file);
}