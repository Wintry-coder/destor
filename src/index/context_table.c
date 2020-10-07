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
	GList* contextList = g_hash_table_lookup(context_table, key);
	return contextList;
}

int context_table_find(char* key) {
	gboolean hit = g_hash_table_contains(context_table, key);
	return hit ? 1 : 0;
}

void context_table_update(char* key, GList* contextList) {
	g_hash_table_replace(context_table, key, contextList);
}

/*
 * contextList: the list in context table
 * minitem: if true, find the min item; otherwise, find the max item;
 * return the max/min item
 */

struct contextItem *find_item(GList *contextList, bool minitem) {
    struct contextItem* item = contextList -> data;
    struct contextItem* current = item;

    contextList = g_list_next(contextList);

    while (contextList){
        current = contextList -> data;
        if(minitem){
            if (item -> score > currItem -> score) {
                item = current;
            }
        }else{
            if (item -> score < currItem -> score) {
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
    context_table = g_hash_table_new_full(g_feature_hash, g_feature_equal, free, NULL);
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
	newItem ->followers = 4;
	newItem ->score = 0;
    newItem ->id = segment ->id;
    newItem ->updatetime = 0;
	return newItem;
}

void free_contextItem(struct contextItem* item) {
	printf("free context table item\n");
    free(item);
}