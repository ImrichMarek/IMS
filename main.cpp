/**
 * File:            main.h
 * Subject:         IMS
 * Project variant: 05 - Výrobný proces zo strojárenskej / poľnohospodárskej výroby
 * Project:         Výroba oceľového puzdra
 * Date:            December 2018
 * Authors:         Samuel Móro xmoros01
 *                  Marek Imrich ximric01
**/

#include "main.h"

Facility counter("Product counter");
int workingTime = 0;
int hodina = 60; //60 minut
int machFailureCnt = 0;
bool workTime = false;


Queue material_1_q; //material pre obrabanie
Queue material_2_q; //material pre frezovanmie

//Denny cyklus -> 8 hodinova pracovna doba
class Day_cycle : public Event {
    void Behavior() {

        if(workTime) {
            workTime = false;
            Activate(Time + (WORK_TIME * hodina));
        }
        else {
            workTime = true;
            (new Machine_1())->Activate();
            Activate(Time + (NON_WORK_TIME * hodina));
        }
    }
};

//--from: http://www.fit.vutbr.cz/~peringer/SIMLIB/examples/model2.html
//Generator nahodnej chyby exp()
class Failure_generator : public Event {
    void Behavior() {
        (new Failure)->Activate();
        Activate(Time+Exponential(RANDOM_FAILURE_TIME));
    }
};

class Material_generator : public Event {
    Material* mtr;
    void Behavior() {
        mtr = new Material;
        mtr->Activate();
    }
};

void StatrMachine_1() {
    if(material_1_q.Length() > 0) {
        Material* mtr = (Material*)material_1_q.GetFirst();
        mtr->workMach = 1;
        mtr->Activate();
    }
}

void StatrMachine_2() {
    if(material_2_q.Length() > 12) {
        Material* mtr = (Material*)material_2_q.GetFirst();
        mtr->workMach = 2;
        mtr->Activate();
    }
}

class Machine_1 : public Process {
    double startTime = Time;
    int madeParts = 0;
    bool errorF1 = false; //indikator chyby - stroj 1
    void Behavior() {
        startTime = Time;
        if (workMach == 1) {
            
        }
    }
};

class Machine_2 : public Process {
    double startTime = Time;
    int madeParts = 0;
    bool errorF2 = false; //indikator chyby - stroj 2
    void Behavior() {
        startTime = Time;
        if (workMach == 2) {
            
        }
    }
};

int main() {
    (new Day_cycle)->Activate;
    (new Failure_generator)->Activate;

	Run();

	return 0;
}
