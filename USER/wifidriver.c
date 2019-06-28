#include "wifidriver.h"
#include "uartdriver.h"
#include "protocol.h"
#include "time.h"
#include "brd_cfg.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"
#include "switch.h"
#include "delay.h"
#include "speed.h"


unsigned char gWIFI_UARTRevFinishFlag;

WIFI_CONFIG_ENUM gWIFI_Config_Enum = WIFI_CONFIG_SET_MODE;  

WIFI_ATTRIBUTE_STRUCT gTcp_Atttibute_Struct;	//tcp属性

WIFI_FLOWPATH_ENUM gWIFIEvent = SEND_REGISTRATION_MESSAGE;

WIFI_SEND_RECEIVE_STRUCT gWIFI_Send_Receive_Struct;

WIFI_CHECKUNLINK_STRUCT gWIFI_CheckUnlink_Struct;


unsigned short Resistance_Now = 1;	//阻力值
unsigned char gSpeed = 0;

void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	WIFI_SEND_RECEIVE_STRUCT *pWIFI_Send_Receive_Struct = &gWIFI_Send_Receive_Struct;
	//WIFI_ATTRIBUTE_STRUCT *pTcp_Atttibute_Struct = &gTcp_Atttibute_Struct;	
//	unsigned char wifi_string_buff[TCPRECEIVEBUFF_MAXLEN*2] = {0};
	if(GPIO_Pin == GPIO_PIN_1)
	{
	   pWIFI_Send_Receive_Struct->tcpReceiveFlag = SET;	//置位
	}
}

void WIFI_Receive_AND_AnalysisData()
{
	WIFI_SEND_RECEIVE_STRUCT *pWIFI_Send_Receive_Struct = &gWIFI_Send_Receive_Struct;
	WIFI_ATTRIBUTE_STRUCT *pTcp_Atttibute_Struct = &gTcp_Atttibute_Struct;	
	 pWIFI_Send_Receive_Struct->tcpReceivelen = WIFI_Receive_data(pWIFI_Send_Receive_Struct->tcpReceiveBuff,\
																	TCPRECEIVEBUFF_MAXLEN,  \
																	5*1000,NULL,\
																	&pTcp_Atttibute_Struct->errstatus);
		if(pWIFI_Send_Receive_Struct->tcpReceivelen != 0)
		{
			
			//IntToHexChar(wifi_string_buff,pWIFI_Send_Receive_Struct->tcpReceiveBuff,(pWIFI_Send_Receive_Struct->tcpReceivelen >= TCPRECEIVEBUFF_MAXLEN)?TCPRECEIVEBUFF_MAXLEN : pWIFI_Send_Receive_Struct->tcpReceivelen);
			//UART2_SendData(wifi_string_buff,pWIFI_Send_Receive_Struct->tcpReceivelen*2);
			//DEBUG_INFO(wifi_string_buff,(pWIFI_Send_Receive_Struct->tcpReceivelen >= TCPRECEIVEBUFF_MAXLEN)?TCPRECEIVEBUFF_MAXLEN : pWIFI_Send_Receive_Struct->tcpReceivelen*2);
			DEBUG_HEX(pWIFI_Send_Receive_Struct->tcpReceiveBuff,pWIFI_Send_Receive_Struct->tcpReceivelen);
			PROTOCOL_ParseCode(pWIFI_Send_Receive_Struct->tcpReceiveBuff, pWIFI_Send_Receive_Struct->tcpReceivelen);
			//解析结束，将缓冲区清零
			memset(pWIFI_Send_Receive_Struct->tcpReceiveBuff,0,TCPRECEIVEBUFF_MAXLEN);
		}
}

void WIFI_ATCmdInit()
{
	WIFI_ATTRIBUTE_STRUCT *pTcp_Atttibute_Struct = &gTcp_Atttibute_Struct;	
		switch(gWIFI_Config_Enum)
		{
		case WIFI_CONFIG_SET_MODE:
				if(M8266WIFI_SPI_Set_Opmode(1, 0, &pTcp_Atttibute_Struct->errstatus))
				{
					DEBUG_LOG(OK set mode);
					gWIFI_Config_Enum = WIFI_CONFIG_SSID_PASSWORD;
				}
				else
				{
					DEBUG_LOG(ERR set mode);
					gWIFI_Config_Enum = WIFI_CONFIG_ERROR; 
				}
		break;
		case WIFI_CONFIG_SSID_PASSWORD:		
				if(M8266WIFI_SPI_STA_Connect_Ap(AP_SSID, AP_KEY, 0, 20, &pTcp_Atttibute_Struct->errstatus)) //设置SSID,密码
				{
					DEBUG_LOG(OK SSID PASSWORD);
					gWIFI_Config_Enum = WIFI_CONFIG_GET_IP;
				}
				else
				{
					DEBUG_LOG(ERR SSID PASSWORD);
					//gWIFI_Config_Enum = WIFI_CONFIG_ERROR; 
				}
		break;		
		case WIFI_CONFIG_GET_IP:
				if(M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip(pTcp_Atttibute_Struct->tcpip, 10))		//获取IP
				{
					DEBUG_LOG(ok get ip);
					
					DEBUG_INFO((unsigned char *)pTcp_Atttibute_Struct->tcpip,15);
					gWIFI_Config_Enum = WIFI_CONFIG_SET_SOCKETNETPRO;
				}
 				else
				{
					DEBUG_LOG(ERR get ip);
					//gWIFI_Config_Enum = WIFI_CONFIG_ERROR; 
				}
				//DEBUG_INFO((const char *)gDebugStruct.TranmitBuff);
		break;
		case WIFI_CONFIG_SET_SOCKETNETPRO:
				if(M8266WIFI_SPI_Setup_Connection(TEST_CONNECTION_TYPE, TEST_LOCAL_PORT, TEST_REMOTE_ADDR, TEST_REMOTE_PORT, pTcp_Atttibute_Struct->link_id, 20, &pTcp_Atttibute_Struct->errstatus))
				{
					DEBUG_LOG(OK set socket protocol);
					gWIFI_Config_Enum = WIFI_CONFIG_FINISH;
					//softTimer_Stop(WIFI_TIME_ID); //暂停当前任务
					//softTimer_Restart(WIFI_CHECKSTATUS_TIME_ID);
				}
				else 
				{
					//DEBUG_INFO(&pTcp_Atttibute_Struct->errstatus,1);
					DEBUG_LOG(ERR set socket protocol);
					//gWIFI_Config_Enum = WIFI_CONFIG_ERROR;
				}
		break;
		case WIFI_CONFIG_FINISH:  //配置结束
			DEBUG_LOG(config finish);
			softTimer_Stop(WIFI_CONFIG_ID);
			delay_ms(500);
			softTimer_Create(WIFI_REGIST_ID,MODE_ONE_SHOT,2000,WIFI_ConfigFinishDelay,0,0);
		break;
		case WIFI_CONFIG_ERROR:
		//	WIFI_ResetFlow();	//重启
			DEBUG_LOG(err );
		break;
		default:
		break;
	}	
}




void WIFI_FlowPath()
{
	PROTOCOL_SENDPACKET_STRUCT *pProtocol_SendPacket_Struct = &gProtocol_SendPacket_Struct;
	DI_STRUCT *pDI_Struct = &gDI_Struct;

//	WIFI_SEND_RECEIVE_STRUCT *pWIFI_Send_Receive_Struct = &gWIFI_Send_Receive_Struct;
	WIFI_ATTRIBUTE_STRUCT *pTcp_Atttibute_Struct = &gTcp_Atttibute_Struct;

	MICE_STRUCT *pMICE_Struct = &gMICE_Struct;

	static unsigned char Rnt = 0;		//用来做延时时间，计数延时
	
	if(gWIFI_Config_Enum == WIFI_CONFIG_FINISH)
	{
		switch(gWIFIEvent)
			{
				case SEND_REGISTRATION_MESSAGE:
					DEBUG_LOG("send registration message");
					//pDI_Struct->valuebuff = registerbuff;
					pDI_Struct->valuebuff = PROTOCOL_CreateRegistPack();	
					pDI_Struct->length = 16;
					PROTOCOL_PackedData(0x03 , pDI_Struct);
					WIFI_Send_Data(pProtocol_SendPacket_Struct->Protocol_PacketBuff, \
																	pProtocol_SendPacket_Struct->len,\
																	(u8)pTcp_Atttibute_Struct->link_id,\
																	&pTcp_Atttibute_Struct->errstatus);
					
					//if(pWIFI_Send_Receive_Struct->tcpSendlen != 0)
					//{
						gWIFIEvent = WAIT_REGISTRATION_RESPONSE;
					//	DEBUG_LOG(tcplen send success);
					//}
					//else
					//{
					////	DEBUG_LOG(tcplen send false);
					//	softTimer_Stop(WIFI_REGIST_ID);
					//	gWIFI_Config_Enum = WIFI_CONFIG_SET_SOCKETNETPRO;		//回到重建链接
					//	gWIFIEvent = SEND_REGISTRATION_MESSAGE;					//注册流程初始化
					//	softTimer_Create(WIFI_CONFIG_ID,MODE_PERIODIC,WIFI_CONFIG_DELAY,WIFI_ATCmdInit,0,0);
					//}
				break;
				case WAIT_REGISTRATION_RESPONSE:
					if(Rnt++ < 6)
					{
						if(gResponseMark == REGISTERED_MARK)
						{
							gResponseMark = 0;
							DEBUG_LOG(registered sucess);		
							gWIFIEvent = SEND_HEARTBEAT_MESSAGE;
						}
					}
					else
					{
						Rnt = 0;
						DEBUG_LOG(registered faild);	
						WIFI_ResetFlow();		//注册失败重连
						gWIFIEvent = SEND_REGISTRATION_MESSAGE;
					}
				break;
				case SEND_HEARTBEAT_MESSAGE:		
					softTimer_Stop(WIFI_REGIST_ID);
					softTimer_Create(WIFI_HEART_ID,MODE_PERIODIC,WIFI_HEART_DELAY,Task_SendHeart,NULL,NULL);//心跳
					if(pMICE_Struct->RunStatus == SET)
					{
						softTimer_Create(SEND_REAL_DATA,MODE_PERIODIC,SEND_REAL_DATA_DELAY,Task_SendRealData,NULL,NULL);	
					}
				break;
				default :
				break;
			}
	}
}


//发送心跳
void Task_SendHeart(void)
{
	PROTOCOL_SENDPACKET_STRUCT *pProtocol_SendPacket_Struct = &gProtocol_SendPacket_Struct;
	DI_STRUCT *pDI_Struct = &gDI_Struct;

//	WIFI_SEND_RECEIVE_STRUCT *pWIFI_Send_Receive_Struct = &gWIFI_Send_Receive_Struct;
	WIFI_ATTRIBUTE_STRUCT *pTcp_Atttibute_Struct = &gTcp_Atttibute_Struct;

//	MICE_STRUCT *pMICE_Struct = &gMICE_Struct;
//	if(pMICE_Struct->RunStatus == SET)
	{		
		//gWIFI_CheckUnlink_Struct.time_cnt++;	//每秒+1
		if(gWIFI_CheckUnlink_Struct.time_cnt++ >= 5)
		{
			gWIFI_CheckUnlink_Struct.time_cnt = 0;	
			WIFI_ResetFlow();	//重连
			/*if(M8266WIFI_SPI_Query_Connection(pTcp_Atttibute_Struct->link_id,NULL,&pTcp_Atttibute_Struct->tcpconnectstatus,NULL,NULL,NULL,&pTcp_Atttibute_Struct->errstatus))
			{
				switch((WIFI_CONNECT_STATUS_ENUM)pTcp_Atttibute_Struct->tcpconnectstatus)
					{
					case invain_status:
						DEBUG_LOG(invain status);
						//softTimer_Stop(SEND_REAL_DATA);
						WIFI_ResetFlow();
					break;
					case wait_status:
						DEBUG_LOG(wait status);
					break;
					case service_monitoring:
						DEBUG_LOG(service monitoring);
					break;
					case connect_status:
						DEBUG_LOG(connect status);
					break;
					case sendbuff_status:
						DEBUG_LOG(sendbuff status);
					break;
					case readbuff_status:
						DEBUG_LOG(readbuff status);
					break;
					case close_status:
						DEBUG_LOG(close status);
						//softTimer_Stop(SEND_REAL_DATA);
						WIFI_ResetFlow();
					break;
					default :	DEBUG_LOG(unknow status);
					break;
				}
			}
		*/
			//断链重连
		}
	}
	DEBUG_LOG(send heart);
	memset(pProtocol_SendPacket_Struct->Protocol_PacketBuff,0,PROTOCOLPACKET_MAXLEN);
	//pDI_Struct->valuebuff = heartbuff;
	pDI_Struct->valuebuff = PROTOCOL_CreateHeartPack(0x02);	//空闲
	pDI_Struct->length = 16;
	PROTOCOL_PackedData(0x04,pDI_Struct);
	WIFI_Send_Data(pProtocol_SendPacket_Struct->Protocol_PacketBuff, \
																	pProtocol_SendPacket_Struct->len,\
																	(u8)pTcp_Atttibute_Struct->link_id,\
																	&pTcp_Atttibute_Struct->errstatus);
//	{
//		DEBUG_LOG(send heart faild);
//		softTimer_Stop(SEND_REAL_DATA);
//		WIFI_ResetFlow();
//	}
}

//发送实时数据 开始之后，由于没有按钮，获取到处方，解析处方，然后直接开始
void Task_SendRealData(void)
{
	PROTOCOL_SENDPACKET_STRUCT *pProtocol_SendPacket_Struct = &gProtocol_SendPacket_Struct;
	DI_STRUCT *pDI_Struct = &gDI_Struct;

//	WIFI_SEND_RECEIVE_STRUCT *pWIFI_Send_Receive_Struct = &gWIFI_Send_Receive_Struct;
	WIFI_ATTRIBUTE_STRUCT *pTcp_Atttibute_Struct = &gTcp_Atttibute_Struct;

	DEBUG_LOG(send realdata);
	memset(pProtocol_SendPacket_Struct->Protocol_PacketBuff,0,PROTOCOLPACKET_MAXLEN);
//	pDI_Struct->valuebuff = realdatabuff;
	pDI_Struct->valuebuff = PROTOCOL_CreateRealDataPack(Resistance_Now);
	pDI_Struct->length = 48;
	PROTOCOL_PackedData(0x0A,pDI_Struct);		//实时消息更改为 0x0A,上一次是08
	
	WIFI_Send_Data(pProtocol_SendPacket_Struct->Protocol_PacketBuff, \
																	pProtocol_SendPacket_Struct->len,\
																	(u8)pTcp_Atttibute_Struct->link_id,\
																	&pTcp_Atttibute_Struct->errstatus);
	Speed_calculate();	//计算速度
	
//	{
//		DEBUG_LOG(send realdata faild);
//		softTimer_Stop(SEND_REAL_DATA);
//		WIFI_ResetFlow();
//	}
	
}

//MICE功能说明
/*
	MICE训练方式分三个阶段，热身阶段、训练阶段、恢复阶段。
	热身阶段：阻力值每10s加1
	训练阶段：首先判断阻力值是否达到指定阻力值。
			  如果没达到，以10s间隔，继续加，直到阻力值到达指定阻力值，当到达之后，开始30s计算一次平均心率，计算的平均心率和处方的最大最小心率进行比较，决定加减。
			  如果达到，开始30s计算一次平均心率，用平均心率和处方的最大最小值进行比较，决定加减。
	恢复阶段：每10s减1阻力，直到减为1，若时间先到，阻力值还不为1，则直接将阻力减小到1.
*/
void Task_MICE(void)
{
	CONTROLCMD_DATA_STRUCT *pControlCMD_Data_Struct = &gControlCMD_Data_Struct;
//	PROTOCOL_SENDPACKET_STRUCT *pProtocol_SendPacket_Struct = &gProtocol_SendPacket_Struct;
//	DI_STRUCT *pDI_Struct = &gDI_Struct;
	MICE_STRUCT *pMICE_Struct = &gMICE_Struct;
	ECG_STRUCT *pECG_Struct = &gECG_Struct;
	static unsigned int time_cnt = 10,time_heartrate = 0;
//	unsigned  char i = 0;
	DEBUG_LOG("Task_MICE");
	switch(gMICE_Enum)
	{		
		//热身阶段	
		case WARM_UP_FASE:
			DEBUG_LOG("WARM_UP_FASE");
			if(time_cnt < pMICE_Struct->warm_up_time)
			{
				if((Resistance_Now < pControlCMD_Data_Struct->resistance) && (Resistance_Now <= RESIST_MAX))
				{
					DEBUG_LOG("+1");
					Switch_Add();	//增加
					Resistance_Now++;
				}
				time_cnt += 10;
			}
			else			//当时间到达，也增加一
			{
				gMICE_Enum = TRAIN_FASE_1; 
				DEBUG_LOG("TRAIN_FASE_1");
				time_cnt = 10;
			}
			//UART_SendByte(Resistance_Now+0x30);
			
		break;
		//训练阶段
		case TRAIN_FASE_1:
			DEBUG_LOG("TRAIN_FASE_1");
			if(time_cnt < pMICE_Struct->train_time)
			{
				if((Resistance_Now < pControlCMD_Data_Struct->resistance) &&(Resistance_Now < pControlCMD_Data_Struct->resistance_max)&& (Resistance_Now <= RESIST_MAX))
				{
					DEBUG_LOG("+1");
					Switch_Add();
					Resistance_Now++;
					//UART_SendByte(Resistance_Now+0x30);
				}
				else
				{
					gMICE_Enum = TRAIN_FASE_2;	//达到处方指定阻力值，进入训练第二阶段
				}
				time_cnt += 10;
			}
			else
			{
				gMICE_Enum = RECOVER_FASE;	//训练结束仍没加到处方设定的阻力值，直接进入恢复阶段。
				DEBUG_LOG("recover fase");
			}
		break;
		case TRAIN_FASE_2:
			DEBUG_LOG("TRAIN_FASE_2");
			if(time_cnt < pMICE_Struct->train_time)
			{
				pECG_Struct->Heart_Rate_Struct.heart_rate_startflag = SET;
				if(time_heartrate++ >= 2)	//30秒计算一次平均心率
				{
					time_heartrate = 0;
					pECG_Struct->Heart_Rate_Struct.heart_rate_average =	pECG_Struct->Heart_Rate_Struct.heart_rate_sum / (pECG_Struct->Heart_Rate_Struct.heart_rate_cnt - 1);
					memset(pECG_Struct->Heart_Rate_Struct.heart_rate,0,pECG_Struct->Heart_Rate_Struct.heart_rate_cnt);
					pECG_Struct->Heart_Rate_Struct.heart_rate_cnt = 0;
					pECG_Struct->Heart_Rate_Struct.heart_rate_sum = 0;
					if((pECG_Struct->Heart_Rate_Struct.heart_rate_average >= pControlCMD_Data_Struct->HB_min) && \
						(pECG_Struct->Heart_Rate_Struct.heart_rate_average <= pControlCMD_Data_Struct->HB_max))
					{
								//阻力保持不变
								DEBUG_LOG("unchange");
					}
					if((pECG_Struct->Heart_Rate_Struct.heart_rate_average < pControlCMD_Data_Struct->HB_min) &&(Resistance_Now < pControlCMD_Data_Struct->resistance_max) && (Resistance_Now <= RESIST_MAX))
					{
								//阻力加
								DEBUG_LOG("+1");
								Switch_Add();
								Resistance_Now++;
					}
					if((pECG_Struct->Heart_Rate_Struct.heart_rate_average > pControlCMD_Data_Struct->HB_max) && (Resistance_Now >= RESIST_MIN))
					{
								//阻力减
								DEBUG_LOG("-1");
								Switch_Sub();
								Resistance_Now--;
					}
				}
				time_cnt += 10;
				//UART_SendByte(Resistance_Now+0x30);	
			}
			else
			{
				gMICE_Enum = RECOVER_FASE;
				time_cnt = 10;
			}
		break;
		//恢复阶段
		case RECOVER_FASE:
			DEBUG_LOG("RECOVER_FASE");
			pECG_Struct->Heart_Rate_Struct.heart_rate_startflag = RESET;
			if((time_cnt < pMICE_Struct->recover_time-10))
			{
				if(Resistance_Now > RESIST_MIN)
				{
					DEBUG_LOG("-1");
					Switch_Sub();
					Resistance_Now--;
				}
				//UART_SendByte(Resistance_Now+0x30);
				time_cnt += 10;
			}
			else
			{
				gMICE_Enum = FINISH_FASE;
				time_cnt = 0;
				pMICE_Struct->RunStatus = RESET;	//运行结束
				softTimer_Stop(MICE_ID);	//训练结束，停止定时器
				softTimer_Stop(SEND_REAL_DATA);
				DEBUG_LOG(train finish);
				DEBUG_LOG("-1");
				Switch_Sub();
				Resistance_Now--;
			}
		break;
		case FINISH_FASE:
		break;
		default :
		break;
	}
	//UART_SendByte(pControlCMD_Data_Struct->resistance+0x30); //目标阻力
}



//当一个流程结束之后，需要重启模块，重新注册
void WIFI_ResetFlow()
{
	DEBUG_LOG(reconnect);
	softTimer_Stop(SEND_REAL_DATA);
	softTimer_Stop(WIFI_REGIST_ID);
	softTimer_Stop(WIFI_HEART_ID);							//关闭心跳
	gWIFI_Config_Enum = WIFI_CONFIG_SET_SOCKETNETPRO;		//回到重建链接
	gWIFIEvent = SEND_REGISTRATION_MESSAGE;					//注册流程初始化
	softTimer_Create(WIFI_CONFIG_ID,MODE_PERIODIC,WIFI_CONFIG_DELAY,WIFI_ATCmdInit,0,0);
}

void WIFI_ConfigFinishDelay()
{
	softTimer_Create(WIFI_REGIST_ID,MODE_PERIODIC,WIFI_REGIST_DELAY,WIFI_FlowPath,0,0);
}

unsigned char WIFI_Send_Data(u8 Data[], u16 Data_len, u8 link_no, u16* status)
{
	unsigned char sent = 0,printIN[2] = {0},printOUT[4];
	u16 loops     = 0;
	 for(sent=0, loops=0; (sent<Data_len)&&(loops<=M8266WIFI_SPI_SEND_DATA_BLOCK_MAX_LOOPS); loops++)
         {		
				    sent += M8266WIFI_SPI_Send_Data(Data+sent,((Data_len-sent)>tcp_packet_size)?tcp_packet_size:(Data_len-sent), link_no, status);
            		if(sent>=Data_len)  break;
			      if((*status&0xFF) == 0x00)
			      {
			      			//DEBUG_LOG(send faild);
							 loops = 0;
			      }
		        else
			      {
			      	
				    if(   ((*status&0xFF) == 0x14)      // 0x14 = connection of link_no not present
                 || ((*status&0xFF) == 0x15)    // 0x15 = connection of link_no closed
                 || ((*status&0xFF) == 0x18) )  // 0x18 = TCP server in listening states and no tcp clients connecting to so far
		            {
	                 // add some failure process here
	                 WIFI_ResetFlow();
		            }
				 	else if((*status&0xFF) == 0x13)
				 	{
						DEBUG_LOG(link no change);
					}
		            else
		            {
		            	DEBUG_LOG(send delay);
		               M8266HostIf_delay_us(100);
		            }
			     }
         } // end of for(...
		 
		 if((loops > M8266WIFI_SPI_SEND_DATA_BLOCK_MAX_LOOPS) && (sent<Data_len))	//重发失败
         	{
         		DEBUG_LOG(re send faild);
				printIN[0] = *status >> 8;
				printIN[1] = *status;
				IntToHexChar(printOUT,printIN,2);
				DEBUG_INFO(printOUT,4);
			 }
	return sent;
}

unsigned char WIFI_Receive_data(u8 Data[], u16 max_len, uint16_t max_wait_in_ms,u8* link_no, u16* status)
{
	u16 received = 0;
//	unsigned char link_id = 0;
	if(M8266WIFI_SPI_Has_DataReceived())
	{
			//if(link_id != 0)
			//{
			//	DEBUG_LOG(link num no 0);
			//}
				//u16 M8266WIFI_SPI_RecvData(u8 data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u16* status);
	      received = M8266WIFI_SPI_RecvData(Data, max_len, 5*1000,link_no,status);
			//if(link_id != 0)
			//{
			//	DEBUG_LOG(rev link num no 0);
			//}
			if(received == 0)	//没有接收到数据，重新接收一次
		  	{
				received = M8266WIFI_SPI_RecvData(Data, max_len, 5*1000,link_no,status);
		  	}
		  if((*status&0xFF)!= 0 )  
			  {
				    if( (*status&0xFF)==0x22 )      // 0x22 = Module buffer has no data received
				    {  
	
			         	//M8266HostIf_delay_us(250); 
					   //M8266WIFI_Module_delay_ms(1);
				    }
				    else if( (*status&0xFF)==0x23 )   
				    { 
				    	DEBUG_LOG( have no receive completed);
				    // the packet had not been received completed in the last call of M8266WIFI_SPI_RecvData()
							// and has continued to be received in this call of M8266WIFI_SPI_RecvData()
							// do some work here if necessary
				    }
				    else if(  (*status&0xFF)==0x24)   
				    { 
				    		DEBUG_LOG(out of range);
	           			 // the packet is large in size than max_len specified and received only the max_len 
						// normally caused by the burst transmision by the routers after some block. 
						// Suggest to stop the TCP transmission for some time
					    // do some work here if necessary
				    }
					else
					{
						
					    // do some work here if necessary
					}
			  }
		  
	}
	return received;
}

void WIFI_Struct_Init()
{
	WIFI_ATTRIBUTE_STRUCT *pTcp_Atttibute_Struct = &gTcp_Atttibute_Struct;	
	pTcp_Atttibute_Struct->link_id = 0;
	memset(pTcp_Atttibute_Struct->tcpip,0,16);
}

//恢复阻力值
void WIFI_Recover_ResValue(void)
{
	unsigned char i = 1;
	for(i = 1;i <= Resistance_Now;i++)
	{
		Switch_Sub();
		delay_ms(500);
	}
	Resistance_Now = 1;
}
