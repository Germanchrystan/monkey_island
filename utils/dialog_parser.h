#pragma once

typedef struct DialogNode DialogNode;

#ifdef __cplusplus
extern "C" {
#endif
DialogNode* parse_dialog_tree(const char* filename);
char* xstrdup(const char* s);
#ifdef __cplusplus
}
#endif