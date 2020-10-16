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
#include "../recipe/recipestore.h"
int ItemId;
struct LIPA_cacheItem {
	// id is ctxtTableItem id
	int64_t id;

	struct contextItem* tableItemPtr;

	// k is fingerprint and value is container id
	// kvpairs records every chunk in segment its fingerprint and container id
	// each cache item means a segment
	// when feature has no corresponding container id , it will be TEMPORARY_ID otherwise it will be the container id
	GHashTable* kvpairs;

	// hit_score records the segment hit counts
	int hit_score;

	//flag means it is champion or followers
	int flag;
};

void feedback(struct LIPA_cacheItem* cacheItem, char* feature);
int lipa_cache_check_id(struct LIPA_cacheItem* cache, int*id);
struct LIPA_cacheItem* new_lipa_cache_item(struct contextItem* ctxtTableItem, struct segmentRecipe* sr);
void free_lipa_cache(struct LIPA_cacheItem* cache);
int lookup_fingerprint_in_lipa_cache(struct LIPA_cacheItem* cacheItem, fingerprint* fp);
#endif