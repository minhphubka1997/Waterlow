#include "main.h"
#include "sim.h"
#include "string.h"
#include "ring_buffer.h"
#include "STM_MY_LCD16X2.h"
#include "app.h"
#include "stdio.h"
#include "stdlib.h"




/*------------ Private function ----------------*/
void sim_receive_process(void);
void sim_clear_buffer(void);
void sim_send_data(uint8_t data[], int lenght);

void sim_poll(void);
void sim_poll_init(void);
void sim_poll_send_msg(void);
void sim_init_ok_callback(void);
void sim_init_failed_callback(void);
void sim_init_step_process(char currentCMD[], char nextCMD[], uint8_t nextStep);
void sim_cmd_failed_callback(char cmd[]);

void sim_sms_send_msg_ok_callback(void);
void sim_sms_send_msg_failed_callback(void);



/*------------- Private flag -------------------*/
bool SFlagSendingMSG = false;
bool SFlagRdySendMsg = false;		// Flag đã gửi lệnh AT để gửi tin nhắn
bool SFlagSendMsgOK		= false;	// Flag đã gửi tin nhắn OK
bool SFlagMakeCall	= false;	// Flag đã gửi lệnh AT thực hiện cuộc gọi
bool SFlagBusy			= false;		// SIM is busy (ex: sending msg, calling..)
bool SFlagRetryInit	= false;
bool SFlagInitializing = false;

bool SIMReady	 = false;
bool SIMOn		= false;
bool SIMCallReady = false;
bool SIMSMSReady = false;
bool SIMOK				= false;
bool SIMError 		= false;


/*----------- Private variables for init sim----------------*/
uint8_t sim_init_step = SIM_INIT_STEP0;	// Biến thể hiện tiến trình init
uint32_t sim_init_time = 0;							// Biến phục vụ tính timeout cho mỗi lần thực hiện init chức năng nào đó
uint8_t sim_cmd_count = 0;							// Biến đếm số lần gửi AT CMD failed
uint8_t sim_init_num	= 0;							// Biến đếm số lần init failed



bool sim_is_initializing(void) { return SFlagInitializing; }
bool sim_is_init_ok(void)
{
	return (SIMReady&&SIMOn&&SIMCallReady&&SIMSMSReady&&(SFlagInitializing == false));
}
//
bool sim_is_sendingMsg(void) {return SFlagSendingMSG; };


uint32_t TimeCheckSIM = 0;
void sim_check(void)
{
	#if DBG_SIM_VIA_LCD > 0
	LCD1602_1stLine();
	LCD1602_print("check sim");
	HAL_Delay(500);
	#endif
	
	if (HAL_GetTick() - TimeCheckSIM >= 5010)
	{
		sim_printf("AT\r\n");
		TimeCheckSIM = HAL_GetTick();
	}
}
//

/*
 *	sim_process
 *	Public function
 * 	This function handler all action of module SIM
 * 	This fucntion must be call in loop (like while(1))
*/
void sim_process(void)
{
	sim_receive_process();
	sim_poll();
}
//




uint8_t sim_mode = MODE_OFF;
uint32_t	sim_life_time = 0;
/*
 * sim_poll	[private function]
 * This function do something like send async message, init sim in async mode
*/
void sim_poll(void)
{
	switch (sim_mode)
	{
		case MODE_OFF: 
			if (SFlagRetryInit)
				if (HAL_GetTick() - sim_init_time >= 3000)
					sim_mode = MODE_INIT;
			
			break;
		case MODE_IDLE: 

			break;
		case MODE_INIT:
			sim_poll_init();
			break;
		case MODE_SEND_MSG:
			sim_poll_send_msg();
			break;
	}
}
//


/*
 * sim_off [private sim]
 * Turn SIM power off
*/
void sim_off(void)
{ 
	OFF_4V();
	sim_mode = MODE_OFF;
	sim_init_step = SIM_INIT_STEP0;

	SFlagRetryInit = false;
	SIMOn = false;
	SIMCallReady = false;
	SIMSMSReady = false;
	SIMReady = false;
	SIMOK = false;
	SIMError = false;
	
	SFlagBusy = false;
	SFlagMakeCall = false;
	SFlagRdySendMsg = false;
	SFlagSendMsgOK = false;
	SFlagSendingMSG = false;
	
	sim_clear_buffer();
	sim_init_time = HAL_GetTick();
	
	
	#if NOTIFY_SIM_STATE_VIA_LCD > 0

	#endif
}
//

/*
 * sim_retry_init [Private function]
 * retry initialize SIM
*/
void sim_retry_init(void)
{
	sim_off();
	SFlagRetryInit = true;
	
	if (sim_init_num >= SIM_INIT_RETRY_NUM)
	{
		SFlagBusy = false;
		SFlagRetryInit = false;
		sim_mode = MODE_OFF;
		sim_init_failed_callback();
		sim_init_num = 0;
		return;
	}
	
	#if NOTIFY_SIM_STATE_VIA_LCD > 0
	LCD1602_clear();
	LCD1602_1stLine();
	LCD1602_print("SIM: init error");
	LCD1602_2ndLine();
	LCD1602_print("Retry..         ");
	#endif
	
}
//

#define BASE_STEP_X		7
uint32_t	waitime = 0;
uint32_t 	waitsecondcount  = 0;

/*
 *	sim_poll_init[ Private function]
 *	Do everything to maintain abiliti to sync initialize SIM 
 * 	must be call in loop
*/
void sim_poll_init(void)
{
	switch(sim_init_step)
	{
		case SIM_INIT_STEP0: 	// Delay 500ms then ON 4V Power
			#if NOTIFY_SIM_STATE_VIA_LCD > 0
			LCD1602_clear();
			LCD1602_1stLine();
			LCD1602_print("SIM: On Power");
			waitsecondcount = 0;
			#endif
			sim_init_num ++;
			ON_4V();
			SIMOn = true;
			sim_init_step = SIM_INIT_STEP1;
			sim_init_time = HAL_GetTick();
		
			break;
		case SIM_INIT_STEP1:
			if (SIMOn && SIMCallReady && SIMReady && SIMSMSReady)
			{
				#if NOTIFY_SIM_STATE_VIA_LCD > 0
				LCD1602_1stLine();
				LCD1602_print("SIM: .          ");
				#endif
				
				sim_init_step = SIM_INIT_STEP2;
				SIMOK = false;
				SIMError = false;
				
				sim_clear_buffer();
				sim_printf("%s", CMD_TURNOFF_ECHO); // Gửi lệnh tắt Echo
				sim_init_time  = HAL_GetTick();
				break;
			}
			else
			{
				if (HAL_GetTick() - waitime >= 1000)
				{	
					#if NOTIFY_SIM_STATE_VIA_LCD > 0
					waitsecondcount ++;
					if (waitsecondcount > 16)
					{
						LCD1602_2ndLine();
						LCD1602_print("                ");
						waitsecondcount = 3;
					}

					LCD1602_setCursor(2, waitsecondcount);
					LCD1602_print(".");
					if (waitsecondcount == 2)
					{
						LCD1602_1stLine();
						LCD1602_print("SIM: Wait ready");
					}
					#endif
					waitime = HAL_GetTick();
				}
				
				if (HAL_GetTick()  - sim_init_time >= 20000) // Timeout -> mean failed -> ReInit
					sim_retry_init();
			}
			break;
		case SIM_INIT_STEP2:
			sim_init_step_process(CMD_TURNOFF_ECHO, CMD_SMS_TEXTMODE, SIM_INIT_STEP3);
			break;
		case SIM_INIT_STEP3:
			sim_init_step_process(CMD_SMS_TEXTMODE, CMD_SHOW_SMD_IME, SIM_INIT_STEP4);
			break;
		case SIM_INIT_STEP4:
			sim_init_step_process(CMD_SHOW_SMD_IME, CMD_AUTO_SLEEPMODE, SIM_INIT_STEP5);
			break;
		case SIM_INIT_STEP5:
			sim_init_step_process(CMD_AUTO_SLEEPMODE, CMD_SAVE, SIM_INIT_STEP6);
			break;
		case SIM_INIT_STEP6:
			sim_init_step_process(CMD_SAVE, CMD_SMS_DELE_ALL, SIM_INIT_STEP7);
			break;
		case SIM_INIT_STEP7:
			sim_init_step_process(CMD_SMS_DELE_ALL, CMD_NONE, SIM_INIT_STEP7);
			break;
	}
}
//

/*
 * sim_init_step_process
 * This function follow a command was sent, check is SIM respone ok, error or timeout
 * if (ok ) send next command, and goto next step
 * if (error) resend current command, and keep follow, if resend time count > SIM_CMD_RETRY_NUM, notify failed and break
 * if (timeout) do the same error case.
*/
void sim_init_step_process(char currentCMD[], char nextCMD[], uint8_t nextStep)
{
	if (SIMOK || SIMError)
	{
		if (SIMOK)
		{
			SIMOK = false;
			SIMError = false;
			
			if (strlen(nextCMD) < 1)
			{
				sim_init_ok_callback();
				return;
			}
			
			#if NOTIFY_SIM_STATE_VIA_LCD > 0
			LCD1602_setCursor( 1, BASE_STEP_X + nextStep - 1);
			LCD1602_print(".");
			LCD1602_setCursor(2,1);
			LCD1602_print("                ");
			#endif
			
			
			sim_printf("%s",nextCMD);	
			sim_init_time  = HAL_GetTick();
			sim_init_step = nextStep;
			sim_cmd_count = 1;
			return;
		}
		if (SIMError)
		{
			SIMOK = false;
			SIMError = false;
			
			if (sim_cmd_count >= SIM_CMD_RETRY_NUM)
			{
				sim_cmd_failed_callback(currentCMD);
				sim_retry_init();
				return;
			}
			
			#if NOTIFY_SIM_STATE_VIA_LCD > 0
			if (sim_cmd_count > 1)
			{
			LCD1602_setCursor( 2, sim_cmd_count);
			LCD1602_print(".");
			}
			#endif
			
			sim_printf("%s", currentCMD); 
			sim_cmd_count ++;
			sim_init_time  = HAL_GetTick();
			return;
		}
	}
	else
	if (HAL_GetTick()  - sim_init_time >= 3000) // Timeout -> mean failed -> ReInit
	{
		SIMOK = false;
		SIMError = false;
		
		if (sim_cmd_count >= SIM_CMD_RETRY_NUM)
		{
			sim_cmd_failed_callback(currentCMD);
			sim_retry_init();
			return;
		}
		
		#if NOTIFY_SIM_STATE_VIA_LCD > 0
		if (sim_cmd_count > 1)
		{
		LCD1602_setCursor( 2, sim_cmd_count);
		LCD1602_print(".");
		}
		#endif
		
		sim_printf("%s", currentCMD); 
		sim_cmd_count ++;
		sim_init_time  = HAL_GetTick();
		return;
	}
}
//


/*
 * sim_async_init
 * async init sim, run one time. and sim_poll() function will automatic do all thing.
 * if init success, sim_init_ok_callback() function will be call
 * else sim_init_failed_callback() function will be call
*/
void sim_async_init(void)
{
	sim_off();
	sim_mode = MODE_INIT;
	SFlagInitializing  = true;
	sim_init_num = 0;
	
	#if DBG_SIM_VIA_LCD > 0
	char snum[5];
	LCD1602_1stLine();
	LCD1602_print("SIM init: ");
	LCD1602_setCursor(1,11);
	sprintf(snum,"%d",sim_init_num);
	LCD1602_print(snum);
	#endif
	
	sim_init_time = HAL_GetTick();
}
//



char sim_msg_content[128];
char phone_number[20];

uint8_t sim_sndmsg_step	 = SIM_SNDMSG_STEP0;
uint32_t TimeSendCMD = 0;
uint8_t sim_msg_at_count = 0;

/*
 * sim_sendmsg_at [private funciton]
 * send AT+CMGS="phone number"\r\n to SIM
 * no response, Just send and forget
*/
void sim_sendmsg_at(void)
{
	char cmd[60];
	sprintf(cmd, "%s\"%s\"\r\n", CMD_SEND_MSG, phone_number);
	sim_printf(cmd);
	sim_msg_at_count ++;
	TimeSendCMD = HAL_GetTick();
}
//


/*
 * sim_msg_follow_at_cmd [private funciton]
 * Check is AT\r\n command was sent get OK, ERROR, or timout response
 * if (OK) send AT+CMGS= ""phone number"\r\n
 * if (ERROR) resend AT\r\n, if resend time > SIM_CMD_RETRY_NUM, failed_callback will be call
 * if (timeout) resend AT\r\n if resend time > SIM_CMD_RETRY_NUM, failed_callback will be call
*/
void sim_msg_follow_at_cmd(void)
{
	if (SIMOK || SIMError)
	{
		if (SIMOK)			// AT : OK
		{
			SIMOK = false;
			SIMError = false;
			SFlagRdySendMsg = false;
			
			sim_sendmsg_at();		// Gửi lệnh gửi tin nhắn AT+CMGS="SDT"
			sim_msg_at_count  = 0;
			TimeSendCMD = HAL_GetTick();
			sim_sndmsg_step = SIM_SNDMSG_STEP2;
		}
		else
		if (SIMError)	// AT: ERROR
		{
			if (sim_msg_at_count >= SIM_CMD_RETRY_NUM)
			{
				SIMOK = false;
				SIMError = false;
				
				sim_sms_send_msg_failed_callback();
				sim_mode = MODE_IDLE;
				sim_sndmsg_step = SIM_SNDMSG_STEP0;
				return;
			}
			
			SIMOK = false;
			SIMError = false;
			
			sim_printf("AT\r\n");	// Gửi lại AT
			TimeSendCMD = HAL_GetTick();
		}
	}
	else
	{
		if (HAL_GetTick() - TimeSendCMD >= 2000) // Không có phản hồi khi gửi AT
		{
			if (sim_msg_at_count >= SIM_CMD_RETRY_NUM)
			{
				SIMOK = false;
				SIMError = false;
				sim_sms_send_msg_failed_callback();
				sim_mode = MODE_IDLE;
				sim_sndmsg_step = SIM_SNDMSG_STEP0;
				return;
			}
			
			SIMOK = false;
			SIMError = false;
			sim_printf("AT\r\n");	// Gửi lại AT
			TimeSendCMD = HAL_GetTick();
		}
	}
}
//

/*
 * sim_msg_write_content [private funciton]
 * Check is AT+CMGS= "phone number"\r\n command response > or timeout
 * if (">") send message content
 * if (timeout) resend AT+CMGS= "phone number"\r\n, if resend time > SIM_CMD_RETRY_NUM, failed_callback will be call
*/
void sim_msg_write_content(void)
{
	if (SFlagRdySendMsg)	// Đã nhận được phản hồi '>'
	{
		SIMOK = false;
		SIMError = false;
		
		uint8_t len = strlen (sim_msg_content);
		sim_msg_content[len] = 0x1A;
		sim_msg_content[len+1] = 0;
		
		sim_printf(sim_msg_content);	// Gửi đi nội dung tin nhắn
		
		TimeSendCMD = HAL_GetTick();
		sim_sndmsg_step = SIM_SNDMSG_STEP3;
	}
	else
	{
		if (HAL_GetTick() - TimeSendCMD >= 4000)	// Không nhận được phản hồi '>'
		{
			if (sim_msg_at_count >= SIM_CMD_RETRY_NUM)
			{
				SIMOK = false;
				SIMError = false;
				SFlagRdySendMsg = false;
				
				sim_sms_send_msg_failed_callback();
				sim_mode = MODE_IDLE;
				sim_sndmsg_step = SIM_SNDMSG_STEP0;
				return;
			}
			
			TimeSendCMD = HAL_GetTick();
			SFlagRdySendMsg = false;
			sim_sendmsg_at();	// Gửi lại lệnh AT+CMGS="SDT"
		}
	}
}
//


/*
 * sim_msg_follow_content [Private function]
 * Check is content sent OK, Error or Timeout
 * if (OK) ok callback whill be call
 * if (Error) failed callback will be call
 * if (timout ) failed callback will be call
*/
void sim_msg_follow_content(void)
{
	if (SIMOK)
	{
			SIMOK = false;
			SIMError = false;
			SFlagRdySendMsg = false;
			
		
			sim_sms_send_msg_ok_callback();
			sim_mode = MODE_IDLE;
			sim_sndmsg_step = SIM_SNDMSG_STEP0;
			return;
	}
	else
	if (SIMError)
	{
		SIMOK = false;
		SIMError = false;
		SFlagRdySendMsg = false;

		sim_sms_send_msg_failed_callback();
		sim_mode = MODE_IDLE;
		sim_sndmsg_step = SIM_SNDMSG_STEP0;
		return;
	}
	else
	{
		if (HAL_GetTick() - TimeSendCMD >= 4000)
		{
			SIMOK = false;
			SIMError = false;
			SFlagRdySendMsg = false;
			
			sim_sms_send_msg_failed_callback();
			sim_mode = MODE_IDLE;
			sim_sndmsg_step = SIM_SNDMSG_STEP0;
			return;
		}
	}
}
//


/*
 * sim_poll_send_msg [Private function]
 * maintain everything to send async message
 * must be call in loop
*/
void sim_poll_send_msg(void)
{
	switch(sim_sndmsg_step)
	{
		case SIM_SNDMSG_STEP0:	// Ghi AT để Wakeup SIM
			sim_printf("AT\r\n");
			sim_msg_at_count ++;
			sim_sndmsg_step = SIM_SNDMSG_STEP1;
			break;
		case SIM_SNDMSG_STEP1:
			sim_msg_follow_at_cmd();	// Chờ phản hồi OK, nếu OK thì ghi AT+CMSG= "std", nếu Error hoặc timout thì ghi lại AT\r\n
			break;
		case SIM_SNDMSG_STEP2:
			sim_msg_write_content();	// Chờ phản hồi > từ AT+CMGS= "std", nếu có thì ghi nội dung tin nhắn, nếu timout thì gửi lại lệnh AT+CMGS= "std"
			break;
		
		case SIM_SNDMSG_STEP3:
			sim_msg_follow_content();	// Chờ phản hồi OK, nếu OK gọi callback ok, nếu error hoặc timout thì gọi callback failed.
			break;
	}
}
//


/*
 * sim_async_sendmsg [ public funtion]
 * This function to let sim_poll start process async send message
 * which phonenumber and msg(message content).
*/
void sim_async_sendmsg(char phonenumber[], char msg[])
{
	if (sim_is_init_ok())
	if ((SFlagBusy == false)&&
		(SFlagInitializing== false)&&
			(SFlagSendingMSG == false))
	{
		strcpy(sim_msg_content, msg);
		strcpy(phone_number, phonenumber);
		
		SFlagSendMsgOK = false;
		SFlagSendingMSG = true;
		
		sim_sndmsg_step = SIM_SNDMSG_STEP0;
		sim_mode = MODE_SEND_MSG;
		TimeSendCMD = HAL_GetTick();
	}
}
//


