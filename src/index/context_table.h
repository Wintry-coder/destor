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
	double score;
	int followers;
	int updatetime;
};

void init_context_table();
void close_context_table();

extern void (*close_context)();
extern GList* (*context_lookup)(char *key);
extern void (*context_update)(char *key, int64_t id);

#endif 