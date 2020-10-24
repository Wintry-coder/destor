/*
 * lipa_cache.c
 *
 *  Created on: Oct 5, 2020
 *      Author: Yanweijie
 */
#include "lipa_cache.h"


void feedback(struct segmentRecipe* sr, char* feature) {

    struct contextItem* arr = sr->champion;
    assert(arr);
    // if the champion exists in context table and sr is champion sr, update its score
    if(sr->flag == 1){
        //flag == 1 means it is champion
        //flag == 0 means it is follower
        ++arr->updatetime;
        arr->score = arr->score + (sr->hit - arr->score) * (1.0 / arr->updatetime);        
    }
}



