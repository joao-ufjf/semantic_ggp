#include "EvoStratInd.h"

EvoStratInd::EvoStratInd(int* dim){
    this->dim = dim;
    this->fitness = INFINITY;

    this->problem    = new double*[conf->numTree];
    this->Std        = new double*[conf->numTree];

    for(int i = 0; i < conf->numTree; i++){
        this->problem[i]      = new double[this->dim[i]];
        this->Std[i]          = new double[this->dim[i]];

        for(int j = 0; j < this->dim[i]; j++){
            this->problem[i][j] = -1;
            this->Std[i][j] = -1;
        }
    }
}

EvoStratInd::~EvoStratInd(){
    for(int i = 0; i < conf->numTree; i++){
        delete [] this->problem[i];
        delete [] this->Std[i];
    }

    delete [] this->problem;
    delete [] this->Std;
}

void EvoStratInd::print(){
    for(int i = 0; i < conf->numTree; i++){
        for(int j = 0; j < this->dim[i]; j++){
            cout << this->problem[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}
