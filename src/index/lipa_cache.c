/*
 * lipa_cache.c
 *
 *  Created on: Oct 5, 2020
 *      Author: Yanweijie
 */
#include "lipa_cache.h"

static double total_score = 0;
static double total_updatenum = 0;
void feedback(struct segmentRecipe* sr, char* feature) {

    struct contextItem* arr = sr->champion;
    assert(arr);
    // if the champion exists in context table and sr is champion sr, update its score
    ++total_updatenum;
    total_score = total_score + (sr->hit - total_score) * (1.0 / total_updatenum);        
    if(sr->flag == 1){
        //flag == 1 means it is champion
        //flag == 0 means it is follower
        ++arr->updatetime;
        arr->score = arr->score + (sr->hit - arr->score) * (1.0 / arr->updatetime);        
    }
    if(sr->flag != 1 && arr->score && sr->hit == 0)
    {
        if(arr->followers > 0)
            --arr->followers;
    }
    if(sr->flag == 2)
    {
        if(sr->hit > 0.5 * total_score)
            ++arr->followers;
    }

}



