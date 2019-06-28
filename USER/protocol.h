#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#define WIFI_REGISTRATION_DI {0x50,0x01,0x00,0x0A,0x11,0x22,0x33,0x44,0x55,0x66,0x50,0x02,0x00,0x06,0x00,0x02}
#define WIFI_HEART_DI {0x50,0x01,0x00,0x0A,0x11,0x22,0x33,0x44,0x55,0x66,0x50,0x21,0x00,0x06,0x00,0x02}
#define WIFI_REALDATA_DI {0x50,0x01,0x00,0x0A,0x11,0x22,0x33,0x44,0x55,0x66,0x50,0x02,0x00,0x06,0x00,0x02\
						 ,0x50,0x05,0x00,0x08,0x00,0xD5,0x9F,0x81,0x50,0x20,0x00,0x12,0x5C,0x80,0xB8,0x77,0x00\
						 ,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x07,0x50,0x30,0x00,0x06,0x00,0x01}

#define WIFI_ACKNOWLEDGE_DI	{0x50,0x01,0x00,0x0A,0x11,0x22,0x33,0x44,0x55,0x66,0x50,0x06,0x00,0x05,0x01}

#define PROTOCOL_DEVICE_VERSION 0x02
#define PROTOCOL_IDENTIFIER 0x00
#define PROTOCOL_VERSION 0x02

#define REAPONSE_MARK 1

#define HEART_RATEBUFF_MAXLEN 30





#define PROTOCOLPACKET_MAXLEN 100

typedef struct
{
	unsigned char Protocol_PacketBuff[PROTOCOLPACKET_MAXLEN];
	unsigned char len;
}PROTOCOL_SENDPACKET_STRUCT;
extern PROTOCOL_SENDPACKET_STRUCT gProtocol_SendPacket_Struct; 

typedef enum
{
	REGISTERED_MARK = REAPONSE_MARK,//注册标志
	HEARTBEAT_MARK,
	
}RESPONSE_MARK_ENUM;

//处方
typedef struct
{
	unsigned int user_id;					//用户ID
	unsigned int prescription_id;			//处方ID
	unsigned short prescription_type;		//处方类型
	unsigned short BO_min_alarm;			//报警最小血氧
	unsigned short HB_max_alarm;			//报警最大心率
	unsigned short HB_min_alarm;			//报警最小心率
	unsigned short speed_max;				//最大速度
	unsigned short speed_doctor_advice;		//医嘱速度
	unsigned short HB_max;					//最大心率
	unsigned short HB_min;					//最小心率
	unsigned short resistance;				//阻力
	unsigned short resistance_max;			//最大阻力
	unsigned short isallowadjust;			//是否允许调节
	unsigned short weight;					//体重
}CONTROLCMD_DATA_STRUCT;
extern CONTROLCMD_DATA_STRUCT gControlCMD_Data_Struct;

typedef struct
{
	unsigned char resistance_now;	//当前阻力值
	unsigned char time_cnt;			//各阶段时间
	unsigned char time_heartrate;	//计算平均心率的时间
}NOWINFO_STRUCT;

//MICE训练流程
typedef enum 
{
	WARM_UP_FASE= 1,
	TRAIN_FASE_1,	//第一阶段	阻力值未达到期望值
	TRAIN_FASE_2,	//第二阶段	阻力值达到期望值
	RECOVER_FASE,
	FINISH_FASE,
}MICE_ENUM;
extern MICE_ENUM gMICE_Enum;

//MICE训练各阶段时间
typedef struct
{
	unsigned short warm_up_time;		//热身阶段
	unsigned short train_time;			//训练阶段
	unsigned short recover_time;		//恢复阶段
	unsigned char RunStatus;
}MICE_STRUCT;
extern MICE_STRUCT gMICE_Struct;

//HIIT训练流程
typedef enum
{
	GPP_INTENSITY_FASE = 1,		//高强度训练阶段
	GPP_INTENSITY_LAST_FASE,	//高强度训练持续阶段
	HITT_RECOVER_FASE,			//恢复阶段
	RECOVER_LASE_FASE,	//恢复持续阶段
	HITT_FINISH_FASE,
}HIIT_ENUM;

//HIIT训练各阶段时间
typedef struct
{
	unsigned short GPP_Intensity_time;	//高强度运动时间
	unsigned short recover_last_time;	//恢复持续时间
	unsigned short group_count;			//组数
}HIIT_STRUCT;
extern HIIT_STRUCT gHIIT_Struct;

//心率
typedef struct
{
	unsigned char heart_rate[HEART_RATEBUFF_MAXLEN];	//心率数组，保存每次收到的心率
	unsigned char heart_rate_cnt;		//心率计数
	unsigned short heart_rate_sum;		//心率总和
	unsigned char heart_rate_average;	//平均心率
	unsigned char heart_rate_startflag;	//开始计算心率的标志
}HEART_RATE_STURCT;


//ECG数据
typedef struct
{
	
	HEART_RATE_STURCT Heart_Rate_Struct;
	unsigned char blood_o;		//血氧
	unsigned char blood_p;		//血压
}ECG_STRUCT;
extern ECG_STRUCT gECG_Struct;

typedef struct
{
	unsigned char borg;				//疲劳值
	unsigned char finish_strtus;	//训练完成状态 0 提前完成，1正常完成
}TRAIN_FINISH_DATA_STRUCT;
extern TRAIN_FINISH_DATA_STRUCT gTrain_Finish_Data_Struct;

typedef struct
{
	unsigned short length;		//代表所有DI组的总长度
	unsigned char *valuebuff;
}DI_STRUCT;
extern DI_STRUCT gDI_Struct;

//CODE 类型
typedef enum
{
	HARNDWARE_DEVICE_SYSTEM_MESSAGE = 0x01,
	ECG_DATA_MESSAGE,
	DEVICE_REGISTRATION_MESSAGE,		//设备注册消息
	HEART_MESSAGE,						//心跳
	CONTROL_CMD_MESSAGE,				//控制命令
	REAL_TIME_DATA = 0x06,						//实时数据
	TRAIN_START_ESSAGE = 0x0C,	//开始或停止
	DRIVER_GET_BORG = 0x0D,		//接受borg
}CODE_ENUM;


//
typedef enum
{
	ERR_CODE = 0xF000,						//错误码
	DEVICE_ID = 0x5001,						//设备ID
	DEVICE_TYPE = 0x5002,					//设备类型
	PRESCRIPTION_FINISH_STATUS = 0x5003,	//处方完成状态
	STOP_CMD = 0x5004,						//停止命令
	USER_ID = 0x5005,						//用户ID
	START_TRAIN_CMD = 0x5006,				//启动训练命令
	PRESCRIPTION_PARAMETER = 0x5010,		//处方参数
	PRESCRIPTION_PARAMETER_TREADMILL = 0x5011, //踏车处方
	SPEED_CMD = 0x5012,						//速度命令
	BLOOD_O = 0x5023,						//血氧
	BLOOD_P = 0x5024,						//血压
	HEART_RATE = 0x5025,					//心率
	MICE_TRAINING_PARAMETERS = 0x5026,		//MICE训练参数
	HITT_TRAINING_PARAMETERS = 0x5027,		//HITT训练参数
	DEVICE_TRAINING_PARAMETERS = 0x5028,	//设备训练状态
	PRESCRIPTION_FINISH_RESPONSE = 0x5029,	//处方完成响应
	TRAINING_STATUS = 0x5030,				//训练状态
	GET_BORG = 0x5032,						//获取borg值
}TYPE_ENUM;



extern unsigned char gResponseMark;



void PROTOCOL_StructInit(void);

void PROTOCOL_PackedData(unsigned char code,DI_STRUCT *DI);
void PROTOCOL_CheckPacket(unsigned char *packetdata,unsigned char packlen);
unsigned char PROTOCOL_ParseCode(unsigned char *packetdata,unsigned char packlen);
void PROTOCOL_ParseDI(unsigned char *packetdata,unsigned short DIlen);

unsigned char* PROTOCOL_CreateRegistPack(void);
unsigned char* PROTOCOL_CreateHeartPack(unsigned char runstatus);
unsigned char* PROTOCOL_CreateRealDataPack(unsigned char resistance);
unsigned char* PROTOCOL_CreatePrescriptionFinshPack(void);




void IntToHexChar(unsigned char *ReturnStr,unsigned char *HexStr,unsigned char len);

char HexStr2Integer(char *in,char *out ,int len);


#endif /* __MAIN_H */

