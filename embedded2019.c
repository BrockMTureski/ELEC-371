#include "nios2_control.h"

#define OUTPORT (unsigned volatile int*) 0x00006000
#define INPORT (unsigned volatile int*) 0x00006800
#define TIMER_STATUS (unsigned volatile int*) 0x00005000
#define TIMER_CONTROL (unsigned volatile int*) 0x00005004
#define TIMER_START_LO (unsigned volatile int*) 0x00005008
#define TIMER_START_HI (unsigned volatile int*) 0x0000500C
#define JTAG_UART_DATA (unsigned volatile int*) 0x00006A00
#define JTAG_UART_STATUS (unsigned volatile int*) 0x00006A04

#define DATA_MASK 0xFFFF0000
#define ASCII_NUM_START 0x30
#define ASCII_LETTER_START 0x41

unsigned volatile int count;
unsigned volatile int second;
unsigned volatile int input;
unsigned volatile int inputFlag;
unsigned volatile int totalNotifications;
unsigned volatile int dataSum;



void init(){
    *TIMER_START_LO=0x5E10;
    *TIMER_START_HI=0x005F;
    *TIMER_CONTROL=7;
    *TIMER_STATUS=0;

    count=0;
    second=0;
    inputFlag=0;
    totalNotifications=0;

    *OUTPORT=0x01;

    NIOS2_WRITE_STATUS(0x20);
    NIOS2_WRITE_IENABLE(0x3);
}

void PrintChar(unsigned int ch){
    while((*JTAG_UART_STATUS & DATA_MASK)==0){
        (*JTAG_UART_DATA)=ch;
    }
}

void PrintHexDigit(unsigned int n){
    if(n<=9){
        PrintChar(n+ASCII_NUM_START);
    } else PrintChar(n+ASCII_LETTER_START-10);
}

void HandleInputPort(void){
    input=*INPORT;
    inputFlag=1;
}

void HandleTimer(void){
    unsigned int out=*OUTPORT;
    *TIMER_STATUS=0;
    count=count+1;
    if(count>=8){
        count=0;
        second=1;
        *OUTPORT=0x01;
    }else *OUTPORT = (out<<1);
    
}

void interrupt_handler(void){
    unsigned int ipending = NIOS2_READ_IPENDING();

    if(ipending &0x1){
        HandleTimer();
    }
    if((ipending &0x2)==0x2){
        HandleInputPort();
    }
}

int main(){
    unsigned int recentNotifications=0;
    unsigned int totalNotifications=0;
    unsigned int print_data_lo;
    unsigned int print_data_hi;
    unsigned int average;


    while(1){
        if(second){
            print_data_hi=((recentNotifications &0xF0)>>4);
            print_data_lo=((recentNotifications &0x0F));
            PrintHexDigit(print_data_hi);
            PrintChar('\b');
            PrintHexDigit(print_data_lo);
            recentNotifications=0;
        }
        if(inputFlag){
            recentNotifications=recentNotifications+1;
            totalNotifications=totalNotifications+1;
            dataSum=dataSum+input;
            average=dataSum/totalNotifications;
        }
    }

}