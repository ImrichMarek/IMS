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

//Facility counter("Product counter");
Facility lathe("lathe");
Facility cutter("cutter");
//Histogram Table("Table",0,25,20);

const int hodina = 60; //hodina ->60 minut
const int den = 1440; //hodina ->1440 minut
const int rok = 525600; //hodina ->525600 minut
int machFailureCnt = 0;
bool machFailureSet = false;
bool workTime = false;

int working_lathe = 0;
int working_cutter = 0;
int inserted_mtr = 0;
int lathe_products = 0;
int cutter_products = 0;


Queue queue_material1; //material pre obrabanie
Queue queue_material2; //material pre frezovanmie
Queue queue_lathe[100]; //

//Denny cyklus -> 8 hodinova pracovna doba
class Day_cycle : public Event {
    void Behavior() {
        if(workTime) {
            workTime = false;
            Activate(Time + (NON_WORK_TIME * hodina));
        }
        else {
            workTime = true;
            (new Steel_generator())->Activate();
            Activate(Time + (WORK_TIME * hodina));
        }
    }
};

//Generator nahodnej chyby sustruhu exp()
class Lathe_failure_generator : public Event {
    void Behavior() {
        (new Lathe_failure)->Activate();
        Activate(Time + Exponential(RANDOM_FAILURE_TIME));
    }
};

//Generator nahodnej chyby frezy exp()
class Cutter_failure_generator : public Event {
    void Behavior() {
        (new Cutter_failure)->Activate();
        Activate(Time + Exponential(RANDOM_FAILURE_TIME));
    }
};

class Lathe_failure : public Process {
    void Behavior() {
        machFailureCnt++;
        Seize(lathe);
        Wait(Exponential(REPAIR_TIMEOUT));
        Release(lathe);
    }
};

class Cutter_failure : public Process {
    void Behavior() {
        machFailureCnt++;
        Seize(cutter);
        Wait(Exponential(REPAIR_TIMEOUT));
        Release(cutter);
    }
};

//generator sustruhov
class Lathe_generator : public Event {
    void Behavior() {
        if(workTime && (working_lathe < LATHE_NB)) {
            (new Lathe)->Activate();
            working_lathe++;
            Activate(Time + workTime);
        }
    }
};

//generator frez
class Cutter_generator : public Event {
    void Behavior() {
        if(workTime && (working_cutter < CUTTER_NB)) {
            (new Cutter)->Activate();
            working_cutter++;
            Activate(Time + workTime);
        }
    }
};

class Steel_generator : public Event {
    int steelNB = 0;
    int piecesNB = PIECES_NB;
    void Behavior() {
        (new Steel)->Activate();
        steelNB++;
        
        if (steelNB < PIECES_NB) {
            Activate(Time + Uniform(0,2));
        }
    }
    public:
        Steel_generator(int stl) : piecesNB(stl) {
            steelNB = 0;
            Activate();
        }
};

class Insert_steel : public Event {
    void Behavior() {
        while (!queue_material1.Empty()) {
            bool full = false;
            for(int i = 0; i < LATHE_NB; i++) {
                if(queue_lathe[i].Length() >= LATHE_CAPACITY) {
                    full = true;
                    for(int j=0; j < LATHE_CAPACITY; j++) {
                        queue_lathe[i].GetFirst()->Activate();
                    }
                    break;
                }
            }
            if (full) {
                (queue_material1.GetFirst())->Activate();
            }
            else {
                break;
            }
        }
        Activate(Time + 5);
    }
};

class Lathe : public Process {
    double startTime;
    double endTime;
    void Behavior() {
        startTime = Time;
        while(1) {
            if(workTime) { //ak neskoncila pracovna doba
                Wait(30);
                double d = Random(); //1% -> chybny kus
                if (d < 0.01) { //nastala chyba
                    (new Repair_lathe)->Activate();
                }
                else { //nenastala chyba
                    (new Cutter_generator)->Activate();
                    if (lathe_products = 270) { //po 270 kusoch nutena vymena brusky
                        (new Repair_lathe)->Activate();
                    }
                }
            }
        }
    }
};

class Cutter : public Process {
    double startTime;
    double endTime;
    void Behavior() {
        startTime = Time;
        while(1) {
            if(workTime) { //ak neskoncila pracovna doba
                Wait(30);
                double d = Random(); //1% -> chybny kus
                if (d < 0.01) { //nastala chyba
                    (new Repair_cutter)->Activate();
                }
                else { //nenastala chyba
                        cutter_products++;
                }
            }
        }
    }
};

class Steel : public Process {
    void Behavior() {
        double insert_time = Time;
        inserted_mtr++;
        
        //material ide do sustruhu, kontrola ci je nejaky dostupny
        if (working_lathe == 0) {
                queue_material1.Insert(this);
                Passivate();
        }
        
    }
};

class Repair_lathe : public Process {
    void Behavior() {
        machFailureCnt++;
        Seize(lathe);
        Wait(Exponential(REPAIR_TIMEOUT));
        Release(lathe);
    }
};

class Repair_cutter : public Process {
    void Behavior() {
        machFailureCnt++;
        Seize(cutter);
        Wait(Exponential(REPAIR_TIMEOUT));
        Release(cutter);
    }
};

int main() {
    Init(0, SIM_TIME); //cas simulacie 0..SIM_TIME
    (new Day_cycle)->Activate();
    (new Lathe_failure_generator)->Activate();
    (new Cutter_failure_generator)->Activate();
    (new Lathe_generator)->Activate();
    (new Cutter_generator)->Activate();
    (new Insert_steel)->Activate();
    
	Run();

    //Table.Output();

	return 0;
}
