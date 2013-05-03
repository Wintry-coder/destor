#include "dedup.h"

Chunk* allocate_chunk(){
    Chunk* chunk = (Chunk*)malloc(sizeof(Chunk));

    chunk->container_id = TMP_CONTAINER_ID;
    chunk->data = 0;
    chunk->length = 0;
    chunk->feature = 0;
    chunk->status = UNIQUE;

    return chunk;
}

void free_chunk(Chunk* chunk){
    if(chunk == NULL)
        return;
    if(chunk->data && chunk->length>0)
        free(chunk->data);
    if(chunk->feature)
        free(chunk->feature);
    chunk->data = 0;
    free(chunk);
}

gboolean g_fingerprint_cmp(gconstpointer k1, gconstpointer k2)
{
    if (memcmp(k1, k2, sizeof(Fingerprint)) == 0)
        return TRUE;
    return FALSE;
}
