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
Facility Lathe("Lathe");
Facility Cutter("Cutter");
Histogram Table("Table",0,25,20);

const int hodina = 60; //hodina ->60 minut
const int den = 1440; //hodina ->1440 minut
const int rok = 525600; //hodina ->525600 minut
int machFailureCnt = 0;
bool machFailureSet = false;
bool workTime = false;

int working_lathe = 0;
int working_cutter = 0
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
            Activate(Time + (WORK_TIME * hodina));
        }
        else {
            workTime = true;
            (new Material_generator())->Activate();
            Activate(Time + (NON_WORK_TIME * hodina));
        }
    }
};

//Generator nahodnej chyby exp()
class Failure_generator : public Event {
    void Behavior() {
        (new Failure)->Activate();
        Activate(Time + Exponential(RANDOM_FAILURE_TIME));
    }
};

//generator sustruhov
class Lathe_generator : public Event {
    int LATHE; //pocet sustruhov
    int LATHE_IN_WORK; //aktualne v prevádzke
    void Behavior() {
        if(workTime && (working_lathe < LATHE)) {
            (new Lathe)->Activate();
            working_lathe++;
            Activate(Time + workTime);
        }
};

//generator frez
class Cutter_generator : public Event {
    int CUTTER; //pocet sustruhov
    int CUTTER_IN_WORK; //aktualne v prevádzke
    void Behavior() {
        if(workTime && (working_cutter < CUTTER)) {
            (new Cutter)->Activate();
            working_cutter++;
            Activate(Time + workTime);
        }
};

class Lathe : public Process {
    double startTime;
    double endTime;
    public : int todesk;
    void Behavior() {
        startTime = Time;

        while(1){
            if(Time - startTime < workTime){ //ak neskoncila pracovna doba
                Wait(30);
            }
            
            double d = Random(); //1% -> chybny kus
            if(d < 0.01){ //nastala chyba
                Wait(REPAIR_TIME);
            }
            else{ //nenastala chyba
                (new Cutter_generator)->Activate();
                if (lathe_products = 270) {
                    (new Repair)->Activate();
                }
            }
        }
};

class Insert_steel : public Event {
    void Behavior() {
        while (!queue_material1.Empty()) {
            bool full = false;
            for(int i = 0; i < LATHE; i++) {
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

class Steel_generator : public Event {
    int steelNB = 0;
    void Behavior() {
        (new Steel)->Activate();
        steelNB++;
        
        if (steelNB < PIECES_NB) {
            Activate(Time + Uniform(0,2));
        }
        
        public:
        Steel_generator(int stl) : PIECES_NB(stl) {
            steelNB = 0;
            Activate();
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

class Repair : public Process {
    Seize(Lathe);
    Wait(10);
    Release(Lathe);
};

int main() {
    init(0, SIM_TIME); //cas simulacie 0..SIM_TIME
    (new Day_cycle)->Activate();
    (new Failure_generator)->Activate();
    (new Lathe_generator)->Activate();
    (new Insert_steel)->Activate();
    (new Steel_generator)->Activate();
    
	Run();

    Box.Output();
    Table.Output();

	return 0;
}
