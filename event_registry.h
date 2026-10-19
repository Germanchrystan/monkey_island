#pragma once

#include <stdbool.h>

typedef struct Game Game;
typedef struct Event Event;

#include "event_queue.h"

typedef bool (*EventUpdate)(float dt, bool previousEventFinished, Game *game, Event *event);

bool updateItemPickup(float dt, bool previousEventFinished, Game *game, Event *event);
bool updateDialogStart(float dt, bool previousEventFinished, Game *game, Event *event);
bool updateDoorMessage(float dt, bool previousEventFinished, Game *game, Event *event);
bool updateKinematicMovement(float dt, bool previousEventFinished, Game *game, Event *event);
bool updateSetStartDialogNodeContext(float dt, bool previousEventFinished, Game *game, Event *event);

EventUpdate getEventUpdateFunction(EventType type);