/*
 * lipa.c
 *
 *  Created on: Oct 5, 2020
 *      Author: Yanweijie
 */

#include "index_buffer.h"
#include "kvstore.h"
#include "fingerprint_cache.h"
#include "context_table.h"
#include "../recipe/recipestore.h"
#include "../storage/containerstore.h"
#include "../jcr.h"

extern struct index_overhead index_overhead;

extern struct index_buffer index_buffer;

extern struct{
	/* accessed in dedup phase */
	struct container *container_buffer;
	/* In order to facilitate sampling in container,
	 * we keep a queue for chunks in container buffer. */
	GSequence *chunks;
} storage_buffer;

extern int (*champion_choose)(struct contextItem *arr);

/**
 * Input: segment s
 * Segment s corresponds to several features.
 * Key: each feature in s. 
 * Each feature is mapped to several segment IDs.
 * 
 */
void index_lookup_lipa(struct segment *s) {
    assert(s->features);
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, s->features);
	/* Each feature will map many segments 
	 * key is feature	
	 * value is ids
	 */
    while(g_hash_table_iter_next(&iter, &key, &value)) {
        struct contextItem* arr = context_lookup(key);
		if(arr){
        	int champion = champion_choose(arr);
        	fingerprint_lipa_prefetch(arr, champion, (char*) key);
		}
    }
    /**
	 * The following part is same as the part of similar detection
	 */
    GSequenceIter *it = g_sequence_get_begin_iter(s->chunks);
	GSequenceIter *end = g_sequence_get_end_iter(s->chunks);
	for (; it != end; it = g_sequence_iter_next(it)) {
		struct chunk* c = g_sequence_get(it);

		if (CHECK_CHUNK(c, CHUNK_FILE_START) || CHECK_CHUNK(c, CHUNK_FILE_END))
			continue;

		/* First check it in the storage buffer */
		if(storage_buffer.container_buffer
				&& lookup_fingerprint_in_container(storage_buffer.container_buffer, &c->fp)){
			c->id = get_container_id(storage_buffer.container_buffer);
			SET_CHUNK(c, CHUNK_DUPLICATE);
			SET_CHUNK(c, CHUNK_REWRITE_DENIED);
		}
		/*
		 * First check the buffered fingerprints,
		 * recently backup fingerprints.
		 */
		GQueue *tq = g_hash_table_lookup(index_buffer.buffered_fingerprints, &c->fp);
		if (!tq) {
			tq = g_queue_new();
		} else if (!CHECK_CHUNK(c, CHUNK_DUPLICATE)) {
			struct indexElem *be = g_queue_peek_head(tq);
			c->id = be->id;
			SET_CHUNK(c, CHUNK_DUPLICATE);
		}

		/* Check the fingerprint cache */
		if (!CHECK_CHUNK(c, CHUNK_DUPLICATE)) {
			/* Searching in fingerprint cache */
			int64_t id = fingerprint_cache_lookup(&c->fp);
			if(id != TEMPORARY_ID){
				c->id = id;
				SET_CHUNK(c, CHUNK_DUPLICATE);
			}
		}
        
		/* Insert it into the index buffer */
		struct indexElem *ne = (struct indexElem*) malloc(
				sizeof(struct indexElem));
		ne->id = c->id;
		memcpy(&ne->fp, &c->fp, sizeof(fingerprint));

		g_queue_push_tail(tq, ne);
		g_hash_table_replace(index_buffer.buffered_fingerprints, &ne->fp, tq);

		index_buffer.chunk_num++;
	}
}
