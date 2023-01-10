CFLAGS=-g -std=c++11 
CC=g++

sched: sched.cpp
	$(CC) $(CFLAGS) FCFS_Sched.cpp Scheduler.cpp sched.cpp Event.cpp DES_Layer.cpp Process.cpp Number_Generator.cpp LCFS_Sched.cpp SRTF_Sched.cpp RR_Sched.cpp PRIO_Sched.cpp PREPRIO_Sched.cpp -o sched 
clean:
	rm -f sched *~
