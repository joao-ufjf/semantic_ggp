#include "SimpleReplace.h"

SimpleReplace::SimpleReplace() {

}
void SimpleReplace::Replace(Subject **pop) {
    for(int i = 0; i < conf->popSize * conf->elitism; i++) {
        swap(pop[conf->popSize + i - 1], pop[conf->popSize + i]);
    }
}

SimpleReplace::~SimpleReplace() {

}
