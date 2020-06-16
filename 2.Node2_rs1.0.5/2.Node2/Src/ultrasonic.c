#include "main.h"
#include "dwt_stm32_delay.h"
#include "app.h"
#include "sim.h"

#define delayMicroseconds	DWT_Delay_us

#define UL_TRIG_PORT	GPIOB
#define UL_TRIG_PIN		GPIO_PIN_13

#define UL_ECHO_PORT	GPIOB
#define UL_ECHO_PIN		GPIO_PIN_12

#define UL_Trig_Low()	HAL_GPIO_WritePin(UL_TRIG_PORT, UL_TRIG_PIN, GPIO_PIN_RESET)
#define UL_Trig_High()	HAL_GPIO_WritePin(UL_TRIG_PORT, UL_TRIG_PIN, GPIO_PIN_SET)
#define UL_Echo_read()	HAL_GPIO_ReadPin(UL_ECHO_PORT, UL_ECHO_PIN)


void distance_warning_check(void);


uint16_t measure_distance(void)
{
	double dist = 0;
	uint32_t lt = 0;
	uint32_t ft = 0;
	
	UL_Trig_Low();
	delayMicroseconds(5);
	UL_Trig_High();
	delayMicroseconds(70);
	UL_Trig_Low();

	// Wait for Echo is High
	bool IsError = false;
	ft = HAL_GetTick();
	while(UL_Echo_read() == 0)
	{
		if (HAL_GetTick() - ft >= 20) 
			return 9999;
	}
	while(UL_Echo_read() != 0)
	{
		lt ++;
		DWT_Delay_us(1);
		if (lt >= 7352) 
			return 9999;
	}

	dist = lt*0.034;
	
	return (signed int)dist;
}
//


extern uint16_t Distance ;
extern bool SensorValueChanged;
#define DT_MEASURE_NUM		10			// Number of measure to average
#define DT_MEASURE_INTVL	100			// Measure interval (100ms)
#define DT_MEASURE_NUM_BELIVE 7		// Number of value < 250cm can be trust

uint32_t DTLog[DT_MEASURE_NUM];		// Biến Lưu trữ lại các kết quả đo gần đây nhất
uint8_t DTLogCount = 0;
uint32_t DTmeasureTime = 0;

void dt_log_shif(void)						// xóa giá trị cũ nhất trong DTLog, để có chỗ trống cho lần đo mới
{
	for (int i = 0; i< DT_MEASURE_NUM -1; i++)
		DTLog[i]= DTLog[i+1];
	if ( DTLogCount < DT_MEASURE_NUM) DTLogCount ++;
}
//

uint32_t dt_average(void)		// Tính trung bình các kết quả đo được lưu trữ trong DTLog
{
	int usabledatacount = 0;
	uint32_t ave = 0;
	for (int i = 0; i< DT_MEASURE_NUM; i++)
	{
		if (DTLog[i] < 250) // Chỉ lấy dữ liệu <250cm
		{
			ave += DTLog[i];
			usabledatacount ++;
		}
	}
	
	if (usabledatacount >=DT_MEASURE_NUM_BELIVE) // Đủ Lượng dữ liệu <250cm 
		return ave / usabledatacount;
	else return 9999; // Không đủ dữ liệu < 250cm
}
//

void distance_measure_process(void)
{
	if (HAL_GetTick() - DTmeasureTime >= DT_MEASURE_INTVL)
	{
		uint32_t _distance = measure_distance();
		
		dt_log_shif();
		DTLog[DT_MEASURE_NUM - 1] = _distance;
		
		if (DTLogCount >= DT_MEASURE_NUM)
		{
			_distance = dt_average();	// Lấy kết quả trung bình của các lần đo gần đây
			if (_distance == 9999)
			{
				_distance = FIX_ULTRASONIC_POSITON;
				Distance = _distance;
				SensorValueChanged = true;
				return;
			}
			
			_distance = FIX_ULTRASONIC_POSITON - _distance; // Đổi từ khoảng cách giữa cảm biến và mặt nước thành độ cao nước.
			

			if (_distance < 250) 
			{
				if (_distance != Distance)
				{
					Distance = _distance;
					SensorValueChanged = true;
				}
			}
		}
		
		DTmeasureTime = HAL_GetTick();
	}
	
	distance_warning_check();
}	
//


extern bool Node1PumpNeedOn ;
extern bool FlagN1PumpOnViaSMS;
extern bool FlagN1PumpOffViaSMS ;

#define DISTANCE_CHECK_INTVL							10000//(15*60*1000)	// 15p check độ cao nước 1 lần
#define	DISTANCE_WARNING_DELTAL_LEVEL			10						// 10cm

#define PUMPON_WATER_LEVEL								100						// Mực nước điều kiện để có thể bật bơm tống nước ra ngoài
#define PUMPOFF_WATER_LEVEL								80						// Mực nước điều kiện để tắt bơm

uint32_t distance_time_check	 = 0;

uint16_t OldDistance = 0;

void distance_warning_check(void)
{
	if (HAL_GetTick() - distance_time_check >= DISTANCE_CHECK_INTVL)
	{
		if ((Distance - OldDistance >= 10)&&(Distance > PUMPON_WATER_LEVEL)&&(Distance != FIX_ULTRASONIC_POSITON))
		{
			if (Node1PumpNeedOn == false)	// Event cmd turn on pump
			{
				Node1PumpNeedOn = true;
				FlagN1PumpOnViaSMS = true;
			}
		}
		if (Distance < PUMPOFF_WATER_LEVEL) 
		{
			if (Node1PumpNeedOn )	// Event cmd turn off pump
			{
				Node1PumpNeedOn = false;
				FlagN1PumpOffViaSMS = true;
			}
		}
		
		OldDistance = Distance;
		distance_time_check = HAL_GetTick();
	}
}
//


