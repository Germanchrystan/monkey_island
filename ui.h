#pragma once
#include <raylib.h>
#include <stdbool.h>
#include "inventory.h"

#define TEXT_BOX_MAX_TEXT_LENGTH 64
#define TEXT_BOX_DISPLAY_TIME 2.0f

typedef struct TalkTree TalkTree;
typedef struct Game Game;

typedef struct 
{
  Rectangle rect;
  int pageSize;
  int position;
} InventoryUI;

typedef struct 
{
  char text[TEXT_BOX_MAX_TEXT_LENGTH];
} TextBoxMessage;

typedef struct TextBoxUI
{
  bool active;
  char *text;
  float timer;
  TextBoxMessage message;
} TextBoxUI;

typedef struct 
{
  Rectangle rect;
  float transparency;
} BlackScreen;

typedef struct TalkTreeUI
{
  bool active;
  Rectangle rect;
} TalkTreeUI;

typedef struct UI
{
  InventoryUI inventoryUI;
  TextBoxUI textBoxUI;
  TalkTreeUI talkTreeUI;
  BlackScreen blackScreen;
} UI;

void drawInventory(Inventory *inventory, InventoryUI *ui);

bool updateTextBox(TextBoxUI *textBox, float dt);
void drawTextBox(TextBoxUI *textBox);
void addTextToTextBox(TextBoxUI *textBox, const char *text);

void updateBlackScreen(BlackScreen *blackScreen, float dt);
void drawBlackScreen(BlackScreen *blackScreen);

void updateTalkTreeUI(Game *game, float dt);
void drawTalkTreeUI(Game *game);

void clearTextBoxUI(TextBoxUI *textBox);
void clearUI(UI *ui);

void initUI(UI *ui);