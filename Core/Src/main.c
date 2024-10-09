/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdlib.h"

#include "ili9341.h"
#include "ili9341_gfx.h"

#include "guitar.h"
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
const int noteFrequencies[15] = {
    247, // B3
    262, // C4
    294, // D4
    311, // D#4 
    330, // E4
    370, // F#4 
    392, // G4
    440, // A4
    494, // B4
    523, // C5
    587, // D5
    622, // D#5 / Eb5
    659, // E5
    698, // F5
    784  // G5
};



// Game
int life;
int score;

// Screen
extern ili9341_t *_screen;

// Systick
volatile int frameUp = 1;
int timerFPS = 0;
float fps = 75; 	// 75ms (1 frame par 75ms)

extern volatile int nextNoteUp;
float tempoNextNote = 900;

int full_timer;

// Ultrason
extern volatile float position_us; //TODO


volatile int swt=0,tempsMontant=0,tempsDesc=0,TEmpsDePropagationEnus=0;
volatile float distance=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
		if (swt == 0) {
		tempsMontant = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_4);
		swt = 1;
		}
		else {
		tempsDesc = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_4);
		swt = 0;
			TEmpsDePropagationEnus = abs(tempsDesc-tempsMontant);
			
			distance = TEmpsDePropagationEnus/58.82;
			
		}
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_SYSTICK_Callback(void) {
	full_timer++;
	timerFPS++;
	
	if (timerFPS >= fps){
		frameUp = 1;
		timerFPS = 0;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t detect){
		
	if(HAL_GPIO_ReadPin(detect_GPIO_Port, detect_Pin) == GPIO_PIN_RESET){
					HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
		}
		else{
					HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
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
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM10_Init();
  MX_UART5_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	
ili9341_text_attr_t text_attr = {&ili9341_font_11x18, ILI9341_WHITE, ILI9341_BLACK,0,0};

	// Screen
	_screen = ili9341_new(
		&hspi1,
		Void_Display_Reset_GPIO_Port, Void_Display_Reset_Pin,
		TFT_CS_GPIO_Port, TFT_CS_Pin,
		TFT_DC_GPIO_Port, TFT_DC_Pin,
		isoLandscape,
		NULL,  NULL,
		NULL, NULL,
		itsNotSupported,
		itnNormalized);

HAL_TIM_Base_Start_IT(&htim2);
HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_4);

HAL_TIM_Base_Start_IT(&htim10);
HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);
HAL_TIM_IC_Start_IT(&htim10, TIM_CHANNEL_2);



void Play_Note_Based_On_Index(int idex);
void Play_Note_Based_On_DistancePERSO(float dist);
HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		initGame();
		el_condor();
		
		
		
		while(life) {
		uint8_t tx_mot[1];
		uint8_t rx_mot[1];
	
		for(int i = 0; i <10000; i++){
				char buffer[30] = {0};
				
				 tx_mot[0] = i;	
				HAL_UART_Transmit_DMA(&huart5, tx_mot, 1);
				HAL_UART_Receive(&huart5, rx_mot, 1, 150);
	
				sprintf(buffer, "message envoyer : %2i", tx_mot[0]);
				
				ili9341_text_attr_t text_attr1 = {&ili9341_font_11x18,ILI9341_WHITE, ILI9341_BLACK,0,0};
				
				ili9341_draw_string(_screen, text_attr1, buffer);
				
				sprintf(buffer, "message recu : %2i", rx_mot[0]);
				
			ili9341_text_attr_t text_attr2 = {&ili9341_font_11x18,ILI9341_WHITE, ILI9341_BLACK,0,120};
				
				ili9341_draw_string(_screen, text_attr2, buffer);
				
				
				ili9341_fill_screen(_screen, ILI9341_BLACK);	
			
		}

			while(!frameUp) {
				display_guitar(position_us);
				char buffer[30]={0};
			sprintf(buffer, "Value : %-6.1f", distance);
			ili9341_draw_string(_screen, text_attr, buffer);
						Play_Note_Based_On_DistancePERSO(distance);

				// play the right frequency
			}
			frameUp = 0;
			next_state();
			display_score();
			display_guitar(position_us);
		}	
		gameOver();
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void Play_Note_PWMPERSO(uint32_t note_frequency) {
		
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
	
		// Calcul de la période du signal PWM basée sur la fréquence de la note
    // Clock source est PCLK1 x 2 si APB1 prescaler est != 1 sinon c'est égale à PCLK1
    int timer_clock = HAL_RCC_GetPCLK1Freq() << 1; // Multipliez par 2 si APB1 prescaler est != 1
    int pwm_period = timer_clock / (htim3.Init.Prescaler + 1) / note_frequency - 1;

    // Mettre à jour ARR pour changer la fréquence du PWM
    __HAL_TIM_SET_AUTORELOAD(&htim3, pwm_period);

    // Pour un rapport cyclique de 50%, le CCR doit être la moitié du ARR
    uint32_t ccr_value = pwm_period / 2;
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, ccr_value);

    // Démarrer ou redémarrer le PWM
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

void Play_Note_Based_On_DistancePERSO(float dist) {

		if(dist >= 0 && dist < 4) {
        Play_Note_PWMPERSO(noteFrequencies[0]);   // B3
    } else if(dist >= 4 && dist < 8) {
        Play_Note_PWMPERSO(noteFrequencies[1]); //C4
    } else if(dist >= 8 && dist < 12) {
        Play_Note_PWMPERSO(noteFrequencies[2]);  // D4
    } else if(dist >= 12 && dist < 16) {
        Play_Note_PWMPERSO(noteFrequencies[3]); // D4#
    }	else if(dist >= 20 && dist < 24) {
        Play_Note_PWMPERSO(noteFrequencies[4]);// E4
    }	else if(dist >= 24 && dist < 28) {
        Play_Note_PWMPERSO(noteFrequencies[5]); // F4#
    }	else if(dist >= 28 && dist < 32) {
        Play_Note_PWMPERSO(noteFrequencies[6]); // G4
    }	else if(dist >= 32 && dist < 36) {
        Play_Note_PWMPERSO(noteFrequencies[7]); // A4
    }	else if(dist >= 36 && dist < 40) {
        Play_Note_PWMPERSO(noteFrequencies[8]);// B4
    }	else if(dist >= 40 && dist < 44) {
        Play_Note_PWMPERSO(noteFrequencies[9]); // C5
    }	else if(dist >= 44 && dist < 48) {
					Play_Note_PWMPERSO(noteFrequencies[10]); // D5
    }	else if(dist >= 48 && dist < 52) {
					Play_Note_PWMPERSO(noteFrequencies[11]);// D5#
    }	else if(dist >= 52 && dist < 56) {
					Play_Note_PWMPERSO(noteFrequencies[12]); // E5
    }	else if(dist >= 56 && dist < 60) {
        Play_Note_PWMPERSO(noteFrequencies[13]); //F5#
    }	else if(dist >= 60) {
        Play_Note_PWMPERSO(noteFrequencies[14]); // G5
    }
}
//duo
void Play_Note_PWMDUO(uint32_t note_frequency) {
		
	HAL_TIM_PWM_Stop(&htim10, TIM_CHANNEL_1);
	
		// Calcul de la période du signal PWM basée sur la fréquence de la note
    // Clock source est PCLK1 x 2 si APB1 prescaler est != 1 sinon c'est égale à PCLK1
    int timer_clock = HAL_RCC_GetPCLK1Freq() << 1; // Multipliez par 2 si APB1 prescaler est != 1
    int pwm_period = timer_clock / (htim10.Init.Prescaler + 1) / note_frequency - 1;

    // Mettre à jour ARR pour changer la fréquence du PWM
    __HAL_TIM_SET_AUTORELOAD(&htim10, pwm_period);

    // Pour un rapport cyclique de 50%, le CCR doit être la moitié du ARR
    uint32_t ccr_value = pwm_period / 2;
    __HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, ccr_value);

    // Démarrer ou redémarrer le PWM
    HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);
}

void Play_Note_Based_On_Index(int idex) {

		 if(idex ==1) {
        Play_Note_PWMDUO(noteFrequencies[0]);   // B3
    } else if(idex ==2) {
        Play_Note_PWMDUO(noteFrequencies[1]); //C4
    } else if(idex ==3) {
        Play_Note_PWMDUO(noteFrequencies[2]);  // D4
    } else if(idex ==4) {
        Play_Note_PWMDUO(noteFrequencies[3]); // D4#
    }	else if(idex ==5) {
        Play_Note_PWMDUO(noteFrequencies[4]);// E4
    }	else if(idex ==6) {
        Play_Note_PWMDUO(noteFrequencies[5]); // F4#
    }	else if(idex ==7) {
        Play_Note_PWMDUO(noteFrequencies[6]); // G4
    }	else if(idex ==8) {
        Play_Note_PWMDUO(noteFrequencies[7]); // A4
    }	else if(idex ==9) {
        Play_Note_PWMDUO(noteFrequencies[8]);// B4
    }	else if(idex ==10) {
        Play_Note_PWMDUO(noteFrequencies[9]); // C5
    }	else if(idex ==11) {
					Play_Note_PWMDUO(noteFrequencies[10]); // D5
    }	else if(idex ==12) {
					Play_Note_PWMDUO(noteFrequencies[11]);// D5#
    }	else if(idex ==13) {
					Play_Note_PWMDUO(noteFrequencies[12]); // E5
    }	else if(idex ==14) {
        Play_Note_PWMDUO(noteFrequencies[13]); //F5#
    }	else if(idex ==15) {
        Play_Note_PWMDUO(noteFrequencies[14]); // G5
    }
}
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

#ifdef  USE_FULL_ASSERT
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
