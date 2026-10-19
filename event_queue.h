#pragma once
#include <stdbool.h>
#include "grid.h"

typedef struct Game Game;

#define INITIAL_EVENT_QUEUE_CAPACITY 4

typedef enum EventType {
  EVENT_TYPE_NONE = 0,
  EVENT_TYPE_ITEM_PICKUP,
  EVENT_TYPE_DOOR_INTERACTION,
  EVENT_TYPE_DIALOG_TRIGGER,
  EVENT_TYPE_KINEMATIC_MOVEMENT,
  EVENT_SET_DIALOG_RUNTIME_START,
  EVENT_TYPE_COUNT
} EventType;

typedef enum EventPayloadType {
  EVENT_PAYLOAD_NONE = 0,
  EVENT_PAYLOAD_ITEM_PICKUP,
  EVENT_PAYLOAD_DOOR_INTERACTION,
  EVENT_PAYLOAD_DIALOG_TRIGGER,
  EVENT_PAYLOAD_KINEMATIC_MOVEMENT,
  EVENT_PAYLOAD_SET_DIALOG_RUNTIME_START,
  EVENT_PAYLOAD_COUNT
} EventPayloadType;

typedef struct EventPayloadItemPickup
{
  int itemIndex;
} EventPayloadItemPickup;

typedef struct EventPayloadDialogStart {
  char startNodeId[32];
  int npcId;
  int dialogId;
} EventPayloadDialogStart;

typedef struct EventPayloadDoorInteraction {
  int sourceScreenIndex;
  int sourceDoorId;
  int destinationScreenIndex;
  int destinationSpawnIndex;
  int sourceRequiredItemId;
  bool defaultLocked;
} EventPayloadDoorInteraction;

typedef struct EventPayloadKinematicMovement {
  GridPosition targetPosition;
} EventPayloadKinematicMovement;

typedef struct EventPayloadDialogTrigger {
  int dialogId;
} EventPayloadDialogTrigger;

typedef struct EventPayloadSetDialogRuntimeStart {
  int dialogId;
  char *startNode;
} EventPayloadSetDialogRuntimeStart;

typedef union EventPayload {
  EventPayloadItemPickup itemPickup;
  EventPayloadDialogStart dialogStart;
  EventPayloadDoorInteraction doorInteraction;
  EventPayloadKinematicMovement kinematicMovement;
  EventPayloadSetDialogRuntimeStart dialogRuntimeStart;
  EventPayloadDialogTrigger dialogTrigger;
} EventPayload;

typedef struct Event
{
  bool finished; 
  float timer;
  // GridPosition targetPosition;
  
  EventType type;
  EventPayloadType payloadType;
  EventPayload payload;
} Event;

typedef struct 
{
  int count;
  int capacity;
  Event *events;
} EventQueue;

void initEventQueue(EventQueue *queue);
void enqueueEvent(EventQueue *queue, Event event);
void updateEventQueue(EventQueue *queue, float dt, Game *game);
void clearEventQueue(EventQueue *queue);
void freeEventQueue(EventQueue *queue);
