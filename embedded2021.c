#include "nios2_control.h"

#define OUTPORT (unsigned volatile int*) 0x00006800
#define INPORT (unsigned volatile int*) 0x00006700
#define TIMER0_STATUS (unsigned volatile int*) 0x00005000
#define TIMER0_CONTROL (unsigned volatile int*) 0x00005004
#define TIMER0_START_LO (unsigned volatile int*) 0x00005008
#define TIMER0_START_HI (unsigned volatile int*) 0x0000500C
#define TIMER1_STATUS (unsigned volatile int*) 0x00006000
#define TIMER1_CONTROL (unsigned volatile int*) 0x00006004
#define TIMER1_START_LO (unsigned volatile int*) 0x00006008
#define TIMER1_START_HI (unsigned volatile int*) 0x0000600C
#define JTAG_UART_DATA (unsigned volatile int*) 0x00006A00
#define JTAG_UART_STATUS (unsigned volatile int*) 0x00006A04

#define DATA_MASK 0xFFFF0000

unsigned volatile int sum;
unsigned volatile int timer1;
unsigned volatile int count;
unsigned volatile int average;

void init(){
    *TIMER0_START_HI=0x005F;
    *TIMER0_START_LO=0x5E10;
    *TIMER0_CONTROL=7;
    *TIMER0_STATUS=0;

    *TIMER1_START_HI=0x017D;
    *TIMER1_START_LO=0x7840;
    *TIMER1_CONTROL=7;
    *TIMER1_STATUS=0;

    timer1=0;
    sum=0;
    count=0;
    average=0;

    *OUTPORT=0x80;

    NIOS2_WRITE_IENABLE(0x60);
    NIOS2_WRITE_STATUS(0x1);
}


void HandleTimer0(){
    unsigned int outp=*OUTPORT;
    *TIMER0_STATUS=0;
    if(outp!=0x1){
        *OUTPORT=outp>>1;
    } else *OUTPORT= 0x80;
}


void PrintChar(unsigned int ch){
    while((*JTAG_UART_STATUS &DATA_MASK)==0){
        (*JTAG_UART_DATA)=ch;
    }
}

void HandleTimer1(){
    *TIMER1_STATUS=0;
    timer1=1;
}

void interrupt_handler(){
    unsigned int ipending = NIOS2_READ_IPEDNING();
    
    if(ipending &0x20){
        HandleTimer0();
    } if(ipending &0x40){
        HandleTimer1();
    }
}

int main(){
    unsigned int inp=0;
    unsigned int inp_prev;

    while(1){
    if(timer1){
        inp_prev=inp;
        inp=*INPORT;
        if(inp>inp_prev) PrintChar('>');
        if(inp<inp_prev) PrintChar('<');
        if(inp==inp_prev) PrintChar('=');
        PrintChar('\n');
        sum=sum+inp;
        count=count+1;
        average=sum/count;
    }}

}