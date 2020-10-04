/*
 * context_table.c
 *
 *  Created on: Oct 3, 2020
 *      Author: Yanweijie
 */

#include "context_table.h"

struct contextItem *find_item(GList *contextList, bool minitem) {
    struct contextItem* item = contextList -> data;
    struct contextItem* current = item;

    contextList = g_list_next(contextList);

    while (contextList){
        current = contextList -> data;
        if(minitem){
            if (item -> score > currItem -> score) {
                item = current;
            }
        }else{
            if (item -> score < currItem -> score) {
                item = current;
            }            
        }
        contextList = g_list_next(contextList);
    }

    return item;
}


