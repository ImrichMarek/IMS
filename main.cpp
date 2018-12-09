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

const int minuta = 1; //1 minuta
const int hodina = 60; //1 hodina ->60 minut
const int den = 1440; //1 den ->1440 minut
const int rok = 525600; //1 rok ->525600 minut
int machFailureCnt = 0;
bool machFailureSet = false;
bool workTime = false;
int latheFailureCnt = 0;
int cutterFailureCnt = 0;
int lathe_products = 0;
int cutter_products = 0;
int latheALLCnt = 0;
int cutterALLCnt = 0;


class Lathe_failure : public Process {
    void Behavior() {
        latheFailureCnt++;
        Seize(lathe);
        Wait(Exponential(REPAIR_TIMEOUT));
        Release(lathe);
    }
};

class Cutter_failure : public Process {
    void Behavior() {
        cutterFailureCnt++;
        Seize(cutter);
        Wait(Exponential(REPAIR_TIMEOUT));
        Release(cutter);
    }
};

class Linkk : public Process {
    double startTime;
    double endTime;
    public : int workst;
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
            latheALLCnt++;
            bool failure = false;
            Leave(inputMaterial1, LATHE_CAPACITY);
            for(int i = 0; i < 3; i++) {
                double d = Random(); //1% -> chybny kus
                if (((latheALLCnt % LATHE_HARD_REPAIR) == 0) && (latheALLCnt != 0)) {
                    (new Lathe_failure)->Activate();
                    failure = true;
                    break;
                }
                else if (d <= 0.01) { //nastala chyba
                    (new Lathe_failure)->Activate();
                    failure = true;
                    break;
                }
                double e = Random();
                double c = ((LATHE_ROTATIONS_NB/minuta)*(LATHE_HARD_REPAIR/PIECES_NB)/100);
                if (e <= c) {
                    (new Lathe_failure)->Activate();
                    failure = true;
                }
            }
            if (!failure) {
                lathe_products++;
                Enter(inputMaterial2, LATHE_CAPACITY);
                if (((inputMaterial2.Used() % CUTTER_CAPACITY) == 0) && (inputMaterial2.Used() != 0)) {
                    Linkk* lnk = new Linkk;
                    lnk->workst = 2; //start cutter
                    lnk->Activate();
                }
            }
        }
    }

    void Cutter() {
        if(workTime) { //ak neskoncila pracovna doba
            cutterALLCnt++;
            bool failure = false;
            Leave(inputMaterial2, CUTTER_CAPACITY);
            for(int i = 0; i < 2; i++) {
                double d = Random(); //1% -> chybny kus
                if (((cutterALLCnt % LATHE_HARD_REPAIR) == 0) && (cutterALLCnt != 0)) {
                    (new Lathe_failure)->Activate();
                    failure = true;
                    break;
                }
                else if (d < 0.01) { //nastala chyba
                    (new Cutter_failure)->Activate();
                    failure = true;
                    break;
                }
                double e = Random();
                double c = ((CUTTER_ROTATIONS_NB/minuta)*(CUTTER_HARD_REPAIR/CUTTER_CAPACITY)/100);
                if (e <= c) {
                    (new Cutter_failure)->Activate();
                    failure = true;
                    break;
                }
            }
            if (!failure) {
                cutter_products++;
                Enter(product, CUTTER_CAPACITY);
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
        for (int i = 0; i < PIECES_NB; i++) {
            lnk = new Linkk;
            lnk->workst = 1; //start lathe
            lnk->Activate();
        }
    }
};

int main() {
    Init(0, SIM_TIME*hodina); //cas simulacie 0..SIM_TIME
    (new Day_cycle)->Activate();
    (new Link_generator)->Activate();
    
	Run();
    
    std::cout<<"/***********************************************************************/\n\n";
    std::cout<<"/**************************  Vstupy simulácie  *************************/\n\n";
    std::cout<<"/***********************************************************************/\n\n";
    std::cout<<"Celkový čas simulácie bol nastavený na: "<<SIM_TIME<<" hodín.\n";
    std::cout<<"Pracovná doba bola nastavená na: "<<WORK_TIME<<" hodín.\n";
    std::cout<<"Pracovná doba bola nastavená na: "<<NON_WORK_TIME<<" hodín.\n";
    std::cout<<"Čas opravy stroja po chybe exp() bol nastavený na: "<<REPAIR_TIMEOUT<<" hodín.\n\n";
    
    std::cout<<"Celkový počet vstupného materálu bol nastavený na "<<PIECES_NB<<" ks.\n\n";
    
    std::cout<<"Kapacita sústruhu bola nastavená na: "<<LATHE_CAPACITY<<" ks materíalu.\n";
    std::cout<<"Otáčky sústruhov boli nastavený na: "<<LATHE_ROTATIONS_NB<<" ot/min.\n";
    std::cout<<"Nútená výmena vrtáku u sústruhov bola nastavená po opracovaní: "<<LATHE_HARD_REPAIR<<" ks.\n\n";
    
    std::cout<<"Kapacita fréz bola nastavená na: "<<CUTTER_CAPACITY<<" ks materíalu.\n";
    std::cout<<"Otáčky fréz boli nastavený na: "<<CUTTER_ROTATIONS_NB<<" ot/min.\n";
    std::cout<<"Nútená výmena vrtáku u frézy bola nastavená po opracovaní: "<<CUTTER_HARD_REPAIR<<" ks.\n\n\n\n";

    std::cout<<"/***********************************************************************/\n\n";
    std::cout<<"/*************************  Výstupy simulácie  *************************/\n\n";
    std::cout<<"/***********************************************************************/\n\n";
    
    std::cout<<"Počet kusov, ktore vyrobil sústruh bol: "<<lathe_products<<" ks.\n";
    std::cout<<"Počet chýb, kedy zlyhanie zapríčinil sústruh bolo: "<<latheFailureCnt<<".\n";
    
    std::cout<<"Počet kusov, ktore vyrobila fréza bol: "<<cutter_products*CUTTER_CAPACITY<<".\n";
    std::cout<<"Počet chýb, kedy zlyhanie zapríčinila fréza bolo: "<<cutterFailureCnt*CUTTER_CAPACITY<<".\n";

	return 0;
}
