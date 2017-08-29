
#include "include.h"

 unsigned char Cartime = 0;
int start_time = 0;
uint8 SS=0;
uint8 slow_flag=1;
unsigned char stop_mode=0;
unsigned char slow_mode=0;
unsigned char CCD_flag;

void PIT0_IRQHandler() // 每1MS执行1次
{       
  PIT_Flag_Clear(PIT0);       //清中断标志位
     if(go_flag==0&&stop_mode==1)
     start_time++;
     if(start_time>50000)     //起跑后启动停车检测
       start_time=50000;
     Cartime++;
    if(Cartime==1)
    {
       CCD_TSL1401(1);
    }
    if(Cartime==2)
    {
       ImageFilter(&ccd1);//中值滤波
       ccd1.PixelAverageValue = PixelAverage(128,ccd1.CCD_BUFF);
       Threshold_Calculation(&ccd1) ;//计算阈值 
    }
    if(Cartime==3)
    {
        Find_Black_Line(&ccd1);
        AnalyzeLine(&ccd1);
    }
     if(Cartime==4)
    {
        Direction_Calculate(&ccd1);
    }
    
     if(Cartime==5)
    {
       CCD_TSL1401(2);
    }
    if(Cartime==6)
    {
       ImageFilter(&ccd2);//中值滤波
       ccd2.PixelAverageValue = PixelAverage(128,ccd2.CCD_BUFF);
       Threshold_Calculation(&ccd2) ;//计算阈值 
    }
    if(Cartime==7)
    {
        Find_Black_Line(&ccd2);
        AnalyzeLine2(&ccd2);
    }
     if(Cartime==8)
    {
        Direction_Calculate2(&ccd2);
    }
     if(Cartime==9)
    {
        Direction_Control();
    }
    if( Cartime==10)
    {   
       Cartime=0;
       BMQ_read();  //采左右电机编码器值
       Motor_PID_control();
       AUTO_slow();
       down_count=0;
       up_count=0;
       
    }
          
             
} 
void AUTO_slow()
{
//   if(ABS(ccd2.Direction_Error_New)>7&&(speed_Lnum>170&&speed_Rnum>170)&&ABS(ccd1.Direction_Error_New)<7)//高前瞻看到弯道，低前瞻看到直线，
//    {                                                                                                      //且编码器大于165，就减速
//      SS++;
//      Set_Speed(0,0);
//      ftm_pwm_duty(FTM0,FTM_CH4,2000*SS);
//      ftm_pwm_duty(FTM0,FTM_CH7,2000*SS);
//    }
//    else
//      SS=0;   
if(slow_mode==0)
{
 if(speed_Lnum>200&&speed_Rnum>200)
   slow_flag=1;
 if(speed_Lnum<180&&speed_Rnum<180)
   slow_flag=0;
}
 if(slow_flag==1)
 {
   if(ABS(ccd2.Direction_Error_New)>9&&(speed_Lnum>175&&speed_Rnum>175)&&ABS(ccd1.Direction_Error_New)<7)//高前瞻看到弯道，低前瞻看到直线，
    {                                                                                                      //且编码器大于165，就减速
      KS++;
      Set_Speed(0,0);
      L_base_speed=-500*KS; 
      R_base_speed=-500*KS; // CCD2.ERR:  7~20
      // R_base_speed=-ABS(ccd2.Direction_Error_New)*ABS(ccd2.Direction_Error_New)*KS;
      // L_base_speed=-ABS(ccd2.Direction_Error_New)*ABS(ccd2.Direction_Error_New)*KS;
      Set_Speed(L_base_speed,R_base_speed);
      
    }
   else
     KS=0;
 }
}

               

