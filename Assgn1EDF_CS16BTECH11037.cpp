#include <iostream>
#include <vector>
#include <fstream>
#include <stdio.h>

using namespace std;

struct process{
	int pid;			// process id
	int t;				//time for execution
	int p;     			//period
	int k;				//number of repetitions required
	int time_left;		// time left for execution
	int priority;		// priority 
	int times_ran;		// number of times the process ran
	bool deadline_missed;	// deadlines missed
	bool last;			
	int start,stop;		// starting time and stopping time
	int begin,end;		// begin time and end time

};


struct time_block{		// event block
	int time;
	struct process p;
};
struct Output{		// the final stats output
	int pid;
	int k;
	int ex;
	int turn;
};

bool compare_R(process a, process b){
	return a.priority<b.priority;
}

bool compare_C(time_block a, time_block b){
	if(a.time<b.time) return true;
	else if(a.time==b.time && a.p.priority<b.p.priority) return true;
	return false; 
}

int find_process(vector<process> p, int pid){			// returns index of the process

	for(int i=0;i<p.size();i++){
		if(p.at(i).pid==pid) return i;
	}
	return -1;
}

int find_output(vector<Output> p, int pid){			// returns index of the process

	for(int i=0;i<p.size();i++){
		if(p.at(i).pid==pid) return i;
	}
	return -1;
}

vector<Output> output;
void insert_in_output_ex(process p){
	int pid = p.pid;
	for(int i=0;i<output.size();i++){
		if(output.at(i).pid==pid){
			output.at(i).ex+=(p.stop-p.start);
		}
	}
}
void insert_in_output_turn(process p){
	int pid = p.pid;
	for(int i=0;i<output.size();i++){
		if(output.at(i).pid==pid){
			output.at(i).turn+=(p.end-p.begin);
		}
	}
}

int misses;
int total;

int main(){

	ofstream output_log ;
	output_log.open("EDF_log.txt",ios::out);
	ofstream output_stat;
	output_stat.open("EDF_stat.txt",ios::out);
	ofstream output_average_times;
	output_average_times.open("Average_times.txt",ios::out|ios::app);

	misses=0; total=0;
	process null;
	null.pid=-1; null.p=0; null.t=0; null.k=0; null.time_left=0; null.times_ran=0; null.deadline_missed=false;
	vector<process> ready_queue;
	vector<time_block> critical_queue; 
	int n;cin>>n;
	process P[n];
	int max_time=0;
	for(int i=0;i<n;i++){
		scanf("%d %d %d %d",&P[i].pid,&P[i].t,&P[i].p,&P[i].k);
		total+=P[i].k;
		P[i].time_left=P[i].t;
		P[i].times_ran=1;
		P[i].priority=P[i].p;
		P[i].start=0; P[i].stop=0; P[i].begin=0; P[i].end=0;
		Output o;
		o.pid=P[i].pid;
		o.ex=0;
		o.k=P[i].k;
		o.turn=0;
		output.push_back(o);
		if(max_time<P[i].k*P[i].p) 
			max_time=P[i].k*P[i].p;
	}

	for(int i=0;i<n;i++){
		ready_queue.push_back(P[i]);
		output_log<<"process "<<P[i].pid<<": entered the system\n";
		if(P[i].k>1){
			time_block tb;
			tb.p=P[i]; tb.time=P[i].p;
			tb.p.priority=tb.p.priority+tb.p.p;
			critical_queue.push_back(tb);
		}
	}
	sort(ready_queue.begin(),ready_queue.end(),compare_R);				// maintaining the sorted vector
	sort(critical_queue.begin(),critical_queue.end(),compare_C);		// maintaining the sorted vector
	process current_process=null;
	process new_process;
	//print_critical_queue(critical_queue);
	bool cpu_ideal=false;
	for(int tm=0;tm<2*max_time;tm++){
	
		if(current_process.pid!=null.pid){
			
			//output_log<<"Process"<<current_process.pid<<": k:"<<current_process.k<<" times ran"<<current_process.times_ran<<endl;
			output_log<<"Process "<<current_process.pid<<": finishes execution at time "<<tm<<endl;
			current_process.end=tm;
			insert_in_output_turn(current_process);
			current_process.stop=tm;
			insert_in_output_ex(current_process);
			current_process=null;
		}

		if(ready_queue.size()!=0) {
			current_process = ready_queue.front();		// take first job from the ready queue and start executing it
		}else{
			cpu_ideal=true;
			//output_log<<"running"<<tm<<endl;
			if(critical_queue.size()==0) break;
			/////else print_critical_queue(critical_queue);
				// block to add new processes in the queue if no current process is executing
// new process is entering
			if(tm==critical_queue.front().time){
				current_process=critical_queue.front().p;
				current_process.begin=tm;
				ready_queue.push_back(current_process);
				sort(ready_queue.begin(),ready_queue.end(),compare_R);	
				//output_log<<"1"<<endl;
				critical_queue.erase(critical_queue.begin());
				//output_log<<"2"<<endl;
// new time block is being created
				if(current_process.times_ran<current_process.k-1){
					//output_log<<"time block created: "<<current_process.pid<<"times ran->"<<current_process.times_ran<<endl;
						time_block tb;
						tb.time=tm+current_process.p;
						current_process.priority=current_process.priority+current_process.p;
						current_process.times_ran++;
						tb.p=current_process;
						critical_queue.push_back(tb);
						sort(critical_queue.begin(),critical_queue.end(),compare_C);
				}
			}
// other new processes are entering			
			while(critical_queue.size()!=0 && tm==critical_queue.front().time){
				process next_process = critical_queue.front().p;
				next_process.begin=tm;
				ready_queue.push_back(next_process);
				sort(ready_queue.begin(),ready_queue.end(),compare_R);				// maintaining the sorted vector

				critical_queue.erase(critical_queue.begin());
// new time block is being created
				if(next_process.times_ran<next_process.k-1){
						//output_log<<"time block created: "<<next_process.pid<<"times ran->"<<next_process.times_ran<<endl;

						time_block tb;
						tb.time=tm+next_process.p;
						next_process.priority=next_process.priority+next_process.p;
						next_process.times_ran++;
						tb.p=next_process;
						critical_queue.push_back(tb);
						sort(critical_queue.begin(),critical_queue.end(),compare_C);
					}
			}
		}
		// new process entering check
		bool first_run=true;
// current process starts 
		while(current_process.time_left!=0){
			if(cpu_ideal==true){
				output_log<<"CPU idle till "<<tm<<endl;
				cpu_ideal=false;
			}
			//output_log<<"@##@"<<current_process.times_ran<<endl;

			// if new process is arriving at the same time
			if(critical_queue.size()!=0 && tm==critical_queue.front().time){	   
				//output_log<<"hahahahaha"<<critical_queue.front().time<<endl;
				process new_process=critical_queue.front().p;				// if its critical time
				critical_queue.erase(critical_queue.begin());
				//print_critical_queue(critical_queue);
				// check if this is process is already in ready queue
				int index=find_process(ready_queue,new_process.pid);
// deadline missed case 
				if(index!=-1){		// deadline miss
					output_log<<"Process "<<new_process.pid<<": missed deadline at time "<<tm<<endl;
					misses++;
					new_process.stop=tm;
					insert_in_output_ex(new_process);
					new_process.end=tm;
					insert_in_output_turn(new_process);
					//terminate that process
					ready_queue.erase(ready_queue.begin()+index);
					
// current process is being preempted, new process is starting
					if(!new_process.last){
						ready_queue.push_back(new_process);
						sort(ready_queue.begin(),ready_queue.end(),compare_R);	
						if(current_process.priority>new_process.priority){
							//preempt current process
							//output_log<<"Process "<<current_process.priority<< "preempted by "<<new_process.priority<<endl;
							output_log<<"Process "<<current_process.pid<<" preempted by Process"<<new_process.pid<<" at time "<<tm<<endl;
							int ind=find_process(ready_queue,current_process.pid);
							current_process.stop=tm;
							insert_in_output_ex(current_process);
							// remove the current process from ready queue
							ready_queue.erase(ready_queue.begin()+ind);
							// add it again to the ready queue
							ready_queue.push_back(current_process);
							// make current process = new process
							current_process=new_process;
							//sort ready queue
							sort(ready_queue.begin(),ready_queue.end(),compare_R);	
						}
						else if(current_process.pid==new_process.pid){
							current_process=new_process;
						}
					}
				}else{
//new process is entering
					if(new_process.priority>=current_process.priority){
						new_process.begin=tm;
						ready_queue.push_back(new_process);
						sort(ready_queue.begin(),ready_queue.end(),compare_R);	
					}else{
// current process preempting, new process entering
						//preempt current process
						//output_log<<"Process "<<current_process.priority<< "preempted by "<<new_process.priority<<endl;
						output_log<<"Process "<<current_process.pid<<" preempted by Process"<<new_process.pid<<" at time "<<tm<<endl;
						int ind=find_process(ready_queue,current_process.pid);
						// remove the current process from ready queue
						ready_queue.erase(ready_queue.begin()+ind);
						// add it again to the ready queue
						ready_queue.push_back(current_process);
						// add new process to the ready queue
						current_process.stop=tm;
						insert_in_output_ex(current_process);
						// insert_in_output_stop();
						// insert_in_output_begin();
						new_process.begin=tm;
						ready_queue.push_back(new_process);
						// make current process = new process
						current_process=new_process;
						//sort ready queue
						sort(ready_queue.begin(),ready_queue.end(),compare_R);	

					}
				}
				//create new time block for new process
// new time block is being created
				if(new_process.times_ran<new_process.k-1){
					//output_log<<"time block created: "<<new_process.pid<<"times ran->"<<new_process.times_ran<<endl;

					time_block tb;
					tb.time=tm+new_process.p;
					new_process.times_ran++;
					new_process.priority=new_process.priority+new_process.p;
					tb.p=new_process;
					critical_queue.push_back(tb);
					sort(critical_queue.begin(),critical_queue.end(),compare_C);
				}
				// for other processes that comes along
				while(critical_queue.size()!=0 && tm==critical_queue.front().time){
					//output_log<<"imp:: process got in simultaniously "<<critical_queue.front().p.pid<<endl;
					process next_process=critical_queue.front().p;				// if its critical time
					critical_queue.erase(critical_queue.begin());

					int index=find_process(ready_queue,next_process.pid);	
// other processes missed deadline and next process entering
					if(index!=-1){	
						process missed_process = ready_queue.at(index);											// if deadline is missed
						output_log<<"Process "<<next_process.pid<<": missed deadline at time "<<tm<<endl;
						misses++;
						// insert_in_output_stop();
						// insert_in_output_end();
						missed_process.stop=tm;
						insert_in_output_ex(missed_process);
						missed_process.end=tm;
						insert_in_output_turn(missed_process);
						//terminate that process
						ready_queue.erase(ready_queue.begin()+index);
						// make times ran ++
						//next_process.times_ran++;
						// add this process to ready queue
						// insert_in_output_begin();
						next_process.begin=tm;
						ready_queue.push_back(next_process);
						sort(ready_queue.begin(),ready_queue.end(),compare_R);
					}else{
// next process entering
						// insert_in_output_begin();
						next_process.begin=tm;
						ready_queue.push_back(next_process);
						sort(ready_queue.begin(),ready_queue.end(),compare_R);
					}
					// create new time block
// new time block being created
					if(next_process.times_ran<next_process.k-1){
						//output_log<<"time block created: "<<next_process.pid<<"times ran->"<<next_process.times_ran<<endl;
						time_block tb;
						tb.time=tm+next_process.p;
						next_process.priority=next_process.priority+next_process.p;
						next_process.times_ran++;
						tb.p=next_process;
						critical_queue.push_back(tb);
						sort(critical_queue.begin(),critical_queue.end(),compare_C);
					}
				}
				
			}
//start execution
			if(current_process.time_left==current_process.t) {
				// insert_in_output_start();
				current_process.start=tm;
				output_log<<"Process "<<current_process.pid<<": starts execution at time "<<tm<<endl;
				first_run=false;
			}
			else if(first_run==true){
// resumes execution
				// insert_in_output_start();
				current_process.start=tm;
				output_log<<"Process "<<current_process.pid<<": resumes execution at time "<<tm<<endl;
				first_run=false;
			}

			current_process.time_left--;

			if(current_process.time_left!=0) 
				tm++;
		}

		// message_time_block(critical_queue,current_process.pid);	
		//if(current_process.pid==3) print_critical_queue(critical_queue);
		// if(ind!=-1) {
		// 	critical_queue.at(ind).p.times_ran++;
		// 	if(critical_queue.at(ind).p.times_ran==critical_queue.at(ind).p.k){
		// 		critical_queue.erase(critical_queue.begin()+ind);
		// 	}
		// }
		// if(ready_queue.size()!=0) {
		// 	int index=find_process(ready_queue,current_process.pid);
		// 	if(index!=-1) ready_queue.erase(ready_queue.begin()+index);
		// }
		if(current_process.pid!=null.pid){
			int ind = find_process(ready_queue,current_process.pid);
			if(ind!=-1){
				ready_queue.erase(ready_queue.begin()+ind);
			}
		}
	}
	
	output_stat<<"total number of processes that came into the system "<<total<<endl;
	output_stat<<"total number of successful processes "<<total-misses<<endl;
	output_stat<<"total number of processes that missed deadline "<<misses<<endl;


	long avg_wait=0,avg_turn=0;
	for(int i=0;i<output.size();i++){
		long wait=(output.at(i).turn-output.at(i).ex)/output.at(i).k;
		long turn=(output.at(i).turn)/output.at(i).k;
		if(wait<0)
			output_stat<<"average waiting time for process "<<output.at(i).pid<<" is "<<-wait<<endl;
		else
			output_stat<<"average waiting time for process "<<output.at(i).pid<<" is "<<wait<<endl;
		avg_wait+=wait;
		avg_turn+=turn;
	}

	avg_wait=avg_wait/output.size();
	avg_turn=avg_turn/output.size();
	
	output_average_times<<"average waiting time for edf: "<<avg_wait<<endl;
	output_average_times<<"average turnaround time for edf: "<<avg_turn<<endl;
	

	return 0;
}







