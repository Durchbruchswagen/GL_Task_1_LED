#include "main.h"

#define LED_ADDR(num) (GPIO_PIN_12 << (num))
#define M1SND_LED_ADDR(num) (GPIO_PIN_12 << ((led + 1) & 3))
#define M0LED_ADDR(num) (GPIO_PIN_12 << (3 - num))

uint8_t led = 0;
int8_t delay_mult = 10;
uint8_t mode = 0;
uint8_t on = 0;
uint8_t m2sequence[] = {0, 2, 1, 3};

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if((GPIO_Pin == GPIO_PIN_6) | (GPIO_Pin == GPIO_PIN_8) | (GPIO_Pin == GPIO_PIN_15))
	HAL_GPIO_WritePin(GPIOD, LED_ADDR(0), GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, LED_ADDR(1), GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, LED_ADDR(2), GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, LED_ADDR(3), GPIO_PIN_RESET);

	switch (GPIO_Pin)
	{
		case GPIO_PIN_6:
		{
			mode = (mode + 1) % 3;
			break;
		}
		case GPIO_PIN_8:
		{
			//Fix for quirk of c that results in value 255 when mode = 0 due to promotion to signed 32 bit type;
			mode = (mode + 3 - 1) % 3;
			break;
		}
		case GPIO_PIN_15:
		{
			on ^= 1;
			break;
		}
		case GPIO_PIN_11:
		{
			delay_mult = ((delay_mult + 1) <= 16) ? (delay_mult + 1) : 16;
			break;
		}
		case GPIO_PIN_9:
		{
			delay_mult = ((delay_mult - 1) >= 0) ? (delay_mult - 1) : 0;
			break;
		}
	}
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  while (1)
  {
	  while(!on)
	  {
		  HAL_SuspendTick();
		  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		  HAL_ResumeTick();
	  }

	  switch(mode)
	  {
	  	  case 0:
	  			  HAL_GPIO_WritePin(GPIOD, M0LED_ADDR(led), on & GPIO_PIN_SET);
	  	  	  	  HAL_Delay(delay_mult * 100);
	  	  	  	  HAL_GPIO_WritePin(GPIOD, M0LED_ADDR(led), GPIO_PIN_RESET);
	  	  	  	  HAL_Delay(delay_mult * 100);
	  	  	  	  break;
	  	  case 1:
	  			  HAL_GPIO_WritePin(GPIOD, M0LED_ADDR(led), on & GPIO_PIN_SET);
	  	  	  	  HAL_GPIO_WritePin(GPIOD, M1SND_LED_ADDR(led), on & GPIO_PIN_SET);
			  	  HAL_Delay(delay_mult * 100);
			  	  HAL_GPIO_WritePin(GPIOD, M0LED_ADDR(led), GPIO_PIN_RESET);
			  	  HAL_GPIO_WritePin(GPIOD, M1SND_LED_ADDR(led), GPIO_PIN_RESET);
			  	  HAL_Delay(delay_mult * 100);
			  	  break;
	  	  case 2 :
	  		  	  HAL_GPIO_WritePin(GPIOD, LED_ADDR(m2sequence[led]), on & GPIO_PIN_SET);
	  		  	  HAL_Delay(delay_mult * 100);
	  		  	  HAL_GPIO_WritePin(GPIOD, LED_ADDR(m2sequence[led]), GPIO_PIN_RESET);
	  		  	  HAL_Delay(delay_mult * 100);
	  		  	  break;
	  }
	  led = (led + 1) & 3;
  }
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif /* USE_FULL_ASSERT */
