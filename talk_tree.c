#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "talk_tree.h"
#include "utils/dialog_parser.h"

static const int MAX_DIALOG_ID = 99999;
static const double CACHE_DECAY_LAMBDA = 1.0 / 300.0;
static const int MAX_DIALOG_CACHE_SIZE = 10;
static const char *DIALOG_FILE_FORMAT = "dialogs/dialog_%d.xml";

static DialogNode *getDialogFromCache(int dialogId);
static void addDialogToCache(int dialogId, DialogNode *dialog);
static time_t getCurrentTime(void);
static float calculateCacheEntryScore(DialogCacheEntry *entry);
static void evictOldestDialogFromCache(void);
static void freeDialogNode(DialogNode *dialog);
static DialogNode* findDialogNodeById(DialogNode *head, const char* id);

static DialogNode* getDialogById(int id)
{
	if (id < 0 || id > MAX_DIALOG_ID)
	{
		fprintf(stderr, "Invalid dialog ID %d.\n", id);
		return NULL; 
	}

	// Get from cache
	DialogNode *cachedDialog = getDialogFromCache(id);
	if (cachedDialog != NULL)
	{
		return cachedDialog;
	}

	int requiredFilenameLength = snprintf(NULL, 0, DIALOG_FILE_FORMAT, id);
	if (requiredFilenameLength < 0)
	{
		fprintf(stderr, "Failed to calculate filename length.\n");
		return NULL;
	}

	char *filename = (char *)malloc(requiredFilenameLength + 1);
	if (filename == NULL)	
	{
		fprintf(stderr, "Failed to allocate memory for filename.\n");
		return NULL;
	}

	snprintf(filename, requiredFilenameLength + 1, DIALOG_FILE_FORMAT, id);
	DialogNode* dialog = parse_dialog_tree(filename);
	if (dialog == NULL)
	{
		fprintf(stderr, "Failed to load dialog tree from file: %s\n", filename);
		free(filename);
		return NULL;
	}
	free(filename);

	addDialogToCache(id, dialog);
	return dialog;
}

void initTalkTree(Game *game, int dialogId)
{
	if (game->currentTalkTree != NULL)
	{
		game->currentTalkTree = NULL;
	}
	DialogNode *dialog = getDialogById(dialogId);
	if (dialog == NULL)
	{
		fprintf(stderr, "Failed to initialize dialog tree for dialog ID %d\n", dialogId);
		return;
	}
	const char *startNodeId = dialog->id; // Default to first node in dialog
	getDialogRuntimeChangeState(&game->context, dialogId, &startNodeId);
	if (startNodeId == NULL) startNodeId = dialog->id;

	TalkTree *talkTree = (TalkTree *)malloc(sizeof(TalkTree));
	if (talkTree == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for TalkTree\n");
		return;
	}
	
	talkTree->head = dialog;
	talkTree->currentNodeId = startNodeId;
	talkTree->currentNode = findDialogNodeById(dialog, talkTree->currentNodeId);

	game->currentTalkTree = talkTree;
	game->ui.talkTreeUI.active = true;
}

static DialogNode* findDialogNodeById(DialogNode *head, const char* id)
{
	if (id == NULL)
	{
		return head;
	}
	DialogNode *current = head;
	while (current != NULL)
	{
		if (strcmp(current->id, id) == 0)
			return current;
		current = current->next;
	}
	return NULL;
}

static void freeDialogOption(DialogOption *option)
{
	while (option != NULL)
	{
		DialogOption *nextOption = option->next;
		free(option->text);
		free(option->goto_id);
		free(option);
		option = nextOption;
	}
}

static void freeDialogNode(DialogNode *dialog)
{
	while (dialog != NULL)
	{
		DialogNode *nextNode = dialog->next;
		free(dialog->id);
		free(dialog->text);
		if (dialog->eventDataList != NULL)
		{
			for (int i = 0; i < dialog->eventDataList->eventDataQ; i++)
			{
				EventData *eventData = &dialog->eventDataList->eventData[i];
				if (eventData->payloadType == EVENT_PAYLOAD_SET_DIALOG_RUNTIME_START)
				{
					free(eventData->payload.dialogRuntimeStart.startNode);
				}
			}
			free(dialog->eventDataList->eventData);
			free(dialog->eventDataList);
		}
		freeDialogOption(dialog->options);
		free(dialog);
		dialog = nextNode;
	}
}
 
void setTalkTreeCurrentNode(Game *game, const char *nodeId)
{
	TalkTree *talkTree = game->currentTalkTree;
	if (talkTree == NULL || talkTree->head == NULL)
		return;

	DialogNode *node = findDialogNodeById(talkTree->head, nodeId);
	if (node != NULL)
	{
		talkTree->currentNode = node;
		talkTree->currentNodeId = nodeId;
		
		if (node->eventDataList != NULL)
		{
			loadEventQueue(game, node->eventDataList);
		}
	}
	else
	{
		printf("Error: Dialog node with ID '%s' not found.\n", nodeId);
	}
}

void freeTalkTree(TalkTree *talkTree)
{
	if (talkTree == NULL) return;

	freeDialogNode(talkTree->head);
	free(talkTree);
}

// Cache implementation
// -----------------------------------------
typedef struct DialogCacheEntry
{
	int dialogId;
	DialogNode *dialog;
	int accessCount;
 	time_t lastAccessTime;
} DialogCacheEntry;

typedef struct DialogCache
{
	int size;
	DialogCacheEntry entries[MAX_DIALOG_CACHE_SIZE];
} DialogCache;

static DialogCache cache = {0};

void initDialogCache()
{
	cache.size = 0;
	for (int i = 0; i < MAX_DIALOG_CACHE_SIZE; i++)
	{
		cache.entries[i].dialogId = -1;
		cache.entries[i].dialog = NULL;
		cache.entries[i].accessCount = 0;
		cache.entries[i].lastAccessTime = 0;
	}
}

void clearDialogCache()
{
	for (int i = 0; i < cache.size; i++)
	{
		freeDialogNode(cache.entries[i].dialog);
		cache.entries[i].dialog = NULL;
	}
	cache.size = 0;
}

static time_t getCurrentTime(void) { return time(NULL); }

static float calculateCacheEntryScore(DialogCacheEntry *entry)
{
	time_t currentTime = getCurrentTime();
	double age = difftime(currentTime, entry->lastAccessTime);
	return entry->accessCount / (1.0f + age * CACHE_DECAY_LAMBDA);
}

static void evictOldestDialogFromCache()
{
	if (cache.size == 0) return;
	float lowestScore = calculateCacheEntryScore(&cache.entries[0]);
	int oldestIndex = 0;
	for (int i = 1; i < cache.size; i++)
	{
		float score = calculateCacheEntryScore(&cache.entries[i]);
		if (score < lowestScore)
		{
			lowestScore = score;
			oldestIndex = i;
		}
	}
	freeDialogNode(cache.entries[oldestIndex].dialog);
	for (int i = oldestIndex; i < cache.size - 1; i++)
	{
		cache.entries[i] = cache.entries[i + 1];
	}
	cache.size--;
}

static void addDialogToCache(int dialogId, DialogNode *dialog)
{
	if (cache.size >= MAX_DIALOG_CACHE_SIZE)
	{
		evictOldestDialogFromCache();
	}
	cache.entries[cache.size].dialogId = dialogId;
	cache.entries[cache.size].dialog = dialog;
	cache.entries[cache.size].accessCount = 1;
	cache.entries[cache.size].lastAccessTime = getCurrentTime();
	cache.size++;
}

static DialogNode* getDialogFromCache(int dialogId)
{
	for (int i = 0; i < cache.size; i++)
	{
		if (cache.entries[i].dialogId == dialogId)
		{
			cache.entries[i].accessCount++;
			cache.entries[i].lastAccessTime = getCurrentTime();
			return cache.entries[i].dialog;
		}
	}

	return NULL;
}
