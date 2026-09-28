/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*
Période du bit :
1250ns
212.5 ticks ~213
ARR = (213-1)

Bit 0 (T0H)
400ns
68 ticks
CCR = 68

Bit 1 (T1H)
800ns
136 ticks
CCR = 136
*/
#define CCR_T0H 68
#define CCR_T1H 136

#define LED_NUMBER 1
#define COLOR_NUMBER 3
#define BITS_PER_COLOR 8

#define NP_BUFFER_LENGTH (LED_NUMBER * COLOR_NUMBER * BITS_PER_COLOR + 1)

typedef struct color_struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} color_t;

static color_t red   = {.r = 50, .g =  0, .b =  0};
static color_t green = {.r =  0, .g = 50, .b =  0};
static color_t blue  = {.r =  0, .g =  0, .b = 50};

uint8_t np_dma_buffer[NP_BUFFER_LENGTH];

/* Transformer une couleur (rgb) en tableau de 24 valeurs de CCR
 * Chaque periode de timer code un bit Neopixel
 * En gros, on construit le buffer DMA
 */
void color_to_buffer(color_t * color, uint8_t * buffer) {
	/* Green */
	for (int i = 0 ; i < BITS_PER_COLOR ; i++) {
		buffer[i] = ((color->g >> (7-i) & 0x01)) ? CCR_T1H : CCR_T0H;
	}
	/* Red */
	for (int i = 0 ; i < BITS_PER_COLOR ; i++) {
		buffer[BITS_PER_COLOR + i] = ((color->r >> (7-i) & 0x01)) ? CCR_T1H : CCR_T0H;
	}
	/* Blue */
	for (int i = 0 ; i < BITS_PER_COLOR ; i++) {
		buffer[2*BITS_PER_COLOR + i] = ((color->b >> (7-i) & 0x01)) ? CCR_T1H : CCR_T0H;
	}

	/* Dernier bit à 0 pour forcer un 0V sur le neopixel */
	buffer[NP_BUFFER_LENGTH-1] = 0;
}

/* Callback d'interruption de DMA de fin de transfert */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (TIM2 == htim->Instance) {
		/* On arrête la PWM quand le transfert DMA est terminé */
		HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
	}
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_I2C1_Init();
	MX_TIM3_Init();
	MX_TIM2_Init();
	MX_USART2_UART_Init();
	MX_SPI1_Init();
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		color_to_buffer(&red, np_dma_buffer);

		HAL_TIM_PWM_Start_DMA (
				&htim2,
				TIM_CHANNEL_1,
				(uint32_t *)np_dma_buffer,
				NP_BUFFER_LENGTH
		);

		HAL_Delay(500);

		color_to_buffer(&green, np_dma_buffer);

		HAL_TIM_PWM_Start_DMA (
				&htim2,
				TIM_CHANNEL_1,
				(uint32_t *)np_dma_buffer,
				NP_BUFFER_LENGTH
		);

		HAL_Delay(500);

		color_to_buffer(&blue, np_dma_buffer);

		HAL_TIM_PWM_Start_DMA (
				&htim2,
				TIM_CHANNEL_1,
				(uint32_t *)np_dma_buffer,
				NP_BUFFER_LENGTH
		);

		HAL_Delay(500);
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
	RCC_OscInitStruct.PLL.PLLN = 85;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
