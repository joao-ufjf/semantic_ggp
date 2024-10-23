#include "No.h"
#include <iostream>   // std::cout
#include <string>     // std::string, std::to_string

No::No() {
    //ctor
}

No::No(double t, double v, string m, int h) {
    type = t;
    value = v;
    mask = m;
    deep = h;
}

void No::print(int format) {
    for(int i = 0; i < format; i++)
        cout << " ";
    cout << height << " " << mask << endl;

    for(No* n : sons)
        n->print(format + 1);
}

void No::detailed(int format) {
    for(int i = 0; i < format; i++)
        cout << " ";
    cout << height << " " << mask << "(" << type << ", " << value << ")" << endl;

    for(No* n : sons)
        n->detailed(format + 1);
}

int No::replaceAllC(double* constants, int index){
    if(type == 9){
//        cout << index << " ";
        std::ostringstream strs;
        strs << constants[index];
        mask = strs.str();
        type = 0;
        value = constants[index++];
    }
    for(No* n : sons)
        index = n->replaceAllC(constants, index);
    return index;
}
//Deve ser realizada a contagem de todos os _c_ para que possa retornar quantas constantes ha atualmente
int No::replaceAllConst(int constantCount){
    int counter = 0;
    if(type == 0){
        mask = "_c_";
        type = 9;
        value = 0;
        counter++;
    }
    for(No* n : sons)
        counter += n->replaceAllConst(constantCount);
    return counter;
}

void No::print() {
    if(type != -1)
        cout << mask << " ";
        //cout << "[" << mask << "," << type << "] ";

    for(No* n : sons)
        n->print();
}

void No::addSon(No* n) {
    sons.push_back(n);
}

void No::erase() {
    for(No* n : sons)
        delete n;
    sons.clear();
}

No* No::clone() {
    No* n = new No(type, value, mask, deep);

//    copy(semantic.begin(), semantic.end(), back_inserter(n->semantic));

    for(double d : semantic){
        n->semantic.push_back(d);
    }

    return n;
}

No::~No() {
#ifdef debug
    cout << "DELETANDO NO" << endl;
    cin.get();
#endif // debug

    for(No* n : sons)
        delete n;
    sons.clear();

    semantic.clear();

}

void No::destrutor(No * p) {
    if(p == NULL) {
        delete p;
    } else {
        for(int i = 0; i < p->sons.size(); i++) {
            p->sons[i]->father = NULL;
            destrutor(p->sons[i]);
        }
    }
}

void No::printSemantic(){
    cout << "[";
    for(double d : semantic)
        cout << d << ", ";
    cout << "]" << endl;
}

double dotProduct(const vector<double>& vec1, const vector<double>& vec2) {
    if (vec1.size() != vec2.size()) {
//        cerr << "Os vetores têm tamanhos diferentes!" << endl;
        return 0.0;
    }

    double product = 0.0;
    for (size_t i = 0; i < vec1.size(); ++i) {
        product += vec1[i] * vec2[i];
    }
    return product;
}

// Função para calcular a magnitude de um vetor
double magnitude(const vector<double>& vec) {
    double mag = 0.0;
    for (double value : vec) {
        mag += value * value;
    }
    return sqrt(mag);
}

// Função para calcular a distância de cosseno entre dois vetores
double cosineDistance(const vector<double>& vec1, const vector<double>& vec2) {
    double dot = dotProduct(vec1, vec2);
    double mag1 = magnitude(vec1);
    double mag2 = magnitude(vec2);

    if (mag1 == 0 || mag2 == 0) {
//        cerr << "Um dos vetores tem magnitude zero!" << endl;
        return 0.0;
    }

    return dot / (mag1 * mag2);
}

double No::semanticDistance(No* n, int method){
    double distance = 0;

    if(method == 0){
        for(int i = 0; i < semantic.size(); i++){
            distance += abs(semantic.at(i) - n->semantic.at(i));
        }

        distance = distance / semantic.size();
    }
    else if(method == 1){
        distance = 1 - cosineDistance(semantic, n->semantic);
    }

    return distance;
}

double No::semanticDifference(No* n){
    double difference = 0;

    for(int i = 0; i < semantic.size(); i++){
        if(semantic.at(i) - n->semantic.at(i) != 0){
            difference++;
        }
    }

    return difference;
}

void No::getLinearModel(vector<tuple<double, double> >* linearModel){
//    cout << "getLinearModel " << mask << endl;
    if(type == -1){
        for(No* n : sons){
            n->getLinearModel(linearModel);
        }
    }
    else{
//        cout << "        vai pro linear"<< endl;
        linearModel->push_back(make_tuple(type, value));

//        cout << "        até então" << endl;
//        for(tuple<double, double> t : *linearModel){
//            cout << "(" << get<0>(t) << ", " << get<1>(t) << ") ";
//        }
//        cout<< endl;
    }
}
