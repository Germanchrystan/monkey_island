#pragma once
#include "game.h"

typedef struct DialogOption 
{
  char* text;
  char* goto_id;
  struct DialogOption* next;
} DialogOption;

typedef struct DialogNode
{
  char* id;
  char* text;
  EventDataList* eventDataList;
  DialogOption* options;
  struct DialogNode* next;
} DialogNode;

typedef struct TalkTree
{
  DialogNode* head;
  DialogNode* currentNode;
  const char *currentNodeId;
} TalkTree;

void initTalkTree(Game *game, int dialogId);
void setTalkTreeCurrentNode(Game *game, const char *nodeId);
void freeTalkTree(TalkTree *talkTree);

// Cache
typedef struct DialogCacheEntry DialogCacheEntry;
typedef struct DialogCache DialogCache;

void initDialogCache();
void resetTalkTree();
void clearDialogCache();