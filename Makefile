CC=g++
C_FLAGS=-g -O0
# C_FLAGS=-g

all: worker loadbalancer presenter

worker: worker.cpp defines.hpp
	$(CC) $(C_FLAGS) worker.cpp -o worker

loadbalancer: lb.cpp defines.hpp strlib.cpp strlib.hpp
	$(CC) $(C_FLAGS) lb.cpp strlib.cpp -o loadbalancer
	
presenter: presenter.cpp defines.hpp strlib.cpp strlib.hpp
	$(CC) $(C_FLAGS) presenter.cpp strlib.cpp -o presenter

clean:
	rm worker loadbalancer presenter