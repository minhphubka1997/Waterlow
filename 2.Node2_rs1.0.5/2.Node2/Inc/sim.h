#ifndef _SIM_H_
#define _SIM_H_

#include "stdint.h"
#include "stdbool.h"


#define SIM_END_CODE	"\r\n"
#define SIM_LINE_LEN		128

void sim_process(void);
void sim_async_sendmsg(char phonenumber[], char msg[]);
void sim_async_init(void);
void sim_printf(char *fmt, ...);
void sim_off(void);
bool sim_is_init_ok(void);
bool sim_is_initializing(void);
bool sim_is_sendingMsg(void);


#define RES_DRY					"RDY\r\n"
#define RES_CALL_RDY			"Call Ready\r\n"
#define RES_SMS_RDY				"SMS Ready\r\n"
#define RES_BUSY				"BUSY\r\n"
#define RES_NO_CARRIER			"NO CARRIER\r\n"
#define RES_OK					"OK\r\n"
#define RES_ERROR				"ERROR\r\n"
#define RES_RING				"RING\r\n"


#define CMD_TURNOFF_ECHO		"ATE0\r\n"					// Lệnh Tắt chế độ Echo (tức là sim sẽ không gửi lại những gì nó nhận được)
#define CMD_SMS_TEXTMODE		"AT+CMGF=1\r\n"				// Lệnh Cài tin nhắn ở Text mode
#define CMD_SHOW_SMD_IME		"AT+CNMI=1,2,0,0,0\r\n"			// Lệnh Hiển thị nội dung tin nhắn khi tin nhắn vừa đến
#define CMD_SAVE				"AT&W\r\n"					// Lệnh Lưu các cài đặt
#define CMD_SMS_DELE_ALL		"AT+CMGDA=\"DEL ALL\"\r\n"	// Lệnh Xóa hết tin nhắn trong hộp thư
#define CMD_NONE				""


#define CMD_STOP_INCOMINGCALL	"ATH\r\n"			// Lệnh Từ chối cuộc gọi đến
#define CMD_RPLY_INCOMINGCALL	"ATA\r\n"			// Lệnh Trả lời cuộc gọi đến
#define CMD_SEND_MSG			"AT+CMGS="			// Lệnh Gửi tin nhắn
#define END_MSG					0x1A				// Mã kết thúc nội dung tin nhắn
#define CMD_ENTER_SLEEPMODE		"AT+CSCLK=1\r\n"	// Lệnh đưa SIM vào sleep mode
#define CMD_EXIT_SLEEPMODE		"AT+CSCLK=0\r\n"	// Lệnh đưa SIM thoát sleep mode
#define CMD_AUTO_SLEEPMODE		"AT+CSCLK=2\r\n"	// Lệnh đưa SIM vào chế độ Auto sleep mode (lúc không có hoạt động sim sẽ tự ngủ)



#define SIM_CMD_RETRY_NUM		4		// Số lần gửi lại lệnh cho SIM nếu ERROR
#define SIM_INIT_RETRY_NUM		2		// Số lần Init lại nếu init bị failed


#define DBG_SIM_VIA_LCD				0
#define NOTIFY_SIM_STATE_VIA_LCD	1




/*---------------- Private define -------------------*/
#define SIM_INIT_STEP0		0	// wait 300ms -> ON 4V Power -> next step
#define SIM_INIT_STEP1		1	// Wait SIMOn, SIMcall ready, SIM SMS ready -> Send cmd turn off echo->next step
#define SIM_INIT_STEP2		2	// follow previous cmd, if failed, retry, if ok next step
#define SIM_INIT_STEP3		3
#define SIM_INIT_STEP4		4
#define SIM_INIT_STEP5		5
#define SIM_INIT_STEP6		6
#define SIM_INIT_STEP7		7



#define 	SIM_SNDMSG_STEP0 	1
#define 	SIM_SNDMSG_STEP1 	2
#define 	SIM_SNDMSG_STEP2 	3
#define 	SIM_SNDMSG_STEP3 	4

#define MODE_OFF				0
#define MODE_IDLE				1
#define MODE_INIT				2
#define MODE_SEND_MSG			3

typedef struct
{
	char msg_content[200];
	char phone_number[40];
	uint16_t msg_len ;
	bool needfillcontent ;
}	SMSReceiveBuffStructure;
//

#endif

