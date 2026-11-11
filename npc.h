#pragma once
#include "grid.h"
#include "components/animator.h"

typedef enum 
{
  NPC_TEXTURE_LEADERS,
  NPC_TEXTURE_TABLE,
} NPCTexture;

extern const char *textureRegistry[];

typedef struct 
{
  GridPosition position;
  int widthInCells;
  int heightInCells;
  Animator *animator;
} Npc;

void updateNpc(Npc *npc, float dt);
void drawNpc(Npc *npc, int cellSize, int drawMode);


