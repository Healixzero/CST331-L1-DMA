/*******************************************************************************
* Brent Honzaki
* CST331
* Douglas Lynn
* Lab 01: DMA Lab
*******************************************************************************/

#include dma.h

/* Include in main file:
 *#pragma interrupt DMA0_Int ip17 vector 36 
 */

uint8_t gBufferSending     = 0;
uint8_t gBufferBlockOffset = 0;
uint8_t gBufferByteOffset  = 0;

// DMA interrupt ISR (this does NOT go in the header file)
void DMA0_Int() {
	IFS1CLR		= 0x00010000;	// clear DMA0 interrupt flag
	DCH0INTCLR 	= 0x8;		// clear the block tranfer complete flag
	LATDCLR 	= 0x1;
	TRISDCLR 	= 0x1;
	ODCDCLR		= 0x1;

	if( gBufferSending ) { // not recieving
		if( gBufferBlockOffset ) {
			--gBufferBlockOffset;
			DCH0SSA	+= BLOCK_SIZE;
			DCH0CONSET	= 0x80;	// enable DMA Channel 0
			DCH0ECONSET	= 0x80;	} // Force Transfer (Prime) DMA Channel 0
		else if ( gBufferByteOffset ) {
			DCH0SSA += BLOCK_SIZE;
			DCH0SSIZ = gBufferByteOffset;
			gBufferByteOffset = 0;
			DCH0CONSET	= 0x80;
			DCH0ECONSET	= 0x80; }
		else {	// wait for the last character to shift out
			while( !( U1STA & 0x0100 ) );
			U1STACLR	= 0x4000;
			setDMA0recieve();
			U1STASET	= 0x1000; }
		}
	else { // Recieving
		++gBufferBlockOffset;
		// enable DMA chan 0 when buffer not full
		if( gBufferBlockOffset < ( BUFFER_SIZE / BLOCK_SIZE ) ) {
			DCH0SDA += BLOCK_SIZE;
			DCH0CONSET = 0x80; }
		// handle overflow
		else {
			U1STA_CLR	= 0x80;	// disable recieve
			DCH0CONCLR	= 0x90; // disable DMA Channel 0
			LED_OFF( RECV_OVERFLOW ); } //Turn on Overflow LED
	}
}

// initalize DMA channels 0-3
void DMA_Init() {
	init_DMA0();
	init_DMA1();
	init_DMA2();
	init_DMA3();
	DMA0_SetRecieve();
	DMACON = 0x8000; } // turn on the DMA controller last

// initialize DMA channel 0
void DMA0_Init() {
	IEC1CLR		= 0x00010000;	// disable DMA0 interrupt
	IFS1CLR		= 0x00010000;	// clr DMA0 interrupt flag
	DCH0ECONSET	= 0x10;		// en cell transfer on IRQ
	DCH0INTSET	= 0x080000;	// trigger IRQ on block transfer complete
	DCH0CSIZ	= 1;		// set transcer cell size
	DCH0CON		= 0c00;		// reset DMA0 config
	IPC9CLR		= 0x0000001F;	// clr DMA0 priority & sub-priority
	IPC9SET		= 0x0000001C;	// IPL 7, sub 2
	IEC1SET		= 0x00010000; }	// en DMA0 interrupts again
	
// initialize DMA channel 1 (recieve timer reset DMA)
// note: this one only clears the upper half of the timer, was previously
// clearing both halves, not sure if this is causing the timing error.
void DMA1_Init() {
	DCH1ECON	= 27 << 8;	// set DMA1 to trigger on UART1
	DCH1ECONSET	= 0x10;		// en cell transfer on IRQ
	DCH1SSA		= KVA_TO_PA(&gRecieveLightTimerReset);	// set source
	DCH1DSA		= KVA_TO_PA(&TMR2);	// set destination (upper half of timer 2. is this causing errors?)
	DCH1SSIZ	= 4;		// set source size
	DCH1DSIZ	= 4;		// set dest size
	DCH1CSIZ	= 4;		// set cell size
	DCH1CON		= 0x90; }	// en DMA1, with auto re-enable

// initialize DMA Channel 2 ( recieve LED set )
void DMA2_Init(){
	DCH2ECON	= 27 << 8;	// set DMA2 to trigger on UART1
	DCH2ECONSET	= 0x10;		// en cell transfer on IRQ
	DCH2SSA		= KVA_TO_PA(&gRecieveLightBits); // set source to LED mask variable
	DCH2DSA		= KVA_TO_PA(&LATBSET);	// set the dest to LATB pins
	DCH2SSIZ	= 2;		// set source size
	DCH2DSIZ	= 2;		// set dest size
	DCH2CSIZ	= 2;		// set cell size
	DCH2CON		= 0x90; }	// en DMA2, with auto re-enable

// initialize DMA channel 3 ( recieve LED clr )
void DMA3_Init() {
	DCH3ECON	= 12 << 8;	// set DMA3 to trigger on timer 3
	DCH3ECONSET	= 0x10;		// en cell transfer on IRQ
	DCH3SSA		= KVA_TO_PA(&grecieveLightBits); // set source to LED Mask variable
	DCH3DSA		= KVA_TO_PA(&LATBCLR);	// set dest bits to LATB pins
	DCH3SSIZ	= 2;		// set source size
	DCH3DSIZ	= 2;		// set dest size
	DCH3DSIZ	= 2;		// set cell size
	DCH3CON		= 0x90;		// en DMA3 with auto re-enable
void DMA0_Send_Set();
void DMA0_Recv_Set();


