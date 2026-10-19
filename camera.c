#include "camera.h"

void initCamera(Game *game)
{
  Camera2D camera = {0};
  camera.offset = (Vector2){GetMonitorWidth(0) / 2.0f, GetMonitorHeight(0) / 2.0f};
  camera.target = (Vector2){game->level.player.x * game->level.screens[game->level.currentScreen].grid.cellSize, game->level.player.y * game->level.screens[game->level.currentScreen].grid.cellSize};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
  game->camera = camera;
}

void updateCamera(Game *game)
{
  Camera2D *camera = &game->camera;
  camera->offset = (Vector2){GetMonitorWidth(0) / 2.0f, GetMonitorHeight(0) / 2.0f};
  camera->target = (Vector2){game->level.player.x * game->level.screens[game->level.currentScreen].grid.cellSize, game->level.player.y * game->level.screens[game->level.currentScreen].grid.cellSize};
  clampCameraToGrid(game);
}

static float clampf(float value, float min, float max) 
{
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

void clampCameraToGrid(Game *game)
{
  Camera2D *camera = &game->camera;
  Screen *currentScreen = &game->level.screens[game->level.currentScreen];
  float gridWidth = currentScreen->grid.cols * currentScreen->grid.cellSize;
  float gridHeight = currentScreen->grid.rows * currentScreen->grid.cellSize;
  
  float halfScreenWidth = GetMonitorWidth(0) / (2.0f * camera->zoom); // Cam zoom should always be 1
  float halfScreenHeight = GetMonitorHeight(0) / (2.0f * camera->zoom); // Cam zoom should always be 1
  
  float minX = halfScreenWidth;
  float maxX = gridWidth - halfScreenWidth;
  float minY = halfScreenHeight;
  float maxY = gridHeight - halfScreenHeight;

  camera->target.x = clampf(camera->target.x, minX, maxX);
  camera->target.y = clampf(camera->target.y, minY, maxY);

}