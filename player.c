#include <math.h>

#include "player.h"
#include "constants.h"
#include "raylib.h"

#define EPSILON 0.01f
#define PLAYER_SPEED 5.0f

static Frame idleFrames[] = {
  {
    .source = (Rectangle){0, 342, 32, 48},
    .duration = 0.2f,
  },
};

static Frame walkFrames[] = {
  {.source = (Rectangle){0, 342, 32, 48}, .duration = 0.1f},
  {.source = (Rectangle){32, 66, 32, 48}, .duration = 0.1f},
  {.source = (Rectangle){64, 66, 28, 48}, .duration = 0.1f},
  {.source = (Rectangle){96, 66, 28, 48}, .duration = 0.1f},
  {.source = (Rectangle){128, 66, 28, 48}, .duration = 0.1f},
  {.source = (Rectangle){150, 66, 28, 48}, .duration = 0.2f},
};

void initPlayer(Player *player, int x, int y)
{
  player->x = (float)x;
  player->y = (float)y;
  player->faceRight = true;
  player->status = 0; // Idle
  player->speed = PLAYER_SPEED; // Cells per second
  player->path = NULL;
  player->animator = newAnimator("assets/guy.png");
  AddAnimation(player->animator, (Animation){.frames = walkFrames, .frameCount = 6});
  AddAnimation(player->animator, (Animation){.frames = idleFrames, .frameCount = 1});
}

void drawPlayer(Player *player, int cellSize, int drawMode)
{
  if (drawMode == 1)
  {
    DrawRectangle(player->x * cellSize + 1,
       (player->y - 2) * cellSize + 1, cellSize - 1, (cellSize * 3) - 2, BLUE);
    DrawCircle(player->x * cellSize + cellSize / 2, player->y * cellSize + cellSize / 2, cellSize / 3, RED);
  }
  if (drawMode == 0)
  {
    Draw(player->animator,
      (Rectangle){player->x * cellSize - cellSize / 2, (player->y - 2) * cellSize, cellSize * 2, cellSize * 3},
      player->faceRight
    );
    // DrawTexturePro(
    //   *player->animator->texture,
    //   (Rectangle){0, 342, 32, 48},
    //   (Rectangle){player->x * cellSize - cellSize / 2, (player->y - 2) * cellSize, cellSize * 2, cellSize * 3},
    //   (Vector2){0, 0},
    //   0.0f,
    //   WHITE
    // );
  }
}

void freePlayerPath(Player *player)
{
  FreeCellList(player->path);
  player->path = NULL;
}

void setPlayerPath(Player *player, CellList *path)
{
  freePlayerPath(player);
  player->path = path;
}

void updatePlayer(Player *player, float deltaTime)
{
  if (player->path == NULL) return;
  if (player->path->size == 0) return;

  Cell *nextCell = player->path->cell[0];
  float step = player->speed * deltaTime;
  float dx = nextCell->col - player->x;
  float dy = nextCell->row - player->y;
  float dist = sqrt(dx * dx + dy * dy);
  float move = fmin(step, dist);

  float px = (dist > 0) ? (dx / dist) * move : 0;
  float py = (dist > 0) ? (dy / dist) * move : 0;
  
  player->x += px;
  player->y += py;

  if (dist <= step + EPSILON)
  {
    player->x = nextCell->col;
    player->y = nextCell->row;
    // Remove the reached cell from the path
    for (int i = 0; i < player->path->size - 1; ++i)
    {
      player->path->cell[i] = player->path->cell[i + 1];
    }
    --player->path->size;
  }

  player->faceRight = (px <= 0);
  Update(player->animator, deltaTime, player->path->size > 0 ? 0 : 1);
}