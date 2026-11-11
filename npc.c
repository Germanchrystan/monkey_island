#include "npc.h"

const char *textureRegistry[] = {
    [NPC_TEXTURE_LEADERS] = "assets/leaders.png",
    [NPC_TEXTURE_TABLE] = "assets/table.png"
};

void updateNpc(Npc *npc, float dt)
{
  Update(npc->animator, dt, 0);
}

void drawNpc(Npc *npc, int cellSize, int drawMode)
{
  (void)drawMode;

  Draw(npc->animator,
       (Rectangle){npc->position.col * cellSize - cellSize / 2,
            (npc->position.row) * cellSize,
            cellSize * npc->widthInCells,
            cellSize * npc->heightInCells},
       false);
}