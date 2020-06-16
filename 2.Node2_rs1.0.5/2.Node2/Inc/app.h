#ifndef _APP_H_
#define _APP_H_

#define EN4V_PORT		GPIOA
#define EN4V_PIN		GPIO_PIN_4

#define OFF_4V()		HAL_GPIO_WritePin(EN4V_PORT, EN4V_PIN, GPIO_PIN_SET)
#define ON_4V()			HAL_GPIO_WritePin(EN4V_PORT, EN4V_PIN, GPIO_PIN_RESET)


#define FIX_ULTRASONIC_POSITON	170		// Vị trí đặt cảm biến siêu âm = 170cm, so với nền đất



#define USING_LORA_RECEIVE_IT		0
#define USING_LORA_RECEIVE_DMA	1

#define USING_SIM_RECEIVE_DMA		0
#define USING_SIM_RECEIVE_IT		1

#if (USING_LORA_RECEIVE_IT == 1) && (USING_LORA_RECEIVE_DMA ==1 )
#error "Can not using two configuration at the same time"
#endif

#if (USING_SIM_RECEIVE_DMA == 1) && (USING_SIM_RECEIVE_IT ==1 )
#error "Can not using two configuration at the same time"
#endif






#define MASTER_PNB	"+84384544727"	// Hoang Loi's phone number	
#define MASTER_PNB2	"+84981240081"		// Dong Phu's phone number	

#define USER_PNB		"0384544727"
#define N1_PHONENUMBER		"0396226489"
#define GW_PHONENUMBER		"0393614489"

#endif
//

