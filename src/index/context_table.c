/*
 * context_table.c
 *
 *  Created on: Oct 3, 2020
 *      Author: Yanweijie
 */

#include "context_table.h"

static GHashTable *context_table;
typedef char* contextpair;

#define get_table_key(cp) (cp)
#define get_table_context(cp) ((struct contextItem*)(cp+destor.index_key_size))

void (*close_context)();
struct contextItem* (*context_lookup)(char *key);
void (*context_update)(char *key, int64_t id);


static int32_t contextpair_size;

static contextpair new_contextpair_full(char* key){
    contextpair cp = malloc(contextpair_size);
    memcpy(get_table_key(cp), key, destor.index_key_size);
    struct contextItem* contextarr = get_table_context(cp);
    int i;
    for(i = 0; i<destor.context_length; i++){
    	contextarr[i].id = TEMPORARY_ID;
    }
    return cp;
}

static contextpair new_contextpair(){
	contextpair cp = malloc(contextpair_size);
    struct contextItem* contextarr = get_table_context(cp);
    int i;
    for(i = 0; i<destor.context_length; i++){
    	contextarr[i].id = TEMPORARY_ID;
    }
	return cp;
}

static inline void free_contextpair(contextpair cp){
	free(cp);
}
void close_context_table() {
    sds indexpath = sdsdup(destor.working_directory);
    indexpath = sdscat(indexpath, "index/contexttable");

    FILE *fp;
    if ((fp = fopen(indexpath, "w")) == NULL) {
        perror("Can not open index/contexttable for write because:");
        exit(1);
    }

    NOTICE("flushing context table!");
    int key_num = g_hash_table_size(context_table);
//  fprintf(stderr,"the key num is %d\n",key_num);
    fwrite(&key_num, sizeof(int), 1, fp);
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, context_table);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
		contextpair cp = value;
		if(fwrite(get_table_key(cp), destor.index_key_size, 1, fp) != 1){
			perror("Fail to write a key!");
			exit(1);
		}
        int num = 0;
        while(num <= destor.context_length-1 &&get_table_context(cp)[num].id != TEMPORARY_ID) ++num;

		if(fwrite(&num, sizeof(int), 1, fp) != 1){
			perror("Fail to write a length!");
			exit(1);
		}
		int i;
		for (i = 0; i < num; i++)
			if(fwrite(&get_table_context(cp)[i], sizeof(struct contextItem), 1, fp) != 1){
				perror("Fail to write a contextItem!");
				exit(1);
			}
    }

    /* It is a rough estimation */
    destor.index_memory_footprint = g_hash_table_size(context_table)
                  * (destor.index_key_size + sizeof(struct contextItem) * destor.context_length);
    NOTICE("flushing context table successfully!");
    fclose(fp);
    sdsfree(indexpath);
    g_hash_table_destroy(context_table);
}

struct contextItem* context_table_lookup(char* key) {
	contextpair cp = g_hash_table_lookup(context_table, key);
	return cp ? get_table_context(cp) : NULL;
}

void context_table_update(char* key, int64_t id) {
	contextpair cp = g_hash_table_lookup(context_table, key);
	if (!cp) {
		cp = new_contextpair_full(key);
		g_hash_table_replace(context_table, get_table_key(cp), cp);
	}
    struct contextItem* arr = get_table_context(cp);
    int len = 0;
    while(len <= destor.context_length - 1  && get_table_context(cp)[len].id != TEMPORARY_ID) ++len;

    if(destor.lipa_update_method == LIPA_MIN && len == destor.context_length){
        //remove the lowest score
        int index = 0, i = 1;
        for(;i<len;i++){
            if(arr[index].score > arr[i].score)
                index = i;
        }
        if(index)
            memmove(&arr[1], arr, index * sizeof(struct contextItem)); 
    }	    
    else{
        //FIFO
        memmove(&arr[1], arr, (destor.context_length - 1) * sizeof(struct contextItem));
    }
    arr[0].id = id;
    arr[0].followers = 4;
    arr[0].score = 0;
    arr[0].updatetime = 0;
}

/*
 * Mapping a feature to the context list.
 * The length of context list is fixed
 * Each item in the context list include segment id, score and followers
 */
void init_context_table() {
    contextpair_size = destor.index_key_size + destor.context_length * sizeof(struct contextItem);
    context_table = g_hash_table_new_full(g_feature_hash, g_feature_equal, free_contextpair, NULL);

    close_context = close_context_table;
    context_lookup = context_table_lookup;
    context_update = context_table_update;

    sds indexpath = sdsdup(destor.working_directory);
    indexpath = sdscat(indexpath, "index/contexttable");
    /* Initialize the feature index from the dump file. */
    FILE *fp;
    if ((fp = fopen(indexpath, "r"))) {

		int key_num;
		fread(&key_num, sizeof(int), 1, fp);
		for (; key_num > 0; key_num--) {
			contextpair cp = new_contextpair();
			fread(get_table_key(cp), destor.index_key_size, 1, fp);

			int id_num, i;
			fread(&id_num, sizeof(int), 1, fp);
			assert(id_num <= destor.context_length);

			for (i = 0; i < id_num; i++)
            {
                fread(&get_table_context(cp)[i], sizeof(struct contextItem), 1, fp);
            }
				
			g_hash_table_insert(context_table, get_table_key(cp), cp);
		}
        fclose(fp);
    }
	sdsfree(indexpath);
    VERBOSE("initial context table");
    NOTICE("Initial context table sucessfully!");
}
