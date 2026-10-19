#pragma once

#include <raylib.h>

typedef struct frame
{
  Rectangle source;
  float duration;
} Frame;

typedef struct 
{
  Frame *frames;
  int frameCount;
} Animation;

typedef struct animator
{
  float timer;
  Texture2D *texture;
  int currentFrame;
  int currentAnimation;
  Animation *animations;
  int animationCount;
} Animator;

Animator *newAnimator(const char *texturePath);
void AddAnimation(Animator *animator, Animation animation);
void Update(Animator *animator, float deltaTime, int animationIndex);
void Draw(Animator *animator, Rectangle destination, bool flipX);