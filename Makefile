# File:            main.h
# Subject:         IMS
# Project variant: 05 - Výrobný proces zo strojárenskej / poľnohospodárskej výroby
# Project:         Výroba oceľového puzdra
# Date:            December 2018
# Authors:         Samuel Móro    xmoros01
#                  Marek Imrich ximric01

CC =g++
CFLAGS = -g -O2
NAME = main

all: $(NAME).cpp $(NAME).h
	$(CC) $(CFLAGS) -o $(NAME) $(NAME).cpp -lsimlib -lm 

run: $(NAME)
	./$(NAME)

tar:
	tar -zcvf 04_xlabas00_xbanca00.tar.gz $(NAME).cpp $(NAME).h Makefile doc.pdf