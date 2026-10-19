#include <stdio.h>

#include "grid.h"
#include "cell.h"
#include "player.h"
#include "constants.h"

void initializeGrid(Grid *grid, int rows, int cols, int cellSize)
{
  grid->rows = rows;
  grid->cols = cols;
  grid->cellSize = cellSize;
  grid->cell = (Cell **)malloc(rows * sizeof(Cell *));
  for (int i = 0; i < rows; ++i)
  {
    grid->cell[i] = (Cell *)malloc(cols * sizeof(Cell));
    for (int j = 0; j < cols; ++j)
    {
      Cell *cell = &grid->cell[i][j];
      cell->col = j;
      cell->row = i;
      cell->G = 0;
      cell->H = 0;
      cell->F = 0;
      cell->solid = false;
    }
  }
}

void resetSearchData(Grid *grid)
{
  for (int i = 0; i < grid->rows; ++i)
  {
    for (int j = 0; j < grid->cols; ++j)
    {
      Cell *cell = &grid->cell[i][j];
      cell->G = 0;
      cell->H = 0;
      cell->F = 0;
      cell->parent = NULL;
    }
  }
}

void drawGrid(Grid *grid)
{
  for (int i = 0; i < grid->rows; ++i)
  {
    for (int j = 0; j < grid->cols; ++j)
    {
      Cell *cell = &grid->cell[i][j];
      DrawRectangle(
          j * grid->cellSize + 1,
          i * grid->cellSize + 1,
          grid->cellSize - 1,
          grid->cellSize - 1,
          colors[cell->solid]);
      DrawText(
          TextFormat("%d, %d", i, j),
          j * grid->cellSize + 1,
          i * grid->cellSize + 1,
          10,
          RED
      );
    }
  }
}

void updateCellPathValues(Grid *grid, Cell *origin, GridPosition *target)
{
  for (int i = 0; i < grid->rows; ++i)
  {
    for (int j = 0; j < grid->cols; ++j)
    {
      Cell *cell = &grid->cell[i][j];
      cell->G = abs(origin->col - j) + abs(origin->row - i);
      cell->H = abs(target->col - j) + abs(target->row - i);
      cell->F = cell->G + cell->H;
    }
  }
}

GridPosition defineNewTargetPosition(Grid *grid, GridPosition *target, Player *player)
{
  bool isValidPosition = false;
  GridPosition newPos = {target->col, target->row};
  while(!isValidPosition)
  { 
    bool isPosSolid = grid->cell[newPos.row][newPos.col].solid;
    Cell *leftNeighbor = (newPos.col > 0) ? &grid->cell[newPos.row][newPos.col - 1] : NULL;
    Cell *topNeighbor = (newPos.row > 0) ? &grid->cell[newPos.row - 1][newPos.col] : NULL;
    Cell *rightNeighbor = (newPos.col < grid->cols - 1) ? &grid->cell[newPos.row][newPos.col + 1] : NULL;
    Cell *bottomNeighbor = (newPos.row < grid->rows - 1) ? &grid->cell[newPos.row + 1][newPos.col] : NULL;
    Cell *neighbors[] = {leftNeighbor, topNeighbor, rightNeighbor, bottomNeighbor};
      
    for (int i = 0; i < 4; ++i)
    {
      if (neighbors[i] != NULL)
      {
        neighbors[i]->G = abs((int)(player->x) - neighbors[i]->col) + abs((int)(player->y) - neighbors[i]->row);

        if (!neighbors[i]->solid && !isPosSolid)
        {
          isValidPosition = true;
        }
      } 
    }

    if (!isValidPosition)
    {
      for (int i = 0; i < 4; ++i)
      {
        if (neighbors[i] == NULL) continue;
        for (int j = 0; j < 4; ++j)
        {
          if (neighbors[j] != NULL && neighbors[i]->G < neighbors[j]->G)
          {
            Cell *temp = neighbors[i];
            neighbors[i] = neighbors[j];
            neighbors[j] = temp;
          }
        }
      }

      newPos.col = neighbors[0]->col;
      newPos.row = neighbors[0]->row;
    }
  }

  return newPos;
}

CellList *search(Grid *grid, Player *player, GridPosition *target)
{
  resetSearchData(grid);
  Cell *startCell = &grid->cell[(int)(player->y)][(int)(player->x)];

  startCell->G = 999; // COLUMNS * ROWS;
  startCell->H = abs(target->col - (int)(player->x)) + abs(target->row - (int)(player->y));
  startCell->F = startCell->G + startCell->H;

  CellList *openList = CreateCellList(10, startCell);
  CellList *closedList = CreateCellList(10, NULL);
  startCell->parent = NULL;
  AddCellToList(openList, startCell);
  updateCellPathValues(grid, startCell, target);
  while (openList->size > 0)
  {
    sortList(openList);
    Cell *closestCell = openList->cell[0];
    if (closestCell->col == target->col && closestCell->row == target->row)
    {
      CellList *path = reconstructPath(closestCell);
      FreeCellList(openList);
      FreeCellList(closedList);
      return path;
    }

    RemoveCellFromList(openList, closestCell);
    AddCellToList(closedList, closestCell);

    Cell *neighbors[] = {
        (closestCell->col > 0) ? &grid->cell[closestCell->row][closestCell->col - 1] : NULL,
        (closestCell->row > 0) ? &grid->cell[closestCell->row - 1][closestCell->col] : NULL,
      (closestCell->col < grid->cols - 1) ? &grid->cell[closestCell->row][closestCell->col + 1] : NULL,
      (closestCell->row < grid->rows - 1) ? &grid->cell[closestCell->row + 1][closestCell->col] : NULL};
    
    for (int i = 0; i < 4; ++i)
    {
      Cell *neighbor = neighbors[i];
      if (neighbor == NULL || neighbor->solid || IsCellInList(closedList, neighbor))
        continue;
        
      int tentativeG = closestCell->G + getCellDistance(closestCell, neighbor);
      
      if (!IsCellInList(openList, neighbor)) 
      {
        AddCellToList(openList, neighbor);
      }
      else if (tentativeG >= neighbor->G)
        continue;

      neighbor->parent = closestCell;
      neighbor->G = tentativeG;
      neighbor->H = abs(target->col - neighbor->col) + abs(target->row - neighbor->row);
      neighbor->F = neighbor->G + neighbor->H;
    }
  }
  sortListByH(closedList);
  CellList *path = reconstructPath(closedList->cell[0]);
  FreeCellList(openList);
  FreeCellList(closedList);
  return path;
}

bool translateToGridPosition(Grid *grid, Vector2 mousePosition, Camera2D *camera, Vector2 *outVector, GridPosition *outPosition)
{
  if (camera != NULL)
  {
    Vector2 mouseWorld = GetScreenToWorld2D(mousePosition, *camera);
    outVector->x = mouseWorld.x;
    outVector->y = mouseWorld.y;
    outPosition->col = (int)(mouseWorld.x / grid->cellSize);
    outPosition->row = (int)(mouseWorld.y / grid->cellSize);

    return (outPosition->col >= 0 && outPosition->col < grid->cols && outPosition->row >= 0 && outPosition->row < grid->rows);
  }

  GridPosition pos;
  pos.col = (int)(mousePosition.x / grid->cellSize);
  pos.row = (int)(mousePosition.y / grid->cellSize);

  outVector->x = mousePosition.x;
  outVector->y = mousePosition.y;
  outPosition->col = pos.col;
  outPosition->row = pos.row;

  return (outPosition->col >= 0 && outPosition->col < grid->cols && outPosition->row >= 0 && outPosition->row < grid->rows);
}

void LoadWalls(Grid *grid, const GridPosition wallData[], int wallCount)
{
  for (int i = 0; i < wallCount; ++i)
  {
    GridPosition pos = wallData[i];
    if (pos.row >= 0 && pos.row < grid->rows && pos.col >= 0 && pos.col < grid->cols)
    {
      grid->cell[pos.row][pos.col].solid = true;
    }
  }
}