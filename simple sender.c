
/*
*17010130056 
*张炳建
*/
simple sender
{
    parameters:
         @display("i=block/process");
    gates:
        input in;
        output out;
}

simple receiver
{
    parameters:
         @display("i=block/process");
    gates:
        input in;
        output out;
}

network arqnet
{
    @display("bgb=774,582");
    submodules:
        sender:sender{
            parameters:
                @display("i=,cyan;p=545,141");
        }
        receiver:receiver{
            parameters:
                @display("i=,gold;p=350,274");
        }
     connections:
         sender.out --> {delay=100ms;} --> receiver.in;
         sender.in <--{delay=100ms;} <-- receiver.out;
}



/*
*17010130056 
*张炳建
*/
#include<string.h>
#include<omnetpp.h>
using namespace omnetpp;
class receiver:public cSimpleModule  //接受类定义
{
protected:  //受保护部分
    virtual void handleMessage(cMessage *msg);
};
Define_Module(receiver);  //定义接受模块
void receiver::handleMessage(cMessage *msg)
{
    if(uniform(0,1)<0.4)  //误码率部分
      {
          EV<<"\"Losing\"message"<<msg<<endl;  //显示发送信息失败
          bubble("message lost");
          delete msg;
       }
    else
      {
         EV<<msg<<"recv,send ack.\n";  //发送ACK
         delete msg;
         send(new cMessage("ack"),"out");
      }
}

/*
*17010130056 
*张炳建
*/
#include<string.h>
#include<omnetpp.h>
using namespace omnetpp;
class sender:public cSimpleModule  //sender类定义
{
private:  //私有部分
    simtime_t txtime;
    simtime_t ack_time;
    simtime_t avg_time;

    cHistogram Avg_Time_Stats;
    cOutVector Avg_Time_Vector;


    int seq;//sequence number
    cMessage *timeoutEvent; //timeout self-message
    simtime_t timeout;  //timeout
    cMessage *message;

    public:  //公共部分
        sender();
       virtual ~sender();

    protected:  //受保护的部分
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void sendCopyof(cMessage *msg);
        virtual cMessage *generateNewMessage();
        virtual void finish();
    };
 /*17010130056 张炳建*/
    Define_Module(sender);  //定义发送模块

    sender::sender()  //sender函数定义
{
    timeoutEvent=NULL;//初始化、
    message=NULL;
}
sender::~sender()  //sender娄的析构函数
{
    cancelAndDelete(timeoutEvent);
    delete message;
}
cMessage *sender::generateNewMessage()//发送信息函数定义
{
    txtime=simTime();
//generate a message
    char msgname[20];
    sprintf(msgname,"send-%d",--seq);
    cMessage *msg=new cMessage(msgname);
    return msg;
}
void sender::sendCopyof(cMessage *msg)//重发函数定义
{
    cMessage *copy=(cMessage *)msg ->dup();
    send(copy,"out");
}
 /*17010130056 张炳建*/
void sender::initialize() //初始化函数定义
{
    txtime=0;
    ack_time=0;
    avg_time=0;

    WATCH(txtime);
    WATCH(ack_time);

    Avg_Time_Stats.setName("avg_timestats");
    Avg_Time_Vector.setName("avg_time");

    //Initialize variable
    seq=0;
    timeout=1.0;
    timeoutEvent=new cMessage("timeoutEvent");
    //generate and send initial message
    EV<<"Sending initial message\n";
    message=generateNewMessage();
    sendCopyof(message);
    scheduleAt(simTime()+timeout,timeoutEvent);
}
 /*17010130056 张炳建*/
void sender::handleMessage(cMessage *msg)//处理信息函数定义
{
    if(msg== timeoutEvent)//timeout resend超时重发
    {
        EV<<"Timeout expired,re-sending message\n";
        sendCopyof(message);
        scheduleAt(simTime()+timeout,timeoutEvent);
    }
    else//rcv ACK
    {
        ack_time=simTime();
        avg_time=ack_time-txtime;
        Avg_Time_Vector.record(avg_time);
        Avg_Time_Stats.collect(avg_time);
    EV<<"Received:"<<msg->getName()<<"\n";
    delete msg;
    EV<<"Timer cancelled.\n";
    cancelEvent(timeoutEvent);
    delete message;

    //send new message
    message=generateNewMessage();
    sendCopyof(message);
    scheduleAt(simTime()+timeout,timeoutEvent);
    }
}

 /*17010130056 张炳建*/
void sender::finish()//结束发送函数处理
{
    EV<<"avg,mean:"<<Avg_Time_Stats.getMean()<<endl;//平均值
    EV<<"avg,min:"<<Avg_Time_Stats.getMin()<<endl;//最小值
    EV<<"avg,max:"<<Avg_Time_Stats.getMax()<<endl;//最大值
    EV<<"avg,seddev:"<<Avg_Time_Stats.getStddev()<<endl;//seddev
    recordScalar("#tx time",txtime);
    recordScalar("#ack time",ack_time);
Avg_Time_Stats.recordAs("avg time");
}
