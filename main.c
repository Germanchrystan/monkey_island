#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <string.h>

#include "cell.h"
#include "constants.h"
#include "grid.h"
#include "game.h"
#include "macos_presentation.h"

typedef enum 
{
  DRAW_MODE_DEFAULT,
  DRAW_MODE_GRID,
} DRAW_MODE;

char *getLevelName(int args, char *argv[])
{
  char levelName[20] = "test";
  if (args > 1)
  {
    int result = snprintf(levelName, sizeof(levelName), "%s", argv[1]);
    if (result < 0 || result >= sizeof(levelName))
    {
      fprintf(stderr, "Failed to format level name\n");
      return NULL;
    }
  }

  char levelFileLocation[30];
  snprintf(levelFileLocation, sizeof(levelFileLocation), "levels/%s.dat", levelName);

  return strdup(levelFileLocation);
}

int main(int args, char *argv[])
{
  char *levelFileLocation = getLevelName(args, argv);
  if (levelFileLocation == NULL)
  {
    fprintf(stderr, "Failed to get level name\n");
    return 1;
  }
  Game *game = malloc(sizeof(Game));
  if (game == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for game\n");
    return 1;
  }
  
  SetConfigFlags(FLAG_WINDOW_UNDECORATED);
  InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Pathfinder");
  SetWindowPosition(0, 0);
  SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
  enableMacOSKioskPresentation(); // MACOS
  SetTargetFPS(60);

  initGame(game, levelFileLocation);

  float dt;
  int drawMode = DRAW_MODE_DEFAULT;
  while (!WindowShouldClose())
  {
    // Update
    dt = GetFrameTime();
    if (IsKeyPressed(KEY_M))
    {
      drawMode = (drawMode == DRAW_MODE_DEFAULT) ? DRAW_MODE_GRID : DRAW_MODE_DEFAULT;
    }
    updateGame(game, dt);
    // Draw
    BeginDrawing();
    ClearBackground(BLACK);
    drawGame(game, drawMode);
    EndDrawing();
  }
  freeGame(game);
  free(game);
  free(levelFileLocation);
  disableMacOSKioskPresentation(); // MACOS
  CloseWindow();
  return 0;
}