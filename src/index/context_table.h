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
	//struct segment* segment_ptr;
	int followers;
	double score;
	int updatetime;
};

struct contextTableList{
	double meanhit;  //the mean hits of the champion and the followers
	double deviation;
	GList* contextList;
};
#define CONTEXT_TABLE_LENGTH 4

void init_context_table();


struct contextItem *find_item(GList *contextList, int minitem);
void free_contextItem(struct contextItem* item);
struct contextItem* new_contextItem(struct segment* segment);
struct contextTableList* new_contextTableList(GList *contextList);
struct segment* copy_segment(struct segment* src, struct segment* dst);
void LIPA_context_update(struct segment* s);

extern void (*close_context)();
extern GList* (*context_lookup)(char *key);
extern int (*context_find)(char *key);
extern void (*context_update)(char *key, int64_t id);

#endif 