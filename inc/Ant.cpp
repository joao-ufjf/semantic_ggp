#include "Ant.h"

Ant::Ant(int n){
    //ctor
    this->n = n;
    this->fitness = INFINITY;
}

Ant::Ant(Ant* ant){
    //ctor
    this->n = ant->n;
    this->fitness = ant->fitness;

    this->route = new int[this->n];
    for(int i = 0; i < this->n; i++){
        this->route[i] = ant->route[i];
    }

    this->constants = new double[this->n];
    for(int i = 0; i < this->n; i++){
        this->constants[i] = ant->constants[i];
    }
}

Ant::~Ant()
{
    //dtor
    delete [] constants;
    delete [] route;
}

void Ant::print(){
    cout << "fitness = " << this->fitness << " " << setw(20);

    cout << "route = [";
    for(int i = 0; i < this->n; i++){
        cout << route[i] << ", ";
    }
    cout << "] " << setw(20);

    cout << "constants = [";
    for(int i = 0; i < this->n; i++){
        cout << constants[i] << ", ";
    }
    cout << "]" << endl;
}
