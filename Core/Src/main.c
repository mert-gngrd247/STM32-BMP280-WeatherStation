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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/**
 * Project: STM32 Weather Station
 * Author: Mert Gungordu
 * Date: Jan 2026
 * Description: Custom I2C driver for the BMP280 Temperature/Pressure sensor on STM32G0.
 * Hardware: STM32 Nucleo-G031K8
 */

#define CSV_FORMAT_ON 1
#include <stdio.h>
#include <string.h>
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
// Calibration parameters (factory constants)
uint16_t dig_T1;
int16_t  dig_T2, dig_T3;
uint16_t dig_P1;
int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

// Variable to hold "fine temperature" for pressure calculation
int32_t t_fine;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int32_t BMP280_Compensate_T(int32_t adc_T) {
	int32_t var1, var2, T;

	var1 = ((((adc_T >> 3) - ((int32_t) dig_T1 << 1))) * ((int32_t) dig_T2))
			>> 11;

	var2 = (((((adc_T >> 4) - ((int32_t) dig_T1))
			* ((adc_T >> 4) - ((int32_t) dig_T1))) >> 12) * ((int32_t) dig_T3))
			>> 14;

	t_fine = var1 + var2; // Store this for pressure calc later

	T = (t_fine * 5 + 128) >> 8;
	return T; // Returns temperature in DegC * 100 (e.g., 2550 = 25.50 C)
}

// Returns pressure in Pascals (Pa)
uint32_t BMP280_Compensate_P(int32_t adc_P) {
	int64_t var1, var2, p;

	// Uses t_fine (calculated during Temp step)
	var1 = ((int64_t) t_fine) - 128000;

	var2 = var1 * var1 * (int64_t) dig_P6;
	var2 = var2 + ((var1 * (int64_t) dig_P5) << 17);
	var2 = var2 + (((int64_t) dig_P4) << 35);

	var1 = ((var1 * var1 * (int64_t) dig_P3) >> 8)
			+ ((var1 * (int64_t) dig_P2) << 12);
	var1 = (((((int64_t) 1) << 47) + var1)) * ((int64_t) dig_P1) >> 33;

	if (var1 == 0) {
		return 0; // Avoid exception caused by division by zero
	}

	p = 1048576 - adc_P;
	p = (((p << 31) - var2) * 3125) / var1;

	var1 = (((int64_t) dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((int64_t) dig_P8) * p) >> 19;

	p = ((p + var1 + var2) >> 8) + (((int64_t) dig_P7) << 4);

	return (uint32_t) p / 256;
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

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
	MX_I2C1_Init();
	MX_USART2_UART_Init();

	/* USER CODE BEGIN 2 */
#define BMP280_ADDR (0x76 << 1)

	// --- STEP 1: WAKE UP THE SENSOR ---
	uint8_t config[2];
	config[0] = 0xF4; // Register Address: ctrl_meas
	config[1] = 0x27; // Value: 00100111 (Normal Mode, x1 Oversampling)

	// Send the configuration FIRST
	if (HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, config, 2, 100)
			== HAL_OK) {
		HAL_UART_Transmit(&huart2, (uint8_t*) "Sensor Configured.\r\n", 20,
				100);
	} else {
		HAL_UART_Transmit(&huart2, (uint8_t*) "Config Failed.\r\n", 16, 100);
	}

	// --- STEP 2: READ CALIBRATION DATA (Do this AFTER transmit) ---
	uint8_t trimdata[24];
	HAL_I2C_Mem_Read(&hi2c1, BMP280_ADDR, 0x88, 1, trimdata, 24, 100);

	// Combine bytes (Little Endian) into our global variables
	dig_T1 = (trimdata[1] << 8) | trimdata[0];
	dig_T2 = (trimdata[3] << 8) | trimdata[2];
	dig_T3 = (trimdata[5] << 8) | trimdata[4];

	dig_P1 = (trimdata[7] << 8) | trimdata[6];
	dig_P2 = (trimdata[9] << 8) | trimdata[8];
	dig_P3 = (trimdata[11] << 8) | trimdata[10];
	dig_P4 = (trimdata[13] << 8) | trimdata[12];
	dig_P5 = (trimdata[15] << 8) | trimdata[14];
	dig_P6 = (trimdata[17] << 8) | trimdata[16];
	dig_P7 = (trimdata[19] << 8) | trimdata[18];
	dig_P8 = (trimdata[21] << 8) | trimdata[20];
	dig_P9 = (trimdata[23] << 8) | trimdata[22];

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		uint8_t rawData[6];
		HAL_I2C_Mem_Read(&hi2c1, BMP280_ADDR, 0xF7, 1, rawData, 6, 100);

		// 1. Reconstruct Raw Values
		int32_t rawPress = (rawData[0] << 12) | (rawData[1] << 4)
				| (rawData[2] >> 4);
		int32_t rawTemp = (rawData[3] << 12) | (rawData[4] << 4)
				| (rawData[5] >> 4);

		// 2. Calculate Real Values (ORDER MATTERS!)
		int32_t temp_100 = BMP280_Compensate_T(rawTemp); // Updates t_fine
		uint32_t press_Pa = BMP280_Compensate_P(rawPress); // Uses t_fine

		// 3. Format and Print
		// We convert Pascals (Pa) to Hectopascals (hPa) by dividing by 100
		// Standard sea level pressure is roughly 1013 hPa.
		char msg[64];


#if CSV_FORMAT_ON
		sprintf(msg, "%ld,%ld\r\n", (temp_100 / 100), (press_Pa / 100));
#else
		sprintf(msg, "Temp: %ld.%02ld C | Press: %ld hPa\r\n", temp_100 / 100,
				temp_100 % 100, press_Pa / 100);
#endif
		HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), 100);
		HAL_Delay(500);
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
