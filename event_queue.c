#include "event_queue.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "event_registry.h"

void initEventQueue(EventQueue *queue) {
    queue->events = malloc(sizeof(Event) * INITIAL_EVENT_QUEUE_CAPACITY);
    if (queue->events == NULL) {
        printf("Failed to allocate memory for event queue\n");
        queue->count = 0;
        queue->capacity = 0;
        return;
    }
    queue->count = 0;
    queue->capacity = INITIAL_EVENT_QUEUE_CAPACITY;
}

void enqueueEvent(EventQueue *queue, Event event) {
    if (queue->count >= queue->capacity) {
        int newCapacity = queue->capacity * 2;
        Event *newEvents = realloc(queue->events, sizeof(Event) * newCapacity);
        if (newEvents == NULL) {
            printf("Failed to reallocate memory for event queue\n");
            return;
        }
        queue->events = newEvents;
        queue->capacity = newCapacity;
    }
    queue->events[queue->count++] = event;
}

void updateEventQueue(EventQueue *queue, float dt, Game *game) 
{
  if (queue == NULL) return;
  bool previosuEventFinished = true;
  for (int i = 0; i < queue->count; i++)
  {
    Event *event = &queue->events[i];
    EventUpdate updateFunction = getEventUpdateFunction(event->type);
    if (updateFunction == NULL)    {
      printf("No update function registered for event type %d\n", event->type);
      continue;
    }
    event->finished = updateFunction(dt, previosuEventFinished, game, event);
    previosuEventFinished = event->finished;
  }
  for (int i = 0; i < queue->count; i++)
  {
    if (queue->events[i].finished)
    {
      for (int j = 0; j < queue->count - i - 1; j++)
      {
        queue->events[i + j] = queue->events[i + j + 1];
      }
      queue->count--;
      i--;
    }
  }
}

void freeEventQueue(EventQueue *queue) {
    free(queue->events);
    queue->events = NULL;
    queue->count = 0;
    queue->capacity = 0;
}
