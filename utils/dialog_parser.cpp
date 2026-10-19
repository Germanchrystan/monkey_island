#include <vector>
#include <stdlib.h>

#include "dialog_parser.h"
#include "../lib/tinyxml2.h"
#include "../talk_tree.h"
#include "../event_queue.h"

extern "C"
{
  static char *copyString(const char *s)
  {
    if (!s)
      return NULL;
    size_t len = strlen(s) + 1;
    char *copy = (char *)malloc(len);
    if (copy)
      memcpy(copy, s, len);
    return copy;
  }

  static void checkDebug(EventDataList *eventDataList)
  {
    if (eventDataList == NULL)
    {
      fprintf(stderr, "NULL EVENT DATA LIST\n");
    }
    else
    {
      fprintf(stderr, "LOADED EVENT DATA LIST\n");
    }
  }

  static DialogNode *printDialogNode(DialogNode *node, int depth = 0)
  {
    if (!node)
      return NULL;
    for (int i = 0; i < depth; i++)
      printf("  ");
    printf("Node ID: %s\n", node->id);
    for (int i = 0; i < depth; i++)
      printf("  ");
    printf("Text: %s\n", node->text);

    DialogOption *opt = node->options;
    while (opt)
    {
      for (int i = 0; i < depth + 1; i++)
        printf("  ");
      printf("Option: %s -> %s\n", opt->text, opt->goto_id);
      opt = opt->next;
    }
    return printDialogNode(node->next, depth);
  }

  static void parse_node_events(tinyxml2::XMLElement *eventsElem, DialogNode *dialogNode)
  {
    int length = eventsElem->IntAttribute("length", -1);
    if (length <= 0)
    {
      fprintf(stderr, "Invalid event length: %d\n", length);
      dialogNode->eventDataList = NULL;
      return;
    }

    EventDataList *eventDataList = (EventDataList *)malloc(sizeof(EventDataList));
    if (eventDataList == NULL)
    {
      fprintf(stderr, "Failed to allocate memory for eventDataList.\n");
      return;
    }
    eventDataList->eventDataQ = 0;

    eventDataList->eventData = (EventData *)malloc(sizeof(EventData) * length);
    if (!eventDataList->eventData)
    {
      fprintf(stderr, "Failed to allocate memory for events.\n");
      free(eventDataList);
      return;
    }

    dialogNode->eventDataList = eventDataList;

    for (tinyxml2::XMLElement *eventElem = eventsElem->FirstChildElement("event");
         eventElem;
          eventElem = eventElem->NextSiblingElement("event"))
    {
      int eventType = eventElem->IntAttribute("type");
      if (eventType <= 0)
      {
        fprintf(stderr, "Event type %d not found.\n", eventType);
        continue;
      }

      EventData *event = &eventDataList->eventData[eventDataList->eventDataQ++];
      event->type = static_cast<EventType>(eventType);
      tinyxml2::XMLElement *payloadElem = eventElem->FirstChildElement("payload");
      if (payloadElem == NULL)
      {
        fprintf(stderr, "Event missing payload.\n");
        continue;
      }

      int payloadType = payloadElem->IntAttribute("type", -1);
      if (payloadType < 0)
      {
        fprintf(stderr, "Payload missing type attribute.\n");
        continue;
      }
      if (payloadType != eventType)
      {
        fprintf(stderr, "WARNING: mismatching event type / payload type\n");
      }

      event->payloadType = static_cast<EventPayloadType>(payloadType);
      switch (event->payloadType)
      {
      case EVENT_PAYLOAD_ITEM_PICKUP:
      {
        tinyxml2::XMLElement *itemId = payloadElem->FirstChildElement("itemId");
        if (itemId == NULL)
        {
          fprintf(stdout, "Null Item Id\n");
          break;
        }
        event->payload.itemPickup.itemIndex = atoi(itemId->GetText());
        break;
      }
      case EVENT_PAYLOAD_SET_DIALOG_RUNTIME_START:
      {
        tinyxml2::XMLElement *dialogIdElem = payloadElem->FirstChildElement("dialogId");
        tinyxml2::XMLElement *startNodeElem = payloadElem->FirstChildElement("startNode");
        event->payload.dialogRuntimeStart.dialogId = atoi(dialogIdElem->GetText());
        event->payload.dialogRuntimeStart.startNode = copyString(startNodeElem ? startNodeElem->GetText() : NULL);
        break;
      }
      // Add parsing for other payload types as needed
      default:
      {
        fprintf(stderr, "Unsupported payload type: %d\n", event->payloadType);
        break;
      }
      }
    }
  }

  DialogNode *parse_dialog_tree(const char *filename)
  {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filename) != tinyxml2::XML_SUCCESS)
    {
      fprintf(stderr, "Error loading XML file: %s\n", filename);
      return NULL;
    }
    tinyxml2::XMLElement *root = doc.FirstChildElement("dialogTree");
    if (!root)
      return NULL;

    DialogNode *head = NULL;
    DialogNode *last = NULL;

    for (tinyxml2::XMLElement *xmlNode = root->FirstChildElement("node");
         xmlNode;
         xmlNode = xmlNode->NextSiblingElement("node"))
    {
      DialogNode *dnode = (DialogNode *)calloc(1, sizeof(DialogNode));
      dnode->id = copyString(xmlNode->Attribute("id"));
      tinyxml2::XMLElement *textElem = xmlNode->FirstChildElement("text");
      dnode->text = copyString(textElem ? textElem->GetText() : NULL);

      tinyxml2::XMLElement *eventsNode = xmlNode->FirstChildElement("events");
      if (eventsNode != NULL)
      {
        parse_node_events(eventsNode, dnode);
      }

      DialogOption *opt_head = NULL;
      DialogOption *opt_last = NULL;

      for (tinyxml2::XMLElement *opt = xmlNode->FirstChildElement("option");
           opt;
           opt = opt->NextSiblingElement("option"))
      {
        DialogOption *dopt = (DialogOption *)calloc(1, sizeof(DialogOption));
        dopt->text = copyString(opt->GetText());
        dopt->goto_id = copyString(opt->Attribute("goto"));
        if (!opt_head)
          opt_head = dopt;
        else
          opt_last->next = dopt;
        opt_last = dopt;
      }
      dnode->options = opt_head;

      if (!head)
        head = dnode;
      else
        last->next = dnode;
      last = dnode;
    }

    // printDialogNode(head);
    return head;
  }
}
