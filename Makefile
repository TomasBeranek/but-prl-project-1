#  File    Makefile
# Author  Tomas Beranek <xberan46@stud.fit.vutbr.cz>
# Brief   A simple SW implementation of Odd-even merge sort using OpenMPI
# Date    9.4.2022
# Up2date sources can be found at: https://github.com/TomasBeranek/but-prl-project-1

all: run

run:
	./test

zip:
	zip xberan46.zip oems.c test xberan46.pdf
