
/**
* ***************************************************************************
* - This program will simulate the process of Machine fxinig 
*    through a period of time with one Technican and 3 machines 
* - Every machine finishes its repairing process after "5 units of time" since 
*    it enters the fixing state. 
* - And the every machine automatically breaks after "10 units of time" 
* - The "Time unit" here is simulated as "1 second"
* - The initial values for each machine "break time" is hard coded here
*    just for simplicity. 
* *****************************************************************************
*/


#include <iostream>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <deque>
#include <algorithm>
#include <string>



enum Status
{
	idle,
	busy
};

struct Tech {
	int techNum;
	Status techStaus;

	Tech(int n, Status s) : techNum(n), techStaus(s) {}
};

class Machine
{
public:
	Machine() {}
	Machine(int machine_ID, int timeToBreak)
		: _machine_ID(machine_ID), _timeToBreak(timeToBreak) {}
	
	int get_timeToBreak() const
	{
		return this->_timeToBreak;
	}
	void set_timeToBreak(int t)
	{
		this->_timeToBreak = t;
	}
	void set_next_timeToBreak()
	{
		// The number "10" will be replaced in future with a member field,
		// where every machine has its own "Life time" to be broken. 
		this->_timeToBreak += 10;
	}
	//~Machine() {}

public:
	bool operator < (const Machine& m) const
	{
		return this->_timeToBreak < m._timeToBreak;
	}
	bool operator == (const Machine& m) const
	{
		return this->_machine_ID == m._machine_ID;
	}
	bool operator () (const Machine& a, const Machine& b) const
	{
		return a._timeToBreak < b._timeToBreak;
	}
private:
	int _machine_ID;
	int _timeToBreak;
};


/**
* This data structure represents a machine in fixing state
* and the time it finished its fixing. 
*/
class FixingClock
{
public:
	FixingClock() : _fixTime(0) {}
	FixingClock(Machine m, int fixTime) : _m(m), _fixTime(fixTime) {}
	Machine get_Machine()
	{
		return this->_m;
	}
	int get_FixTime()
	{
		return this->_fixTime;
	}
	void set_FixTime(int ft)
	{
		this->_fixTime = ft;
	}
	void set_Machine(Machine m)
	{
		this->_m = m;
	}
private:
	Machine _m;
	int _fixTime;
};


int main()
{
	// Same as MC
	int global_Timer = 0;

	// Three machines in a double end vector
	std::deque<Machine> machines;
	machines.push_back(std::move(Machine(0, 1)));
	machines.push_back(std::move(Machine(1, 4)));
	machines.push_back(std::move(Machine(2, 9)));


	Tech tech(0, Status::idle);
	// List of broken machines "to be fixed" in queue
	std::queue<FixingClock> fx;

	auto prevTime = std::time(0);
	int counter = -1;
	std::cout << "Tech\t" << "MC\t" << "CL4\t" << "CL3\t" << "CL2\t" << "CL1\t" << "n" << std::endl;
	std::cout << "----\t" << "----\t" << "----\t" << "----\t" << "----\t" << "----\t" << "----" << std::endl;
	//char x; std::cin >> x; 

	while (counter < 20)
	{
		if (std::time(0) - prevTime > 1)
		{
			++counter;
			prevTime = std::time(0);

			if (counter == 0)
			{
				// This is the initial state, the process has just started  
				// No machines in the Fixing queue, "fx" is empty
				std::deque<Machine>::iterator it = std::min_element(machines.begin(), machines.end(),
					[](Machine& a, Machine& b) {
						return a.get_timeToBreak() < b.get_timeToBreak();
					});

				/** Printing job row */
				std::cout << (tech.techStaus == Status::idle ? "Idle\t" : "Busy\t") << global_Timer << "\t" << "-" << "\t" <<
					(machines[2].get_timeToBreak() < 9999 ? std::to_string(machines[2].get_timeToBreak()) : "-") << "\t" <<
					(machines[1].get_timeToBreak() < 9999 ? std::to_string(machines[1].get_timeToBreak()) : "-") << "\t" <<
					(machines[0].get_timeToBreak() < 9999 ? std::to_string(machines[0].get_timeToBreak()) : "-") << "\t" << 
					fx.size() << std::endl;
				/** END Printing job row */
				global_Timer = it->get_timeToBreak();
			}
			else
			{
				if (counter == global_Timer)
				{
					if (fx.empty())
					{
						// get the first machine to break
						std::deque<Machine>::iterator it =
							std::min_element(machines.begin(), machines.end(),
								[&](const Machine& a, const Machine& b) {
									return a.get_timeToBreak() < b.get_timeToBreak();
								});

						// Add this machine to the fixing queue
						fx.push(std::move(FixingClock(*it, it->get_timeToBreak() + 5)));
						tech.techStaus = Status::busy;

						// every machine enters the fixing will be set to "9999" as a flag. 
						// that help us in (std::min_element(...) function) to not include it in the next result. 
						it->set_timeToBreak(9999);

						/** Printing job row */
						std::cout << (tech.techStaus == Status::idle ? "Idle\t" : "Busy\t") << global_Timer << "\t" << fx.front().get_FixTime() << "\t" <<
							(machines[2].get_timeToBreak() < 9999 ? std::to_string(machines[2].get_timeToBreak()) : "-") << "\t" <<
							(machines[1].get_timeToBreak() < 9999 ? std::to_string(machines[1].get_timeToBreak()) : "-") << "\t" <<
							(machines[0].get_timeToBreak() < 9999 ? std::to_string(machines[0].get_timeToBreak()) : "-") << "\t" << 
							fx.size() << std::endl;
						/** END Printing job row */

						// Now find the next machine to be broken or to be fixed 
						// and set the "global_Timer" (MC), since the next event 
						// maybe a broken machine OR a machine has been fixed. 
						it = std::min_element(machines.begin(), machines.end(),
							[&](const Machine& a, const Machine& b) {
								return a.get_timeToBreak() < b.get_timeToBreak();
							});
						if (it->get_timeToBreak() < fx.front().get_FixTime())
							// Then the next event is a broken machine
							global_Timer = it->get_timeToBreak();
						else
							// Then the next event is a machine has been fixed
							global_Timer = fx.front().get_FixTime();
					}
					else // fx (queue) is not empty
					{
						// get the first machine to break
						std::deque<Machine>::iterator it =
							std::min_element(machines.begin(), machines.end(),
								[&](const Machine& a, const Machine& b) {
									return a.get_timeToBreak() < b.get_timeToBreak();
								});
						/*
						* Now check who's happening first and set the global_Time (MC): 
						* a machine is broken OR a machine has been fixed
						*/
						
						// if(true) So the event is a machine is borken.
						if (it->get_timeToBreak() < fx.front().get_FixTime())
						{
							// So the event is a machine is borken.

							// Get the pervious machine in the queue and its time to be fixed
							// and then the fixing time for this machine will be next with five units
							// of time added. 
							int fix_Time = fx.back().get_FixTime() + 5;
							// Add this machine to the fixing queue
							fx.push(std::move(FixingClock(*it, fix_Time)));
							tech.techStaus = Status::busy;
							
							// every machine enters the fixing will be set to "9999" as a flag. 
							// that help us in (std::min_element(...) function) to not include it in the next result. 
							it->set_timeToBreak(9999);

							/** Printing job row */
							std::cout << (tech.techStaus == Status::idle ? "Idle\t" : "Busy\t") << global_Timer << "\t" << fx.front().get_FixTime() << "\t" <<
								(machines[2].get_timeToBreak() < 9999 ? std::to_string(machines[2].get_timeToBreak()) : "-") << "\t" <<
								(machines[1].get_timeToBreak() < 9999 ? std::to_string(machines[1].get_timeToBreak()) : "-") << "\t" <<
								(machines[0].get_timeToBreak() < 9999 ? std::to_string(machines[0].get_timeToBreak()) : "-") << "\t" << 
								fx.size() << std::endl;
							/** END Printing job row */

							// Now find the next machine to be broken or to be fixed 
							// and set the "global_Timer" (MC), since the next event 
							// maybe a broken machine OR a machine has been fixed. 
							it = std::min_element(machines.begin(), machines.end(),
								[&](const Machine& a, const Machine& b) {
									return a.get_timeToBreak() < b.get_timeToBreak();
								});
							if (it->get_timeToBreak() < fx.front().get_FixTime())
								// Then the next event is a broken machine
								global_Timer = it->get_timeToBreak();
							else
								// Then the next event is a machine has been fixed
								global_Timer = fx.front().get_FixTime();
						}

						// the condition if(false) is satisfied So the event is a machine has been fixed.
						else 
						{
							// Get this machine in top of the queue
							FixingClock f = fx.front();
							// Get this machine in the machines vector 
							std::deque<Machine>::iterator it_m = std::find(machines.begin(), machines.end(), f.get_Machine());
							// Set the machine's next break time. 
							it_m->set_timeToBreak(f.get_FixTime());
							it_m->set_next_timeToBreak();
							
							// Remove this machine from the queue
							fx.pop();

							// check if the Fixing queue is empty
							if (!fx.empty())
							{
								/** Printing job row */
								std::cout << (tech.techStaus == Status::idle ? "Idle\t" : "Busy\t") << global_Timer << "\t" << (fx.empty() ? '-' : fx.front().get_FixTime()) << "\t" <<
									(machines[2].get_timeToBreak() < 9999 ? std::to_string(machines[2].get_timeToBreak()) : "-") << "\t" <<
									(machines[1].get_timeToBreak() < 9999 ? std::to_string(machines[1].get_timeToBreak()) : "-") << "\t" <<
									(machines[0].get_timeToBreak() < 9999 ? std::to_string(machines[0].get_timeToBreak()) : "-") << "\t" << 
									fx.size() << std::endl;
								/** END Printing job row */
								std::deque<Machine>::iterator it = std::min_element(machines.begin(), machines.end(),
									[&](const Machine& a, const Machine& b) {
										return a.get_timeToBreak() < b.get_timeToBreak();
									});
								if (it->get_timeToBreak() < fx.front().get_FixTime())
									global_Timer = it->get_timeToBreak();
								else
									global_Timer = fx.front().get_FixTime();
							}
							else
							{
								tech.techStaus = Status::idle;
								counter--;
							}

						}
					}

				}

			}
		}

	}

	return 0;
}

