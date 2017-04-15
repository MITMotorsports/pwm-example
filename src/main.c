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
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 8);
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_8, (IOCON_FUNC2 | IOCON_MODE_INACT));
  Chip_GPIO_SetPinState(LPC_GPIO, 0, 8, false);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 9);
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_9, (IOCON_FUNC2 | IOCON_MODE_INACT));
  Chip_GPIO_SetPinState(LPC_GPIO, 0, 9, false);

	// Timer Initialization
	Chip_TIMER_Init(LPC_TIMER16_0);
	Chip_TIMER_Reset(LPC_TIMER16_0);		/* Reset the timer */
	Chip_TIMER_PrescaleSet(LPC_TIMER16_0, 18);	/* Set the prescaler */
  Chip_TIMER_SetMatch(LPC_TIMER16_0, 0, 90);
  Chip_TIMER_SetMatch(LPC_TIMER16_0, 1, 80);
  /* Set match register 2. Don't care about the value. Just want it to be over 100 so it 
   * doesn't interfer with match register 0 and 1 which control the PWM signal */
  Chip_TIMER_SetMatch(LPC_TIMER16_0, 2, 0); 
  /* Set match register 3. Don't care about the value. Just want it to be over 100 so it 
   * doesn't interfer with match register 0 and 1 which control the PWM signal */
  Chip_TIMER_SetMatch(LPC_TIMER16_0, 3, 100); 
  Chip_TIMER_ResetOnMatchEnable(LPC_TIMER16_0, 3);
  LPC_TIMER16_0->PWMC |= (0b01 | 0b10); /* Enable PWM mode for pin CT16B0_MAT0 and CT16B0_MAT1 */ 
  Chip_TIMER_ExtMatchControlSet(LPC_TIMER16_0, 0, TIMER_EXTMATCH_TOGGLE, 0);
  Chip_TIMER_ExtMatchControlSet(LPC_TIMER16_0, 0, TIMER_EXTMATCH_TOGGLE, 1);

	// Start the timer
	Chip_TIMER_Enable(LPC_TIMER16_0);

	Board_Println("Started up");
										
	while(1){

	}

	return 0;
}
