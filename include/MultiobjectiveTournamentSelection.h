#ifndef MULTIOBJECTIVETOURNAMENTSELECTION_H
#define MULTIOBJECTIVETOURNAMENTSELECTION_H
#include "Selection.h"
#include "Subject.h"

class MultiobjectiveTournamentSelection : public Selection{
    public:
        MultiobjectiveTournamentSelection();
        virtual ~MultiobjectiveTournamentSelection();

        virtual Subject * selects(Subject ** pop){
            int a = rand()%conf->popSize;
            int b = rand()%conf->popSize;

            Subject * x = pop[a];
            Subject * y = pop[b];

            if(x->ranking < y->ranking) {
//                cout << "Selec " << a << endl;
//                cin.get();
                return x;
            }
            else if (x->ranking == y->ranking && x->crowdingDistance >= y->crowdingDistance){
//                cout << "Selec " << a << endl;
//                cin.get();
                return x;
            }
//            cout << "Selec " << b << endl;
//            cin.get();
            return y;
        }

    protected:

    private:
};

#endif // MULTIOBJECTIVETOURNAMENTSELECTION_H
