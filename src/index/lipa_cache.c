/*
 * lipa_cache.c
 *
 *  Created on: Oct 5, 2020
 *      Author: Yanweijie
 */
#include "lipa_cache.h"


void feedback(struct segmentRecipe* sr, char* feature) {

    struct contextItem* arr = context_lookup(feature);
    assert(arr);
    segmentid id = sr ->id;
    int index = -1;
    // if the feature exists in context table, update score
    for(int i = 0;i<destor.context_length;i++){
        if(id == arr[i].id)
        {
            index = i;
            break;
        }
    }
    if(index != -1)  
    {
        ++arr[index].updatetime;
        arr[index].score = arr[index].score + (sr->hit - arr[index].score) * (1.0 / arr[index].updatetime);
    }
}



