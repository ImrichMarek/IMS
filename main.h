/**
 * File: 		    main.h
 * Subject:         IMS
 * Project variant: 05 - Výrobný proces zo strojárenskej / poľnohospodárskej výroby
 * Project:		    Výroba oceľového puzdra
 * Date:		    December 2018
 * Authors:		    Samuel Móro	xmoros01
 *				    Marek Imrich ximric01
**/

#include <simlib.h>
#include <cstdio>
#include <cstring>

#define END_TIME 1000		    //cas simulacie
#define WORK_TIME 8             //cas pracovnej doby
#define NON_WORK_TIME 16        //cas mimopracovnej doby
#define SET_MACHINE_TIME 1      //cas nastavenia stroja
#define REPAIR_TIME 1           //cas opravy / vymeny suciastky stroja
#define RANDOM_FAILURE_TIME 240  //cas vyskytu nahodnej chyby exp()

#define ROTATIONS 1             //pocet otacok
#define PIECES_NB 100           //pocet kusov

void OpenNewDesk();
