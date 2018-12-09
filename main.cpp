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

Facility lathe("lathe");
Facility cutter("cutter");

Store inputMaterial1("vstupny_materia_sustruhu", std::numeric_limits<unsigned long>::max());
Store inputMaterial2("vstupny_materia_frezy", std::numeric_limits<unsigned long>::max());
Store product("vystupny_produkt", std::numeric_limits<unsigned long>::max());

const int hodina = 60; //hodina ->60 minut
const int den = 1440; //hodina ->1440 minut
const int rok = 525600; //hodina ->525600 minut
int machFailureCnt = 0;
bool machFailureSet = false;
bool workTime = false;

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

class Linkk : public Process {
    double startTime;
    double endTime;
    public : int workst;
    public : int lathe_products;
    public : int cutter_products;
    void Behavior() {
        startTime = Time;
        if (workst == 1) {
            Lathe();
        }
        if (workst == 2) {
            Cutter();
        }
    }
    
    void Lathe() {
        if(workTime) { //ak neskoncila pracovna doba
            bool failure = false;
            Leave(inputMaterial1, LATHE_CAPACITY);
            for(int i = 0; i < 3; i++) {
                double d = Random(); //1% -> chybny kus
                if (d < 0.01 || lathe_products == LATHE_HARD_REPAIR) { //nastala chyba
                    (new Lathe_failure)->Activate();
                    failure = true;
                }
            }
            if (!failure) {
                lathe_products++;
                Enter(inputMaterial2, 1);
            }
        }
    }

    void Cutter() {
        if(workTime) { //ak neskoncila pracovna doba
            bool failure = false;
            Leave(inputMaterial2, CUTTER_CAPACITY);
            for(int i = 0; i < 2; i++) {
                double d = Random(); //1% -> chybny kus
                if (d < 0.01 || lathe_products == CUTTER_HARD_REPAIR) { //nastala chyba
                    (new Cutter_failure)->Activate();
                    failure = true;
                }
            }
            if (!failure) {
                cutter_products++;
                Enter(product, 12);
            }
        }
    }
};

//nalní "store" vstupnym materialom
class Insert_steel : public Process {
    void Behavior() {
        Enter(inputMaterial1, PIECES_NB);
    }
};

//Denny cyklus -> 8 hodinova pracovna doba
class Day_cycle : public Event {
    void Behavior() {
        if(!workTime) {
            workTime = true;
            (new Insert_steel)->Activate();
            Activate(Time + (WORK_TIME * hodina));
        }
        else {
            workTime = false;
            Activate(Time + (NON_WORK_TIME * hodina));
        }
    }
};

//generator výrobných liniek
class Link_generator : public Event {
    Linkk* lnk;
    void Behavior() {
        for (int i = 0; i < (LATHE_NB); i++) {
            lnk = new Linkk;
            lnk->workst = 1; //start lathe
            lnk->Activate();
        }
        
        for (int i = 0; i < (CUTTER_NB); i++) {
            lnk = new Linkk;
            lnk->workst = 1; //start cutter
            lnk->Activate();
        }
    }
};

int main() {
    Init(0, SIM_TIME); //cas simulacie 0..SIM_TIME
    (new Day_cycle)->Activate();
    (new Link_generator)->Activate();
    
	Run();

	return 0;
}
