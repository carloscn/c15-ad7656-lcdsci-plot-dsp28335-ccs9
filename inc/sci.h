/*
 * sci.h
 *
 *  Created on: 2018Äê5ÔÂ9ÈÕ
 *      Author: weihaochen
 */

#ifndef INC_SCI_H_
#define INC_SCI_H_


#define SCI_FREQ     100E3
#define SCI_PRD      (LSPCLK_FREQ/(SCI_FREQ*8))-1


void scia_fifo_init();
void scib_fifo_init();
void scic_fifo_init();
extern void    SCI_INIT( void );
extern void scib_msg(char * msg);
extern void scib_xmit(int a);
extern void scia_msg(char * msg);
extern void scia_xmit(int a);
extern void scic_msg(char * msg);
extern void scic_xmit(unsigned char a);

// Prototype statements for functions found within this file.
__interrupt void sciaRxFifoIsr(void);
__interrupt void scibRxFifoIsr(void);
__interrupt void scicRxFifoIsr(void);
// Global variables
extern Uint16 sdataA[8];    // Send data for SCI-A
extern Uint16 sdataB[8];    // Send data for SCI-B
extern Uint16 rdataA[8];    // Received data for SCI-A
extern Uint16 rdataB[8];    // Received data for SCI-A
extern Uint16 rdata_pointA; // Used for checking the received data
extern Uint16 rdata_pointB;


#endif /* INC_SCI_H_ */
