#include "../destor.h"
#include "context_table.h"

#define EPSILON 0.1
struct contextItem *(*champion_choose)(GList *contextList);

static struct contextItem* champion_choose_random(GList* contextList) {
    srand(time(NULL));
    int contextList_length = g_list_length(contextList);
    double rand_num = rand()/(RAND_MAX/1.0);
    int contextList_length = g_list_length(ctxtList);
    int index_ = (int) (rand_num * contextList_length);
    return g_list_nth_data(contextList, index_);
}

static struct contextItem* champion_choose_greedy(GList* contextList) {
    srand(time(NULL));
    double rand_num = rand()/(RAND_MAX/1.0);
    if (rand_num < 1 - EPSILON) {
        return find_max_Item(contextList);
    } else {
        return champion_choose_random(contextList);
    }
}

static struct contextItem* champion_choose_recent(GList* contextList) {
    GList* p = g_list_last(contextList);
}

void init_champion_method(){
    switch (destor.champion_choose_method) {
        case CHAMPIOM_CHOOSE_RANDOM:
            champion_choose = champion_choose_random;
            break;
        case CHAMPIOM_CHOOSE_GREEDY:
            champion_choose = champion_choose_greedy;
            break;
        case CHAMPIOM_CHOOSE_RECENT:
            champion_choose = champion_choose_greedy;
            break;
        default:
            fprintf(stderr, "Invalid champion choose method!\n");
            exit(1);
    }
}