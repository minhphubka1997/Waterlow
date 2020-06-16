#include "main.h"
#include "stdarg.h"
#include "hl_printf.h"
#include "string.h"
#include "ring_buffer.h"
#include "sim.h"
#include "STM_MY_LCD16X2.h"
#include "app.h"
#include "stdio.h"
#include "stdlib.h"
#include "sim.h"


/*------------- Private flag -------------------*/
extern bool SFlagSendingMSG;
extern bool SFlagRdySendMsg;		// Flag đã gửi lệnh AT để gửi tin nhắn
extern bool SFlagSendMsgOK;			// Flag thể hiện đã gửi tin nhắn xong ok
extern bool SFlagMakeCall;	// Flag đã gửi lệnh AT thực hiện cuộc gọi
extern bool SFlagBusy;		// SIM is busy (ex: sending msg, calling..)
extern bool SFlagInitializing;
	
extern bool SIMReady;
extern bool SIMOn;
extern bool SIMCallReady;
extern bool SIMSMSReady;
extern bool SIMOK;
extern bool SIMError;


extern uint8_t sim_mode;


extern uint8_t sim_init_step;	// Biến thể hiện tiến trình init
extern uint32_t sim_init_time;							// Biến phục vụ tính timeout cho mỗi lần thực hiện init chức năng nào đó
extern uint8_t sim_cmd_count;							// Biến đếm số lần gửi AT CMD failed
extern uint8_t sim_init_num;							// Biến đếm số lần init failed



void debugsimlcd(char number)
{
	#if DBG_SIM_VIA_LCD > 0
	char nb[2] = {number + 48, 0};
	LCD1602_clear();
	LCD1602_setCursor(1,6);
	LCD1602_print("SIM:");
	LCD1602_setCursor(1,10);
	LCD1602_print(nb);
	HAL_Delay(500);
	#endif
}

//




/* -----------------------------------------------------------
							        Call back zone
 * ------------------------------------------------------------
*/
//

/*
 *	sim_ok_callback
 *	This function is trigger when received response "OK"
*/
void sim_ok_callback(void)
{
	//debugsimlcd(4);
	SIMOK = true;
}
//

/*
 *	sim_on_callback
 *	This function is trigger when received response "DRY"
*/
void sim_on_callback(void)
{
	//debugsimlcd(1);
	SIMReady = true;
}
//


/*
 *	sim_error_callback
 *	This function is trigger when received response "ERROR"
*/
void sim_error_callback(void)
{
	//debugsimlcd(5);
	SIMError = true;
}
//


/*
 *	sim_sms_text_msg_callback
 *	This function is trigger when received character '>', after send AT command send message
*/
void sim_sms_text_msg_callback(void)
{
	SFlagRdySendMsg = true;
}
//

/*
 *	sim_sms_send_msg_ok_callback
 *	When send a message done, this function will trigger
*/
void sim_sms_send_msg_ok_callback(void)
{
	SFlagSendMsgOK = true;
	SFlagSendingMSG = false;
}
//

/*
 *	sim_sms_send_msg_failed_callback
 *	When send a message failed, this function will trigger
*/
void sim_sms_send_msg_failed_callback(void)
{
	SFlagSendingMSG = false;
	SFlagSendMsgOK = false;
}
//


/*
 *	sim_call_ready_callback
 *	This function is trigger when received response "Call ready"
 *
*/
void sim_call_ready_callback(void)
{
	//debugsimlcd(2);
	SIMCallReady = true;
}
//


/*
 *	sim_sms_ready_callback
 *	This function is trigger when received response "SMS ready"
 *
*/
void sim_sms_ready_callback(void)
{
	//debugsimlcd(3);
	SIMSMSReady = true;
}
//


/*
 *	sim_ring_callback
 *	This function is trigger when received response "RING"
 *
*/
void sim_ring_callback(void)
{
	//debugsimlcd(7);
	sim_printf("%s", CMD_STOP_INCOMINGCALL);
}
//

/*
 *	sim_busy_callback
 *	This function is trigger when received response "BUSY"
 *
*/
void sim_busy_callback()
{
	//debugsimlcd(6);
}
//

/*
 *	sim_nocarrier_callback
 *	This function is trigger when received response "NO CARRIER"
 *
*/
void sim_nocarrier_callback()
{
	//debugsimlcd(8);
}
//

/*
 *	sim_init_ok_callback
 *	Affter one or afew time init and succes this function will trigged
*/
void sim_init_ok_callback(void)
{
	SFlagRdySendMsg = false;		// Flag đã gửi lệnh AT để gửi tin nhắn
	SFlagMakeCall	= false;	// Flag đã gửi lệnh AT thực hiện cuộc gọi
	SFlagBusy			= false;		// SIM is busy (ex: sending msg, calling..)
	SFlagInitializing = false;
	SFlagSendMsgOK = false;
	SFlagSendingMSG = false;
	
	SIMOK				= false;
	SIMError 		= false;

	sim_mode = MODE_IDLE;
	sim_init_step = SIM_INIT_STEP0;	
	sim_init_time = HAL_GetTick();				
	sim_cmd_count = 0;							
	sim_init_num	= 0;		
	
	#if DBG_SIM_VIA_LCD > 0
	LCD1602_1stLine();
	LCD1602_print("SIM: Init OK");
	#endif
}
//

/*
 *	sim_init_failed_callback
 *	Affter SIM_INIT_RETRY_NUM time init failed, this function will be call
*/
void sim_init_failed_callback(void)
{
	SFlagInitializing = false;
	#if DBG_SIM_VIA_LCD > 0
	LCD1602_clear();
	LCD1602_1stLine();
	LCD1602_print("SIM: Init Failed");
	#endif
}
//

/*
 *	sim_cmd_failed_callback
 *	Affter SIM_CMD_RETRY_NUM time send cmd failed, this function will be call
*/
void sim_cmd_failed_callback(char cmd[])
{
	#if DBG_SIM_VIA_LCD > 0
	LCD1602_1stLine();
	LCD1602_print(cmd);
	LCD1602_2ndLine();
	LCD1602_print("SIM: CMD Failed");
	#endif
}
//


SMSReceiveBuffStructure		sim_sms_receive;

/*
 * sim_sms_receive_new_callback
 * This callback will trigger affter receive new sms message
*/
void sim_sms_receive_new_callback(void)
{
	#if NOTIFY_SIM_STATE_VIA_LCD	> 0
//	LCD1602_clear();
//	LCD1602_1stLine();
//	LCD1602_print(sim_sms_receive.phone_number);
//	LCD1602_2ndLine();
//	LCD1602_print(sim_sms_receive.msg_content);
	
	#endif
}
//




/* -----------------------------------------------------------
							Receive and decode data from SIM 
 * ------------------------------------------------------------
*/
//

typedef struct
{
	char buffer[SIM_LINE_LEN];
	uint16_t	length;
} BUFFER_typdef;
//



BUFFER_typdef      	sim_data_line;
ring_buffer_typdef 	sim_data_buffer;


bool isPhonenumberCharacter(char ch)
{
	if ((ch == '+')||((ch >= '0')&&(ch <= '9'))) return true;
	else return false;
}
//


/*
 * sim_receive_decode
 * This function decode all response from SIM
*/
void sim_receive_decode(void)
{
	if (sim_sms_receive.needfillcontent == true)
	{
		if (sim_data_line.length > 0)
		{
			strcpy(sim_sms_receive.msg_content, sim_data_line.buffer);
			sim_sms_receive.needfillcontent = false;
			sim_sms_receive_new_callback();
			return;
		}
	}
	
	if (strcmp(sim_data_line.buffer, RES_OK) == 0)
	{
		sim_ok_callback();
		return;
	}
	else
	if (strcmp(sim_data_line.buffer, RES_ERROR) == 0)
	{
		sim_error_callback();
		return;
	}
	else
	if (strcmp(sim_data_line.buffer, RES_CALL_RDY) == 0)
	{
		sim_call_ready_callback();
		return;
	}
	else
	if (strcmp(sim_data_line.buffer, RES_SMS_RDY) == 0)
	{
		sim_sms_ready_callback();
		return;
	}
	else
	if (strcmp(sim_data_line.buffer, RES_BUSY) == 0)
	{
		sim_busy_callback();
		return;
	}
	else
	if (strcmp(sim_data_line.buffer, RES_NO_CARRIER) == 0)
	{
		sim_nocarrier_callback();
		return;
	}
	else	
	if (strcmp(sim_data_line.buffer, RES_RING) == 0)
	{
		sim_ring_callback();
		return;
	}
	else	
	if (strcmp(sim_data_line.buffer, RES_DRY) == 0)
	{
		sim_on_callback();
		return;
	}
	if (strstr(sim_data_line.buffer, "+CMT:") != NULL )
	{
		sim_sms_receive.needfillcontent = true;
		uint8_t index = 0;
		bool begin = false;
		// Get phone number from message
		for (int i = 0; i< 22; i++)
		{
			char cc = sim_data_line.buffer[i];
			if (begin == true)
			{
				if (isPhonenumberCharacter(cc))
					sim_sms_receive.phone_number[index ++] = cc;
				else
					if (cc == '"')	// End number
						return;
			}
			
			if (begin == false)
			if (cc == '"')
			{
				begin = true;
				continue;
			}
		}
		//
	}
}
//



/*
 *	sim_receive_process
 *	read data from module SIM
 *	check and call:
						+ OK callback
						+ ERROR callback
						+ Ready to text message callback
						+ Call ready callback
						+ SMS ready callback
						+ RING callback (incomming call)
						+ Receive new message callback
						+ BUSY callback
						+ NO CARRIER callback
*/
void sim_receive_process(void)
{
	volatile char c = 0;
	uint8_t endcode_len = strlen(SIM_END_CODE);
	
	
	while ((c=ring_buffer_pop(&sim_data_buffer)) >0)
	{
		if (c != 0)
		{
			if (sim_data_line.length < SIM_LINE_LEN )
			{
				sim_data_line.buffer[sim_data_line.length++] = c;
				
				if (sim_data_line.length>= endcode_len)
				{
					char found_endcode = 1;
					for (int i = 0; i< endcode_len; i++) if (SIM_END_CODE[i] != sim_data_line.buffer[sim_data_line.length-(endcode_len-i)]) found_endcode = 0;
					if (found_endcode == 1) 
					{
						sim_data_line.buffer[sim_data_line.length++] = 0; // set end of line
						sim_receive_decode();
						sim_data_line.length = 0;
					}
				}
				
				if (sim_data_line.length >= SIM_LINE_LEN ) sim_data_line.length = 0;
			}
			else sim_data_line.length = 0;
			
			if ((SFlagSendingMSG) &&(c == '>')) sim_sms_text_msg_callback();
		}
	}
}
//


void sim_clear_buffer(void)
{
	ring_buffer_clear(&sim_data_buffer);
}
//
