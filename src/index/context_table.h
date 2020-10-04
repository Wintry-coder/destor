/*
 * context_table.h
 *
 *  Created on: Oct 3, 2020
 *      Author: Yanweijie
 */
#ifndef CONTEXT_TABLE_H
#define CONTEXT_TABLE_H

struct contextItem {
    //contextid id; // context table item id

	//segmentid segment_id;
	struct segment* segment_ptr;
	int followers;
	double score;
};




#endif 