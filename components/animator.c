#include "animator.h"
#include <stdlib.h>
#include "raylib.h"

Animator *newAnimator(const char *texturePath)
{
  Animator *animator = malloc(sizeof(Animator));
  animator->texture = malloc(sizeof(*animator->texture));
  if (animator->texture != NULL)
  {
    *animator->texture = LoadTexture(texturePath);
  }
  animator->animations = NULL;
  animator->animationCount = 0;
  animator->currentAnimation = -1;
  animator->currentFrame = 0;
  animator->timer = 0.0f;
  return animator;
}

void Update(Animator *animator, float deltaTime, int animationIndex)
{
  if (animator->currentAnimation != animationIndex)
  {
    animator->currentAnimation = animationIndex;
    animator->timer = 0.0f;
    animator->currentFrame = 0;
    return;
  }

  if (animator->currentAnimation < 0 || animator->currentAnimation >= animator->animationCount) return;

  animator->timer += deltaTime;
  if (animator->timer >= animator->animations[animator->currentAnimation].frames[animator->currentFrame].duration)
  {
    animator->timer = 0.0f;
    animator->currentFrame++;
    if (animator->currentFrame >= animator->animations[animator->currentAnimation].frameCount)
    {
      animator->currentFrame = 0;
    }
  }
}

void Draw(Animator *animator, Rectangle destination, bool flipX)
{
  if (!animator->texture || animator->currentAnimation < 0 || animator->currentAnimation >= animator->animationCount) return;
  Rectangle source = animator->animations[animator->currentAnimation].frames[animator->currentFrame].source;
  if (flipX) 
  {
    source.width = -source.width;
  }
  DrawTexturePro(*animator->texture, source, destination, (Vector2){0, 0}, 0.0f, WHITE);
}

void AddAnimation(Animator *animator, Animation animation)
{
  if (animator->animationCount == 0)
  {
    animator->animations = malloc(sizeof(Animation));
  }
  else
  {
    animator->animations = realloc(animator->animations, sizeof(Animation) * (animator->animationCount + 1));
  }
  animator->animations[animator->animationCount] = animation;
  animator->animationCount++;
}