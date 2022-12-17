#include "nios2_control.h"

#define INPORT (unsigned volatile int*) 0x0000
#define OUTPORT (unsigned volatile int*) 0x0000
#define TIMER_CONTROL (unsigned volatile int*) 0x0000
#define TIMER_STATUS (unsigned volatile int*) 0x0000
#define TIMER_START_LO (unsigned volatile int*) 0x0000
#define TIMER_START_HI (unsigned volatile int*) 0x0000
#define JTAG_UART_DATA (unsigned volatile int*) 0x0000
#define JTAG_UART_STATUS (unsigned volatile int*) 0x0000

#define NEWLINE 0x0D
#define DATA_MASK 0xFFFF0000
#define ASCII_NUM 0x30
#define ASCII_LETTER 0x41

unsigned volatile int count;
unsigned volatile int second;

void init(){
    *TIMER_START_HI=0x017D;
    *TIMER_START_LO=0x7840;
    *TIMER_CONTROL=7;
    *TIMER_STATUS=0;

    count=0;
    second=0;

    *OUTPORT=0x0F;

    NIOS2_WRITE_IENABLE(0x1);
    NIOS2_WRITE_STATUS(0x1);
}


void printChar(unsigned int ch){
    while((*JTAG_UART_STATUS & DATA_MASK)==0){
        *(JTAG_UART_DATA)=ch;
    }
}

void printHexDig(unsigned int n){
    if(n<=9){
        printChar(n+ASCII_NUM);
    }
    else{
        printChar(n+ASCII_LETTER-10);
    }
}

void UpdateLEDs(void){
    unsigned int outport1= *OUTPORT;
    unsigned int outport2= *OUTPORT;
    outport1= ((~outport1) &0x00FF) | outport2;
}

interrupt_handler(void){
    unsigned int ipending = NIOS2_READ_IPENDING();
    
    if(ipending & 0x1){
        *TIMER_STATUS=0;
        ++count;
        UpdateLEDs();
        if(count>=2){
            count=0;
            second=1;
        }

    }
}

int main(){
    unsigned int total_secs = 0;
    unsigned int data_o;
    unsigned int data_i;


   if(second){
    second=0;
    ++total_secs;
   }
   if(total_secs%10==0&&total_secs!=0){
        data_i=*INPORT;
        data_o=*OUTPORT;
        data_i=data_i &0xFF00;
        data_o=data_o &0x00FF;
        data_i=data_i|data_o;
        *OUTPORT=data_i;
   }
   data_i=*INPORT;
   data_i=(data_i &0x0080)>>7;
   if(data_i&0x1){
    data_i=*INPORT;
    data_i=(data_i &0x007F)>>3;
    printHexDig(data_i);
    printChar(NEWLINE);
   }
}