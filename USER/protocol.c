#include "protocol.h"
#include "uartdriver.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "wifidriver.h"
#include "M8266WIFIDrv.h"
#include "speed.h"

PROTOCOL_SENDPACKET_STRUCT gProtocol_SendPacket_Struct; 
DI_STRUCT gDI_Struct;
ECG_STRUCT gECG_Struct;
CONTROLCMD_DATA_STRUCT gControlCMD_Data_Struct;
MICE_STRUCT gMICE_Struct;
HIIT_STRUCT gHIIT_Struct;
unsigned char gResponseMark = 0;

MICE_ENUM gMICE_Enum = WARM_UP_FASE;

CODE_ENUM gCode_Enum;

TRAIN_FINISH_DATA_STRUCT gTrain_Finish_Data_Struct;


const unsigned char gDeviceID[6] = {0x11,0x22,0x33,0x44,0x55,0x66};
const unsigned char gDeviceType = 0x03;

extern unsigned short Resistance_Now ;
extern unsigned char gSpeed;

void PROTOCOL_StructInit()
{
	CONTROLCMD_DATA_STRUCT *pControlCMD_Data_Struct = &gControlCMD_Data_Struct;
	ECG_STRUCT *pECG_Struct = &gECG_Struct;

	TRAIN_FINISH_DATA_STRUCT *pTrain_Finsh_Data_Struct = &gTrain_Finish_Data_Struct;
//	MICE_STRUCT *pMICE_Struct = &gMICE_Struct;
//	HIIT_STRUCT *pHIIT_Struct = &gHIIT_Struct;

	pControlCMD_Data_Struct->BO_min_alarm = 0;
	pControlCMD_Data_Struct->HB_max = 0;
	pControlCMD_Data_Struct->HB_max_alarm = 0;
	pControlCMD_Data_Struct->HB_min = 0;
	pControlCMD_Data_Struct->HB_min_alarm = 0;
	pControlCMD_Data_Struct->isallowadjust = 0;
	pControlCMD_Data_Struct->prescription_id = 0;
	pControlCMD_Data_Struct->prescription_type = 0;
	pControlCMD_Data_Struct->resistance = 0;
	pControlCMD_Data_Struct->resistance_max = 0;
	pControlCMD_Data_Struct->speed_doctor_advice = 0;
	pControlCMD_Data_Struct->speed_max =0 ;
	pControlCMD_Data_Struct->user_id = 0;
	pControlCMD_Data_Struct->weight = 0;

	pECG_Struct->Heart_Rate_Struct.heart_rate_cnt = 0;
	pECG_Struct->Heart_Rate_Struct.heart_rate_startflag = RESET;
	pECG_Struct->Heart_Rate_Struct.heart_rate_sum = 0;
	pECG_Struct->Heart_Rate_Struct.heart_rate_average = 0;
	memset(pECG_Struct->Heart_Rate_Struct.heart_rate,0,HEART_RATEBUFF_MAXLEN);

	pTrain_Finsh_Data_Struct->borg = 0;
	pTrain_Finsh_Data_Struct->finish_strtus = 0;
}


//打包数据
void PROTOCOL_PackedData(unsigned char code,DI_STRUCT *DI)
{
	PROTOCOL_SENDPACKET_STRUCT *pProtocol_SendPacket_Struct = &gProtocol_SendPacket_Struct;
	unsigned char *pProtocolBuff = pProtocol_SendPacket_Struct->Protocol_PacketBuff;
	
	unsigned short len = 8 + DI->length,sum = 0;	//帧头长度 + 数据长度
	unsigned char i = 0;
	
	pProtocol_SendPacket_Struct->len = len; 		//保存包长度
	pProtocolBuff[0] = code;
	pProtocolBuff[1] = PROTOCOL_VERSION;
	pProtocolBuff[2] = (len & 0xff00) >> 8;
	pProtocolBuff[3] = len & 0xff;
	pProtocolBuff[4] = 0x00;
	pProtocolBuff[5] = 0x00;
	pProtocolBuff[6] = PROTOCOL_IDENTIFIER;
	pProtocolBuff[7] = PROTOCOL_VERSION;
	for(i=0;i< DI->length;i++)
	{
		pProtocolBuff[8 + i] = DI->valuebuff[i];
	}
	for(i=0;i<len;i++)
	{
		sum += pProtocolBuff[i];
	}
	pProtocolBuff[4] = (sum & 0xff00) >> 8;
	pProtocolBuff[5] = sum & 0xff;
}

//接收数据
//检查接收包是否完整
//不用检查
void PROTOCOL_CheckPacket(unsigned char *packetdata,unsigned char packlen)
{
/*
	unsigned short len = 0; //, sum = 0;
	unsigned char i = 0;
//	unsigned short CRC_Value;
//	unsigned char i = 0;
	if(packlen >= 13)	//完整的包至少13个字节
	{
		len = ((packetdata[2] & 0xff) << 8) | packetdata[3];
		do
			{
				PROTOCOL_ParseCode(packetdata,len);
				packetdata += len;
				len = packlen - len;
			}
		while (len > 13)
		
	}
	
//	CRC_Value = ((packetdata[4] & 0xff) << 8) | packetdata[5];
//	for(i=0;i<len;i++)
//	{
//		sum += packetdata[i];
//	}
//	if(CRC_Value == sum/2)
//	{
			//PROTOCOL_ParseCode(packetdata,len); //解析
			//if(packlen - len > 8)
			//{
				//PROTOCOL_CheckPacket(packetdata+len,packlen-len);
			//}
//	}
*/
}

//解析code
//参数1：数据包
//参数2：整包长度
unsigned char PROTOCOL_ParseCode(unsigned char *packetdata,unsigned char packlen)
{
	unsigned short code;
	unsigned char len = 0;
//	unsigned char remain_package_len = 0;	//剩下包长度
	do
		{	
			if(packlen < 13)
			{
				return 0;
			}
			
			packetdata += len;
			len = ((packetdata[2] & 0xff) << 8) | packetdata[3];
			code = packetdata[0];
			switch(code)
			{
				case HARNDWARE_DEVICE_SYSTEM_MESSAGE :
					gCode_Enum = HARNDWARE_DEVICE_SYSTEM_MESSAGE;	//硬件设备系统消息
				break;
				case ECG_DATA_MESSAGE :
					gCode_Enum = ECG_DATA_MESSAGE;					//ECG数据消息
					//PROTOCOL_ParseDI(packetdata+8,len-8);
				break;
				case DEVICE_REGISTRATION_MESSAGE :
					gCode_Enum = DEVICE_REGISTRATION_MESSAGE;		//设备注册消息
					//PROTOCOL_ParseDI(packetdata+8,len-8);
				break;
				case HEART_MESSAGE :
					gCode_Enum = HEART_MESSAGE;						//心跳消息
					//len = ((packetdata[2] & 0xff) << 8) | packetdata[3];
					if(len != 0x0E) return 0;
					if(packlen < 0x0E) return 0;
					gWIFI_CheckUnlink_Struct.time_cnt = 0;	//清除
					//PROTOCOL_ParseDI(packetdata+8,len-8);
				break;
				case CONTROL_CMD_MESSAGE :							//控制命令消息
					gCode_Enum = CONTROL_CMD_MESSAGE;
					//PROTOCOL_ParseDI(packetdata+8,len-8);
				break;
				case REAL_TIME_DATA:		
					gCode_Enum = REAL_TIME_DATA;					//实时数据消息
					//len = ((packetdata[2] & 0xff) << 8) | packetdata[3];
					if(len != 0x19) return 0;
					if(packlen < 0x19) return 0;	//接收到的包不完整					
					
				break;
				case TRAIN_START_ESSAGE:
				break;
				case DRIVER_GET_BORG:
				break;
				default : return 0;
					 
				//break;
			}
			PROTOCOL_ParseDI(packetdata+8,len-8);
			packlen = packlen - len;	//剩下包的长度
		}
	while (packlen >= 13);	
    return 0;
}


//解析DI值
//参数1:DI数据包
//参数2:DI数据包长度
void PROTOCOL_ParseDI(unsigned char *packetdata,unsigned short DIlen)
{
	ECG_STRUCT *pECG_Struct = &gECG_Struct;
	CONTROLCMD_DATA_STRUCT *pControlCMD_Data_Struct = &gControlCMD_Data_Struct;
	MICE_STRUCT *pMICE_Struct = &gMICE_Struct;
	HIIT_STRUCT *pHIIT_Struct = &gHIIT_Struct;

	PROTOCOL_SENDPACKET_STRUCT *pProtocol_SendPacket_Struct = &gProtocol_SendPacket_Struct;
	DI_STRUCT *pDI_Struct = &gDI_Struct;
	TRAIN_FINISH_DATA_STRUCT *pTrain_Finsh_Data_Struct = &gTrain_Finish_Data_Struct;

//	WIFI_SEND_RECEIVE_STRUCT *pWIFI_Send_Receive_Struct = &gWIFI_Send_Receive_Struct;
	WIFI_ATTRIBUTE_STRUCT *pTcp_Atttibute_Struct = &gTcp_Atttibute_Struct;

	unsigned short type = 0 , len = 0,Value = 0x0001;
//	unsigned char remain_package_len = 0;

	unsigned char acknowledge[] = WIFI_ACKNOWLEDGE_DI;
//	unsigned char temp_h[2] = {0},temp_str[2] = {0};
	if(DIlen >= 5)
	{

		do
		{
			packetdata += len;

			type = (packetdata[0] << 8) & 0xff00 | packetdata[1];
			len = (packetdata[2] << 8) & 0xff00 | packetdata[3];
			
			switch((TYPE_ENUM)type)
			{
				case ERR_CODE:
						Value = (packetdata[4] << 8) & 0xff00 | packetdata[5];
						if(0x0000 == Value) //成功
						{
							DEBUG_LOG(sucess);
							gResponseMark = REGISTERED_MARK;
						}
						else if(0x0001 == Value)	//失败
						{
							DEBUG_LOG(faild);
							//softTimer_Stop(SEND_REAL_DATA);
							WIFI_ResetFlow();
						}
				break;
				case DEVICE_ID:
						//DEBUG_INFO((const char *)packetdata+4);
				break;
				case DEVICE_TYPE:
						
				break;
				case PRESCRIPTION_FINISH_STATUS:
						
				break;
				case STOP_CMD:
						
				break;
				case USER_ID:	//用户ID
						if(pControlCMD_Data_Struct->user_id != 0)
						{
							break;
						}
						pControlCMD_Data_Struct->user_id = (packetdata[4] << 24) | (packetdata[5] << 16) | (packetdata[6] << 8) | packetdata[7];
						DEBUG_LOG(user id);
				break;
				case START_TRAIN_CMD:
						pTrain_Finsh_Data_Struct->finish_strtus = packetdata[4];
						if(packetdata[4] == 1)
						{
							memset(pProtocol_SendPacket_Struct->Protocol_PacketBuff,0,PROTOCOLPACKET_MAXLEN);
							pDI_Struct->valuebuff = acknowledge;
							pDI_Struct->length = 15;
							PROTOCOL_PackedData(0x0c,pDI_Struct);
							if(WIFI_Send_Data(pProtocol_SendPacket_Struct->Protocol_PacketBuff, \
																	pProtocol_SendPacket_Struct->len,\
																	(u8)pTcp_Atttibute_Struct->link_id,\
																	&pTcp_Atttibute_Struct->errstatus))
							{
								softTimer_Create(SEND_REAL_DATA,MODE_PERIODIC,SEND_REAL_DATA_DELAY,Task_SendRealData,NULL,NULL);	//实时数据
								softTimer_Create(MICE_ID,MODE_PERIODIC,MICE_DELAY,Task_MICE,NULL,NULL);
								DEBUG_LOG(start train);
							}
							else
							{
								DEBUG_LOG(send key_ack faild);
								//softTimer_Stop(SEND_REAL_DATA);
								//WIFI_ResetFlow();	
							}
							
						}
						else if(packetdata[4] == 0)
						{
							//DEBUG_LOG("resistance set 1");	//阻力设置为 1
							//softTimer_Stop(MICE_TIME_ID);	//训练结束，停止定时器
							//softTimer_Stop(TASK_REALDATA_TIME_ID);//实时数据也停止
							pTrain_Finsh_Data_Struct->finish_strtus = 0x02;	//提前结束
							DEBUG_LOG(early train);
							softTimer_Stop(MICE_ID);	//训练结束，停止定时器
							softTimer_Stop(SEND_REAL_DATA);
							//收到疲劳值才能发送完成消息
							//memset(pProtocol_SendPacket_Struct->Protocol_PacketBuff,0,PROTOCOLPACKET_MAXLEN);
							//pDI_Struct->valuebuff = PROTOCOL_CreatePrescriptionFinshPack();
							//pDI_Struct->length = 10;
							//PROTOCOL_PackedData(0x0B,pDI_Struct);		
		
							//WIFI_Send_Data(pProtocol_SendPacket_Struct->Protocol_PacketBuff, \
							//										pProtocol_SendPacket_Struct->len,\
							//										(u8)pTcp_Atttibute_Struct->link_id,\
							//										&pTcp_Atttibute_Struct->errstatus);
						}
						
				break;
				case PRESCRIPTION_PARAMETER:
						
				break;
				case PRESCRIPTION_PARAMETER_TREADMILL:	//踏车处方
						pControlCMD_Data_Struct->prescription_id = (packetdata[4] << 24) | (packetdata[5] << 16) | (packetdata[6] << 8) | packetdata[7];
						pControlCMD_Data_Struct->prescription_type = (packetdata[8] << 8) | packetdata[9];
						pControlCMD_Data_Struct->BO_min_alarm = (packetdata[10] << 8) | packetdata[11];
						pControlCMD_Data_Struct->HB_max_alarm = (packetdata[12] << 8) | packetdata[13];
						pControlCMD_Data_Struct->HB_min_alarm = (packetdata[14] << 8) | packetdata[15];
						pControlCMD_Data_Struct->speed_max = (packetdata[16] << 8) | packetdata[17];
						pControlCMD_Data_Struct->speed_doctor_advice = (packetdata[18] << 8) | packetdata[19];
						pControlCMD_Data_Struct->HB_max = (packetdata[24] << 8) | packetdata[25];
						pControlCMD_Data_Struct->HB_min = (packetdata[26] << 8) | packetdata[27];
						pControlCMD_Data_Struct->resistance = (packetdata[28] << 8) | packetdata[29];
						pControlCMD_Data_Struct->resistance_max = (packetdata[30] << 8) | packetdata[31];
						pControlCMD_Data_Struct->isallowadjust = (packetdata[32] << 8) | packetdata[33];
						pControlCMD_Data_Struct->weight = (packetdata[34] << 8) | packetdata[35];
						DEBUG_LOG(receive prescription);
				break;
				case SPEED_CMD:
						
				break;
				case BLOOD_O:			//血氧
						pECG_Struct->blood_o = packetdata[4];
						DEBUG_LOG(blood_o);
				break;
				case BLOOD_P:			//血压
						pECG_Struct->blood_p = packetdata[4];
						DEBUG_LOG(blood_p);
				break;
				case HEART_RATE:		//心率
						if(pECG_Struct->Heart_Rate_Struct.heart_rate_startflag == SET)
						{
							pECG_Struct->Heart_Rate_Struct.heart_rate[pECG_Struct->Heart_Rate_Struct.heart_rate_cnt++] = packetdata[5];
							pECG_Struct->Heart_Rate_Struct.heart_rate_sum += packetdata[5];
						}
						//temp_h[0] = pECG_Struct->Heart_Rate_Struct.heart_rate_sum>>8;
						//temp_h[1]  = pECG_Struct->Heart_Rate_Struct.heart_rate_sum;
						//IntToHexChar(temp_str,temp_h,2);
						//DEBUG_INFO(temp_str, 2);
						DEBUG_LOG(blood_h);
				break;
				case MICE_TRAINING_PARAMETERS:	//MICE训练
						pMICE_Struct->warm_up_time = (packetdata[4] << 8) | packetdata[5];
						pMICE_Struct->train_time = (packetdata[6] << 8) | packetdata[7];
						pMICE_Struct->recover_time = (packetdata[8] << 8) | packetdata[9];
						pMICE_Struct->RunStatus = SET;	//开始运行
						gMICE_Enum = WARM_UP_FASE;	//进入热身阶段
						DEBUG_LOG(create real MICE);
						
				break;
				case HITT_TRAINING_PARAMETERS:	//HITT训练
						pHIIT_Struct->GPP_Intensity_time = (packetdata[4] << 8) | packetdata[5];
						pHIIT_Struct->recover_last_time = (packetdata[6] << 8) | packetdata[7];
						pHIIT_Struct->group_count = (packetdata[8] << 8) | packetdata[9];
						DEBUG_LOG(create real HITT);
						//softTimer_Create(TASK_REALDATA_TIME_ID,MODE_PERIODIC,TASK_REALDATA_DELAY,Task_SendRealData,NULL,NULL);
						//softTimer_Create(HITT_TIME_ID,MODE_PERIODIC,HITT_TIME_DELAY,Task_HIIT,NULL,NULL); //开始训练
				break;
				case DEVICE_TRAINING_PARAMETERS:
						
				break;
				case PRESCRIPTION_FINISH_RESPONSE:
						
				break;
				case TRAINING_STATUS:
						
				break;
				case GET_BORG:
					DEBUG_LOG(send finish);
					pTrain_Finsh_Data_Struct->borg = packetdata[4];
					memset(pProtocol_SendPacket_Struct->Protocol_PacketBuff,0,PROTOCOLPACKET_MAXLEN);
					pDI_Struct->valuebuff = PROTOCOL_CreatePrescriptionFinshPack();
					pDI_Struct->length = 10;
					PROTOCOL_PackedData(0x0B,pDI_Struct);		
					
					WIFI_Send_Data(pProtocol_SendPacket_Struct->Protocol_PacketBuff, \
																	pProtocol_SendPacket_Struct->len,\
																	(u8)pTcp_Atttibute_Struct->link_id,\
																	&pTcp_Atttibute_Struct->errstatus);
					WIFI_Recover_ResValue();	//提前结束或者恢复阶段阻力值没有到达1，将阻力直接返回到1
					WIFI_ResetFlow();
				break;
				default :	
					return ;	//默认返回
			}
			if(DIlen < len)  //剩余字节个数小于协议包字节个数
			{
				return ;
			}
			DIlen = DIlen - len;	//剩下包的长度
		}
	while (DIlen >= 5);
	}
}

//构建注册数据包
unsigned char* PROTOCOL_CreateRegistPack()
{

	unsigned char i = 0;
	static unsigned char RegistPack[16] = {0};
	RegistPack[0] = 0x50;
	RegistPack[1] = 0x01;
	RegistPack[2] = 0x00;
	RegistPack[3] = 0x0A;
	for(i = 0;i < 6;i++)	//设备ID一共6位
	{
		RegistPack[4+i] = gDeviceID[i];
	}
	RegistPack[10] = 0x50;
	RegistPack[11] = 0x02;
	RegistPack[12] = 0x00;
	RegistPack[13] = 0x06;
	RegistPack[14] = 0x00;
	RegistPack[15] = gDeviceType;
	return RegistPack;
}

//构建心跳数据包
unsigned char* PROTOCOL_CreateHeartPack(unsigned char runstatus)
{
	static unsigned char RegistPack[16] = {0};
	unsigned char i;
	RegistPack[0] = 0x50;
	RegistPack[1] = 0x01;
	RegistPack[2] = 0x00;
	RegistPack[3] = 0x0A;
	for(i = 0;i < 6;i++)	//设备ID一共6位
	{
		RegistPack[4+i] = gDeviceID[i];
	}
	RegistPack[10] = 0x50;
	RegistPack[11] = 0x21;
	RegistPack[12] = 0x00;
	RegistPack[13] = 0x06;
	RegistPack[14] = 0x00;
	RegistPack[15] = runstatus;	//运行状态
	return RegistPack;	
}

//构建实时数据包
unsigned char* PROTOCOL_CreateRealDataPack(unsigned char resistance)
{
		CONTROLCMD_DATA_STRUCT *pControlCMD_Data_Struct = &gControlCMD_Data_Struct;
		volatile unsigned char temp_met = 0;
		volatile float met = (((7 + 1.8 * (6* (4.5 + Resistance_Now*0.25) * (float)(gSpeed_Struct.nowspeed)/100)) /(float)pControlCMD_Data_Struct->weight) /3.5)*10 ;
		static unsigned char RegistPack[48] = {0};
		unsigned char i;
		temp_met = (unsigned char )met;
		RegistPack[0] = 0x50;
		RegistPack[1] = 0x01;
		RegistPack[2] = 0x00;
		RegistPack[3] = 0x0A;
		for(i = 0;i < 6;i++)	//设备ID一共6位
		{
			RegistPack[4+i] = gDeviceID[i];
		}
		RegistPack[10] = 0x50;
		RegistPack[11] = 0x02;
		RegistPack[12] = 0x00;
		RegistPack[13] = 0x06;
		RegistPack[14] = 0x00;
		RegistPack[15] = gDeviceType;	//设备类型
		RegistPack[16] = 0x50;
		RegistPack[17] = 0x05;
		RegistPack[18] = 0x00;
		RegistPack[19] = 0x08;
		RegistPack[20] = (pControlCMD_Data_Struct->user_id >> 24) & 0xff;
		RegistPack[21] = (pControlCMD_Data_Struct->user_id >> 16) & 0xff;
		RegistPack[22] = (pControlCMD_Data_Struct->user_id >> 8) & 0xff;
		RegistPack[23] = pControlCMD_Data_Struct->user_id & 0xff;
		RegistPack[24] = 0x50;
		RegistPack[25] = 0x20;
		RegistPack[26] = 0x00;
		RegistPack[27] = 0x12;
		RegistPack[28] = 0x00;	
		RegistPack[29] = 0x00;
		RegistPack[30] = 0x00;
		RegistPack[31] = 0x00; 	//时间
		RegistPack[32] = 0x00;
		RegistPack[33] = 0x00;	//距离
		RegistPack[34] = temp_met >> 8;
		RegistPack[35] = temp_met;	//met
		RegistPack[36] = gSpeed_Struct.nowspeed >> 8;
		RegistPack[37] = gSpeed_Struct.nowspeed;	//速度
		RegistPack[38] = 0x00;
		RegistPack[39] = 0x00;	//坡度
		RegistPack[40] = 0x00;
		RegistPack[41] = resistance;	//阻力
		RegistPack[42] = 0x50;
		RegistPack[43] = 0x30;
		RegistPack[44] = 0x00;
		RegistPack[45] = 0x06;
		RegistPack[46] = 0x00;
		RegistPack[47] = 0x01;
		return RegistPack;		
}

//构建处方完成消息 prescription
unsigned char* PROTOCOL_CreatePrescriptionFinshPack()
{
	CONTROLCMD_DATA_STRUCT *pControlCMD_Data_Struct = &gControlCMD_Data_Struct;
	static unsigned char RegistPack[10] = {0};

	TRAIN_FINISH_DATA_STRUCT *pTrain_Finsh_Data_Struct = &gTrain_Finish_Data_Struct;

	RegistPack[0] = 0x50;
	RegistPack[1] = 0x31;
	RegistPack[2] = 0x00;
	RegistPack[3] = 0x0A;
	RegistPack[4] = pControlCMD_Data_Struct->prescription_id >> 24;
	RegistPack[5] = pControlCMD_Data_Struct->prescription_id >> 16;
	RegistPack[6] = pControlCMD_Data_Struct->prescription_id >> 8;
	RegistPack[7] = pControlCMD_Data_Struct->prescription_id;
	RegistPack[8] = pTrain_Finsh_Data_Struct->finish_strtus;
	RegistPack[9] = pTrain_Finsh_Data_Struct->borg;

	return RegistPack;
}




void IntToHexChar(unsigned char *ReturnStr,unsigned char *HexStr,unsigned char len)
{
	unsigned char i = 0;
	unsigned char temp = 0;
	for(i = 0;i < len;i++)
	{
		temp = (HexStr[i] & 0xF0) >> 4;  //0xFE & 0XF0 -> 0XF0 >>4 -> 0X0F
		if (temp > 9) temp = temp + 55;	
		else temp = temp + 0x30;
		ReturnStr[2*i] = temp;
		temp = HexStr[i] & 0x0F;
		if (temp > 9) temp = temp + 55;	
		else temp = temp + 0x30;
		ReturnStr[2*i+1] = temp;
	}
}



char HexStr2Integer(char *in,char *out ,int len)
{
    char *str = (char *)malloc(len);
	int i;
    memset(str, 0, len);
    memcpy(str, in, len);
    for (i = 0; i < len; i+=2) {
        //小写转大写
        if(str[i] >= 'a' && str[i] <= 'f') str[i] = str[i] & ~0x20;
        if(str[i+1] >= 'a' && str[i] <= 'f') str[i+1] = str[i+1] & ~0x20;
        //处理第前4位
        if(str[i] >= 'A' && str[i] <= 'F')
            out[i/2] = (str[i]-'A'+10)<<4;
        else
            out[i/2] = (str[i] & ~0x30)<<4;
        //处理后4位, 并组合起来
        if(str[i+1] >= 'A' && str[i+1] <= 'F')
            out[i/2] |= (str[i+1]-'A'+10);
        else
            out[i/2] |= (str[i+1] & ~0x30);
    }
    free(str);
    return 0;
}






















