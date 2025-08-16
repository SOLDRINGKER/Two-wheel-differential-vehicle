#include "sys.h"

// 关闭所有中断(不包括fault和NMI中断)
void WFI_SET(void)
{
	__asm("WFI");
}

// 关闭所有中断
void INTX_DISABLE(void)
{
	__asm("CPSID I");
	__asm("BX LR");
}

// 开启所有中断
void INTX_ENABLE(void)
{
	__asm("CPSIE I");
	__asm("BX LR");
}

// 设置栈顶地址
// addr: 栈顶地址
void MSR_MSP(uint32_t addr)
{
	__asm("MSR MSP, r0");  // set Main Stack value
	__asm("BX LR");
}


/**************************************************************************
Function: Set JTAG mode
Input   : mode:JTAG, swd mode settings��00��all enable��01��enable SWD��10��Full shutdown
Output  : none
�������ܣ�����JTAGģʽ
��ڲ�����mode:jtag,swdģʽ����;00,ȫʹ��;01,ʹ��SWD;10,ȫ�ر�;	
����  ֵ����
**************************************************************************/
//#define JTAG_SWD_DISABLE   0X02
//#define SWD_ENABLE         0X01
//#define JTAG_SWD_ENABLE    0X00	
void JTAG_Set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR|=1<<0;     //��������ʱ��	   
	AFIO->MAPR&=0XF8FFFFFF; //���MAPR��[26:24]
	AFIO->MAPR|=temp;       //����jtagģʽ
} 
