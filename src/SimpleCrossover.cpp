#include "SimpleCrossover.h"

SimpleCrossover::SimpleCrossover() {
    //ctor
}

bool SimpleCrossover::checkRate(double rate) {
    return (rand()%100 < rate*100);
}

void SimpleCrossover::cross(Subject ** vec_individuos, int num_individuos) {
    //    Cross pode ocorrer em cada árvore
    if(conf->crossover_method == 0){
        for(int i = 1; i < num_individuos; i+= 2) {
            for( int j = 0 ; j < vec_individuos[i]->trees.size(); j++) {
                if(checkRate(conf->crossoverRate)) {
                    treeCrossover( vec_individuos[i-1]->trees[j], vec_individuos[i]->trees[j] );
                }
            }

            for( int j = 0 ; j < vec_individuos[i]->functions.size(); j++) {
                if(checkRate(conf->crossoverRate)) {
                    treeCrossover( vec_individuos[i-1]->functions[j], vec_individuos[i]->functions[j] );
                }
            }
        }
    }
    else{
    //    Cross só em uma árvore
        for(int i = 1; i < num_individuos; i+= 2) {
            if(checkRate(conf->crossoverRate)) {
                int t = rand() % (conf->numFunc + conf->numTree);

                if(t < conf->numTree){
                    treeCrossover(vec_individuos[i-1]->trees[t], vec_individuos[i]->trees[t]);
                }
                else{
                    t -= conf->numTree;
                    treeCrossover(vec_individuos[i-1]->functions[t], vec_individuos[i]->functions[t]);
                }
            }
        }
    }
}

void SimpleCrossover::treeCrossover(Tree* a, Tree* b) {
    No* n = a->subTree();
    No* m = b->targetSubTree(n);

    conf->node_evaluations = conf->node_evaluations + 1;

    if(n == NULL || m == NULL) {
        return;
    }
    swap(n->sons, m->sons);
    swap(n->father, m->father);

    a->update();
    b->update();
}

SimpleCrossover::~SimpleCrossover() {
    //dtor
}
