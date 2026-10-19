#include "cell.h"
#include <stdio.h>

CellList* CreateCellList(int capacity, Cell *startingCell)
{
  if (capacity <= 0) capacity = 10;
  
  CellList *list = (CellList *)malloc(sizeof(CellList));
  list->size = 0;
  list->capacity = capacity;
  list->cell = (Cell **)malloc(sizeof(Cell *) * list->capacity);
  if (startingCell != NULL) {
    list->cell[list->size++] = startingCell;
  }
  return list;
}

void sortList(CellList *list)
{
  for (int i = 0; i < list->size - 1; ++i)
  {
    for (int j = 0; j < list->size - i - 1; ++j)
    {
      if (list->cell[j]->F > list->cell[j + 1]->F)
      {
        Cell* temp = list->cell[j];
        list->cell[j] = list->cell[j + 1];
        list->cell[j + 1] = temp;
      }
    }
  }
}

void sortListByH(CellList *list)
{
  for (int i = 0; i < list->size - 1; ++i)
  {
    for (int j = 0; j < list->size - i - 1; ++j)
    {
      if (list->cell[j]->H > list->cell[j + 1]->H)
      {
        Cell* temp = list->cell[j];
        list->cell[j] = list->cell[j + 1];
        list->cell[j + 1] = temp;
      }
    }
  }
}

CellList* AddCellToList(CellList *list, Cell *cell)
{
  if (list->size >= list->capacity)
  {
    list->capacity *= 2;
    list->cell = (Cell **)realloc(list->cell, sizeof(Cell *) * list->capacity);
  }
  list->cell[list->size++] = cell;
 
  return list;
}

void RemoveCellFromList(CellList *list, Cell *cell)
{
  for (int i = 0; i < list->size; ++i)
  {
    if (list->cell[i]->col == cell->col && list->cell[i]->row == cell->row)
    {
      for (int j = i; j < list->size - 1; ++j)
      {
        list->cell[j] = list->cell[j + 1];
      }
      --list->size;
      break;
    }
  }
}

int IsCellInList(CellList *list, Cell *cell)
{
  for (int i = 0; i < list->size; ++i)
  {
    if (list->cell[i]->col == cell->col && list->cell[i]->row == cell->row)
    {
      return 1;
    }
  }
  return 0;
}

int getCellDistance(Cell *a, Cell *b)
{
  return abs(a->col - b->col) + abs(a->row - b->row);
}

void FreeCellList(CellList *list)
{
  if (list == NULL) return;
  free(list->cell);
  free(list);
}

CellList *reconstructPath(Cell *current)
{
  CellList *path = CreateCellList(10, NULL);
  while (current != NULL)
  {
    AddCellToList(path, current);
    current = current->parent;
  }
  // Reverse the path
  for (int i = 0; i < path->size / 2; ++i)
  {
    Cell *temp = path->cell[i];
    path->cell[i] = path->cell[path->size - i - 1];
    path->cell[path->size - i - 1] = temp;
  }
  return path;
}
