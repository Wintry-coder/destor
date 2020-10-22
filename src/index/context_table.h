/*
 * context_table.h
 *
 *  Created on: Oct 3, 2020
 *      Author: Yanweijie
 */
#ifndef CONTEXT_TABLE_H
#define CONTEXT_TABLE_H
#include "../destor.h"
#include "index.h"

struct contextItem {
	int64_t id;
	int followers;
	double score;
	int updatetime;
};

#define CONTEXT_TABLE_LENGTH 4

void init_context_table();

struct contextItem *find_item(GList *contextList, int op);
void free_contextItem(struct contextItem* item);
struct contextItem* new_contextItem(struct segment* segment);


extern void (*close_context)();
extern GList* (*context_lookup)(char *key);
extern int (*context_find)(char *key);
extern void (*context_update)(char *key, int64_t id);

#endif 