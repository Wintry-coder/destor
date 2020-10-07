/*
 * lipa_cache.c
 *
 *  Created on: Oct 5, 2020
 *      Author: Yanweijie
 */

void feedback(struct segmentRecipe* sr, char* feature) {

    GList* contextList = NULL;
    if (context_find((fingerprint *) key)) {
    // if the feature doesn't exist in context table, don't feedback
        contextList = context_lookup((fingerprint *) key);
        int list_length = g_list_length(contextList);
        if (list_length >= CONTEXT_TABLE_LENGTH) {
            struct contextItem *item = NULL;
            if(destor.lipa_update_method == LIPA_MIN){
                item = find_item(contextList, 1);
				//remove  the lowest score  entry  in  the segment list
            }else{
                item = contextList -> data;  
					//remove  the oldest entry  in  the segment list   
            }
            contextList = g_list_remove(contextList, item);
            free_contextItem(item);
        }
    }    
    struct ctxtTableItem* target_ctxtTableItem = cacheItem ->tableItemPtr;
    assert(target_ctxtTableItem);
    target_ctxtTableItem ->update_time ++;
    target_ctxtTableItem -> score = target_ctxtTableItem -> score +
            ((double)(cacheItem->hit_score) - target_ctxtTableItem -> score) * (1.0 / target_ctxtTableItem -> update_time);
}