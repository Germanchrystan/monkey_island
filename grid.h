#pragma once

#include"cell.h"
#include <raylib.h>
#include "player.h"

typedef struct 
{
  Cell **cell;
  int rows;
  int cols;
  int cellSize;
} Grid;

typedef struct vector2
{
  int col;
  int row;
} GridPosition;

void initializeGrid(Grid *grid, int rows, int cols, int cellSize);
void resetSearchData(Grid *grid);
void drawGrid(Grid *grid);
void updateCellPathValues(Grid *grid, Cell *origin, GridPosition *target);
GridPosition defineNewTargetPosition(Grid *grid, GridPosition *target, Player *player);
CellList *search(Grid *grid, Player *player, GridPosition *target);
bool translateToGridPosition(Grid *grid, Vector2 mousePosition, Camera2D *camera, Vector2 *outVector, GridPosition *outPosition);
void LoadWalls(Grid *grid, const GridPosition wallData[], int wallCount);
