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
	segmentid id;;
	int followers;
	double score;
};

void init_context_table();


struct contextItem *find_item(GList *contextList, bool minitem);


#endif 