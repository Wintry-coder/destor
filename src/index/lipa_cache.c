/*
 * lipa_cache.c
 *
 *  Created on: Oct 5, 2020
 *      Author: Yanweijie
 */
#include "lipa_cache.h"


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
        elem ->score = elem ->score + ((double)(sr->hit) - elem ->score) * (1.0 / elem ->updatetime);


    }   
    if(sr->flag == 1)
    {
    /*
     * The last segment  feedbacks its champion
     */
        segmentid champion_id = sr->champion_id;

    }    

}