/*
 * context_table.c
 *
 *  Created on: Oct 3, 2020
 *      Author: Yanweijie
 */

#include "context_table.h"

static GHashTable *context_table;

void (*close_context)();
GList* (*context_lookup)(char *key);
int (*context_find)(char *key);
void (*context_update)(char *key, int64_t id);

void close_context_table() {
	sds indexpath = sdsdup(destor.working_directory);
	indexpath = sdscat(indexpath, "index/contexttable");

	FILE *fp;
	if ((fp = fopen(indexpath, "w")) == NULL) {
		perror("Can not open index/contexttable for write because:");
		exit(1);
	}

	NOTICE("flushing ctt table!");
	int key_num = g_hash_table_size(context_table);
	fwrite(&key_num, sizeof(int), 1, fp);

	GHashTableIter iter;
	gpointer key, value;
	g_hash_table_iter_init(&iter, context_table);
	while (g_hash_table_iter_next(&iter, &key, &value)) {

		if(fwrite(key, destor.index_key_size, 1, fp) != 1){
			perror("Fail to write a key!");
			exit(1);
		}
        GList * contextList = value;
		int numc =  g_list_length(contextList);
		if(fwrite(&numc, sizeof(int), 1, fp) != 1){
			perror("Fail to write a length!");
			exit(1);
		}
        
		int i;
		for (i = 0; i < numc; i++)
        {
            struct contextItem* Item = contextList->data;
		    if(fwrite(Item,  sizeof(struct contextItem), 1, fp)!=1){
			    perror("Fail to write a Item!");
			    exit(1);
		    }
            contextList = g_list_next(contextList);
        }
	}

	/* It is a rough estimation */
	//destor.index_memory_footprint = g_hash_table_size(htable)
	//		* (destor.index_key_size + sizeof(int64_t) * destor.index_value_length + 4);

	fclose(fp);

	NOTICE("flushing context table successfully!");

	sdsfree(indexpath);
    fprintf(stderr,"end the close cttable\n");
    g_hash_table_destroy(context_table);
}

GList* context_table_lookup(char* key) {
    GList * contextList = g_hash_table_lookup(context_table, key);
	return contextList;
}

int context_table_find(char* key) {
	gboolean hit = g_hash_table_contains(context_table, key);
	return hit ? 1 : 0;
}

void context_table_update(char* key, GList* contextList) {
	g_hash_table_replace(context_table, key, contextList);
}


struct contextItem *find_item(GList *contextList, int op) {
    assert(contextList);
    struct contextItem* item = contextList -> data;
    struct contextItem* current = item;
    if(op == 2 && item->id == TEMPORARY_ID)
        return item;
    contextList = g_list_next(contextList);

    while (contextList){
        current = contextList -> data;
        if(op == 2){
            if (current -> id == TEMPORARY_ID) {
                item = current;
                break;
            }            
        }
        if(op == 1){
            if (item -> score > current -> score) {
                item = current;
            }
        }
        if(op == 0){
            if (item -> score < current -> score) {
                item = current;
            }            
        }
        contextList = g_list_next(contextList);
    }

    return item;
}

/*
 * Mapping a feature to the context list.
 * The length of context list is fixed
 * Each item in the context list include segment id, score and followers
 */
void init_context_table() {
    context_table = g_hash_table_new_full(g_feature_hash, g_feature_equal, NULL, NULL);
    close_context = close_context_table;

    context_lookup = context_table_lookup;
    context_find = context_table_find;
    context_update = context_table_update;

	sds indexpath = sdsdup(destor.working_directory);
	indexpath = sdscat(indexpath, "index/contexttable");

	/* Initialize the feature index from the dump file. */
	FILE *fp;
	if ((fp = fopen(indexpath, "r"))) {
		/* The number of features */
		int key_num;
		fread(&key_num, sizeof(int), 1, fp);

		for (; key_num > 0; key_num--) {
			/* Read a feature */
            struct contextItem item;
            struct contextItem *newItem = &item;
            char feature[destor.index_key_size];
			fread(feature, destor.index_key_size, 1, fp);


			int id_num, i;
			fread(&id_num, sizeof(int), 1, fp);
			assert(id_num <= CONTEXT_TABLE_LENGTH);
            GList *contextList = NULL;
            
			for (i = 0; i < id_num; i++)
            {
                fread(newItem, sizeof(struct contextItem), 1, fp);
                contextList = g_list_append(contextList, newItem);
                fprintf(stderr,"%lld   %d\n",newItem->id,newItem->followers);
            }

			g_hash_table_insert(context_table, feature, context_table);
		}
		fclose(fp);
	}

	sdsfree(indexpath);
    VERBOSE("initial context table");
    fprintf(stderr,"end the init cttable\n");
}

/*
 * New a elem in the context List
 */

struct contextItem* new_contextItem(struct segment* segment){
    assert(segment);
	struct contextItem* newItem = (struct contextItem*) malloc(sizeof(struct contextItem));
	newItem->followers = 4;
	newItem->score = 0;
    newItem->id = segment->id;
    newItem->updatetime = 0;
	return newItem;
}


void free_contextItem(struct contextItem* item) {
	printf("free context table item\n");
    free(item);
}

void LIPA_context_update(struct segment* s,int64_t id)
{
    assert(s->features);
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, s->features);

    struct contextItem* newItem = new_contextItem(s);
	/* Each feature will map many segments 
	 * key is feature	
	 * value is ids
	 */
    newItem ->id = id;
    while(g_hash_table_iter_next(&iter, &key, &value)) {
        GList* contextList = context_lookup((fingerprint *) key);
        if (contextList) {
            int list_length = g_list_length(contextList);
            if (list_length >= CONTEXT_TABLE_LENGTH) {
                struct contextItem *item = NULL;
                if(destor.lipa_update_method == LIPA_MIN){
                    item = find_item(contextList, 1);
					//remove  the lowest score  entry  in  the segment list
                }else{
                    item = contextList -> data;  
					//remove  the oldest entry  in  the segment list   
                }
                contextList = g_list_remove(contextList, item);
				if(item)
                	free_contextItem(item);
            }
        }
        contextList = g_list_append(contextList, newItem);
        context_update((fingerprint *) key, contextList);
    }
}