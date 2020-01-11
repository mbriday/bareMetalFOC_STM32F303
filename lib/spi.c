#include "spi.h"
#include "stm32f3xx.h"


//use SPI1:
// CS MCP  : PA11 (I/O) - Arduino D10
// CS TFT  : PA2  (I/O) - Arduino A7
// MOSI: PB5 (AF5)  - Arduino D11
// MISO: PB4 (AF5)  - Arduino D12
// SCK : PB3 (AF5)  - Arduino D13
void setupSPI()
{
	//1 - input clock = 64MHz.
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	__asm("nop");
	//reset peripheral (mandatory!)
	RCC->APB2RSTR |=  RCC_APB2RSTR_SPI1RST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
	//init procedure p.962 - section 30.5.7
	//2 - GPIOs
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | //clock for GPIOA
	               RCC_AHBENR_GPIOBEN;  //clock for GPIOB
	__asm("nop");					//wait until GPIOx clock is Ok.
	GPIOB->OSPEEDR = 3 << GPIO_OSPEEDER_OSPEEDR3_Pos |
					 3 << GPIO_OSPEEDER_OSPEEDR4_Pos |
					 3 << GPIO_OSPEEDER_OSPEEDR5_Pos ;
	GPIOA->MODER |= 1 << GPIO_MODER_MODER11_Pos|	//PA11 output (CS)
	                1 << GPIO_MODER_MODER2_Pos;		//PA2  output (CS)
	GPIOB->AFR[0] |= 5 << GPIO_AFRL_AFRL3_Pos| //alternate func AF5 
	                 5 << GPIO_AFRL_AFRL4_Pos| //for 3 pins
	                 5 << GPIO_AFRL_AFRL5_Pos;
	GPIOB->MODER |= 2 << GPIO_MODER_MODER3_Pos| //alternate function
	                2 << GPIO_MODER_MODER4_Pos| //for these 3 pins
	                2 << GPIO_MODER_MODER5_Pos;
	GPIOB->PUPDR |= 1 << GPIO_PUPDR_PUPDR3_Pos | //pull-up...
				    1 << GPIO_PUPDR_PUPDR4_Pos |
				    1 << GPIO_PUPDR_PUPDR5_Pos ;
	//3 - Write the CR1 register
	SPI1->CR1 = SPI_CR1_BR_1 |	//fPCLK/8 => 8MHz (max 10MHz)
				//SPI_CR1_BR_2 |	//fPCLK/128 => 0.5MHz (tmp)
				SPI_CR1_MSTR ;	//master mode
	SPI1->CR2 = 0x7 << SPI_CR2_DS_Pos | //select 8 bits
				SPI_CR2_FRXTH |			//Fifo RX threshold 8-bits
				SPI_CR2_SSOE;
	SPI1->CR1 |= SPI_CR1_SPE ;	//spi enabled
}

void beginTransaction()
{
	volatile uint16_t __attribute__((unused)) tmp;
	//while(SPI1->SR & SPI_SR_BSY);
	//while(SPI1->SR & SPI_SR_RXNE) //fifo not empty
	//	tmp = SPI1->DR;	//empty the Rx fifo
	GPIOA->BSRR = GPIO_BSRR_BR_11;	//CS=0
}

void endTransaction()
{
	while(SPI1->SR & SPI_SR_BSY);
	GPIOA->BSRR = GPIO_BSRR_BS_11;	//CS=1
}

//low speed transfer:
// * wait until fifo is not full
// * send data
// * be sure that transfer is complete
// * and return the last value.
uint8_t transfer8(uint8_t val)
{
	volatile uint16_t __attribute__((unused)) tmp;
	volatile SPI_TypeDef *__attribute((unused)) spi=SPI1;
	while(SPI1->SR & SPI_SR_BSY);
	while(SPI1->SR & SPI_SR_RXNE)  //fifo not empty
		tmp = (uint16_t)SPI1->DR;	//empty the Rx fifo
	*(__IO uint8_t *)&SPI1->DR = val;
	while(!(SPI1->SR & SPI_SR_RXNE));
	return *(__IO uint8_t *)&SPI1->DR;
}

//higher speed write transfer (than transfer8)
// * wait until TX fifo is not full
// * send data
void write8(uint8_t val)
{
	//Fifo tx level full?
	while(((SPI1->SR & SPI_SR_FTLVL_Msk) >> SPI_SR_FTLVL_Pos) == 3); 
	*(__IO uint8_t *)&SPI1->DR = val;
}

