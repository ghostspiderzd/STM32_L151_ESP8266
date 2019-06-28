#ifndef _WIFI_DRIVER_H
#define _WIFI_DRIVER_H

#include "sys.h"




#define TEST_CONNECTION_TYPE   1	   // 0=WIFI module as UDP, 1=WIFI module as TCP Client, 2=WIFI module as TCP Server
#define TEST_LOCAL_PORT 0 				//0=随机端口

#define AP_SSID "tp_test"
#define AP_KEY	"123456789"

//#define AP_SSID "TP-LINK_2932"
//#define AP_KEY	"smartsens"

//#define AP_SSID "TP-ZKSZJK"
//#define AP_KEY	"85524950"

#define TEST_REMOTE_ADDR    		"192.168.3.188"   // "www.baidu.com"
#define TEST_REMOTE_PORT  	    8885

#define RESIST_MAX 12
#define RESIST_MIN 1

#define TCPRECEIVEBUFF_MAXLEN 200

#define M8266WIFI_SPI_SEND_DATA_BLOCK_MAX_LOOPS   5
#define tcp_packet_size 1024


//tcp
typedef struct
{
	unsigned char link_id;	//链路ID max 4
	unsigned char tcp_type;	//tcp连接类型 UDP TCP
	unsigned char tcpconnectstatus;	//tcp 连接状态	
	unsigned char wificonnectstatus;	//wifi连接状态
	char tcpip[16];		//ip地址
	u16 errstatus;				//错误状态和错误编码
}WIFI_ATTRIBUTE_STRUCT;
extern WIFI_ATTRIBUTE_STRUCT gTcp_Atttibute_Struct;

typedef struct
{
	unsigned char tcpReceiveBuff[TCPRECEIVEBUFF_MAXLEN];	//TCP接收到的数据
	unsigned char tcpReceivelen;			//tcp接收的数据长度
	unsigned char tcpSendlen;				//实际发送长度
	unsigned char tcpReceiveFlag;
}WIFI_SEND_RECEIVE_STRUCT;
extern WIFI_SEND_RECEIVE_STRUCT gWIFI_Send_Receive_Struct;


typedef enum
{
	SEND_REGISTRATION_MESSAGE = 0,				//发送注册消息
	WAIT_REGISTRATION_RESPONSE,				//等待注册响应
	SEND_HEARTBEAT_MESSAGE,					//发送心跳消息
}WIFI_FLOWPATH_ENUM;
extern WIFI_FLOWPATH_ENUM gWIFIEvent;





typedef enum 
{
	WIFI_CONFIG_FINISH = 0,
	WIFI_CONFIG_CANCEL_PASSTHROUGH = 1,  //关闭透传
	WIFI_CONFIG_CONFIRMATION_CODE,		//发送确认码
	WIFI_CONFIG_CLOSE_ECHO,				//关闭回显
	WIFI_CONFIG_CHECK_UART,				//查询uart信息
	WIFI_CONFIG_GET_MAC,				//获取MAC
	WIFI_CONFIG_SET_MODE,				//设置工作模式STA/AP
	WIFI_CONFIG_SSID_PASSWORD,			//连接wifi的账号密码
	WIFI_CONFIG_GET_CONNECTSTATUS,		//获取wifi连接状态
	WIFI_CONFIG_WAIT_CONNECT,			//等待连接
	WIFI_CONFIG_GET_IP,					//获取IP地址
	WIFI_CONFIG_SET_SOCKETMODE,			//设置socket工作模式 透传/http/web
	WIFI_CONFIG_SET_SOCKETNETPRO,		//设置socket网络协议 tcpclient/tcpserver/udpclient/udpserver,服务器IP地址，端口号
	WIFI_CONFIG_WAIT_SOCKETCONNECT,		//等待socket连接
	WIFI_CONFIG_SET_SOCKETCONNECTSTATUS,//查询socket连接状态
	WIFI_CONFIG_SET_QUITATCMDMODE, 		//退出AT命令模式，退回到socket
	WIFI_CONFIG_REPOWER,				//重启
	WIFI_CONFIG_ERROR,					//返回错误
	WIFI_CHECK_CONNECTSTATUS,			//检查连接状态
	WIFI_CHECK_FINISH,					//结束查询
}WIFI_CONFIG_ENUM;


typedef enum
{
	invain_status = 0,
	wait_status,
	service_monitoring,
	connect_status,
	sendbuff_status,
	readbuff_status,
	close_status,
}WIFI_CONNECT_STATUS_ENUM;

typedef struct
{
	unsigned int time_cnt;	//时间计数
	unsigned int realdata_timecnt;	//实时数据计数
	unsigned char reregist_flag;	//重新注册标志
}WIFI_CHECKUNLINK_STRUCT;
extern WIFI_CHECKUNLINK_STRUCT gWIFI_CheckUnlink_Struct;









extern WIFI_CONFIG_ENUM gWIFI_Config_Enum;


void MX_GPIO_Init(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void WIFI_ATCmdInit(void);
void WIFI_FlowPath(void);
void Task_SendHeart(void);
void Task_SendRealData(void);
void WIFI_ResetFlow(void);
void Task_MICE(void);
void WIFI_ConfigFinishDelay(void);
unsigned char WIFI_Send_Data(u8 Data[], u16 Data_len, u8 link_no, u16* status);
unsigned char WIFI_Receive_data(u8 Data[], u16 max_len, uint16_t max_wait_in_ms,u8* link_no, u16* status);
void WIFI_Struct_Init(void);
void WIFI_Receive_AND_AnalysisData(void);
void WIFI_Recover_ResValue(void);
















#endif
