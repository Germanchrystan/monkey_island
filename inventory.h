#pragma once
#include <stdbool.h>
#include <stdlib.h>

#include "raylib.h"
#define BASE_INVENTORY_CAPACITY 10

typedef struct {
  int id;
  char *name;
} InventoryItem;

typedef struct {
  InventoryItem **items;
  int count;
  int capacity;
} Inventory;

typedef enum ItemDefinitionId {
  ITEM_DEFINITION_NONE = 0,
  ITEM_DEFINITION_RUSTED_KEY = 1,
  ITEM_DEFINITION_TRAVELER_KEY = 2,
  ITEM_DEFINITION_COUNT
} ItemDefinitionId;

void initInventory(Inventory *inventory);
bool addItemToInventory(Inventory *inventory, InventoryItem item);
bool removeItemFromInventory(Inventory *inventory, int itemId);
bool hasItemInInventory(Inventory *inventory, int itemId);
char* findItemDefinitionById(int id);