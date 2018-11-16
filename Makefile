CC=g++
C_FLAGS=-g -O0

all: worker presenter loadbalancer 

worker: worker.cpp defines.hpp
	$(CC) $(C_FLAGS) worker.cpp -o worker
	
presenter: presenter.cpp defines.hpp strlib.cpp strlib.hpp globals.hpp
	$(CC) $(C_FLAGS) strlib.cpp presenter.cpp -o presenter

loadbalancer: loadbalancer.cpp defines.hpp strlib.cpp strlib.hpp
	$(CC) $(C_FLAGS) strlib.cpp loadbalancer.cpp -o loadbalancer

clean:
	rm worker loadbalancer presenter