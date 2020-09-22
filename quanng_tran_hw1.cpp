#include <stdio.h>
#include <string>
#include <iostream>
#include <string>
#include <queue> 
#include <iomanip>

using namespace std;
struct Event
{
	int pid;
	int time;//time finish
	int time_ex;//time executed
	string event;
	bool operator < (const Event& p) const
	{
		return p.time < time;
	}
};
//-------------
struct Node {
	string key;
	int time;
	struct Node* pNext;
};
typedef struct Node NODE;
//-------------
struct List
{
	NODE* pHead;
	NODE* pTail;
};
typedef struct List LIST;
//========
LIST a[1000];
int vtp[1000];
int ncore, nprocess, nfreecores, ssd, current_time, isinteract;
priority_queue<Event> e;
queue<int>i_queue, ni_queue, ssdq;
//------
int totalprocesses = 0, totalssd = 0, totaltty = 0;
float usedCore = 0, ssdUti = 0;
//========
void Init(LIST& l)
{
	l.pHead = l.pTail = NULL;
}
//-------create new Node: key,time
NODE* GetNODE(string key, int time)
{
	NODE* p = new NODE;
	if (p == NULL)
	{
		return NULL;
	}
	p->key = key;
	p->time = time;
	p->pNext = NULL;
	return p;
}

void AddLast(LIST& l, NODE* p)
{
	if (l.pHead == NULL)
	{
		l.pHead = l.pTail = p;
	}
	else
	{// add last
		Node* pTemp;
		pTemp = l.pTail;// save the curr last node to temp
		l.pTail = p;  // let last node = p
		pTemp->pNext = l.pTail; //point pTemp(before add p ) to the curr last node - l.pTail
	}
}

//----------
void printOutPut(LIST l)
{
	for (NODE* p = l.pHead; p != NULL; p = p->pNext)
	{
		cout << p->key << " " << p->time << endl;
	}
}
//----------
void readFile() {
	string key;
	int time;
	nprocess = -1;
	cin >> key;
	cin >> ncore;
	//------
	cin >> key;
	int thoat = 0;
	while (key != "END") {
		cin >> time;
		if (key == "START") {//new list
			nprocess++;
			Init(a[nprocess]);
			NODE* p = GetNODE(key, time);
			AddLast(a[nprocess], p);
		}
		else {//add node to list
			NODE* p = GetNODE(key, time);
			AddLast(a[nprocess], p);
		}
		cin >> key;
	}
}
//-------
void core_request(int how_long, int  proc_id, int  isinteract) {
	Event ej;
	if (nfreecores > 0) {
		nfreecores--;
		/*schedule completion at time current_time + how_long for process proc_id;*/
		int v = vtp[proc_id];
		NODE* temp = a[v].pHead;
		ej.pid = proc_id;
		ej.event = "CORE";
		ej.time = current_time + how_long;
		ej.time_ex = how_long;
		e.push(ej);
		cout << left << setw(10) << current_time << "\t"
			<< setw(7) << "Process-" << ej.pid << "\t"
			<< setw(10) << ej.event << ej.time_ex << setw(11) << "\t" << "RUNNING" << endl;
		usedCore += how_long;

		//--------
		a[v].pHead = a[v].pHead->pNext;
	}
	else {
		int v = vtp[proc_id];
		NODE* temp = a[v].pHead;
		ej.pid = proc_id;
		ej.event = "CORE";
		ej.time = current_time;
		ej.time_ex = how_long;
		if (isinteract == 1)
		{
			/*queue proc_id in i_queue*/
			i_queue.push(proc_id);
			cout << left << setw(10) << ej.time << "\t"
				<< setw(7) << "Process-" << ej.pid << "\t"
				<< setw(10) << ej.event << ej.time_ex << setw(11) << "\t" << "READY" << endl;
			usedCore += how_long;
		}
		else {
			/*queue proc_id in ni_queue*/
			ni_queue.push(proc_id);
			cout << left << setw(10) << ej.time << "\t"
				<< setw(7) << "Process-" << ej.pid << "\t"
				<< setw(10) << ej.event << ej.time_ex << setw(11) << "\t" << "READY" << endl;
			usedCore += how_long;
		} // inner if
	} // outer if
} // core_request
//---------
void ssd_request(int how_long, int proc_id) {
	Event ej;
	if (ssd == 1)
	{
		ssd = 0;
		/*schedule completion at time current_time + how_long for process proc_id;*/
		int v = vtp[proc_id];
		NODE* temp = a[v].pHead;
		ej.pid = proc_id;
		ej.event = "SSD";
		ej.time = current_time + how_long;
		ej.time_ex = how_long;
		e.push(ej);
		cout << left << setw(10) << current_time << "\t"
			<< setw(7) << "Process-" << ej.pid << "\t"
			<< setw(10) << ej.event << ej.time_ex << setw(11) << "\t" << "RUNNING" << endl;
		ssdUti += how_long;
		//--------
		a[v].pHead = a[v].pHead->pNext;
	}
	else {
		/*queue process proc_id in ssd queue;*/
		int v = vtp[proc_id];
		NODE* temp = a[v].pHead;
		ej.pid = proc_id;
		ej.event = "SSD";
		ej.time = current_time;
		ej.time_ex = how_long;
		ssdq.push(proc_id);
		cout << left << setw(10) << ej.time << "\t"
			<< setw(7) << "Process-" << ej.pid << "\t"
			<< setw(10) << ej.event << ej.time_ex << setw(11) << "\t" << "READY" << endl;
		ssdUti += how_long;
	} // if
} // ssd_request
//---------
void tty_request(int how_long, int proc_id) {
	/*schedule completion at time current_time + how_long for process process_id;*/
	int v = vtp[proc_id];
	NODE* temp = a[v].pHead;

	Event ej;
	ej.pid = proc_id;
	ej.event = "TTY";
	ej.time = current_time + how_long;
	ej.time_ex = how_long;
	e.push(ej);
	cout << left << setw(10) << current_time << "\t"
		<< setw(7) << "Process-" << ej.pid << "\t"
		<< setw(10) << ej.event << ej.time_ex << setw(11) << "\t" << "RUNNING" << endl;
	//--------
	a[v].pHead = a[v].pHead->pNext;
} // user_request
//----------
void arrival(int time, int proc_id) {

	//--------
	Event ej;
	int v = vtp[proc_id];
	if (a[v].pHead != NULL) {//
		NODE* temp = a[v].pHead;

		if (temp->key == "CORE" || temp->key == "CPU") core_request(temp->time, proc_id, isinteract);
		if (temp->key == "SSD") ssd_request(temp->time, proc_id);
		if (temp->key == "TTY") tty_request(temp->time, proc_id);
	}

} // arrival
//---------
void core_release(int proc_id) {
	Event ej;
	int v;
	//---------
	if (!i_queue.empty()) {
		/*pop first request in i_queue schedule its completion at current_time + how_long;*/
		int pid = i_queue.front();
		i_queue.pop();
		int v = vtp[pid];
		//--------	
		ej.pid = pid;
		ej.event = "CORE";
		ej.time = current_time + a[v].pHead->time;
		ej.time_ex = a[v].pHead->time;
		e.push(ej);

		//-------
		a[v].pHead = a[v].pHead->pNext;
		cout << left << setw(10) << ej.time << "\t"
			<< setw(7) << "Process-" << ej.pid << "\t"
			<< setw(10) << ej.event << ej.time_ex << setw(11) << "\t" << "RELEASE" << endl;

	}
	else if (!ni_queue.empty()) {
		/*pop first request in ni_queue schedule its completion at current_time + how_long;*/
		int pid = ni_queue.front();
		ni_queue.pop();
		int v = vtp[pid];
		//--------	
		ej.pid = pid;
		ej.event = "CORE";
		ej.time = current_time + a[v].pHead->time;
		ej.time_ex = a[v].pHead->time;
		e.push(ej);

		//-------
		a[v].pHead = a[v].pHead->pNext;
		cout << left << setw(10) << ej.time << "\t"
			<< setw(7) << "Process-" << ej.pid << "\t"
			<< setw(10) << ej.event << ej.time_ex << setw(11) << "\t" << "RELEASE" << endl;
	}
	else {
		nfreecores++;
	}

	/*process next process request;*/
	v = vtp[proc_id];
	if (a[v].pHead != NULL) {//if process still have node
		NODE* temp = a[v].pHead;
		if (temp->key == "CORE" || temp->key == "CPU") core_request(temp->time, proc_id, isinteract);
		if (temp->key == "SSD") ssd_request(temp->time, proc_id);
		if (temp->key == "TTY") tty_request(temp->time, proc_id);
	}


} // core_release
//========
void ssd_release(int proc_id, int& isinteract) {
	Event ej;
	int v;

	isinteract = 0;
	if (!ssdq.empty()) {
		/*pop first request in ssd queue schedule its completion at current_time + how_long;*/
		int pid = ssdq.front();
		ssdq.pop();
		int v = vtp[pid];
		//--------	
		ej.pid = pid;
		ej.event = "SSD";
		ej.time = current_time + a[v].pHead->time;
		ej.time_ex = a[v].pHead->time;
		e.push(ej);
		//-------
		a[v].pHead = a[v].pHead->pNext;
		cout << left << setw(10) << ej.time << "\t"
			<< setw(7) << "Process-" << ej.pid << "\t"
			<< setw(10) << ej.event << ej.time_ex << setw(11) << "\t" << "RELEASE" << endl;
	}
	else {
		ssd = 1;
	} // if
	/*process next process request;*/
	v = vtp[proc_id];
	if (a[v].pHead != NULL) {//if process still has node
		NODE* temp = a[v].pHead;
		if (temp->key == "CORE" || temp->key == "CPU") core_request(temp->time, proc_id, isinteract);
		if (temp->key == "SSD") ssd_request(temp->time, proc_id);
		if (temp->key == "TTY") tty_request(temp->time, proc_id);
	}
} // ssd_release

//=============
void tty_release(int proc_id, int& isinteract) {
	Event ej;
	int v = vtp[proc_id];
	ej.pid = proc_id;
	ej.event = "TTY";
	ej.time = current_time + a[v].pHead->time;
	ej.time_ex = a[v].pHead->time;

	cout << left << setw(10) << ej.time << "\t"
		<< setw(7) << "Process-" << ej.pid << "\t"
		<< setw(10) << ej.event << ej.time_ex << setw(11) << "\t" << "RELEASE" << endl;

	isinteract = 1;
	/*process next process request;*/
	if (a[v].pHead != NULL) {
		NODE* temp = a[v].pHead;
		if (temp->key == "CORE" || temp->key == "CPU") core_request(temp->time, proc_id, isinteract);
		if (temp->key == "SSD") ssd_request(temp->time, proc_id);
		if (temp->key == "TTY") tty_request(temp->time, proc_id);
	}
} // user_release
//========

void execute() {
	Event ei, ej;
	int i;
	//sort processes follow by the start time
	for (int i = 0; i < nprocess; i++)
		for (int j = i + 1; j <= nprocess; j++)
			if (a[i].pHead->time > a[j].pHead->time) {
				NODE* temp = a[i].pHead;
				a[i].pHead = a[j].pHead;
				a[j].pHead = temp;

			}
	//put START and PID to event list
	for (i = 0; i <= nprocess; i++) {
		//--- get time process start
		NODE* temp = a[i].pHead;
		a[i].pHead = a[i].pHead->pNext;
		ei.event = temp->key;
		ei.time = temp->time;
		ei.time_ex = temp->time;

		//-----get process pid
		temp = a[i].pHead;
		a[i].pHead = a[i].pHead->pNext;
		ei.pid = temp->time;
		vtp[ei.pid] = i;//save location of process into array a
		cout << ei.event << "-" << ei.time << endl;
		e.push(ei);
		//=======
	}
	//===========init constructor
	isinteract = 0;
	nfreecores = ncore;
	ssd = 1;//ssd free
	current_time = 0;//start=0 
	//----------execute process
	while (!e.empty()) {
		//------take event out
		ei = e.top();
		e.pop();
		//-------	
		current_time = ei.time;
		//cout<<current_time<<" | "<<"Process-"<<ei.pid<<" | "<<ei.event<<"-"<<ei.time_ex<<" | "<<"FINISHED"<<endl;
		if (ei.event == "START")
		{
			arrival(ei.time, ei.pid);
			totalprocesses++;
		}

		if (ei.event == "CORE" || ei.event == "CPU") core_release(ei.pid);

		if (ei.event == "SSD")
		{
			ssd_release(ei.pid, isinteract);
			totalssd++;
		}
		if (ei.event == "TTY")
		{
			tty_release(ei.pid, isinteract);
			totaltty++;
		}
	}
}

int main()
{
	readFile();
	execute();

	cout << "*===================SUMMARY==================*" << endl;
	cout << "SIMULATED TIME: " << current_time << endl;
	cout << "Number of processes that completed: " << totalprocesses << endl;
	cout << "Total number of SSD accesses: " << totalssd << endl;
	cout << "SSD utilization: " << setprecision(3) << fixed << (ssdUti / current_time) << endl;
	cout << "Average number of busy cores: " << setprecision(3) << fixed << (usedCore / current_time) << endl;
	cout << "Total user iteractions: " << totaltty << endl;
}