#pragma once

#include <stdlib.h>
#include <stdbool.h>

typedef struct cell
{
  int col;
  int row;
  int G; // Distancia del player
  int H; // Distancia del target
  int F; // G + H
  bool solid;
  struct cell *parent;
} Cell;

typedef struct cellList
{
  Cell **cell;
  int size;
  int capacity;
} CellList;

CellList* CreateCellList(int capacity, Cell *startingCell);
void sortList(CellList *list);
void sortListByH(CellList *list);
CellList* AddCellToList(CellList *list, Cell *cell);
void RemoveCellFromList(CellList *list, Cell *cell);
int IsCellInList(CellList *list, Cell *cell);
void turnCellData(Cell *cell, int newData);
int getCellDistance(Cell *a, Cell *b);
void FreeCellList(CellList *list);
CellList *reconstructPath(Cell *current);