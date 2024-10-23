#include "SemanticSimpleMutation.h"

SemanticSimpleMutation::SemanticSimpleMutation(SemanticSimpleParser* parser)
{
    //ctor
    this->parser = parser;
}

void SemanticSimpleMutation::mutate(Subject * individuo) {
    individuo->complexity = 0;
    for(Tree* t : individuo->trees) {
        if(checkRate(conf->mutationRate)) {
            auxMutate(t);
            individuo->complexity += t->linearModel.size();
        }
    }
}

bool SemanticSimpleMutation::compareSemantic(Candidate* a, Candidate* b){
    return (a->distance < b->distance);
}

void SemanticSimpleMutation::auxMutate(Tree* t) {
    No* n = t->subTree();
    No* elected;

    vector<Candidate*> candidates;

    double best_semantic_distance = INFINITY;

    for(int i = 0; i < conf->max_tries; i++){
        No* n_ = new No(n->type, n->value, n->mask, n->deep);
        grammar->derivate(n_);

        SimpleIndividuo* s = new SimpleIndividuo(false);
        s->addTree(new Tree(n_));
        s->trees.at(0)->update();
        parser->AuxEvaluateTree(s);

        double semantic_distance = n->semanticDistance(s->trees.at(0)->root, conf->distance_method);

        if(semantic_distance > conf->lower_bound_sensitivity){
            candidates.push_back(new Candidate(semantic_distance, s));
        }
    }

    sort(candidates.begin(), candidates.end(), compareSemantic);

    switch(conf->mutation_type){
        case 1: {
            double totalInverseDistances = 0.0;
            double inverseDistances[candidates.size()];

            for (int i = 0; i < candidates.size(); ++i) {
                double distancia = candidates.at(i)->distance;
                totalInverseDistances += 1.0 / distancia;
                inverseDistances[i] = 1.0 / distancia;
            }

            // Normalizando as probabilidades
            for (int i = 0; i < candidates.size(); ++i) {
                inverseDistances[i] /= totalInverseDistances;
            }

            // Criando um gerador de números aleatórios
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> dis(0.0, 1.0);

            // Sorteando um ponto com base nas probabilidades inversamente proporcionais
            double randomValue = dis(gen);
            double acumulado = 0.0;

            for (int i = 0; i < candidates.size(); ++i) {
                acumulado += inverseDistances[i];
                if (randomValue <= acumulado) {
//                    cout << "SM1 " << i << endl;
                    elected = candidates.at(i)->ind->trees.at(0)->root;
                    break;
                }
            }

            break;
        }
        case 2: {
//            cout << "SM2 0" << endl;
            elected = candidates.at(0)->ind->trees.at(0)->root;
        }
    }

    swap(n, elected);
    t->update();

    for(Candidate* c : candidates){
        delete c;
    }

    candidates.clear();
}

SemanticSimpleMutation::~SemanticSimpleMutation()
{
    //dtor
}
