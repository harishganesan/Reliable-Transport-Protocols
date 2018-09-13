#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/simulator.h"

using namespace std;
 
/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

int expected_sequence_no1 = 0, expected_sequence_no2 = 0, ack = 1;
float time_limit= 20.0;
struct pkt pkt1;


int check_sum(struct pkt p)
{
	int k = 0,i = 0;

	for (i=0 ; i<20 && p.payload[i] != '\0' ; i++)
		k += p.payload[i];
	
	k += p.acknum;
	k += p.seqnum;
	//cout << "checksum is : " << k << "\n";
	return k;
}



/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
int inc1=0,inc2=0;

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
	cout << "--- Inside A_output --- \n";
	struct msg buf[1000];
	
	buf[inc1] = message;
	inc1++;
	cout<<"The msg is:\t";
	for (int j=0; j<20;j++)
	{
		
		cout<<message.data[j];
		
	}


	cout<< "\n";
	if(ack == 1)
	{
		
		ack = 0;
		
		struct pkt p;
		

		for (int j=0; j<20;j++)
			p.payload[j] = buf[inc2].data[j];

		inc2++;

		p.acknum = -1;
		p.seqnum = expected_sequence_no1;
		p.checksum = check_sum(p);
		pkt1 = p;

		cout<<"seq no:"<<p.seqnum<<"\tack no:"<<p.acknum<<"\tchecksum:"<<p.checksum<<"\tpayload:";

		for (int j=0; j<20;j++)
			cout<<p.payload[j];

		tolayer3(0,p);
                starttimer(0,time_limit);
	}

	cout << "--- Leaving A_output --- \n";

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
	cout << "--- Inside A_input --- \n";
	cout<< check_sum(packet);
	cout<< packet.checksum;
	if(packet.acknum == expected_sequence_no1 && check_sum(packet) == packet.checksum)
	{
		ack = 1;
	
		stoptimer(0);
		expected_sequence_no1 = !expected_sequence_no1;

		cout<<"seq no:"<<packet.seqnum<<"\tack no:"<<packet.acknum<<"\tchecksum:"<<packet.checksum<<"\tpayload:";
		for (int j=0; j<20;j++)
			cout<<packet.payload[j];
	}	

	cout << "--- Leaving A_input --- \n";

}

/* called when A's timer goes off */
void A_timerinterrupt()
{	
	cout << " --- Inside A_timerinterrupt --- \n";
	
		

		cout<<"seq no:"<<pkt1.seqnum<<"\tack no:"<<pkt1.acknum<<"\tchecksum:"<<pkt1.checksum<<"\tpayload:";
		for (int j=0; j<20;j++)
			cout<<pkt1.payload[j];

		tolayer3(0,pkt1);
                starttimer(0,time_limit);
		
	cout << " --- Leaving A_timerinterrupt --- \n";

}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	cout << "--- Inside A_init --- \n";

	cout << "--- Leaving A_init --- \n";

}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
	cout << "--- Inside B_input --- \n";
	cout << packet.checksum;

	struct pkt a;

 	if(check_sum(packet) == packet.checksum && expected_sequence_no2 == packet.seqnum)
	{
		cout << "pkt correctly delivered\n";
		// send to application layer and send ack to A
		tolayer5(1,packet.payload);
                for(int i=0; i< 20;i++)
			a.payload[i] = 0;
		a.acknum = packet.seqnum;
		a.seqnum = 0;
		a.checksum = check_sum(a);
		
		cout<<"seq no:"<<packet.seqnum<<"\tack no:"<<packet.acknum<<"\tchecksum:"<<packet.checksum<<"\tpayload:";
		for (int j=0; j<20;j++)
			cout<<packet.payload[j];
		tolayer3(1,a);
		expected_sequence_no2 = !expected_sequence_no2;
	}

	else if(check_sum(packet) == packet.checksum && expected_sequence_no2 != packet.seqnum)
	{
		cout << "Out of order pkt\n";
		for(int i=0; i< 20;i++)
			a.payload[i] = 0;
		cout<<"seq no:"<<packet.seqnum<<"\tack no:"<<packet.acknum<<"\tchecksum:"<<packet.checksum<<"\tpayload:";
		for (int j=0; j<20;j++)
			cout<<packet.payload[j];
		a.seqnum = 0;
		a.acknum = packet.seqnum;
		a.checksum = check_sum(a);
		tolayer3(1,a);
	}

	

	cout << "--- Leaving B_input --- \n";
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{	cout << "--- Inside B_init --- \n";

	cout << "--- Leaving B_init --- \n";
}
