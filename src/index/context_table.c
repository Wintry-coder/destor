/*
 * context_table.c
 *
 *  Created on: Oct 3, 2020
 *      Author: Yanweijie
 */

#include "context_table.h"

static GHashTable *context_table;

void (*close_context)();
GList* (*context_lookup)(char *key);
int (*context_find)(char *key);
void (*context_update)(char *key, int64_t id);

void close_context_table() {

}

GList* context_table_lookup(char* key) {
    GList * contextList = g_hash_table_lookup(context_table, key);
	return contextList;
}

int context_table_find(char* key) {
	gboolean hit = g_hash_table_contains(context_table, key);
	return hit ? 1 : 0;
}

void context_table_update(char* key, GList* contextList) {
	g_hash_table_replace(context_table, key, contextList);
}


struct contextItem *find_item(GList *contextList, int op) {
    assert(contextList);
    struct contextItem* item = contextList -> data;
    struct contextItem* current = item;
    if(op == 2 && item->id == TEMPORARY_ID)
        return item;
    contextList = g_list_next(contextList);

    while (contextList){
        current = contextList -> data;
        if(op == 2){
            if (current -> id == TEMPORARY_ID) {
                item = current;
                break;
            }            
        }
        if(op == 1){
            if (item -> score > current -> score) {
                item = current;
            }
        }
        if(op == 0){
            if (item -> score < current -> score) {
                item = current;
            }            
        }
        contextList = g_list_next(contextList);
    }

    return item;
}

/*
 * Mapping a feature to the context list.
 * The length of context list is fixed
 * Each item in the context list include segment id, score and followers
 */
void init_context_table() {
    context_table = g_hash_table_new_full(g_feature_hash, g_feature_equal, NULL, NULL);
    close_context = close_context_table;

    context_lookup = context_table_lookup;
    context_find = context_table_find;
    context_update = context_table_update;
	
    VERBOSE("initial context table");
}

/*
 * New a elem in the context List
 */

struct contextItem* new_contextItem(struct segment* segment){
    assert(segment);
	struct contextItem* newItem = (struct contextItem*) malloc(sizeof(struct contextItem));
	newItem->followers = 4;
	newItem->score = 0;
    newItem->id = TEMPORARY_ID;
    newItem->updatetime = 0;
	return newItem;
}


void free_contextItem(struct contextItem* item) {
	printf("free context table item\n");
    free(item);
}

void LIPA_context_update(struct segment* s)
{
    assert(s->features);
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, s->features);

    while(g_hash_table_iter_next(&iter, &key, &value)) {
        GList* contextList = context_lookup((fingerprint *) key);
        if (contextList) {
            struct contextItem* item = find_item(contextList, 2);
            item -> id = s->id;
        }
    }
}