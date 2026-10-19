#pragma once
#include "cell.h"
#include "components/animator.h"

typedef struct player
{
  float x;
  float y;
  int status;
  float speed;
  CellList* path; // This can be expanded to hold the player's path
  Animator* animator;
} Player;

void initPlayer(Player *player, int x, int y);
void setPlayerPath(Player *player, CellList *path);
void updatePlayer(Player *player, float deltaTime);
void freePlayerPath(Player *player);
void drawPlayer(Player *player, int cellSize, int drawMode); // TODO: cellSize need to be a global constant, formed from the screen data and level data