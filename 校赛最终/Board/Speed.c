#include  "include.h"
float  Motor_P=30;                          //���PID����ֵ
float  Motor_I=6;
float  Motor_D=0; 
int speed_MAX=9000;
u8 xiaoS=0;
uint8 stop_flag=0;
uint8 KS=0;
uint8 ruwan_flag=0; //�����־
uint8 Straight_flag =0; 
uint8 zhidao_num1=0;
uint8 zhidao_num2=0;
int autoSpeed;
int SUM_L=0;
int SUM_R=0;
u8 count_bmqold_L[3];
u8 count_bmqold_R[3];

void Set_Speed(int16 PWM_L,int16 PWM_R)//���ٶ�
{
  if(PWM_L>0)
    {
      ftm_pwm_duty(FTM0,FTM_CH4,0);
      ftm_pwm_duty(FTM0,FTM_CH5,(u32)(PWM_L));
    }
  else
    {
      ftm_pwm_duty(FTM0,FTM_CH4,(u32)(-PWM_L));
      ftm_pwm_duty(FTM0,FTM_CH5,0);
    }
  if(PWM_R>0)
    {
      ftm_pwm_duty(FTM0,FTM_CH6,(u32)(PWM_R));
      ftm_pwm_duty(FTM0,FTM_CH7,0);
    }
  else
    {
      ftm_pwm_duty(FTM0,FTM_CH6,0);
      ftm_pwm_duty(FTM0,FTM_CH7,(u32)(-PWM_R));
    } 
}

/*************************************************************************
*  �������ƣ�Motor_PID_control
*  ����˵�������PID�㷨����
*  ����˵����uint8 load_type  ��·����
*  �������أ���
*  �޸�ʱ�䣺2012-2-15    �Ѳ���
*  ��    ע�����������ֵľ���Ϊ13.7cm��ǰ���ֵľ���Ϊ19.8cm
*************************************************************************/
int16  L_Speed_err[3]={0};		  //�����ٶ�ƫ��
int16  R_Speed_err[3]={0};	          //�ҵ���ٶ�ƫ��
int16  Speed_err[3]={0};
int16 L_base_speed;                       //������ٶ�
int16 R_base_speed;
int16 L_set_speed;                       //Ԥ����ٶ�
int16 R_set_speed;
int16 L_set_speed_low;                       //Ԥ����ٶ�
int16 R_set_speed_low;
int16 L_set_speed_mid;                       //Ԥ����ٶ�
int16 R_set_speed_mid;
int16 L_set_speed_high;                       //Ԥ����ٶ�
int16 R_set_speed_high;
int16 speed_Lnum;                          //������ʵ���ٶ�
int16 speed_Rnum;                          
float  Angle;                           //�����ǽǶ�
float  tanAngle;                        //���ݽǶ����sinֵ
float CHASU;
int16 real_speed;                        //ʵ�ʵ��ٶ�
uint8 BBCount; //�ж�ֱ������
void Motor_PID_control()     //��ǰհ 80~90 ��ǰհ 45��55
{   
    CHASU=0.6;
    
  //����
   

//   if(ABS(ccd1.Direction_Error_New)<=6)
//   {
//     L_set_speed=L_set_speed_high;
//     R_set_speed=R_set_speed_high;
//   }
//  if(ABS(ccd1.Direction_Error_New)>6&&ABS(ccd2.Direction_Error_New<=15))
//  {
//    L_set_speed=L_set_speed_mid;
//    R_set_speed=R_set_speed_mid;
//  }
//     if(ABS(ccd1.Direction_Error_New)>15)
//     {
//       L_set_speed=L_set_speed_low;
//       R_set_speed=R_set_speed_low;
//     }
  //�ж��Ƿ�Ϊ����ֱ�� 
//  if(ABS(ccd2.Direction_Error_New) <= 5)
//  {
//          BBCount++;
//          if(BBCount>20) // ����
//          {
//                  BBCount = 20;
//          }
//  }
//  else if(ABS(ccd2.Direction_Error_New) <= 10)
//  {
//          if(BBCount)
//          {
//                  BBCount--;
//          }
//          else
//          {
//                  BBCount = 0;
//          }
//  }
//  else //���
//  {
//          if(BBCount>2)
//          {
//                  BBCount -= 6;
//          }
//          else
//          {
//                  BBCount = 0;
//          }
//  }
//  if(BBCount>14)	//��ֱ
//  {
//          L_set_speed = L_set_speed_high;
//          R_set_speed = R_set_speed_high;
//  } 
//  else if(BBCount>9)	//��һ�����
//  {
//         L_set_speed = L_set_speed_mid;
//          R_set_speed = R_set_speed_mid;
//  }
//  else	//����
//  {
//          L_set_speed = L_set_speed_low;
//          R_set_speed = R_set_speed_low;
// }
//          L_set_speed = L_set_speed_high;
//          R_set_speed = R_set_speed_high;
   Speed_control();       
     
  Angle=40.0*((float)(Steer_PWM_New-1535))/180.0; //��ǰǰ�ֽǶ�
  //arm_sin_cos_f32(Angle,&sinAngle,&cosAngle);    //Ϊ�˺�����tan   
  tanAngle=tan(Angle*pi/180);
  // ���������ֵľ���Ϊ13.7cm��ǰ���ֵľ���Ϊ19.8cm                                               
 // if(car.gear4==1)
//if(ccd2.Direction_Error_New<-10)
//{
//  L_set_speed=(int16)(L_set_speed*(1-CHASU*(13.7*tanAngle/39.6)));  
//  R_set_speed=(int16)(R_set_speed*(1+CHASU*(13.7*tanAngle/39.6)));
//      
//}
  if(ccd2.Direction_Error_New>10) //��ת������ȱ�ݣ����ù̶�����
      {
        R_set_speed=0.7*L_set_speed;
        L_set_speed=1.2*L_set_speed;
      }
     if(ccd2.Direction_Error_New<-10)
      {
        L_set_speed=0.7*R_set_speed;
        R_set_speed=1.2*R_set_speed;
      }
   
    real_speed=speed_Lnum;    
    L_Speed_err[2]=L_Speed_err[1];
    L_Speed_err[1]=L_Speed_err[0];
    L_Speed_err[0]=L_set_speed-(int16)(real_speed);   //����������*500/80+190��ת��Ϊռ�ձ�
     real_speed=speed_Rnum; 
    R_Speed_err[2]=R_Speed_err[1];
    R_Speed_err[1]=R_Speed_err[0]; 
    R_Speed_err[0]=R_set_speed-(int16)(real_speed);   //����������*500/79+150��ת��Ϊռ�ձ�
    //��ƫ��Ż���� ʵ���ٶ��������ٶ����ʱ�����ٲ�����
    L_base_speed=L_base_speed+(int16)(Motor_P*(L_Speed_err[0]-L_Speed_err[1])+Motor_I*L_Speed_err[0]+Motor_D*(L_Speed_err[0]-2*L_Speed_err[1]+L_Speed_err[2]));
    R_base_speed=R_base_speed+(int16)(Motor_P*(R_Speed_err[0]-R_Speed_err[1])+Motor_I*R_Speed_err[0]+Motor_D*(R_Speed_err[0]-2*R_Speed_err[1]+R_Speed_err[2]));

      
    //�޷�
//     if(ABS(ccd2.Direction_Error_New)>9&&(speed_Lnum>180&&speed_Rnum>180)&&ABS(ccd1.Direction_Error_New)<7)//��ǰհ�����������ǰհ����ֱ�ߣ�
//    {                                                                                                      //�ұ���������165���ͼ���
//      KS++;
//      L_base_speed=-100*KS; 
//      R_base_speed=-100*KS; // CCD2.ERR:  7~20
//    }
//    else 
//      KS=0;

   if(stop_flag==1)
   {
    L_base_speed=-500;
    R_base_speed=-500;
   }
   if(gpio_get (PTC14)==0) //����
     {
       L_base_speed=4000;
       R_base_speed=4000;
     }
     if(L_base_speed>speed_MAX) L_base_speed=speed_MAX;
    if(L_base_speed<-speed_MAX) L_base_speed=-speed_MAX;
    
    if(R_base_speed>speed_MAX) R_base_speed=speed_MAX;
    if(R_base_speed<-speed_MAX) R_base_speed=(-speed_MAX);
    
     if(speed_Lnum<20&&speed_Rnum<20) 
            {
             Set_Speed(0,0);
              return;
            }         
     
    Set_Speed(L_base_speed,R_base_speed);
}


/*
*  ��������ֵ�˲�
*/
void ffcountbmq_L(void)
{
  u8 i[3];
  int l,m;
  count_bmqold_L[2]=count_bmqold_L[1];
  count_bmqold_L[1]=count_bmqold_L[0];
  count_bmqold_L[0]=ftm_quad_get(FTM1);
  for(int j=0;j<3;j++)
  {
    i[j]=count_bmqold_L[j];
  }
  for(int k=0;k<3;k++)
  {
    for(l=0;l<3-k;l++)
    {
      if(i[l]>i[l+1])
      {
        m=i[l];
        i[l]=i[l+1];
        i[l+1]=m;
      }
    }
  }
  speed_Lnum=i[1];
}
void ffcountbmq_R(void)
{
  u8 i[3];
  int l,m;
  count_bmqold_R[2]=count_bmqold_R[1];
  count_bmqold_R[1]=count_bmqold_R[0];
  count_bmqold_R[0]=-ftm_quad_get(FTM2);
  for(int j=0;j<3;j++)
  {
    i[j]=count_bmqold_R[j];
  }
  for(int k=0;k<3;k++)
  {
    for(l=0;l<3-k;l++)
    {
      if(i[l]>i[l+1])
      {
        m=i[l];
        i[l]=i[l+1];
        i[l+1]=m;
      }
    }
  }
  speed_Rnum=i[1];
}

void BMQ_read(void)  //�����ҵ��������ֵ
{
 // ffcountbmq_L();//����A8A9
 // ffcountbmq_R();//�ҵ��A10A11
    speed_Lnum=ftm_quad_get(FTM1);
    speed_Rnum=-ftm_quad_get(FTM2); 
   // SUM_L+=speed_Lnum;
   // SUM_R+=speed_Rnum;
  ftm_quad_clean(FTM1);
  ftm_quad_clean(FTM1);
  ftm_quad_clean(FTM2);
  ftm_quad_clean(FTM2); 
 
}

void Speed_control(void)                        //�ٶȼ���
{
       int8 piancha;
      piancha=ABS(ccd1.Direction_Error_New);
      if(piancha>40)
        piancha=40; //ƫ���޷� 
      
   //��ccd�ж� && (  ccd1.RLine!=200 && ccd1.LLine!=200 || ccd1.shiziflag==1 ) 
   if(ABS(ccd1.Direction_Error_New) <= 10) //ƫ��С��10�Ҳ�����
   {
      zhidao_num1++;
      if(zhidao_num1>30)
      zhidao_num1=30;


      //�ж�Զccd��Ԫ��&& (ccd2.RLine!=200 && ccd2.LLine!=200 || ccd2.shiziflag==1 ) 
      if(ABS(ccd2.Direction_Error_New) <= 10) //ƫ��С��10�Ҳ�����
      {
          zhidao_num2++;
          if(zhidao_num2>30)
          zhidao_num2=30;
      }
      else
      {
            zhidao_num2=0;
       }
  
   }
   else  //���ccd2������ڣ�����ccd1��
   {
      zhidao_num1=0;   
   }
   
   //�ж�Ԫ��
   //�ж�Ϊֱ��
   if(zhidao_num2>7 && Straight_flag==0) 
   {       
        if(zhidao_num1>7)  //����6���ж�Ϊֱ��
        {
          Straight_flag=1; //ֱ����־ 
          ruwan_flag=0;   //��������    
        }
   }         
   //�ж�Ϊ����
   if(Straight_flag==1 && ruwan_flag==0) //��ֱ�����ǳ���״̬��
   {  
      //Զccd�����������  �� ��ccd�����������
      if(ABS(ccd1.Direction_Error_New) >= 10 && ABS(ccd1.Direction_Error_Old[1]) >=10 || ABS(ccd2.Direction_Error_New) >= 10 && ABS(ccd2.Direction_Error_Old[1]) >= 10)
      { 
        ruwan_flag=1; //�����־
        Straight_flag =0; //ֱ������     
      }
   } 
   if(Straight_flag==0 &&ruwan_flag==1) //����
   {             
     L_set_speed= L_set_speed_mid;
     R_set_speed= R_set_speed_mid;
   }
   
   if(Straight_flag==1 && ruwan_flag==0) //����
   {
     L_set_speed= L_set_speed_high;
     R_set_speed= R_set_speed_high;
   }
   
   if(Straight_flag==0 && ruwan_flag==0) //����
   { 
     L_set_speed= L_set_speed_low;
     R_set_speed= R_set_speed_low;
   }
}



