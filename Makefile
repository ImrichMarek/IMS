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
OUTPUT = ims

all: $(NAME).cpp $(NAME).h
	$(CC) $(CFLAGS) -o $(OUTPUT) $(NAME).cpp -lsimlib -lm 

run: $(OUTPUT)
	./$(OUTPUT)

clean:
	rm -f ims

zip:
	zip 05_xmoros01_ximric01.zip $(NAME).cpp $(NAME).h Makefile doc.pdf