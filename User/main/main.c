
#include "stm32h7xx.h"
#include "main.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./flash/bsp_qspi_flash.h" 
#include "./delay/core_delay.h"

typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;
/* ��ȡ�������ĳ��� */
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize     (countof(Tx_Buffer)-1)

#define  FLASH_WriteAddress     0
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress+BufferSize

   
/* ���ͻ�������ʼ�� */
__ALIGN_BEGIN unsigned char Tx_Buffer123[] __ALIGN_END =
{
	  0x30, 0xC7, 0x00, 0x0C, 0x6B, 0x71, 0xAF, 0x3E, 0x85, 0x74, 0x34, 0x51, 0x69, 0xA3, 0x58, 0xB3, 0xAC, 0x76, 0xA1, 0x59, 0xB7, 0xC3, 0x21, 0x72, 0x16, 0xE1, 0xDF, 0x23, 0x20, 0xB9, 0x2A, 0x54, 0x02, 0x5D, 0x18, 0x12, 0x4E, 0x00, 0x6E, 0x42,
  0x0D, 0x0A,
  0x30, 0xC7, 0x00, 0x0C, 0x6B, 0x71, 0xAF, 0x3E, 0x85, 0x74, 0x34, 0x51, 0x69, 0xA3, 0x58, 0xB3, 0xAC, 0x76, 0xA1, 0x59, 0xB7, 0xC3, 0x21, 0x72, 0x16, 0xE1, 0xDF, 0x23, 0x20, 0xB9, 0x2A, 0x54, 0x02, 0x5D, 0x18, 0x12, 0x4E, 0x00, 0x6E, 0x42,
  0x0D, 0x0A,
		  0x30, 0xC7, 0x00, 0x0C, 0x6B, 0x71, 0xAF, 0x3E, 0x85, 0x74, 0x34, 0x51, 0x69, 0xA3, 0x58, 0xB3, 0xAC, 0x76, 0xA1, 0x59, 0xB7, 0xC3, 0x21, 0x72, 0x16, 0xE1, 0xDF, 0x23, 0x20, 0xB9, 0x2A, 0x54, 0x02, 0x5D, 0x18, 0x12, 0x4E, 0x00, 0x6E, 0x42,
  0x0D, 0x0A,
  0x30, 0xC7, 0x00, 0x0C, 0x6B, 0x71, 0xAF, 0x3E, 0x85, 0x74, 0x34, 0x51, 0x69, 0xA3, 0x58, 0xB3, 0xAC, 0x76, 0xA1, 0x59, 0xB7, 0xC3, 0x21, 0x72, 0x16, 0xE1, 0xDF, 0x23, 0x20, 0xB9, 0x2A, 0x54, 0x02, 0x5D, 0x18, 0x12, 0x4E, 0x00, 0x6E, 0x42,
  0x0D, 0x0A,
	  0x30, 0xC7, 0x00, 0x0C, 0x6B, 0x71, 0xAF, 0x3E, 0x85, 0x74, 0x34, 0x51, 0x69, 0xA3, 0x58, 0xB3, 0xAC, 0x76, 0xA1, 0x59, 0xB7, 0xC3, 0x21, 0x72, 0x16, 0xE1, 0xDF, 0x23, 0x20, 0xB9, 0x2A, 0x54, 0x02, 0x5D, 0x18, 0x12, 0x4E, 0x00, 0x6E, 0x42,
  0x0D, 0x0A,
  0x30, 0xC7, 0x00, 0x0C, 0x6B, 0x71, 0xAF, 0x3E, 0x85, 0x74, 0x34, 0x51, 0x69, 0xA3, 0x58, 0xB3, 0xAC, 0x76, 0xA1, 0x59, 0xB7, 0xC3, 0x21, 0x72, 0x16, 0xE1, 0xDF, 0x23, 0x20, 0xB9, 0x2A, 0x54, 0x02, 0x5D, 0x18, 0x12, 0x4E, 0x00, 0x6E

};
uint8_t Tx_Buffer[] = "��stm32������\r\nhttp://firestm32.taobao.c\r\nhttp://firestm32.taobao.com";
uint8_t Tx_Buffer2[] ={0XCC,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A, 0x28, 0x0A,0XFF,0XAA,0XBB,0XCC,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A,0x65, 0x00, 0x99, 0x1C, 0x28, 0x0A, 0x28, 0x0A};
uint8_t Rx_Buffer[BufferSize];

//��ȡ��ID�洢λ��
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;
__IO TestStatus TransferStatus1 = FAILED;

// ����ԭ������
void Delay(__IO uint32_t nCount);
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);


uint8_t state = QSPI_ERROR;
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
//	int i;
	int t0,t1;
	/* ʹ��ָ��� */
	SCB_EnableICache();
  /* ʹ�����ݻ��� */
  SCB_EnableDCache();
	/* ϵͳʱ�ӳ�ʼ����400MHz */
	SystemClock_Config();
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
	LED_BLUE;
	
	/* ���ô���1Ϊ��115200 8-N-1 */
	DEBUG_USART_Config();
  
	printf("\r\n����һ��32M����flash(W25Q256)ʵ��(QSPI����) \r\n");
	
	/* 32M����flash W25Q256��ʼ�� */
	QSPI_FLASH_Init();
	
	/* ��ȡ Flash Device ID */
	DeviceID = QSPI_FLASH_ReadDeviceID();
	
	Delay( 200 );
	/* ��ȡ SPI Flash ID */
	FlashID = QSPI_FLASH_ReadID();

	printf("\r\nFlashID is 0x%X,  Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);
	printf("\r\nFlash Status Reg1 is 0x%04X,\r\n", QSPI_FLASH_ReadStatusReg(1));	
	printf("\r\nFlash Status Reg2 is 0x%04X,\r\n", QSPI_FLASH_ReadStatusReg(2));
	printf("\r\nFlash Status Reg3 is 0x%04X,\r\n", QSPI_FLASH_ReadStatusReg(3));

	/* ���� SPI Flash ID */
	if (FlashID == sFLASH_ID)
	{	
    int i = 0;
        printf("\r\n��⵽QSPI FLASH W25Q256 !\r\n");
        
    /* ������Ҫд��� QSPI FLASH ������FLASHд��ǰҪ�Ȳ��� */
    printf("\r\n����оƬ������%d~%d������!\r\n", FLASH_WriteAddress, FLASH_WriteAddress+BufferSize);
    for(i = 0;i <= BufferSize;i+=4096)
    {
      state = BSP_QSPI_Erase_Block(FLASH_WriteAddress+i);
    } 
//    state = BSP_QSPI_Erase_Chip();
		HAL_InitTick(5);
		t0 = CPU_TS_TmrRd();
		/* �����ͻ�����������д��flash�� */
		BSP_QSPI_Write(Tx_Buffer, FLASH_WriteAddress, BufferSize);

		t1 = CPU_TS_TmrRd();
		printf("\r\nд������ݴ�СΪ��%d Byte",BufferSize);
		printf("\r\nд��ĺ�ʱΪ��%f ms", (float)(t1-t0) / (float)(GET_CPU_ClkFreq()/1000));
		printf("\r\nд����ٶ�Ϊ��%f MB/s", (float)(BufferSize) / (float)(1024*1024) / ((float)(t1-t0) / (float)(GET_CPU_ClkFreq())));
		
		HAL_InitTick(5);
		t0 = CPU_TS_TmrRd();
		/* ���ո�д������ݶ������ŵ����ջ������� */
		BSP_QSPI_Read(Rx_Buffer, FLASH_ReadAddress, BufferSize);
		printf("%s", Rx_Buffer);
			
		t1 = CPU_TS_TmrRd();
		
		printf("\r\n��ȡ�ĺ�ʱΪ��%f ms", (float)(t1-t0) / (float)(GET_CPU_ClkFreq()/1000));
		printf("\r\n��ȡ���ٶ�Ϊ��%f MB/s", (float)(BufferSize) / (float)(1024*1024) / ((float)(t1-t0) / (float)(GET_CPU_ClkFreq())));
		
		/* ���д�������������������Ƿ���� */
		TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);
		
		if( PASSED == TransferStatus1 )
		{    
			LED_GREEN;
			printf("\r\n32M����flash(W25Q256)���Գɹ�!\n\r");
		}
		else
		{        
			LED_RED;
			printf("\r\n32M����flash(W25Q256)����ʧ��!\n\r");
		}
	}// if (FlashID == sFLASH_ID)
	else
	{    
		LED_RED;
		printf("\r\n��ȡ���� W25Q256 ID!\n\r");
	}
	
	while(1);  
}
/*
 * ��������Buffercmp
 * ����  ���Ƚ������������е������Ƿ����
 * ����  ��-pBuffer1     src������ָ��
 *         -pBuffer2     dst������ָ��
 *         -BufferLength ����������
 * ���  ����
 * ����  ��-PASSED pBuffer1 ����   pBuffer2
 *         -FAILED pBuffer1 ��ͬ�� pBuffer2
 */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
/**
  * @brief  System Clock ����
  *         system Clock ��������: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 400000000 (CPU Clock)
	*            HCLK(Hz)             = 200000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  100MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  100MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  100MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  100MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 160
	*            PLL_P                = 2
	*            PLL_Q                = 4
	*            PLL_R                = 2
	*            VDD(V)               = 3.3
	*            Flash Latency(WS)    = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;
  
  /*ʹ�ܹ������ø��� */
  MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

  /* ��������ʱ��Ƶ�ʵ������ϵͳƵ��ʱ����ѹ���ڿ����Ż����ģ�
		 ����ϵͳƵ�ʵĵ�ѹ����ֵ�ĸ��¿��Բο���Ʒ�����ֲᡣ  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
 
  /* ����HSE������ʹ��HSE��ΪԴ����PLL */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
 
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
	/* ѡ��PLL��Ϊϵͳʱ��Դ����������ʱ�ӷ�Ƶ�� */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK  | \
																 RCC_CLOCKTYPE_HCLK    | \
																 RCC_CLOCKTYPE_D1PCLK1 | \
																 RCC_CLOCKTYPE_PCLK1   | \
                                 RCC_CLOCKTYPE_PCLK2   | \
																 RCC_CLOCKTYPE_D3PCLK1);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;  
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; 
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; 
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; 
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}
/****************************END OF FILE***************************/
