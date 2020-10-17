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

void feedback(struct segmentRecipe* sr, char* feature);
// int lipa_cache_check_id(struct LIPA_cacheItem* cache, int*id);
// struct LIPA_cacheItem* new_lipa_cache_item(struct contextItem* ctxtTableItem, struct segmentRecipe* sr);
// void free_lipa_cache(struct LIPA_cacheItem* cache);
// int lookup_fingerprint_in_lipa_cache(struct LIPA_cacheItem* cacheItem, fingerprint* fp);
#endif