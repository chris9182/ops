#include <iostream>
#include "Point.h"
#include "Ameisen.h"
#include <iomanip>
#include <random>

inline std::ostream& operator<<(std::ostream& o, Ameisen& p) { return p.print(o); }
//inline std::ostream& operator<<(std::ostream& o, Point& p) { return p.print(o); }

int main() {


    time_t seed;
    time(&seed);
    srand (1);
    LogicType type=LOGIC_TYPE;
    double lower_bound = 0;
    double upper_bound = 100;

    const unsigned int pointcount=20;
    const unsigned short dim=2;

    Point *points[pointcount];

    double f;
    for (unsigned int i=0;i<pointcount;i++){
        double *c=new double[dim];
        for(int j=0;j<dim;j++) {
            f = (double)rand() / RAND_MAX;
            c[j]= lower_bound + f * (upper_bound - lower_bound);
        }
        points[i]=new Point(dim,c);
    }



    Ameisen a(pointcount,points,type);
    std::cout<<a;
    a.printprob(std::cout);
    bool go=true;
    std::string input;




    while(go) {


        std::getline(std::cin, input);
        if (input == "") {
            go=false;
        }
        else {
            int count = atoi(input.c_str());
            if (!count){a.printscore(std::cout);}
            else {
                a.calculate(count);
                a.printprob(std::cout);
            }
        }
    }




    std::cout<<"muahaha";
    for (unsigned int i=0;i<pointcount;i++){

        delete points[i];
    }

    return 0;
}