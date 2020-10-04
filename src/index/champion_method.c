#include "../destor.h"
#include "context_table.h"

#define EPSILON 0.1
struct contextItem *(*champion_choose)(GList *contextList);

static struct contextItem* champion_choose_random(GList* contextList) {
    srand(time(NULL));
    int contextList_length = g_list_length(contextList);
    int index = rand()/contextList_length;
    return g_list_nth_data(contextList, index);
}

static struct contextItem* champion_choose_greedy(GList* contextList) {
    srand(time(NULL));
    double randnum = rand()/(RAND_MAX+1.0);
    if (randnum < 1 - EPSILON) {
        return find_max_Item(contextList);
    } else {
        return champion_choose_random(contextList);
    }
}

static struct contextItem* champion_choose_recent(GList* contextList) {
    GList* recentitem = g_list_last(contextList);
    return recentitem -> data;
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