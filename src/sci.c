/*
 * sci.c
 *
 *  Created on: 2018Äê5ÔÂ9ÈÕ
 *      Author: weihaochen
 */


#include "project.h"
extern int A;

void    SCI_INIT( void )
{
    InitSciGpio();
    scia_fifo_init();
    scib_fifo_init();
    scic_fifo_init();
}


void scia_xmit(int a)
{
    while( SciaRegs.SCICTL2.bit.TXRDY != 1 );
    SciaRegs.SCITXBUF=a & 0x00FF;

}

void scia_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        scia_xmit(msg[i]);
        i++;

    }
}

void scib_xmit(int a)
{

    while( ScibRegs.SCICTL2.bit.TXRDY != 1 );
    ScibRegs.SCITXBUF=a & 0x00FF;

}
void scib_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        scib_xmit(msg[i]);
        i++;
    }
}

void scic_xmit(unsigned char a)
{

    while( ScicRegs.SCICTL2.bit.TXRDY != 1 );
    ScicRegs.SCITXBUF=a & 0x00FF;

}
void scic_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        scic_xmit(msg[i]);
        i++;
    }
}
Uint16 rx_count = 0;
unsigned char rx_buffer[10];

__interrupt void sciaRxFifoIsr(void)
{
    Uint16 i;


    PieCtrlRegs.PIEACK.all|=0x100 | PIEACK_GROUP1 | PIEACK_GROUP9 ;       // Issue PIE ack
}



__interrupt void scibRxFifoIsr(void)
{
    rx_buffer[rx_count] = ScibRegs.SCIRXBUF.all;
    rx_count ++;
    if (rx_count >= 3) {
    	rx_count = 0;
    	if (rx_buffer[2] == 0x01) {
    		A += 1;
    	}else{
    		A -= 1;
    		if (A <= 1) {
    			A = 1;
    		}
    	}
    }
    EINT;
    PieCtrlRegs.PIEACK.all|=0x100 | M_INT9;      // Issue PIE ack
}

__interrupt void scicRxFifoIsr(void)
{
    Uint16 i;
    for(i=0;i<8;i++)
    {
        //rdataB[i]=ScicRegs.SCIRXBUF.all;  // Read data
    }
    EINT;
    PieCtrlRegs.PIEACK.all|=0x100 | M_INT9;      // Issue PIE ack
}

void scia_fifo_init()
{
    SciaRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
    // No parity,8 char bits,
    // async mode, idle-line protocol
    SciaRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
    // Disable RX ERR, SLEEP, TXWAKE
    SciaRegs.SCICTL2.bit.TXINTENA =1;
    SciaRegs.SCICTL2.bit.RXBKINTENA =1;

    // value = 37500000 / baud / 8 - 1

    SciaRegs.SCIHBAUD    =0x0000;  // 115200 baud @LSPCLK = 37.5MHz.
    SciaRegs.SCILBAUD    = 39;
    SciaRegs.SCICCR.bit.LOOPBKENA = 0; // Enable loop back
    SciaRegs.SCICTL2.bit.RXBKINTENA =   1;
    SciaRegs.SCICTL2.bit.TXINTENA   =   0;

    SciaRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset

}



void scib_fifo_init()
{
    ScibRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
    // No parity,8 char bits,
    // async mode, idle-line protocol
    ScibRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
    // Disable RX ERR, SLEEP, TXWAKE
    ScibRegs.SCICTL2.bit.TXINTENA =1;
    ScibRegs.SCICTL2.bit.RXBKINTENA =1;

    // value = 37500000 / baud / 8 - 1

    ScibRegs.SCIHBAUD    =0x0000;  // 115200 baud @LSPCLK = 37.5MHz.
    ScibRegs.SCILBAUD    = 39;
    ScibRegs.SCICCR.bit.LOOPBKENA = 0; // Enable loop back
    ScibRegs.SCICTL2.bit.RXBKINTENA =   1;
    ScibRegs.SCICTL2.bit.TXINTENA   =   0;

    ScibRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset
}

void scic_fifo_init()
{
    ScicRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
    // No parity,8 char bits,
    // async mode, idle-line protocol
    ScicRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
    // Disable RX ERR, SLEEP, TXWAKE
    ScicRegs.SCICTL2.bit.TXINTENA =1;
    ScicRegs.SCICTL2.bit.RXBKINTENA =1;

    // value = 37500000 / baud / 8 - 1

    ScicRegs.SCIHBAUD    =0x01;  // 115200 baud @LSPCLK = 37.5MHz.
    ScicRegs.SCILBAUD    = 0xE7;
    ScicRegs.SCICCR.bit.LOOPBKENA = 0; // Enable loop back
    ScicRegs.SCICTL2.bit.RXBKINTENA =   1;
    ScicRegs.SCICTL2.bit.TXINTENA   =   0;

    ScicRegs.SCICTL1.all =0x0023;     // Relinquish SCI from Reset
}
//===========================================================================
// No more.
//===========================================================================
