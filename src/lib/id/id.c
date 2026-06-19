#include <lib/id.h>
#include <stdatomic.h>

#define START_ID 1

static unsigned int id = START_ID;

unsigned int next_id(){
    return id++;
}

void reset_id(){
    id = START_ID;
}
