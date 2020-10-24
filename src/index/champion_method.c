#include "../destor.h"
#include "context_table.h"

int (*champion_choose)(struct contextItem *arr);

/*
 * policy: random
 * choose a segment randomly
 */
static int champion_choose_random(struct contextItem *arr) {
    int len = 0;
    while(len <= destor.context_length-1 && arr[len].id != TEMPORARY_ID) ++len;
    int index = rand()%len;
    return index;
}

/*
 * policy: greedy
 * it may choose a segment with the  highest score with 1 - e probability
 * otherwise, it may choose a random segment with e probability
 */
static int champion_choose_greedy(struct contextItem *arr) {
    double randnum = rand()/(RAND_MAX+1.0);
    int len = 0;
    while(len <= destor.context_length - 1  && arr[len].id != TEMPORARY_ID) ++len;
    if (randnum < 1 - destor.choose_epsilon) {
        //find the highest score
        int index = 0, i = 1;
        for(;i<len;i++){
            if(arr[index].score < arr[i].score)
                index = i;
        }
        return index;
    } else {
        return rand()%len;
    }
}

/*
 * policy: recent
 * choose the newest segment as champion
 */
static int champion_choose_recent(struct contextItem *arr) {
    return 0;
}

void init_champion_method(){
    srand(time(NULL));
    switch (destor.champion_choose_method) {
        case CHAMPIOM_CHOOSE_RANDOM:
            champion_choose = champion_choose_random;
            break;
        case CHAMPIOM_CHOOSE_GREEDY:
            champion_choose = champion_choose_greedy;
            break;
        case CHAMPIOM_CHOOSE_RECENT:
            champion_choose = champion_choose_recent;
            break;
        default:
            fprintf(stderr, "Invalid champion choose method!\n");
            exit(1);
    }
}