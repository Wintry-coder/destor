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

void index_lookup_lipa(struct segment *s) {
    assert(s->features);
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, s->features);

    struct contextItem* newItem = new_contextItem(s);
    struct contextItem* champion;

    while(g_hash_table_iter_next(&iter, &key, &value)) {
        
    }
}