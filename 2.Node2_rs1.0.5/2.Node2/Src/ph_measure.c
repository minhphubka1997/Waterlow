#include "main.h"
#include "app.h"
#include "sim.h"
#include "lora.h"

extern float PHValue;
extern bool SensorValueChanged;
extern ADC_HandleTypeDef hadc2;
void PH_warning_check(void);



uint32_t SensorReadTime = 0;

#define HP_MEASURE_INTVL	50		// Measure interval = 50ms
#define PH_MEASURE_NUM		20		// Number of measure to average

float PHLog[PH_MEASURE_NUM];
uint8_t PHLogCount = 0;

void ph_log_shif(void)
{
	for (int i = 0; i< PH_MEASURE_NUM - 1; i++)
		PHLog[i] = PHLog[i+1];
	if (PHLogCount < PH_MEASURE_NUM) PHLogCount ++;
}
//

float ph_log_average(void)
{
	float ave = 0;
	for (int i = 0; i< PH_MEASURE_NUM; i++) ave += PHLog[i];
	return ave / PH_MEASURE_NUM;
}
//

void ph_measure_process(void)
{
	if (HAL_GetTick() - SensorReadTime >= HP_MEASURE_INTVL)
	{
		uint16_t ADCValue;
		uint32_t PHVoltage = 0;
		
		if (HAL_ADC_PollForConversion(&hadc2, 10) == HAL_OK)
		{
			ADCValue = HAL_ADC_GetValue(&hadc2);
		}
		
		PHVoltage = 330 * ADCValue / 4095;
		float Ph = -0.0983 * PHVoltage + 21.798;
		if (Ph < 0) Ph  = 0;
		if (Ph > 14) Ph = 14;
		
		ph_log_shif();
		PHLog[PH_MEASURE_NUM-1] = Ph;
		
		if (PHLogCount >= PH_MEASURE_NUM) PHValue = ph_log_average();
		
		SensorReadTime = HAL_GetTick();
		SensorValueChanged = true;
	}
	
	PH_warning_check();
}
//


#define PH_CHECK_INTVL			30000//(30 * 60 * 1000)	// 30p check 1 lần
#define PH_WARNING_LEVEL		6.5								// nếu PH nhỏ hơn mức này thì đang ở trong tình trạng không tốt
#define PH_OK_LEVEL					6.8								// Nếu PH >= mức này tức là đang ở mức OK

extern bool GWPumpNeedOn;
extern bool FlagWarningPHValueToUser;
extern bool FlagUpdateDataViaSMS ;

uint32_t ph_check_time = 0;
float OldPHValue = 0;
uint8_t PHonWarningCount = 0;


void PH_warning_check(void)
{
	if (HAL_GetTick() - ph_check_time >=PH_CHECK_INTVL)
	{
		if ((ph_check_time == 0)&& (PHValue <PH_WARNING_LEVEL))
			PHonWarningCount = 1;
		
		if ((OldPHValue >= PH_WARNING_LEVEL)&&
			(PHValue < PH_WARNING_LEVEL))					// Event PH down to under 6.5
		{
			PHonWarningCount  = 1;
		}
		
		
		if ((OldPHValue < PH_WARNING_LEVEL)&&
			(PHValue < PH_WARNING_LEVEL))					
		{
			PHonWarningCount  += 1;
			if (PHonWarningCount == 2)	// ĐỘ PH < 6.5 lần 2
			{
				FlagWarningPHValueToUser = true;	// Set cờ này lên để gửi tin nhắn cảnh báo cho người dùng
			}
			
			if (PHonWarningCount == 3)	// Độ PH < 6.5 lần 3
			{
				FlagWarningPHValueToUser = true;	// Set cờ này lên để gửi tin nhắn cảnh báo cho người dùng
				if (GWPumpNeedOn == false)	// Event cmd on Gateway PUMP
				{
					GWPumpNeedOn = true;		// Yêu cầu Gateway bật bơm
					FlagUpdateDataViaSMS = true;	// Yêu cầu Gateway bật bơm (thông qua SMS -> nếu Lora mất)
				}
			}
		}
		
		if (PHValue > PH_OK_LEVEL)
		{
			if (GWPumpNeedOn == true)	// Event cmd off Gateway PUMP
			{
				GWPumpNeedOn = false;	// Yêu cầu Gateway tắt bơm
				FlagUpdateDataViaSMS = true;	// Yêu cầu Gateway bật bơm (thông qua SMS nếu Lora mất)
			}
		}
		
		OldPHValue = PHValue;
		ph_check_time = HAL_GetTick();
	}
}
//


