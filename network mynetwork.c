network mynetwork
{
    submodules:
        node1:node{
            @dispaly("p=74,61");
        }
        node2:node{
            @dispaly("p=134,126");
        }
        connections:
            node1.out -->node2.in;
            node2.out -->node1.in;
}


simple node
{
    gates:
        input in1;
        output out3;
}



/*
*17010130056 
*张炳建
*/
#include "mycsma.h"
Define_Module(node);
void node::initialize()
{
if (strcmp("node1",getName())==0)
{cMessage *msg=new cMessage("tictocMsg");
send(msg,"out");
}
}
void node::handleMessage(cMessage *msg)
{
send(msg,"out");
}



#ifndef MYCSMA1_H_
#define MYCSMA1_H_
#include <string.h>
#include <omnetpp.h>
using namespace omnetpp;
class node:public cSimpleModule
{ 
protected:
virtual void initialize();
virtual void handleMessage(cMessage *msg);
};
#endif



/*
*17010130056 
*张炳建
*/
void node_2::initialize() {}
void node_2::handleMessage(cMessage *msg)
{
    int arrivalID = msg->getArrivalGateId();
    cGate *gateIn1 = gate("in1");
    cGate *gateIn3 = gate("in3");
    if (gateIn1->getId() == arrivalID){
        send(msg,"out3");
    }else if(gateIn3->getId() == arrivalID){
        send(msg,"out1");
    }
}