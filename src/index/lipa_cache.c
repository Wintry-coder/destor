/*
 * lipa_cache.c
 *
 *  Created on: Oct 5, 2020
 *      Author: Yanweijie
 */
#include "lipa_cache.h"


// void feedback(struct segmentRecipe* sr, char* feature) {

//     GList* contextList = NULL;
//     if (context_find((fingerprint *) feature)) {
//     // if the feature exists in context table, update score
//         contextList = context_lookup((fingerprint *) feature);
//         segmentid id = sr ->id;
//         struct contextItem* elem = contextList -> data;
        
// 	    while (contextList) {
//             elem = contextList -> data;
// 		    if (elem->id == id)
// 			    break;
// 		    contextList = g_list_next(contextList);
// 	    }  
//         elem ->updatetime++;
//         elem ->score = elem ->score + ((double)(sr->hit) - elem ->score) * (1.0 / elem ->updatetime);
//     }   
//     if(destor.prefetch_method == PREFETCH_ADAPTIVE){
//         struct contextItem* champion = sr -> champion;
//         if(sr->flag == 1)//the segment is the last one
//         {
//             /*
//              * The last segment  feedbacks its champion
//              */
//             /*int threshold = champion->meanhit - champion->deviation;
//             if(sr->hit < threshold)
//             {
//                 champion->followers--;
//             }
//             else
//             {
//                 champion->followers++;
//             }*/
//         }
//         if(sr->flag == 0)//update the threshold
//         {
            
//         }
//     }
// }

void feedback(struct LIPA_cacheItem* cacheItem, char* feature) {
    struct ctxtTableItem* target_ctxtTableItem = cacheItem ->tableItemPtr;
    assert(target_ctxtTableItem);
    target_ctxtTableItem ->update_time ++;
    target_ctxtTableItem -> score = target_ctxtTableItem -> score +
            ((double)(cacheItem->hit_score) - target_ctxtTableItem -> score) * (1.0 / target_ctxtTableItem -> update_time);
}

int lipa_cache_check_id(struct LIPA_cacheItem* cache, int*id) {
    return cache ->id == *id;
}
struct LIPA_cacheItem* new_lipa_cache_item(struct contextItem* ctxtTableItem, struct segmentRecipe* sr) {
    struct LIPA_cacheItem* new_cacheItem = (struct LIPA_cacheItem*)(malloc(sizeof(struct LIPA_cacheItem)));

    new_cacheItem ->id = ctxtTableItem ->id;
    new_cacheItem ->hit_score = 0;
    new_cacheItem ->flag = 0;
    new_cacheItem ->tableItemPtr = ctxtTableItem;

    new_cacheItem ->kvpairs = g_hash_table_new_full(g_feature_hash, g_feature_equal, NULL, NULL);
    if (sr == NULL) {

        GSequenceIter* chunkIter = g_sequence_get_begin_iter(ctxtTableItem->segment_ptr->chunks);
        GSequenceIter* chunkEnd = g_sequence_get_end_iter(ctxtTableItem->segment_ptr->chunks);

        for(; chunkIter != chunkEnd; chunkIter = g_sequence_iter_next(chunkIter)){
            struct chunk* c = g_sequence_get(chunkIter);
            g_hash_table_insert(new_cacheItem->kvpairs, &c->fp, TEMPORARY_ID);
        }

    }else {
        GHashTableIter tableIter;
        gpointer key, value;
        g_hash_table_iter_init(&tableIter, sr->kvpairs);
        while (g_hash_table_iter_next(&tableIter, &key, &value)) {
            g_hash_table_insert(new_cacheItem->kvpairs, &key, ((struct chunkPointer*) value)->id);
        }

    }
    return new_cacheItem;
}

void free_lipa_cache(struct LIPA_cacheItem* cache) {
    g_hash_table_destroy(cache->kvpairs);
    free(cache);
}

int lookup_fingerprint_in_lipa_cache(struct LIPA_cacheItem* cacheItem, fingerprint* fp) {
    return g_hash_table_contains(cacheItem->kvpairs, fp);
}