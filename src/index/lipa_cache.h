/*
 * lipa_cache.h
 *
 *  Created on: Oct 5, 2020
 *      Author: Yanweijie
 */
#ifndef LIPA_CACHE_H
#define LIPA_CACHE_H
#include "../destor.h"
#include "context_table.h"
#include "recipestore.h"
void feedback(struct LIPA_cacheItem* cacheItem, char* feature);

struct lipaCache {
	segmentid id;;
	int hit;
    bool flag // champion or follower
};
struct lipaCache* new_lipa_cache(struct segmentRecipe* sr);

void free_lipa_cache(struct lipaCache* cache);

int lookup_fingerprint_in_lipa_cache(struct lipaCache* cacheItem, fingerprint* fp);

#endif