//###########################################################################
// Description:
//! \addtogroup f2833x_example_list
//!  <h1>SCI Digital Loop Back with Interrupts (scia_loopback_interrupts)</h1>
//!
//! This program uses the internal loop back test mode of the peripheral.
//! Other then boot mode pin configuration, no other hardware configuration
//! is required. Both interrupts and the SCI FIFOs are used.
//
//###########################################################################
// $TI Release: F2833x/F2823x Header Files and Peripheral Examples V141 $
// $Release Date: November  6, 2015 $
// $Copyright: Copyright (C) 2007-2015 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "project.h"     // Device Headerfile and Examples Include File



#define             FLASH               0

#if FLASH
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
#endif



#define	  AD7656_BASIC    (*((volatile  Uint16 *)0x200000))
#define   AD_BUSY         GpioDataRegs.GPCDAT.bit.GPIO85
#define   SET_ADRST       GpioDataRegs.GPCSET.bit.GPIO87=1
#define   CLEAR_ADRST     GpioDataRegs.GPCCLEAR.bit.GPIO87=1
#define   SET_ADCLK       GpioDataRegs.GPASET.bit.GPIO31=1
#define   CLR_ADCLK       GpioDataRegs.GPACLEAR.bit.GPIO31=1

#define	  SET_CS			GpioDataRegs.GPBSET.bit.GPIO36 = 1
#define	  CLR_CS			GpioDataRegs.GPBCLEAR.bit.GPIO36 = 1

#define	  SET_RD			GpioDataRegs.GPBSET.bit.GPIO34 = 1
#define	  CLR_RD			GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1
Uint16 ad[6];
void InitXintf(void);
long sampleCount=0;
int testSample1[700];//6个数组存放AD数据
int dispBuffer[700];
int i;
int ad_emif_read();
int A = 0;
int time = 789;
Uint16 adc_ok = 0;
interrupt void ISRTimer0(void);

void ADInit(void)
{
	/*
	EALLOW;
	GpioDATRegs.GPAMUX2.bit.GPIO25=0;
	GpioCtrlRegs.GPADIR.bit.GPIO25=1;
	GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0;  // XWE0
	GpioCtrlRegs.GPBDIR.bit.GPIO61=1;
	GpioCtrlRegs.GPBMUX2.bit.GPIO60=0; //ADBUSY
	GpioCtrlRegs.GPBDIR.bit.GPIO60=0;//INPUT
	GpioCtrlRegs.GPBQSEL2.bit.GPIO60= 0;
	EDIS;
	 */
	EALLOW;
	// ADCONV
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;
	// ADRST
	GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 0;
	GpioCtrlRegs.GPCDIR.bit.GPIO87 = 1;
	// BUSY input
	GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 0;
	GpioCtrlRegs.GPCDIR.bit.GPIO85 = 0;
	// cs
	GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO36 = 1;
	// oe
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;


	// DATA BUS 16-BIT
	GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0;
	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 0;
	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0;
	GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0;
	GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 0;
	GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 0;
	GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 0;

	GpioCtrlRegs.GPADIR.bit.GPIO20 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO19 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO21 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO22 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO23 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO24 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO25 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO27 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO26 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO33 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO32 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO49 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO50 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO51 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO52 = 0;


	GpioCtrlRegs.GPADIR.bit.GPIO26 = 0;

	CLR_ADCLK;
	CLEAR_ADRST;
	EDIS;

}

void main(void)
{
	uint16_t i = 0;
	uint16_t j = 0;
	_IQacos(1);
	A = 1;
	// Step 1. Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	// This example function is found in the DSP2833x_SysCtrl.c file.
	InitSysCtrl();

#if  FLASH
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	// Call Flash Initialization to setup flash waitstates
	// This function must reside in RAM
	InitFlash();
#endif
	// Step 2. Initialize GPIO:
	// This example function is found in the DSP2833x_Gpio.c file and
	// illustrates how to set the GPIO to it's default state.
	InitGpio();
	InitXintf();
	InitXintf16Gpio();
	ADInit();

	// Setup only the GP I/O only for SCI-A and SCI-B functionality
	// This function is found in DSP2833x_Sci.c

	// Step 3. Clear all interrupts and initialize PIE vector table:
	// Disable CPU interrupts
	DINT;

	// Initialize PIE control registers to their default state.
	// The default state is all PIE interrupts disabled and flags
	// are cleared.
	// This function is found in the DSP2833x_PieCtrl.c file.
	InitPieCtrl();

	// Disable CPU interrupts and clear all CPU interrupt flags:
	IER = 0x0000;
	IFR = 0x0000;
	SCI_INIT();
	// Initialize the PIE vector table with pointers to the shell Interrupt
	// Service Routines (ISR).
	// This will populate the entire table, even if the interrupt
	// is not used in this example.  This is useful for debug purposes.
	// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
	// This function is found in DSP2833x_PieVect.c.
	InitPieVectTable();
	scib_msg("hellow world!");
	// Interrupts that are used in this example are re-mapped to
	// ISR functions found within this file.
	EALLOW;  // This is needed to write to EALLOW protected registers
	PieVectTable.SCIRXINTA = &sciaRxFifoIsr;
	PieVectTable.SCIRXINTB = &scibRxFifoIsr;
	PieVectTable.SCIRXINTC = &scicRxFifoIsr;
	PieVectTable.TINT0 = &ISRTimer0;
	EDIS;   // This is needed to disable write to EALLOW protected registers


	InitCpuTimers();   // For this example, only initialize the Cpu Timers
	ConfigCpuTimer(&CpuTimer0, 100, time); //在定时器内进行采样,采样率1.5KHz
	// Step 5. User specific code, enable interrupts:


	// Enable interrupts required for this example
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
	PieCtrlRegs.PIEIER9.bit.INTx1=1;     // PIE Group 9, int1
	PieCtrlRegs.PIEIER9.bit.INTx2=1;     // PIE Group 9, INT2
	PieCtrlRegs.PIEIER9.bit.INTx3=1;     // PIE Group 9, INT3
	PieCtrlRegs.PIEIER9.bit.INTx4=1;     // PIE Group 9, INT4
	PieCtrlRegs.PIEIER9.bit.INTx5=1;     // PIE Group 9, INT3
	PieCtrlRegs.PIEIER9.bit.INTx6=1;     // PIE Group 9, INT3
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;	// timer
	IER |= 0x100 | M_INT1 | M_INT9; // Enable CPU INT
	EINT;
	SET_ADRST;
	DELAY_US(100000);
	CLEAR_ADRST;
	CLR_CS;
	SET_RD;

	lcd_clear();
	// Step 6. IDLE loop. Just sit and loop forever (optional):
	CpuTimer0Regs.TCR.bit.TSS =0;
	for(;;){
		if (adc_ok == 1) {

			for (i = 10; i < 799/3 - 10; i ++) {
				lcd_draw_point(3*i,dispBuffer[i]);
			}
			//lcd_draw_adc(dispBuffer);
			adc_ok = 0;
			//lcd_clear();
			lcd_pic();
		}

	}
}

interrupt void ISRTimer0(void)
{
	CLR_ADCLK;   //启动转换信号
	DELAY_US(5); //给予适当的电平延时
	SET_ADCLK;
	DELAY_US(1);
	//GpioDataRegs.GPADAT.bit.GPIO0 = ~GpioDataRegs.GPADAT.bit.GPIO0;
	while(AD_BUSY) //等待转换结束
	{

	}

	for(i=0;i<6;i++)
	{
		CLR_RD;
		ad[i] = ad_emif_read(); //读取6路AD通道数据
		SET_RD;
	}
	testSample1[sampleCount]=ad[0];//存放在数组里
	//testSample2[sampleCount]=ad[1];
	//testSample3[sampleCount]=ad[2];
	//testSample4[sampleCount]=ad[3];
	//testSample5[sampleCount]=ad[4];
	//testSample6[sampleCount]=ad[5];
	sampleCount++;
	if(sampleCount >= 700) {
		if (adc_ok == 0) {
			for(i = 0; i < 700; i++) {
				dispBuffer[i] = (testSample1[i] + 1200)/10 * A + 100;
			}
			adc_ok = 1;
		}
		sampleCount=0;
	}

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1 | PIEACK_GROUP9;
	CpuTimer0Regs.TCR.bit.TIF=1; // 定时到了指定时间，标志位置位，清除标志
	CpuTimer0Regs.TCR.bit.TRB=1;  // 重载Timer0的定时数据
}


void scic_hex_array(unsigned char *rom, uint16_t len)
{
	uint16_t i;

	for (i = 0; i < len; i ++) {
		scic_xmit(*(rom + i));
	}
}

// x (0 - 799)
// y (0 - 479)
void lcd_clear()
{
	unsigned char cmd[13];

	cmd[0] = 0xF0;
	cmd[1] = 0x5A;
	cmd[2] = 0x82;
	cmd[3] = 0x00;
	cmd[4] = 0x00;
	cmd[5] = 0xFF;
	cmd[6] = 0xA5;
	cmd[7] = 0xF0;
	scic_hex_array(cmd, 8);
}

void lcd_pic(unsigned char p)
{
	unsigned char cmd[18];
	cmd[0] = 0xF0;
	cmd[1] = 0x5A;
	cmd[2] = 0x91;
	cmd[3] = 0;
	cmd[4] = 0;
	cmd[5] = 0;
	cmd[6] = 0;
	cmd[7] = dec_to_bcd((unsigned char)(p / 100));
	cmd[8] = dec_to_bcd((unsigned char)(p % 100));
	cmd[9]= 0xA5;
	cmd[10]= 0xF0;
	scic_hex_array(cmd, 11);
}

void lcd_draw_adc(int *buffer)
{
	int i;
	unsigned char cmd[18];
	unsigned char last_x, last_y;
	unsigned char next_x, next_y;
	for (i = 20; i < 799/8 - 20; i ++) {

		last_x = (unsigned char)(i -1) * 8;
		last_y = (unsigned char)buffer[i -1];
		next_x = (unsigned char)i * 8;
		next_y = (unsigned char)buffer[i];
		cmd[0] = 0xF0;
		cmd[1] = 0x5A;
		cmd[2] = 0x93;
		cmd[3] = dec_to_bcd((unsigned char)(last_x / 100));
		cmd[4] = dec_to_bcd((unsigned char)(last_x % 100));
		cmd[5] = dec_to_bcd((unsigned char)(last_y / 100));
		cmd[6] = dec_to_bcd((unsigned char)(last_y % 100));
		cmd[7] = dec_to_bcd((unsigned char)(next_x / 100));
		cmd[8] = dec_to_bcd((unsigned char)(next_x % 100));
		cmd[9] = dec_to_bcd((unsigned char)(next_y / 100));
		cmd[10] = dec_to_bcd((unsigned char)(next_y % 100));
		cmd[11] = 0xFF;
		cmd[12] = 0x00;
		cmd[13] = 0x00;
		cmd[14]= 0xA5;
		cmd[15]= 0xF0;
		scic_hex_array(cmd, 16);
	}
}


void lcd_draw_point(uint16_t x, uint16_t y)
{
	unsigned char cmd[13];

	cmd[0] = 0xF0;
	cmd[1] = 0x5A;
	cmd[2] = 0x92;
	cmd[3] = dec_to_bcd((unsigned char)(x / 100));
	cmd[4] = dec_to_bcd((unsigned char)(x % 100));
	cmd[5] = dec_to_bcd((unsigned char)(y / 100));
	cmd[6] = dec_to_bcd((unsigned char)(y % 100));
	cmd[7] = 0xFF;
	cmd[8] = 0x00;
	cmd[9] = 0x00;
	cmd[10]= 0x05;
	cmd[11]= 0xA5;
	cmd[12]= 0xF0;
	scic_hex_array(cmd, 13);
}

unsigned char dec_to_bcd(unsigned char c)
{
	unsigned char t;

	t = (c+(c/10)*6);

	return t;
}

int ad_emif_read()
{
	int D = 0x0000;

	D |= GpioDataRegs.GPBDAT.bit.GPIO52;
	D <<= 1;
	D |= GpioDataRegs.GPBDAT.bit.GPIO51;
	D <<= 1;
	D |= GpioDataRegs.GPBDAT.bit.GPIO50;
	D <<= 1;
	D |= GpioDataRegs.GPBDAT.bit.GPIO49;
	D <<= 1;
	D |= GpioDataRegs.GPBDAT.bit.GPIO32;
	D <<= 1;
	D |= GpioDataRegs.GPBDAT.bit.GPIO33;
	D <<= 1;
	D |= GpioDataRegs.GPADAT.bit.GPIO26;
	D <<= 1;
	D |= GpioDataRegs.GPADAT.bit.GPIO27;
	D <<= 1;
	D |= GpioDataRegs.GPADAT.bit.GPIO25;
	D <<= 1;
	D |= GpioDataRegs.GPADAT.bit.GPIO24;
	D <<= 1;
	D |= GpioDataRegs.GPADAT.bit.GPIO23;
	D <<= 1;
	D |= GpioDataRegs.GPADAT.bit.GPIO22;
	D <<= 1;
	D |= GpioDataRegs.GPADAT.bit.GPIO21;
	D <<= 1;
	D |= GpioDataRegs.GPADAT.bit.GPIO19;
	D <<= 1;
	D |= GpioDataRegs.GPADAT.bit.GPIO20;
	D <<= 1;
	D |= GpioDataRegs.GPADAT.bit.GPIO18;
	return D;
}

