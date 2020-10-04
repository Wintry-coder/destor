#include "../destor.h"
#include "context_table.h"


struct contextItem *(*champion_choose)(GList *contextList);

static struct contextItem* champion_choose_random(GList* contextList) {

}

static struct contextItem* champion_choose_greedy(GList* contextList) {

}

static struct contextItem* champion_choose_recent(GList* contextList) {

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