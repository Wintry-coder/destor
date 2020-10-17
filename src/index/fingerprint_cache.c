/*
 * fingerprint_cache.c
 *
 *  Created on: Mar 24, 2014
 *      Author: fumin
 */
#include "../destor.h"
#include "index.h"
#include "../storage/containerstore.h"
#include "../recipe/recipestore.h"
#include "../utils/lru_cache.h"
#include "context_table.h"
#include "lipa_cache.h"

static struct lruCache* lru_queue;

/* defined in index.c */
extern struct {
	/* Requests to the key-value store */
	int lookup_requests;
	int update_requests;
	int lookup_requests_for_unique;
	/* Overheads of prefetching module */
	int read_prefetching_units;
}index_overhead;

void init_fingerprint_cache(){
	switch(destor.index_category[1]){
	case INDEX_CATEGORY_PHYSICAL_LOCALITY:
		lru_queue = new_lru_cache(destor.index_cache_size,
				free_container_meta, lookup_fingerprint_in_container_meta);
		break;
	case INDEX_CATEGORY_LOGICAL_LOCALITY:
	    // if (destor.index_specific == INDEX_SPECIFIC_LIPA) {
	    // 	lru_queue = new_lru_cache(destor.index_cache_size,
	    // 			free_lipa_cache, lookup_fingerprint_in_lipa_cache);
		// 	break;
		// }
		lru_queue = new_lru_cache(destor.index_cache_size,
				free_segment_recipe, lookup_fingerprint_in_segment_recipe);
		break;
	default:
		WARNING("Invalid index category!");
		exit(1);
	}
}

int64_t fingerprint_cache_lookup(fingerprint *fp){
	switch(destor.index_category[1]){
		case INDEX_CATEGORY_PHYSICAL_LOCALITY:{
			struct containerMeta* cm = lru_cache_lookup(lru_queue, fp);
			if (cm)
				return cm->id;
			break;
		}
		case INDEX_CATEGORY_LOGICAL_LOCALITY:{
			// if (destor.index_specific == INDEX_SPECIFIC_LIPA) {
            //     struct LIPA_cacheItem* cacheItem = lru_cache_lookup(lru_queue, fp);
            //     if (cacheItem) {
            //         //update cache item hit
            //         int64_t id = g_hash_table_lookup(cacheItem->kvpairs, fp);
            //         if (id > TEMPORARY_ID) {
            //             cacheItem->hit_score++;
            //         }
            //         return id;
            //     }
			// 	break;
			// }


			struct segmentRecipe* sr = lru_cache_lookup(lru_queue, fp);
			if(sr){
				struct chunkPointer* cp = g_hash_table_lookup(sr->kvpairs, fp);
				if(cp->id <= TEMPORARY_ID){
					WARNING("expect > TEMPORARY_ID, but being %lld", cp->id);
					assert(cp->id > TEMPORARY_ID);
				}
				sr->hit++;
				return cp->id;
			}
			break;
		}
	}

	return TEMPORARY_ID;
}

void fingerprint_cache_prefetch(int64_t id){
	switch(destor.index_category[1]){
		case INDEX_CATEGORY_PHYSICAL_LOCALITY:{
			struct containerMeta * cm = retrieve_container_meta_by_id(id);
			index_overhead.read_prefetching_units++;
			if (cm) {
				lru_cache_insert(lru_queue, cm, NULL, NULL);
			} else{
				WARNING("Error! The container %lld has not been written!", id);
				exit(1);
			}
			break;
		}
		case INDEX_CATEGORY_LOGICAL_LOCALITY:{
			if (!lru_cache_hits(lru_queue, &id,
					segment_recipe_check_id)){
				/*
				 * If the segment we need is already in cache,
				 * we do not need to read it.
				 */
				GQueue* segments = prefetch_segments(id,
						destor.index_segment_prefech);
				index_overhead.read_prefetching_units++;
				VERBOSE("Dedup phase: prefetch %d segments into %d cache",
						g_queue_get_length(segments),
						destor.index_cache_size);
				struct segmentRecipe* sr;
				while ((sr = g_queue_pop_tail(segments))) {
					/* From tail to head */
					if (!lru_cache_hits(lru_queue, &sr->id,
							segment_recipe_check_id)) {
						lru_cache_insert(lru_queue, sr, NULL, NULL);
					} else {
						/* Already in cache */
						free_segment_recipe(sr);
					}
				}
				g_queue_free(segments);
			}
			break;
		}
	}
}

void fingerprint_lipa_prefetch(GList *contextList, struct contextItem *champion, char* feature) {
    assert(champion);
	int prefetchnum = champion ->followers + 1;
	GList *iter = g_list_find(contextList, champion);
	if(champion->id != TEMPORARY_ID)
	{
		GQueue* segments = prefetch_segments(champion->id, prefetchnum);
		index_overhead.read_prefetching_units++;
		VERBOSE("Dedup phase: prefetch %d segments into %d lipa cache",
				g_queue_get_length(segments),
				destor.index_cache_size);	
			
		struct segmentRecipe* sr;
		while ((sr = g_queue_pop_tail(segments))) {
		/* From tail to head */
			if (!lru_cache_hits(lru_queue, &sr->id,
			segment_recipe_check_id)) {
				lru_cache_insert(lru_queue, sr, feedback, feature);
			} else
			/* Already in cache */
				free_segment_recipe(sr);
		}
		g_queue_free(segments);		
	}
    	
	// int64_t* ids = kvstore_lookup(feature);
	// if (ids) {
	// 	index_overhead.lookup_requests++;
	// 	if (ids[0] != TEMPORARY_ID)
	// 	{
    // 		GQueue* segmentRecipes = prefetch_segments(ids[0], 1);
    // 		struct segmentRecipe* sr = g_queue_pop_head(segmentRecipes);
	// 		int counter;
    // 		for (counter = 0; counter <= prefetchnum && iter != NULL; iter = g_list_next(iter), counter++) {
    //     		//LIPA_fingerprint_cache_prefetch(champion->id, feature);
	// 			struct contextItem *item = iter->data;
    // 			if (!lru_cache_hits(lru_queue, &item->id, lipa_cache_check_id)) {
    //     			index_overhead.read_prefetching_units++;
    //     			struct LIPA_cacheItem* cacheItem = new_lipa_cache_item(item, sr);
    //     			assert(cacheItem);
    //     			lru_cache_insert(lru_queue, cacheItem, feedback, feature);
    // 			}
    // 		}
	// 		if (sr != NULL)
    //     		free_segment_recipe(sr);
    // 		g_queue_free(segmentRecipes);
	// 	}
	// }else{
	// 		index_overhead.lookup_requests_for_unique++;
	// }


    //struct segmentRecipe* sr = g_queue_pop_head(segmentRecipes);
	// index_overhead.read_prefetching_units++;
	// VERBOSE("Dedup phase: prefetch %d segments into %d lipa cache",
	// 		g_queue_get_length(segments),
	// 		destor.index_cache_size);	
			
	// struct segmentRecipe* sr;
	// int i = 0;
	// while ((sr = g_queue_pop_tail(segments))) {
	// /* From tail to head */
	// 	if (!lru_cache_hits(lru_queue, &sr->id,
	// 		segment_recipe_check_id)) {
	// 		lru_cache_insert(lru_queue, sr, feedback, feature);
	// 	} else {
	// 		/* Already in cache */
	// 		free_segment_recipe(sr);
	// 	}
	// 	sr->champion = champion;
	// 	if(i == 0)
	// 		sr->flag = 1;//the last one
	// 	else
	// 		sr->flag = 0;
	// 	i++;
	// }
	// g_queue_free(segments);
}

void LIPA_cache_update(fingerprint *fp, containerid id) {

	GList* elem =  g_list_first(lru_queue->elem_queue);
	while (elem) {
	    if (lru_queue->hit_elem((struct LIPA_cacheItem*) (elem->data), fp)) {
	    	g_hash_table_replace(((struct LIPA_cacheItem*)(elem->data))->kvpairs,
	    			fp, id);
	    }
	    elem = g_list_next(elem);
	}

}