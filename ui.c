#include "ui.h"
#include <string.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lib/raygui.h"
#include "talk_tree.h"

void drawInventory(Inventory *inventory, InventoryUI *ui)
{
  DrawRectangleLinesEx(ui->rect, 2, GRAY);
  int startIndex = ui->position;
  int endIndex = ui->position + ui->pageSize;
  if (endIndex > inventory->count)
  {
    endIndex = inventory->count;
  }
  const int itemHeight = ui->rect.height / ui->pageSize; // TODO: check this later
  for (int i = startIndex; i < endIndex; i++)
  {
    InventoryItem *item = inventory->items[i];
    DrawText(item->name, ui->rect.x + 10, ui->rect.y + (i - startIndex) * itemHeight + 10, 20, BLACK);
  }
}

bool updateTextBox(TextBoxUI *textBox, float dt)
{
  if (!textBox->active)
    return true;

  textBox->timer += dt;
  if (textBox->timer >= TEXT_BOX_DISPLAY_TIME)
  {
    textBox->active = false;
    textBox->timer = 0;
    free(textBox->text);
    textBox->text = NULL;
    return true;
  }

  return false;
}

void drawTextBox(TextBoxUI *textBox)
{
  if (!textBox->active)
    return;

  DrawRectangle(10, GetScreenHeight() / 2, GetScreenWidth() - 20, 50, (Color){0, 0, 0, 200});
  DrawText(textBox->text, 200, GetScreenHeight() / 2 + 15, 20, WHITE);
}

void addTextToTextBox(TextBoxUI *textBox, const char *text)
{
  if (textBox->active)
  {
    free(textBox->text);
  }
  textBox->text = strdup(text);
  textBox->active = true;
  textBox->timer = 0;
}

void clearTextBoxUI(TextBoxUI *textBox)
{
  textBox->active = false;
  textBox->timer = 0;
  free(textBox->text);
  textBox->text = NULL;
}

void clearUI(UI *ui)
{
  clearTextBoxUI(&ui->textBoxUI);
}

void updateBlackScreen(BlackScreen *blackScreen, float dt)
{
  blackScreen->rect.x = 0;
  blackScreen->rect.y = 0;
  blackScreen->rect.width = GetScreenWidth();
  blackScreen->rect.height = GetScreenHeight();
  blackScreen->transparency += dt;
}

void drawBlackScreen(BlackScreen *blackScreen)
{
  float t = blackScreen->transparency;
  if (t < 0.0f)
    t = 0.0f;
  if (t > 1.0f)
    t = 1.0f;

  unsigned char alpha = (unsigned char)(t * 255.0f);
  DrawRectangleRec(blackScreen->rect, (Color){0, 0, 0, alpha});
}

void updateTalkTreeUI(Game *game, float dt)
{
  (void)dt;
  TalkTreeUI *talkTreeUI = &game->ui.talkTreeUI;
  DialogOption* options = game->currentTalkTree->currentNode->options;

  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && options == NULL)
  {
    changeGameState(game, PLAYING);
    talkTreeUI->active = false;
    game->currentTalkTree = NULL;
  }
}

void drawTalkTreeUI(Game *game)
{
  TalkTree *talkTree= game->currentTalkTree;
  TalkTreeUI *talkTreeUI = &game->ui.talkTreeUI;
  if (talkTree == NULL || talkTree->head == NULL)
    return;
  
  if (!talkTreeUI->active)
    return;
  
  
  DrawRectangle(talkTreeUI->rect.x, talkTreeUI->rect.y, talkTreeUI->rect.width, talkTreeUI->rect.height, (Color){0, 0, 0, 100});
  
  Vector2 textSize = MeasureTextEx(GetFontDefault(), talkTree->currentNode->text, 20.0f, 2.0f);
  if (talkTree->currentNode != NULL)
  {
    DrawText(talkTree->currentNode->text, (GetScreenWidth() / 2) - (int)(textSize.x / 2), (GetScreenHeight() / 2) - (int)(textSize.y / 2), 20, WHITE);
  }

  int OptionYStart = (GetScreenHeight() / 2) - (int)(textSize.y / 2) + textSize.y + 20;
  int OptionXStart = (GetScreenWidth() / 2) - 100; // Center the buttons horizontally, assuming button width is 200
  for (DialogOption *option = talkTree->currentNode->options; option != NULL; option = option->next)
  {
    if (GuiButton((Rectangle){OptionXStart, OptionYStart, 200, 30}, option->text))
    {
      if (option->goto_id == NULL)
      {
        printf("Error: Option '%s' has no goto_id.\n", option->text);
        continue;
      }
      else {
        setTalkTreeCurrentNode(game, option->goto_id);
      }
    }
    OptionYStart += 40; // Adjust the spacing between buttons
  }
}

static void initInventoryUI(InventoryUI *ui, Rectangle rect, int pageSize)
{
  ui->rect = rect;
  ui->pageSize = pageSize;
  ui->position = 0;
}

static void initTalkTreeUI(TalkTreeUI *talkTreeUI)
{
  talkTreeUI->active = false;
  talkTreeUI->rect = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};

  GuiSetStyle(BUTTON, BORDER_WIDTH, 0);
  GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(MAROON));
  GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
  GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(WHITE));
  GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(WHITE));
  GuiSetStyle(BUTTON, TEXT_COLOR_DISABLED, ColorToInt(WHITE));
}

void initUI(UI *ui)
{
  initInventoryUI(&ui->inventoryUI, (Rectangle){10, 10, 200, 300}, 5);
  initTalkTreeUI(&ui->talkTreeUI);
  ui->blackScreen.rect = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};
  ui->blackScreen.transparency = 0;
}