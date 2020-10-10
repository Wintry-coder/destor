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
    // if the feature exists in context table, update score
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
    if(destor.prefetch_method == PREFETCH_ADAPTIVE){
        struct contextItem* champion = sr -> champion;
        if(sr->flag == 1)//the segment is the last one
        {
            /*
             * The last segment  feedbacks its champion
             */
            /*int threshold = champion->meanhit - champion->deviation;
            if(sr->hit < threshold)
            {
                champion->followers--;
            }
            else
            {
                champion->followers++;
            }*/
        }
        if(sr->flag == 0)//update the threshold
        {
            
        }
    }
}