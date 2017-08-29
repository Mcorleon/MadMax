#include "include.h"
int zhixianMAX;
int wandaoMAX;
uint8 go_flag=1;
void init()
{ 
  uint8 i;
  ccd1.CCD_Left = 10;
  ccd1.CCD_Right = 115;
  ccd1.CCD_Pixelcnt = 115;
  ccd1.Left_Position = 32;                  //CCD1的标准左线，调整该值可以使偏差连续
  ccd1.Right_Position = 95;
  
  ccd2.CCD_Left = 10;
  ccd2.CCD_Right = 115;
  ccd2.CCD_Pixelcnt = 115;
  ccd2.Left_Position = 32;                  //CCD1的标准左线，调整该值可以使偏差连续
  ccd2.Right_Position = 90;
          //CCD//
  gpio_init (PTE10, GPO,0);//SI
  gpio_init (PTE12, GPO,0);//CLK
  adc_init(ADC0_SE18);//AO
  gpio_init (PTE6, GPO,0);//SI
  gpio_init (PTE8, GPO,0);//CLK
  adc_init(ADC0_SE17);//AO
       //拨码开关//
  gpio_init (PTC14, GPI,0);
  gpio_init (PTC12, GPI,0);
  gpio_init (PTC10, GPI,0);
  gpio_init (PTC8, GPI,0);
        //按键//
  gpio_init (PTB1, GPI,0);
  gpio_init (PTB3, GPI,0);
  gpio_init (PTB5, GPI,0);
  gpio_init (PTB7, GPI,0);
  gpio_init (PTB9, GPI,0);
  pit_init_ms(PIT0,1);//1MS中断 
   LCD_Init();//OLED
   ftm_pwm_init(FTM3, FTM_CH2,100,1535);//舵机E7
          //电机//：
   ftm_pwm_init(FTM0, FTM_CH4,10000, 0);  //左
   ftm_pwm_init(FTM0, FTM_CH5,10000, 0);  
   ftm_pwm_init(FTM0, FTM_CH6,10000, 0);  //右
   ftm_pwm_init(FTM0, FTM_CH7,10000, 0); 
   ftm_quad_init(FTM1);//正交解码
   ftm_quad_init(FTM2); 
   // buxian_init();
    if(gpio_get (PTC12)==0)
     {
        
        L_set_speed_low=155;                       //预设的速度
        R_set_speed_low=155;
        L_set_speed_mid=165;                       //预设的速度
        R_set_speed_mid=165;
        L_set_speed_high=240;                       //预设的速度
        R_set_speed_high=240;
     
      }
  if(gpio_get (PTC10)==0)
     {
        L_set_speed_low=165;                       //预设的速度
        R_set_speed_low=165;
        L_set_speed_mid=175;                       //预设的速度
        R_set_speed_mid=175;
        L_set_speed_high=250;                       //预设的速度
        R_set_speed_high=250;
     }
  if(gpio_get (PTC8)==0)
     {
        L_set_speed_low=170;                       //预设的速度
        R_set_speed_low=170;
        L_set_speed_mid=180;                       //预设的速度
        R_set_speed_mid=180;
        L_set_speed_high=255;                       //预设的速度
        R_set_speed_high=255;
     }
}

  uint8 flag_1=0;
  uint8 flag_Board=0;
  uint8 display_flag=0;
  uint8 level=1;
void PID_display() 
{         
  LCD_P6x8Str(0,0,"Steer:");
  LCD_P6x8Str(37,0,"P=");
  LED_PrintValueF(54,0,SteeringP_1,1);
  LCD_P6x8Str(37,1,"D=");
  LED_PrintValueF(54,1,SteeringD,1); 
  LCD_P6x8Str(0,2,"Speed:");
  LCD_P6x8Str(37,2,"P=");
  LED_PrintValueF(54,2,Motor_P,1);
  LCD_P6x8Str(37,3,"I=");
  LED_PrintValueF(54,3,Motor_I,1);
  LCD_P6x8Str(37,4,"D=");
  LED_PrintValueF(54,4,Motor_D,1);
  LCD_P6x8Str(0,5,"Err1:");
  LED_PrintValueF(25,5, ccd1.Direction_Error_New,1);
  LCD_P6x8Str(60,5,"Err2:");
  LED_PrintValueF(90,5,ccd2.Direction_Error_New,1);
  LCD_P6x8Str(0,6,"Thd1:");
  LED_PrintValueF(28,6,ccd1.ccd_threshold,1);
  LCD_P6x8Str(50,6,"Thd2:");
  LED_PrintValueF(78,6, ccd2.ccd_threshold,1);
 // LCD_P6x8Str(0,7,"Stop:");
 // LED_PrintValueF(30,7,stop_mode,1);
 // LCD_P6x8Str(70,7,"Slow:");
 // LED_PrintValueF(100,7,slow_mode,1);
  LED_PrintValueF(0,7,SUM_L,1);
  LED_PrintValueF(60,7,SUM_R,1);
  
  
   
}
void Other_display()
{
  LCD_P6x8Str(0,0,"BMQ:");
  LED_PrintValueF(30,0,speed_Lnum,1);
   LED_PrintValueF(70,0,speed_Rnum,1);
   LCD_P6x8Str(0,1,"Set:");
   LED_PrintValueF(30,1, L_set_speed,1);
   LED_PrintValueF(70,1, R_set_speed,1);
    LCD_P6x8Str(0,2,"ZKB:");
    LED_PrintValueF(30,2, L_base_speed,1);
    LED_PrintValueF(80,2, R_base_speed,1);
    LCD_P6x8Str(0,3,"up:");
    LED_PrintValueF(20,3, up_num,1);
    LCD_P6x8Str(50,3,"down:");
   LED_PrintValueF(80,3, down_num,1);
    LCD_P6x8Str(0,6,"Line2:");
    LED_PrintValueF(35,6,   ccd2.Left_Line,1);
    LED_PrintValueF(75,6,  ccd2.Right_Line,1);
    LCD_P6x8Str(0,7,"Line1:");
    LED_PrintValueF(35,7,   ccd1.Left_Line,1);
    LED_PrintValueF(75,7,  ccd1.Right_Line,1);
}

void Board_Interaction()
{        
      
     if(gpio_get (PTB7)==0)
      { 
       
           LCD_Fill(0x00); 
        display_flag++;
       if(display_flag>3)
         display_flag=0;
        DELAY_MS(200);
      }
        switch(display_flag)
        {
          case 0:
            {
             PID_display();              
              break;
            } 
          case 1:       
            {
               
              CCD_display(1);
              break;
            }
            case 2:       
            {
              Other_display();             
              break;
            }
               case 3:       
            {
               
              CCD_display(2);
              break;
            }
            
        }
     
     if(gpio_get (PTB1)==0) //选择调试参数
     {
      switch(flag_Board)
      {
        case 0:
          {
            LCD_P6x8Str(100,0,"<");
            LCD_P6x8Str(100,1," ");
            LCD_P6x8Str(100,2," ");
            LCD_P6x8Str(100,3," ");
            LCD_P6x8Str(100,4," ");
            LCD_P6x8Str(118,7," ");
            break;
          }           
        case 1:
          {
            LCD_P6x8Str(100,0," ");
            LCD_P6x8Str(100,1,"<");
            LCD_P6x8Str(100,2," ");
            LCD_P6x8Str(100,3," ");
            LCD_P6x8Str(100,4," ");
            LCD_P6x8Str(118,7," ");
            break;
          }
        case 2:
          {
            LCD_P6x8Str(100,0," ");
            LCD_P6x8Str(100,1," ");
            LCD_P6x8Str(100,2,"<");
            LCD_P6x8Str(100,3," ");
            LCD_P6x8Str(100,4," ");
            LCD_P6x8Str(118,7," ");
            break;
          }
        case 3:
          {
            LCD_P6x8Str(100,0," ");
            LCD_P6x8Str(100,1," ");
            LCD_P6x8Str(100,2," ");
            LCD_P6x8Str(100,3,"<");
            LCD_P6x8Str(100,4," ");
            LCD_P6x8Str(118,7," ");
            break;
          }
        case 4:
          {
            LCD_P6x8Str(100,0," ");
            LCD_P6x8Str(100,1," ");
            LCD_P6x8Str(100,2," ");
            LCD_P6x8Str(100,3," ");
            LCD_P6x8Str(100,4,"<");
            LCD_P6x8Str(118,7," ");
            break;
          }
           case 5:
          {
            LCD_P6x8Str(100,0," ");
            LCD_P6x8Str(100,1," ");
            LCD_P6x8Str(100,2," ");
            LCD_P6x8Str(100,3," ");
            LCD_P6x8Str(100,4," ");
            LCD_P6x8Str(118,7,"<");
            break;
          }
        }
        
        flag_Board++;
        if(flag_Board>5)
          flag_Board=0;
        DELAY_MS(200);
     }
     if(gpio_get (PTB3)==0) //参数+0.1
     {
      switch(flag_Board)
      {
        case 0:
          {
            if(go_flag==1)
            {
             go_flag=0;
             LCD_P6x8Str(118,7," ");
             LCD_P6x8Str(115,7,"GO");             
             DELAY_MS(1500);
             while(speed_Lnum<50&&speed_Rnum<50)
             Set_Speed(5000,5000);
             LCD_P6x8Str(115,7,"  ");
             break;
            }
          }           
        case 1:
          {
            SteeringP_1+=0.1;
            break;
          }
        case 2:
          {
            SteeringD+=0.1;
            break;
          }
        case 3:
          {
            Motor_P+=1;
            break;
          }
        case 4:
          {
            Motor_I+=0.1;
            break;
          }
          case 5:
          {
             Motor_D+=0.1;
            break;
          }
        
      }
      DELAY_MS(200);              
     }
    if(gpio_get (PTB5)==0) //参数-0.1
     {
       switch(flag_Board)
       { 
        case 0:
          {
            Motor_D-=0.1;
            break;
          }           
        case 1:
          {
            SteeringP_1-=0.1;
            break;
          }
        case 2:
          {
            SteeringD-=0.1;
            break;
          }
        case 3:
          {
            Motor_P-=1;
            break;
          }
        case 4:
          {
            Motor_I-=0.1;
            break;
          } 
       }
     
      DELAY_MS(200);              
     }     
    if(gpio_get (PTB9)==0) //选档位
    {
      switch(level)
      {
       case 1:
         {
           stop_mode=0;
           slow_mode=0;
           break;
         }
       case 2:
         {
           stop_mode=1;
           slow_mode=0;
           break;
         }
       case 3:
         {
          stop_mode=1;
           slow_mode=1;
           break;
         }
         case 4:
         {
           stop_mode=0;
           slow_mode=1;
           break;
         }   
        
                
      }
      level++;
      if(level>4)
        level=1;
      DELAY_MS(200);
    }
            
            
    
    
      
}
