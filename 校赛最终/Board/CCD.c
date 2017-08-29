#include "include.h"
struct CCD ccd1 ;//定义两个ccd结构体
struct CCD ccd2 ;
float shuzu_1[128]={0};
float shuzu_2[128]={0};
unsigned int CCD_MAX=0,CCD_MIN,CCD_AVG=0;
unsigned char IntegrationTime=10;
unsigned char PixelAverageValue;/* 128个像素点的平均AD值 */

#define TSL_SI2   PTE10_OUT   
#define TSL_CLK2  PTE12_OUT  
#define TSL_SI1   PTE6_OUT   
#define TSL_CLK1  PTE8_OUT  

void Dly_us(unsigned int us)//合理延时
{
  while(us--)
  {
    for(uint16 i=0;i<180;i++)
     asm("nop");  
  }
}

void buxian_init(void)
{
   uint8    i=0;    
                            
float  a0 =        84.9 ;
float  a1 =      -11.97 ;
float  b1 =        2.97 ;
float  a2 =      -1.902 ;
float  b2 =      0.3514 ;
float  a3 =     -0.4302 ;
float  b3 =      0.7204 ;
float  a4 =     -0.2505 ;
float  b4 =       0.219 ;
float  a5 =     -0.2574 ;
float  b5 =      0.2923 ;
float  w =     0.04476  ;
 
   
   float  aa0 =       26.62  ;
   float  aa1 =       -9.474 ;
   float  bb1 =        15.68 ;
   float  aa2 =       -2.752 ;
   float  bb2 =       -2.424 ;
   float  aa3 =       -7.399 ;
   float  bb3 =       0.3746 ;
   float  aa4 =         -2.2 ;
   float  bb4 =        5.424 ;
   float  aa5 =        1.029 ;
   float  bb5 =        1.852 ;
   float  w2 =       0.03339 ;
   
    for(i=0;i<128;i++)
    { 
       shuzu_1[i]=  a0 + a1*cos(i*w) + b1*sin(i*w) + a2*cos(2*i*w) + b2*sin(2*i*w) + a3*cos(3*i*w) + b3*sin(3*i*w)+a4*cos(4*i*w) + b4*sin(4*i*w)+ a5*cos(5*i*w) + b5*sin(5*i*w);
    //shuzu_1[i]=  a0 + a1*cos(i*w) + b1*sin(i*w) + a2*cos(2*i*w) + b2*sin(2*i*w) + a3*cos(3*i*w) + b3*sin(3*i*w)+a4*cos(4*i*w) + b4*sin(4*i*w);
   
    }
    for(i=0;i<128;i++)
    { 
       shuzu_2[i]=  aa0 + aa1*cos(i*w2) + bb1*sin(i*w2) + aa2*cos(2*i*w2) + bb2*sin(2*i*w2) + aa3*cos(3*i*w2) + bb3*sin(3*i*w2)+aa4*cos(4*i*w2) + bb4*sin(4*i*w2)+ aa5*cos(5*i*w2) + bb5*sin(5*i*w2);
    }
}

void Integration(void)   //曝光函数
{
    unsigned char i;
  if(CCD_flag==1)  
  {
    TSL_SI1=1;            /* SI  = 1 */
    Dly_us(1);
    TSL_CLK1 = 1;         /* CLK = 1 */
    Dly_us(1);
    TSL_SI1=0;            /* SI  = 0 */
    Dly_us(1);
    TSL_CLK1 = 0;         /* CLK = 0 */

    for(i=0; i<127; i++) 
    {
        Dly_us(1);
        TSL_CLK1 = 1;       /* CLK = 1 */
        Dly_us(1);
        TSL_CLK1 = 0;       /* CLK = 0 */
    }
    Dly_us(1);
    TSL_CLK1 = 1;           /* CLK = 1 */
    Dly_us(1);
    TSL_CLK1 = 0;           /* CLK = 0 */
  }
  else if(CCD_flag==2)
  {
    TSL_SI2=1;            /* SI  = 1 */
    Dly_us(1);
    TSL_CLK2 = 1;         /* CLK = 1 */
    Dly_us(1);
    TSL_SI2=0;            /* SI  = 0 */
    Dly_us(1);
    TSL_CLK2 = 0;         /* CLK = 0 */

    for(i=0; i<127; i++) 
    {
        Dly_us(1);
        TSL_CLK2 = 1;       /* CLK = 1 */
        Dly_us(1);
        TSL_CLK2 = 0;       /* CLK = 0 */
    }
    Dly_us(1);
    TSL_CLK2= 1;           /* CLK = 1 */
    Dly_us(1);
    TSL_CLK2 = 0;           /* CLK = 0 */
  }
}

void CCD2_TSL1401(uint8 *p) //采集函数
{
	unsigned char i;
         float a;
	unsigned int  temp = 0;
   
    
        TSL_SI1=1;           //SI  = 1 
	Dly_us(1);
	TSL_CLK1= 1;           // CLK = 1 
	Dly_us(1);
	TSL_SI1=0;           // SI  = 0 
	Dly_us(1);

	for(i=0; i<128; i++) 
	{
		Dly_us(1);
		TSL_CLK1 = 1;       // CLK = 1 
		Dly_us(1);
		//Sampling Pixel 2~128
		temp= adc_once(ADC0_SE17,ADC_8bit);       
		*p++ = temp;     
		TSL_CLK1 = 0;       // CLK = 0 
	}
	Dly_us(1);
	TSL_CLK1 = 1;           // CLK = 1 
	Dly_us(1);
	TSL_CLK1 = 0;           // CLK = 0 
    //    for(i=0;i<128;i++)
 //{
 // a=100/shuzu_1[i];
 //  if(CCD_BUFF[i]*a>225) CCD_BUFF[i]=255;  //镜头缺陷，采取补植，第一个点高
 //  else CCD_BUFF[i] = (uint8)(CCD_BUFF[i]*a);
// }
          
}
void CCD_TSL1401(uint8 NO) //采集函数
{
	unsigned char i;
         float a;
	unsigned int  temp = 0;
   
    if(NO==1)
    {
        TSL_SI2=1;           //SI  = 1 
	Dly_us(1);
	TSL_CLK2= 1;           // CLK = 1 
	Dly_us(1);
	TSL_SI2=0;           // SI  = 0 
	Dly_us(1);

	for(i=0; i<128; i++) 
	{
		Dly_us(1);
		TSL_CLK2 = 1;       // CLK = 1 
		Dly_us(1);
		//Sampling Pixel 2~128
		ccd1.CCD_BUFF[i]= adc_once(ADC0_SE18,ADC_8bit)-25;        
		TSL_CLK2 = 0;       // CLK = 0 
	}
	Dly_us(1);
	TSL_CLK2 = 1;           // CLK = 1 
	Dly_us(1);
	TSL_CLK2 = 0;           // CLK = 0 
 
    }
      if(NO==2)
    {
        TSL_SI1=1;           //SI  = 1 
	Dly_us(1);
	TSL_CLK1= 1;           // CLK = 1 
	Dly_us(1);
	TSL_SI1=0;           // SI  = 0 
	Dly_us(1);

	for(i=0; i<128; i++) 
	{
		Dly_us(1);
		TSL_CLK1 = 1;       // CLK = 1 
		Dly_us(1);
		//Sampling Pixel 2~128
		ccd2.CCD_BUFF[i]= adc_once(ADC0_SE17,ADC_8bit);        
		TSL_CLK1 = 0;       // CLK = 0 
	}
	Dly_us(1);
	TSL_CLK1 = 1;           // CLK = 1 
	Dly_us(1);
	TSL_CLK1 = 0;           // CLK = 0 
 
    }
               
}


/*******************************************************************************
*
* 函数名称：ImageFiltter(void);		
* 函数功能：像素中值滤波，低通
* 函数说明：五个步长，原始像素值赋予FilterPixel[i]，num[i]存的是滤波后的数据
*           取五个数据的中间值作为新的像素值；第一、二个点只排了序，还是原始值            
*
*******************************************************************************/

void ImageFilter(struct CCD *ccd)
{
  int i,j,k;
  u8 n;
  u8 num[5],FilterPixel[5];
  
  for(i=0;i<5;i++)
    FilterPixel[i]=ccd->CCD_BUFF[i];
  
  for(i=0;i<123;i++)    //for循环开始
  {
    for(j=0;j<5;j++)
    {
      num[j]=FilterPixel[j];
    }
    for(j=0;j<5;j++)            //冒泡法，由低到高排列
    {
      for(k=0;k<4-j;k++)
      {
        if(num[k]>num[k+1])
        {
          n = num[k];
          num[k] = num[k+1];
          num[k+1] = n; 
        }
      }
    }
   ccd->CCD_BUFF[i+2] = num[2];
    
    for(j=0;j<4;j++)            //传人新的五个原始数据
    FilterPixel[j] = FilterPixel[j+1];   
    FilterPixel[4] = ccd->CCD_BUFF[i+5];
  }     //for循环结束
}

unsigned char PixelAverage(unsigned char len, unsigned char *data) 
{
  unsigned char i;
  unsigned int sum = 0;
  for(i = 0; i<len; i++) 
  {
    sum = sum + *data++;
  }
  return ((unsigned char)(sum/len));
}

/**********************************************************************
*		
* 函数功能：计算阈值--大津阈值法
*
**********************************************************************/

void Threshold_Calculation(struct CCD *ccd) 
{
  int List[256];
  int32 i = 0;                   //计数               
  int32 D = 0;                   //方差
  uint8 tmThdVal = 0;
  static uint8 otherval =0;
  static uint8 otherval_old =0;
  int32 maxD = 0;               //最大方差
  uint32 sum_white = 0;         //ad值总和
  uint32 sum_black = 0; 
  uint32 sum_all = 0;
  uint8 num_white = 0;          //统计点的总数
  uint8 num_black = 0;
  int32 avg_white = 0;           //平均灰度
  int32 avg_black = 0;
  uint8 min_value = 255;
  uint8 temvalue;
  for(i=0; i<256; i++)          //初始化列表
  {
    List[i] = 0;
  } 
  for(i=ccd->CCD_Left;i<=ccd->CCD_Right;i++)   //ad值分类计数
  {
    temvalue = ccd-> CCD_BUFF[i];
    if(min_value>temvalue)
    min_value = temvalue;
    List[temvalue]++;
    sum_all += temvalue;
  }
  for(i=0; i<256; i++)
  {

    if(List[i] == 0) continue;
    num_black += List[i];                     //计算黑点的个数
    if(num_black == ccd->CCD_Pixelcnt) break;         //总点数等于128则推出循环  
    sum_black += List[i] * i;
    num_white = ccd->CCD_Pixelcnt - num_black;   //计算白点个数和ad总和
    sum_white = sum_all - sum_black;
    avg_black = sum_black/num_black;          //计算黑白点的平均ad
    avg_white = sum_white/num_white;

    D=(avg_white-avg_black)*(avg_white-avg_black)*num_black*num_white/(ccd->CCD_Pixelcnt*ccd->CCD_Pixelcnt); 
    if(D > maxD)                       //出最大方差，此时的i即为合适的阈值
    {
      maxD = D;
      ccd->CCDD = maxD ;
      tmThdVal = i;             //合适的阈值i赋值给变量m_ThdVal
    }
  }
  num_black =0;
  num_white =0;
  if(otherval ==0)
  {
    for(i=ccd->CCD_Left; i<=ccd->CCD_Right; i++)
  {
    if(ccd->CCD_BUFF[i]>tmThdVal)
    {
      num_white++;
    }
  }
  }
  else
  {
    for(i=ccd->CCD_Left; i<=ccd->CCD_Right; i++)
  {
    if(ccd->CCD_BUFF[i]>ccd->ccd_threshold)
    {
      num_white++;
    }
  }
  }
  
  num_black = 128-num_white;
  otherval = (uint8)(sum_all/(num_black*0.5+num_white*1.5));
  
  if(maxD < 15)
 {
  if(ccd->PixelAverageValue<55)
   {
     ccd->ccd_threshold = 255;
     return;
    }
   else
   {
     ccd->ccd_threshold = 0;
      return;
   }          
}

  ccd->ccd_threshold = (u8)((tmThdVal*0.5+otherval_old*0.5)*0.5 + ccd->ccd_threshold*0.5);
  otherval_old = otherval;
}



  


//void CalculateIntegrationTime(void)   //蓝宙自调整曝光时间
//{
//    unsigned char PixelAverageVoltage;/* 128个像素点的平均电压值的10倍 */
//    int TargetPixelAverageVoltage = 30;/* 设定目标平均电压值，实际电压的10倍 */
//    int PixelAverageVoltageError = 0;/* 设定目标平均电压值与实际值的偏差，实际电压的10倍 */
 //   int TargetPixelAverageVoltageAllowError = 2;/* 设定目标平均电压值允许的偏差，实际电压的10倍 */
//    PixelAverageValue = PixelAverage(128,CCD_BUFF);/* 计算128个像素点的平均AD值 */
    
 //   PixelAverageVoltage = (unsigned char)((int)PixelAverageValue * 25 / 128);/* 计算128个像素点的平均电压值,实际值的10倍 */

 //   PixelAverageVoltageError = TargetPixelAverageVoltage - PixelAverageVoltage;
//    if(PixelAverageVoltageError < -TargetPixelAverageVoltageAllowError)
//    if(PixelAverageVoltageError > TargetPixelAverageVoltageAllowError)
//        IntegrationTime++;
//    if(IntegrationTime <= 2)
 //       IntegrationTime = 2;
 //   if(IntegrationTime >= 8)
 //       IntegrationTime = 8;
//}
void CalculateIntegrationTime(void)     
{ 
  int PixelAverageValueError;           // 目标值与实际值的偏差  
  int TargetPixelAverageValue = 180;    //目标的128个像素点的平均AD值
  int TargetPixelAverageValueAllowError = 10 ;          //设定目标平均AD值允许的偏差，实际电压的10倍
  PixelAverageValue = PixelAverage(128,ccd1.CCD_BUFF);           //计算128个像素点的平均AD值
  
     
    //for(i=4;i>=1;i--)
     //PixelAverageValue[i]=PixelAverageValue[i-1];
    //PixelAverageValue = PixelAverage(ccd);
    PixelAverageValueError =TargetPixelAverageValue -PixelAverageValue ;
    if(PixelAverageValueError < -TargetPixelAverageValueAllowError)
    {
      IntegrationTime--;
    }
    if(PixelAverageValueError > TargetPixelAverageValueAllowError)
    {   
      IntegrationTime++;
    }
    
    if(IntegrationTime <= 2)
        IntegrationTime = 2;
    if(IntegrationTime >= 8)
        IntegrationTime = 8;
 
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：SendHex
*  功能说明：采集发数程序
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void SendHex(uint8 hex) {
  uint8_t temp;
  temp = hex >> 4;
  if(temp < 10) {
    uart_putchar(UART3,temp + '0');
  } else {
    uart_putchar(UART3,temp - 10 + 'A');
  }
  temp = hex & 0x0F;
  if(temp < 10) {
    uart_putchar(UART3,temp + '0');
  } else {
   uart_putchar(UART3,temp - 10 + 'A');
  }
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：SendImageData
*  功能说明：
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void SendImageData(uint8 NO) {

    uint8 i;
    uint8 crc = 0;
    /* Send Data */
    uart_putchar(UART3,'*');
    uart_putchar(UART3,'L');
    uart_putchar(UART3,'D');

    SendHex(0);
    SendHex(0);
    SendHex(0);
    SendHex(0);
      if(NO==1)
      {
       for(i=0; i<128; i++) 
       {
       SendHex(ccd1.CCD_BUFF[i]);//car.angle
       }
      }
      if(NO==2)
      {
       for(i=0; i<128; i++) 
       {
       SendHex(ccd2.CCD_BUFF[i]);//car.angle
       } 
      }
    SendHex(crc);
    uart_putchar(UART3,'#');
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：SamplingDelay
*  功能说明：CCD延时程序 200ns
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void SamplingDelay(void){
   vuint8 i ;
   for(i=0;i<1;i++)
   {
    asm("nop");
    asm("nop");
   }
}
