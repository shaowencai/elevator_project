#ifndef __STMFLASH_H__
#define __STMFLASH_H__
 
#include "stm32f10x.h"  

#define STM32_FLASH_SIZE 128 	 		//��ѡSTM32��FLASH������С(��λΪK)


#if STM32_FLASH_SIZE<256
    #define STM_SECTOR_SIZE 1024 
#else 
    #define STM_SECTOR_SIZE	2048
#endif

#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ


extern void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
extern void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

#endif

















