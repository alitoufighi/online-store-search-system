CC=g++
C_FLAGS=-g -O0
# C_FLAGS=-g

all: worker loadbalancer presenter

worker: worker.cpp
	$(CC) $(C_FLAGS) worker.cpp -o worker

loadbalancer: lb.cpp
	$(CC) $(C_FLAGS) lb.cpp -o loadbalancer
	
presenter: presenter.cpp
	$(CC) $(C_FLAGS) presenter.cpp -o presenter

clean:
	rm worker loadbalancer presenter