#include "lcd.h"
#include "stdlib.h"	 


	   
//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

//画笔颜色,背景颜色
u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
u16 DeviceCode;	 

/*****************************************************************************
 * @name       :void LCD_WR_REG(u8 data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_REG(uint8_t reg)
{
    LCD_CS_CLR;
    LCD_RS_CLR;                   // 命令模式
    SPI_WriteByte(SPI1, reg);
    LCD_CS_SET;
}


void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet)
{
	SPIx->CR1&=0XFFC7;
	if(SpeedSet==1)//高速
	{
		SPIx->CR1|=SPI_BAUDRATEPRESCALER_2;//Fsck=Fpclk/2	
	}
	else//低速
	{
		SPIx->CR1|=SPI_BAUDRATEPRESCALER_16; //Fsck=Fpclk/32
	}
	SPIx->CR1|=1<<6; //SPI设备使能
} 
/*****************************************************************************
 * @name       :void LCD_WR_DATA(u8 data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_DATA(u16 data)
{
   LCD_CS_CLR;
   LCD_RS_SET;
   SPI_WriteByte(SPI1, (uint8_t)data);
   LCD_CS_SET;
}

u8 LCD_RD_DATA(void)
{
	 u8 data;
	 LCD_CS_CLR;
	 LCD_RS_SET;
	 SPI_SetSpeed(SPI1,0);
	 data = SPI_WriteByte(SPI1,0xFF);
	 SPI_SetSpeed(SPI1,1);
	 LCD_CS_SET;
	 return data;
}


/*****************************************************************************
 * @name       :void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
 * @date       :2018-08-09 
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    		 
}	   

u8 LCD_ReadReg(u8 LCD_Reg)
{
	LCD_WR_REG(LCD_Reg);
  return LCD_RD_DATA();
}

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}	 


void LCD_ReadRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.rramcmd);
}	 

/*****************************************************************************
 * @name       :void Lcd_WriteData_16Bit(u16 Data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :Data:Data to be written
 * @retvalue   :None
******************************************************************************/	 
void Lcd_WriteData_16Bit(u16 Data)
{	
	 LCD_CS_CLR;
	 LCD_RS_SET;
	SPI_WriteByte(SPI1, Data>>8);
	SPI_WriteByte(SPI1, Data);
	 LCD_CS_SET;
}

u16 Lcd_ReadData_16Bit(void)
{
	u16 r,g;
	LCD_CS_CLR;
	LCD_RS_CLR;	  
	SPI_WriteByte(SPI1,lcddev.rramcmd);
	SPI_SetSpeed(SPI1,0);
	LCD_RS_SET;
	SPI_WriteByte(SPI1,0xFF);
	r = SPI_WriteByte(SPI1,0xFF);
	g = SPI_WriteByte(SPI1,0xFF);
	SPI_SetSpeed(SPI1,1);
	LCD_CS_SET;
	r<<=8;
	r|=g;
	return r;
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(u16 x,u16 y)
 * @date       :2018-08-09 
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/	
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);//设置光标位置 
	Lcd_WriteData_16Bit(POINT_COLOR); 
}

u16 LCD_ReadPoint(u16 x,u16 y)
{
	u16 color;
	LCD_SetCursor(x,y);//设置光标位置 
	color = Lcd_ReadData_16Bit();
	return color;
}

/*****************************************************************************
 * @name       :void LCD_Clear(u16 Color)
 * @date       :2018-08-09 
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
******************************************************************************/	
void LCD_Clear(u16 Color)
{
  unsigned int i,m;  
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);   
	LCD_CS_CLR;
	LCD_RS_SET;
	for(i=0;i<lcddev.height;i++)
	{
    for(m=0;m<lcddev.width;m++)
    {	
			SPI_WriteByte(SPI1, Color>>8);
			SPI_WriteByte(SPI1, Color);
		}
	}
	 LCD_CS_SET;
} 



/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_RESET(void)
{
	LCD_RST_CLR;
	HAL_Delay(100);	
	LCD_RST_SET;
	HAL_Delay(50);
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 	 
void LCD_Init(void)
{  			
	LCD_LED_OPEN;						 
 	LCD_RESET(); //LCD 复位
//*************4.0 ST7796S TN初始化**********//	
	LCD_WR_REG(0xF0);
	LCD_WR_DATA(0xC3);
	LCD_WR_REG(0xF0);
	LCD_WR_DATA(0x96);
	LCD_WR_REG(0x36);
	LCD_WR_DATA(0x48);	
	LCD_WR_REG(0x3A);
	LCD_WR_DATA(0x05);	
	LCD_WR_REG(0xB0);
	LCD_WR_DATA(0x80);	
	LCD_WR_REG(0xB6);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x02);	
	LCD_WR_REG(0xB5);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x04);
	LCD_WR_REG(0xB1);
	LCD_WR_DATA(0x80);	
	LCD_WR_DATA(0x10);	
	LCD_WR_REG(0xB4);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xB7);
	LCD_WR_DATA(0xC6);
	LCD_WR_REG(0xC5);
	LCD_WR_DATA(0x1C);
	LCD_WR_REG(0xE4);
	LCD_WR_DATA(0x31);
	LCD_WR_REG(0xE8);
	LCD_WR_DATA(0x40);
	LCD_WR_DATA(0x8A);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x29);
	LCD_WR_DATA(0x19);
	LCD_WR_DATA(0xA5);
	LCD_WR_DATA(0x33);
	LCD_WR_REG(0xC2);
	LCD_WR_REG(0xA7);
	
	LCD_WR_REG(0xE0);
	LCD_WR_DATA(0xF0);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x13);
	LCD_WR_DATA(0x12);
	LCD_WR_DATA(0x12);
	LCD_WR_DATA(0x2B);
	LCD_WR_DATA(0x3C);
	LCD_WR_DATA(0x44);
	LCD_WR_DATA(0x4B);
	LCD_WR_DATA(0x1B);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0x17);
	LCD_WR_DATA(0x1D);
	LCD_WR_DATA(0x21);

	LCD_WR_REG(0XE1);
	LCD_WR_DATA(0xF0);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x13);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x0D);
	LCD_WR_DATA(0x27);
	LCD_WR_DATA(0x3B);
	LCD_WR_DATA(0x44);
	LCD_WR_DATA(0x4D);
	LCD_WR_DATA(0x0B);
	LCD_WR_DATA(0x17);
	LCD_WR_DATA(0x17);
	LCD_WR_DATA(0x1D);
	LCD_WR_DATA(0x21);

	LCD_WR_REG(0xF0);
	LCD_WR_DATA(0x3C);
	LCD_WR_REG(0xF0);
	LCD_WR_DATA(0x69);
	LCD_WR_REG(0X13);
	LCD_WR_REG(0X11);
	LCD_WR_REG(0X29);
 

	//设置LCD属性参数
	LCD_direction(USE_HORIZONTAL);//设置LCD显示方向 
	LCD_Clear(WHITE);//清全屏白色
}
 
/*****************************************************************************
 * @name       :void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
 * @date       :2018-08-09 
 * @function   :Setting LCD display window
 * @parameters :xStar:the bebinning x coordinate of the LCD display window
								yStar:the bebinning y coordinate of the LCD display window
								xEnd:the endning x coordinate of the LCD display window
								yEnd:the endning y coordinate of the LCD display window
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{	
	LCD_WR_REG(lcddev.setxcmd);	
	LCD_WR_DATA(xStar>>8);
	LCD_WR_DATA(0x00FF&xStar);		
	LCD_WR_DATA(xEnd>>8);
	LCD_WR_DATA(0x00FF&xEnd);

	LCD_WR_REG(lcddev.setycmd);	
	LCD_WR_DATA(yStar>>8);
	LCD_WR_DATA(0x00FF&yStar);		
	LCD_WR_DATA(yEnd>>8);
	LCD_WR_DATA(0x00FF&yEnd);

	LCD_WriteRAM_Prepare();	//开始写入GRAM			
}


// void LCD_SetWindows(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd)
// {
//     LCD_WR_REG(0x2A);          // 列地址设置
//     // Lcd_WriteData_16Bit(xStar);
//     // Lcd_WriteData_16Bit(xEnd);
// 	LCD_CS_CLR;
// 	LCD_RS_SET;
// 	SPI_WriteByte(SPI1, xStar>>8);
// 	SPI_WriteByte(SPI1, xStar);
// 	SPI_WriteByte(SPI1, xEnd>>8);
// 	SPI_WriteByte(SPI1, xEnd);
// 	LCD_CS_SET;

//     LCD_WR_REG(0x2B);          // 行地址设置

//     // Lcd_WriteData_16Bit(yStar);
//     // Lcd_WriteData_16Bit(yEnd);
// 	LCD_CS_CLR;
// 	LCD_RS_SET;
// 	SPI_WriteByte(SPI1, yStar>>8);
// 	SPI_WriteByte(SPI1, yStar);
// 	SPI_WriteByte(SPI1, yEnd>>8);
// 	SPI_WriteByte(SPI1, yEnd);
// 	LCD_CS_SET;

//     LCD_WR_REG(0x2C);          // 开始写 GRAM
// }



/*****************************************************************************
 * @name       :void LCD_SetCursor(u16 Xpos, u16 Ypos)
 * @date       :2018-08-09 
 * @function   :Set coordinate value
 * @parameters :Xpos:the  x coordinate of the pixel
								Ypos:the  y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);	
} 

/*****************************************************************************
 * @name       :void LCD_direction(u8 direction)
 * @date       :2018-08-09 
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0-0 degree
                          1-90 degree
													2-180 degree
													3-270 degree
 * @retvalue   :None
******************************************************************************/ 
void LCD_direction(u8 direction)
{ 
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
	lcddev.wramcmd=0x2C;
	lcddev.rramcmd=0x2E;
			lcddev.dir = direction%4;
	switch(lcddev.dir){		  
		case 0:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;		
			LCD_WriteReg(0x36,(1<<3)|(1<<6));
		break;
		case 1:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(1<<3)|(1<<5));
		break;
		case 2:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;	
			LCD_WriteReg(0x36,(1<<3)|(1<<7));
		break;
		case 3:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(1<<3)|(1<<7)|(1<<6)|(1<<5));
		break;	
		default:break;
	}		
}	 

u16 LCD_Read_ID(void)
{
	u8 i,val[3] = {0};
	LCD_WR_REG(0xF0);     // Command Set Control
	LCD_WR_DATA(0xC3);   

	LCD_WR_REG(0xF0);     
	LCD_WR_DATA(0x96);  
	LCD_CS_CLR;
	for(i=1;i<4;i++)
	{
		LCD_RS_CLR;	  
		SPI_WriteByte(SPI1,0xFB);
		LCD_RS_SET;
		SPI_WriteByte(SPI1,0x10+i);
		LCD_RS_CLR;	  
		SPI_WriteByte(SPI1,0xD3);
		SPI_SetSpeed(SPI1,0);
		LCD_RS_SET;
		val[i-1] = SPI_WriteByte(SPI1,0xFF);
		SPI_SetSpeed(SPI1,1);
		LCD_RS_CLR;	  
		SPI_WriteByte(SPI1,0xFB);
		LCD_RS_SET;
		SPI_WriteByte(SPI1,0x00);	
	}
	LCD_CS_SET;
	LCD_WR_REG(0xF0);     // Command Set Control
	LCD_WR_DATA(0x3C);   
	LCD_WR_REG(0xF0);     
	LCD_WR_DATA(0x69);  
	lcddev.id=val[1];
	lcddev.id<<=8;
	lcddev.id|=val[2];
	return lcddev.id;
}


void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t width = ex - sx + 1;
    uint16_t height = ey - sy + 1;
    uint16_t i, j;

    // 设置显示窗口
    LCD_SetWindows(sx, sy, ex, ey);

    // 填充颜色数据
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            Lcd_WriteData_16Bit(*color);
            color++;
        }
    }
}

u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
{
	uint8_t rx_byte;
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(&hspi1, &Byte, &rx_byte, 1, 10);
    return rx_byte;
} 

