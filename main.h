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
#include <iostream>
#include <limits>

#define SIM_TIME 10000000	    //cas simulacie v hodinach
#define WORK_TIME 8             //cas pracovnej doby v hodinach
#define NON_WORK_TIME 16        //cas mimopracovnej doby v hodinach
#define REPAIR_TIMEOUT 5        //čas opravy po chybe

#define LATHE_CAPACITY 1        //kapacita prveho stroja
#define LATHE_ROTATIONS_NB 1    //pocet otacok
#define LATHE_HARD_REPAIR 270   //pocet ks, kedy je vynutena vymena

#define CUTTER_CAPACITY 12      //kapacita druheho stroja
#define CUTTER_ROTATIONS_NB 1   //pocet otacok
#define CUTTER_HARD_REPAIR 900  //pocet ks, kedy je vynutena vymena

#define PIECES_NB 1000          //pocet kusov materiálu na vstupe

