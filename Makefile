#  File    Makefile
#  Author  Tomas Beranek <xberan46@stud.fit.vutbr.cz>
#  Date    1.3.2022
#  Up2date sources can be found at: https://github.com/TomasBeranek/but-flp-project-1

all: run

run:
	./test

zip:
	zip xberan46.zip oems.c test xberan46.pdf
