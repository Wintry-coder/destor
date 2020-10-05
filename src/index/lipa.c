/*
 * lipa.c
 *
 *  Created on: Oct 5, 2020
 *      Author: Yanweijie
 */

#include "index_buffer.h"
#include "kvstore.h"
#include "fingerprint_cache.h"
#include "context_table.h"
#include "../recipe/recipestore.h"
#include "../storage/containerstore.h"
#include "../jcr.h"

extern struct index_overhead index_overhead;

extern struct index_buffer index_buffer;

extern struct contextItem *(*champion_choose)(GList *contextList);

void index_lookup_lipa(struct segment *s) {
    assert(s->features);
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, s->features);

    struct contextItem* newItem = new_contextItem(s);
    struct contextItem* champion;

    while(g_hash_table_iter_next(&iter, &key, &value)) {
        GList* contextList = NULL;
        if (context_find((fingerprint *) key)) {
            contextList = context_lookup((fingerprint *) key);
            int list_length = g_list_length(contextList);
            if (list_length >= CONTEXT_TABLE_LENGTH) {
                struct contextItem *item = NULL;
                if(destor.lipa_update_method == LIPA_MIN){
                    item = find_item(contextList, 1);
                }else{
                    item = contextList -> data;     
                }
                contextList = g_list_remove(contextList, item);
                free_contextItem(item);
            }
        }
        contextList = g_list_append(contextList, newItem);
        context_update((fingerprint *) key, contextList);
        champion = champion_choose(contextList);

        //prefetch champion and followers fingerprint into cache
        fp_prefetch(contextList, champion, (char*) key);
    }
}