#include "inventory.h"
#include <stdbool.h>
#include <stdlib.h>

static char* registry[ITEM_DEFINITION_COUNT] =
    {
        [ITEM_DEFINITION_RUSTED_KEY] = "Rusted Key",
        [ITEM_DEFINITION_TRAVELER_KEY] = "Santo y Seña",
};

void initInventory(Inventory *inventory) 
{
  inventory->capacity = BASE_INVENTORY_CAPACITY;
  inventory->count= 0;
  inventory->items = malloc(sizeof(InventoryItem *) * 10);
}

bool addItemToInventory(Inventory *inventory, InventoryItem item) 
{
  if (inventory->count >= inventory->capacity) 
  {
    int newCapacity = inventory->capacity * 2;
    InventoryItem **newItems = realloc(inventory->items, sizeof(InventoryItem *) * newCapacity + 1);
    if (newItems == NULL) 
    {
      return 0; // Failed to allocate memory
    }
    inventory->items = newItems;
    inventory->capacity = newCapacity;
  }
  inventory->items[inventory->count] = malloc(sizeof(InventoryItem));
  *(inventory->items[inventory->count]) = item;
  inventory->count++;
  return 1;
}

bool removeItemFromInventory(Inventory *inventory, int itemId) 
{
  for (int i = 0; i < inventory->count; i++) 
  {
    if (inventory->items[i]->id == itemId) 
    {
      free(inventory->items[i]);
      for (int j = i; j < inventory->count - 1; j++) 
      {
        inventory->items[j] = inventory->items[j + 1];
      }
      inventory->count--;
      return 1;
    }
  }
  return 0;
}

bool hasItemInInventory(Inventory *inventory, int itemId) 
{
  for (int i = 0; i < inventory->count; i++) 
  {
    if (inventory->items[i]->id == itemId) 
    {
      return 1;
    }
  }
  return 0;
}

char* findItemDefinitionById(int id)
{
  if (id <= 0 || id >= ITEM_DEFINITION_COUNT) 
  {
    return NULL;
  }
  return registry[id];
}