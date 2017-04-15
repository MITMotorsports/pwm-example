#include "chip.h"
#include "board.h"

#define BAUDRATE 57600

const uint32_t OscRateIn = 12000000;

volatile uint32_t msTicks;

void SysTick_Handler(void) {
	msTicks++;
}

int main(void) {

	// Update system core clock rate
	SystemCoreClockUpdate();

	// Initialize and start system tick timer
	if (SysTick_Config(SystemCoreClock / 1000)) {
		Board_Println("Failed SysTick_Config");
		//Error
		while(1);
	}

	/* UART setup */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT)); /* RXD */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT)); /* TXD */
	Chip_UART_Init(LPC_USART);
	Chip_UART_SetBaud(LPC_USART, BAUDRATE);
	// Configure data width, parity, and stop bits
	Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS));
	Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_USART);

  //GPIO Initialization
  Chip_GPIO_Init(LPC_GPIO);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 4);
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_1, (IOCON_FUNC2 | IOCON_MODE_INACT));
  Chip_GPIO_SetPinState(LPC_GPIO, 1, 4, false);

	// Timer Initialization
	Chip_TIMER_Init(LPC_TIMER32_1);
	Chip_TIMER_Reset(LPC_TIMER32_1);		/* Reset the timer */
	Chip_TIMER_PrescaleSet(LPC_TIMER32_1, 18);	/* Set the prescaler */
  Chip_TIMER_SetMatch(LPC_TIMER32_1, 0, 100);
  Chip_TIMER_SetMatch(LPC_TIMER32_1, 1, 0);
  /* Set match register 2. Don't care about the value. Just want it to be over 100 so it 
   * doesn't interfer with match register 0 and 1 which control the PWM signal */
  Chip_TIMER_SetMatch(LPC_TIMER32_1, 2, 90); 
  /* Set match register 3. Don't care about the value. Just want it to be over 100 so it 
   * doesn't interfer with match register 0 and 1 which control the PWM signal */
  Chip_TIMER_SetMatch(LPC_TIMER32_1, 3, 90); 
  Chip_TIMER_ResetOnMatchEnable(LPC_TIMER32_1, 0);
  LPC_TIMER32_1->PWMC |= (0b001 | 0b0001);			/* Enable PWM mode for pin CT16B0_MAT1 */ 
  Chip_TIMER_ExtMatchControlSet(LPC_TIMER32_1, 0, TIMER_EXTMATCH_TOGGLE, 2);
  Chip_TIMER_ExtMatchControlSet(LPC_TIMER32_1, 0, TIMER_EXTMATCH_TOGGLE, 3);

	// Start the timer
	Chip_TIMER_Enable(LPC_TIMER32_1);

	Board_Println("Started up");
										
	while(1){

	}

	return 0;
}
