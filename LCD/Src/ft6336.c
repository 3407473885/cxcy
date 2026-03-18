#include "ft6336.h"
#include "touch.h"
#include "string.h" 
#include "lcd.h"

extern u8 touch_flag;
const u16 FT6336_TPX_TBL[2]={FT_TP1_REG,FT_TP2_REG};
/*****************************************************************************
 * @name       :u8 FT5426_WR_Reg(u16 reg,u8 *buf,u8 len)
 * @date       :2020-05-13 
 * @function   :Write data to ft5426 once
 * @parameters :reg:Start register address for written
								buf:the buffer of data written
								len:Length of data written
 * @retvalue   :0-Write succeeded 
								1-Write failed
******************************************************************************/ 
u8 FT6336_WR_Reg(u8 reg, u8 *buf, u8 len)
{
    HAL_StatusTypeDef status;
    u8 data[32];  // 安全缓冲区

    if (len == 0 || len > 30) return 0;

    data[0] = reg;
    memcpy(data + 1, buf, len);  // 或用 for 循环复制

    status = HAL_I2C_Master_Transmit(&hi2c1, FT_CMD_WR, data, len + 1, 1000);
    return (status == HAL_OK) ? 1 : 0;
}

/*****************************************************************************
 * @name       :void FT5426_RD_Reg(u16 reg,u8 *buf,u8 len)
 * @date       :2020-05-13 
 * @function   :Read data to ft5426 once
 * @parameters :reg:Start register address for read
								buf:the buffer of data read
								len:Length of data read
 * @retvalue   :none
******************************************************************************/			  
u8 FT6336_RD_Reg(u8 reg, u8 *buf, u8 len)
{
    HAL_StatusTypeDef status;

    if (len == 0) return 0;

    // 写寄存器地址
    status = HAL_I2C_Master_Transmit(&hi2c1, FT_CMD_WR, &reg, 1, 10);
    if (status != HAL_OK) return 0;

    // 读数据
    status = HAL_I2C_Master_Receive(&hi2c1, FT_CMD_RD, buf, len, 10);
    if (status != HAL_OK) return 0;

    return 1;
}
/*****************************************************************************
 * @name       :u8 FT5426_Scan(void)
 * @date       :2020-05-13 
 * @function   :Scan touch screen (query mode)
 * @parameters :none
 * @retvalue   :Current touch screen status
								0-No touch
								1-With touch
******************************************************************************/	
u8 FT6336_Scan(void)
{
	u8 buf[4];
	u8 i=0;
	u8 res=0;
	u8 temp;
	u8 mode;
	static u8 t=0;//控制查询间隔,从而降低CPU占用率   
	t++;
	if((t%10)==0||t<10)//空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率
	{
		FT6336_RD_Reg(FT_REG_NUM_FINGER,&mode,1);//读取触摸点的状态  
		if(mode&&(mode<3))
		{
			temp=0XFF<<mode;//将点的个数转换为1的位数,匹配tp_dev.sta定义 
			tp_dev.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES; 
			for(i=0;i<CTP_MAX_TOUCH;i++)
			{
				FT6336_RD_Reg(FT6336_TPX_TBL[i],buf,4);	//读取XY坐标值 
				if(tp_dev.sta&(1<<i))	//触摸有效?
				{
					switch(lcddev.dir)
					{
						case 0:
							tp_dev.x[i]=((u16)(buf[0]&0X0F)<<8)+buf[1];
							tp_dev.y[i]=((u16)(buf[2]&0X0F)<<8)+buf[3];						
							break;
						case 1:
							tp_dev.y[i]=lcddev.height-(((u16)(buf[0]&0X0F)<<8)+buf[1]);
							tp_dev.x[i]=((u16)(buf[2]&0X0F)<<8)+buf[3];						
							break;
						case 2:
							tp_dev.x[i]=lcddev.width-(((u16)(buf[0]&0X0F)<<8)+buf[1]);
							tp_dev.y[i]=lcddev.height-(((u16)(buf[2]&0X0F)<<8)+buf[3]);								
							break;
						case 3:
							tp_dev.y[i]=((u16)(buf[0]&0X0F)<<8)+buf[1];
							tp_dev.x[i]=lcddev.width-(((u16)(buf[2]&0X0F)<<8)+buf[3]);	
							break;
					} 
					//if((buf[0]&0XF0)!=0X80)tp_dev.x[i]=tp_dev.y[i]=0;//必须是contact事件，才认为有效
					//printf("x[%d]:%d,y[%d]:%d\r\n",i,tp_dev.x[i],i,tp_dev.y[i]);
				}			
			} 
			res=1;
			if(tp_dev.x[0]==0 && tp_dev.y[0]==0)mode=0;	//读到的数据都是0,则忽略此次数据
			t=0;		//触发一次,则会最少连续监测10次,从而提高命中率
		}
	}
	if(mode==0)//无触摸点按下
	{ 
		if(tp_dev.sta&TP_PRES_DOWN)	//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);	//标记按键松开
		}else						//之前就没有被按下
		{ 
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
			tp_dev.sta&=0XE0;	//清除点有效标记	
		}	 
	} 
	if(t>240)t=10;//重新从10开始计数
	return res;
}
 

u8 FT6336_Init(void)
{
	u8 temp[2]; 	

	FT_RST_RESET;  // 拉低复位
	HAL_Delay(10);
	FT_RST_SET;    // 拉高释放复位
	HAL_Delay(500);
		
	FT6336_RD_Reg(FT_ID_G_FOCALTECH_ID,&temp[0],1);
	
	if(temp[0]!=0x11)
	{
		return 1;
	}
	FT6336_RD_Reg(FT_ID_G_CIPHER_MID,&temp[0],2);
	if(temp[0]!=0x26)
	{
		return 1;
	}
	if((temp[1]!=0x00)&&(temp[1]!=0x01)&&(temp[1]!=0x02))
	{
		return 1;
	}
	FT6336_RD_Reg(FT_ID_G_CIPHER_HIGH,&temp[0],1);
	if(temp[0]!=0x64)
	{
		return 1;
	}
	return 0;
}


