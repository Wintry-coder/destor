/*
 * lipa_cache.c
 *
 *  Created on: Oct 5, 2020
 *      Author: Yanweijie
 */
#include "lipa_cache.h"

struct lipaCache* new_lipa_cache(struct segmentRecipe* sr)
{
    struct lipaCache* new_cacheItem = (struct lipaCache*)(malloc(sizeof(struct lipaCache)));
    new_cacheItem ->id = sr ->id;
    new_cacheItem ->hit = 0;
    new_cacheItem ->flag = 0;
    return new_cacheItem;
}

void free_lipa_cache(struct lipaCache* cache)
{
    g_hash_table_destroy(cache->kvpairs);
    free(cache);
}

int lookup_fingerprint_in_lipa_cache(struct lipaCache* cacheItem, fingerprint* fp) {
    return g_hash_table_contains(cacheItem->kvpairs, fp) == NULL ? 0 : 1;
}

void feedback(struct segmentRecipe* sr, char* feature) {

    GList* contextList = NULL;
    if (context_find((fingerprint *) feature)) {
    // if the feature doesn't exist in context table, don't feedback
        contextList = context_lookup((fingerprint *) feature);
        segmentid id = sr ->id;
        struct contextItem* elem = contextList -> data;
        
	    while (contextList) {
            current = contextList -> data;
		    if (elem->id == id)
			    break;
		    contextList = g_list_next(contextList);
	    }  
        elem ->updatetime++;
        elem ->score = elem ->score + ((double)(cacheItem->hit_score) - elem -> score) * (1.0 / elem ->update_time);
    }   

}