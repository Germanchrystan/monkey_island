#include "event_registry.h"
#include <stddef.h>
#include <stdio.h>
#include "game.h"
#include "inventory.h"
#include "talk_tree.h"

static EventUpdate registry[EVENT_TYPE_COUNT] =
{
        [EVENT_TYPE_NONE] = NULL,
        [EVENT_TYPE_ITEM_PICKUP] = updateItemPickup,
        [EVENT_TYPE_DOOR_INTERACTION] = updateDoorMessage,
        [EVENT_TYPE_DIALOG_TRIGGER] = updateDialogStart,
        [EVENT_TYPE_KINEMATIC_MOVEMENT] = updateKinematicMovement,
        [EVENT_SET_DIALOG_RUNTIME_START] = updateSetStartDialogNodeContext,
};

EventUpdate getEventUpdateFunction(EventType type)
{
  if (type <= EVENT_TYPE_NONE || type >= EVENT_TYPE_COUNT)
    return NULL;

  return registry[type];
}

bool updateSetStartDialogNodeContext(float dt, bool previousEventFinished, Game *game, Event *event)
{
  (void)dt;
  if (!previousEventFinished) return false;
  if (event->payloadType != EVENT_PAYLOAD_SET_DIALOG_RUNTIME_START) return true;

  
  setDialogRuntimeChangeState(&game->context, event->payload.dialogRuntimeStart.dialogId, event->payload.dialogRuntimeStart.startNode);
  
  return true;
}

bool updateItemPickup(float dt, bool previousEventFinished, Game *game, Event *event)
{
  (void)dt;

  if (!previousEventFinished)
    return false;

  if (event->payloadType != EVENT_PAYLOAD_ITEM_PICKUP)
    return true;

  int itemIndex = event->payload.itemPickup.itemIndex;

  char *itemName = findItemDefinitionById(itemIndex);
  if (itemName == NULL)
  {
    printf("Error: Item definition not found for item ID %d\n", itemIndex);
    itemName = "Unknown Item";
  }

  notifyItemPickup(game, itemName);

  InventoryItem item = {.id = itemIndex, .name = itemName};
  addItemToInventory(&game->inventory, item);
  return true;
}

bool updateDialogStart(float dt, bool previousEventFinished, Game *game, Event *event)
{
  (void)dt;

  if (!previousEventFinished)
    return false;

  if (event->payloadType != EVENT_PAYLOAD_DIALOG_TRIGGER)
    return true;

  int dialogId = event->payload.dialogTrigger.dialogId;

  initTalkTree(game, dialogId);
  if (game->currentTalkTree == NULL)
  {
    printf("Error: Dialog node not found for dialog ID %d\n", dialogId);
    return true;
  }

  changeGameState(game, TALK_TREE);
  return true;
}

bool updateDoorMessage(float dt, bool previousEventFinished, Game *game, Event *event)
{
  (void)dt;

  if (!previousEventFinished)
    return false;

  if (event->payloadType != EVENT_PAYLOAD_DOOR_INTERACTION)
    return true;

  int screenIndex = event->payload.doorInteraction.sourceScreenIndex;
  int requiredItemId = event->payload.doorInteraction.sourceRequiredItemId;
  bool defaultLocked = event->payload.doorInteraction.defaultLocked;
  bool unlocked;

  Screen *screen = &game->level.screens[screenIndex];

  if (defaultLocked)
  {
    getDoorRuntimeChangeState(&game->context, game->levelIndex, screenIndex, event->payload.doorInteraction.sourceDoorId, &unlocked);

    if (unlocked)
    {
      setPendingTransition(game, event->payload.doorInteraction.destinationScreenIndex, event->payload.doorInteraction.destinationSpawnIndex, -1); // TODO: Add support for Level index
      return true;
    }

    if (requiredItemId >= 0)
    {

      bool hasKey = false;
      for (int j = 0; j < game->inventory.count; j++)
      {
        if (game->inventory.items[j]->id == requiredItemId)
        {
          setDoorRuntimeChangeState(&game->context, game->levelIndex, screenIndex, event->payload.doorInteraction.sourceDoorId, true);
          notifyDoorUnlocked(game, game->inventory.items[j]->name);
          setPendingTransition(game, event->payload.doorInteraction.destinationScreenIndex, event->payload.doorInteraction.destinationSpawnIndex, -1); // TODO: Add support for Level index
          return true;
        }
      }
    }
    notifyDoorLocked(game);
    return true;
  }

  setPendingTransition(game, event->payload.doorInteraction.destinationScreenIndex, event->payload.doorInteraction.destinationSpawnIndex, -1); // TODO: Add support for Level index

  return true;
}

bool updateKinematicMovement(float dt, bool previousEventFinished, Game *game, Event *event)
{
  (void)dt;
  (void)previousEventFinished;
  (void)game;
  (void)event;
  if (!previousEventFinished)
    return false;

  if (event->payloadType != EVENT_PAYLOAD_KINEMATIC_MOVEMENT)
    return true;

  return true;
}
