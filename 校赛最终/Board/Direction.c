#include "include.h"

uint8 zhixian=0;
uint8 up_count=0;
uint8 down_count=0;
uint8 up_num;
uint8 down_num;
float DirectionPianCha[3] = {0}; //ȡ����ƫ������
extern float DirectionPianCha[3];
float Steer_PWM_Old=0;
float Steer_PWM_New=0;
float SteeringP_1 = 6.5;    //����
float SteeringP_2 = 5.8;
float SteeringP_3 = 7;
float SteeringP_4 = 7.5;
float SteeringD =0;//2.50;     //΢�֡�I�ǻ���



/**********************************************************************
*		
* �������ܣ�Ѱ����
*
**********************************************************************/

void Find_Black_Line(struct CCD *ccd)
{
  int i;
  ccd->Left_Line_Old=ccd->Left_Line;
  ccd->Right_Line_Old=ccd->Right_Line;
  ccd->Left_Line = 200;
  ccd->Right_Line = 200;
  if(ccd->ccd_threshold==0||ccd->ccd_threshold==255)   //ȫ��ȫ���˳�         
    return;
  /*���������м�ɨ��*/
  if(ccd->CCD_BUFF[10]>ccd->ccd_threshold &&ccd-> CCD_BUFF[115]>ccd->ccd_threshold)          //�����ǰף����������м�ɨ
  {
      for(i=1; i<=105; i++)
      {
        if(ccd->CCD_BUFF[i+10] <ccd->ccd_threshold)  //Ѱ����һ�������
        {
          if(ccd->Right_Line == 200)
          ccd->Right_Line = i+10;         //�����������
        }
        if(ccd->CCD_BUFF[115-i] < ccd->ccd_threshold)      //��һ�������
        {
          if(ccd->Left_Line == 200)
         ccd-> Left_Line = 115-i;        //������
        }
      }
  }
 else   //�м��Ǻڣ�����ʱ̫���лῴ��ȫ�ڣ�Ȼ�������Ͽ���һ���ߣ���ʱ�����ߵߵ���
  {
  /*���м�ɨ��*/
    if(ccd->CCD_BUFF[64]>ccd->ccd_threshold)
    {
      for(i=1;i<=50;i++)
      {
        if(ccd->Right_Line==200)
        {
          if(ccd->CCD_BUFF[64+i]<=ccd->ccd_threshold)
           ccd->Right_Line =64+i;
        }
        if(ccd->Left_Line==200)
        {
          if(ccd->CCD_BUFF[64-i]<=ccd->ccd_threshold)
           ccd->Left_Line = 64-i;
        }
      }
    }
    else
    {
      for(i=1;i<=50;i++)
      {
        if(ccd->Left_Line==200)
        {
          if(ccd->CCD_BUFF[64+i]>ccd->ccd_threshold)
         ccd-> Left_Line = 64+i;
        }
        if( ccd->Right_Line==200)
        {
          if(ccd->CCD_BUFF[64-i]>ccd->ccd_threshold)
          ccd->Right_Line= 64-i;
        }
      }
    }
  }  
}

/***********************************************************************
*  
*  �������ܣ�����ccd1�����Ƿ����
*
************************************************************************/
void AnalyzeLine(struct CCD *ccd)
{ 
  if(ccd->Right_Line==200 && ccd->Left_Line !=200)         //ֻ�����ߣ��ҹգ�����ҹգ�ƫ���Ǵ���0�� 
  {
    if((ccd->Right_Line_Old==200 &&ccd-> Left_Line_Old==200) || ccd->Direction_Error_New<-30)
    {
     if(ccd->Direction_Error_New<-30)          //������ʱ��ȫ���������Ա��ܵ�һ�����ߣ�������
      {
       ccd-> Left_Line =200;
      }
    }
    ccd->norightflag=0;//�����ߣ�0�Ƕ���1��û�� 
   
  if(ccd->shiziflag == 1 ||ccd->noleftflag==1 )     //�������ʮ�֣�������������Ч
    ccd->Left_Line =200;

  }
  
  
  
  if(ccd->Right_Line!=200 && ccd->Left_Line ==200)        //ֻ������ 
  {
    if((ccd->Right_Line_Old==200 &&ccd->Left_Line_Old==200)||ccd-> Direction_Error_New>30)
    {
      if(ccd->Direction_Error_New>30)           //���Ҽ���ʱ��ȫ���������Ա��ܵ�һ�����ߣ�������
      {
        ccd->Right_Line=200;
      }  
    }
    ccd->noleftflag=0;
  
    if(ccd->shiziflag==2||ccd->norightflag==1 )              //���������ʮ�֣�������������Ч
     ccd-> Right_Line =200;
  
  }
  if(ccd->Right_Line!=200 && ccd-> Left_Line!=200)        //������
  { 
   
    ccd->shiziflag = 0;
    if(ccd->Right_Line< ccd->Left_Line )//����С�����ߣ���������������б��ʮ�ֻ�ȫ�ڽ�����
    {                                        //һ���ǿ������ܵ��͸����ܵ����ʮ�ֿ������
     {
      if(ccd->Right_Line_Old!=200 && ccd->Left_Line_Old!=200)//�ϳ�û�ж��ߣ�б��ʮ��
        {
          
          if((ccd->Right_Line + ccd->Left_Line) < 128)
          {
           ccd->Right_Line=200;//�ж�Ϊ������
            ccd->norightflag=1;
          }
          else     //С��128
          {
            ccd->Left_Line=200;
            ccd->noleftflag=1;
          }
        }
        else  //�ϳ������ˣ�ȫ�ڽ�����
        {
          if(ccd->Direction_Error_New>0)//������
            ccd->Right_Line=200;
          
          if(ccd->Direction_Error_New<0)//������
             ccd->Left_Line=200;
        }
    }
   }
  }

 if(ccd->Right_Line==200 &&ccd-> Left_Line ==200)           //ʮ��ȫ�׶�������
  {
      if(ccd->PixelAverageValue >ccd-> ccd_threshold)
    {
     if(ccd->Right_Line_Old!=200 &&ccd-> Left_Line_Old==200)                                //�������ʮ�� 
      ccd->shiziflag=1;
    if(ccd->Right_Line_Old==200 &&ccd-> Left_Line_Old!=200)                                //�������ʮ��
      ccd->shiziflag=2;
    if(ccd->Right_Line_Old!=200 && ccd->Left_Line_Old!=200)                                //ֱ����ʮ��
     ccd-> shiziflag=3;
    }
  }

//  if(ccd->RLine==200 && ccd->LLine!=200)        //ֻ������ 
//  {
//    if(ccd->RLineOld==200 && ccd->LLineOld==200)
//    {
//      if(ccd->CURRENT_ERROR<-30)          //������ʱ��ȫ���������Ա��ܵ�һ�����ߣ�������
//      {
//        ccd->LLine=200;
//      }
//      if(flag_shizi == 1)                 //ʮ�ִ����䣬������������Ч
//        ccd->LLine =200;
//        
//    }
//     }
//      if(ccd->RLine!=200 && ccd->LLine==200)        //ֻ������ 
//  {
//    if(ccd->RLineOld==200 && ccd->LLineOld==200)
//    {
//      if(ccd->CURRENT_ERROR>30)          //������ʱ��ȫ���������Ա��ܵ�һ�����ߣ�������
//      {
//        ccd->RLine=200;
//      }
//      if(flag_shizi == 2)                 //ʮ�ִ����䣬������������Ч
//        ccd->RLine =200;
//        
//    }
//  }
 
   
}
void AnalyzeLine2(struct CCD *ccd)
{
   if(ccd->Right_Line==200 && ccd->Left_Line!=200)        //ֻ�����ߣ��ҹգ�����ҹգ�ƫ���Ǵ���0��
  {
    if((ccd->Right_Line_Old==200 && ccd->Left_Line_Old==200) ||ccd->Direction_Error_New<-15) //��һ���������߻���һ��ƫ��С��-15������
    {
      if(ccd->Direction_Error_New<-15)      //������ʱ��ȫ���������Ա��ܵ�һ�����ߣ���������Сһ����ã�
      {
        ccd->Left_Line=200;
      }
    } 
    
    ccd->norightflag=0;  //�����ߣ�0�Ƕ���1��û��
  if(ccd->shiziflag == 1 || ccd->noleftflag==1)                //�������ʮ�֣�������������Ч
    ccd->Left_Line =200;
  }  
  
  if(ccd->Right_Line!=200 &&ccd->Left_Line==200)        //ֻ������ 
  {
    if((ccd->Right_Line_Old==200 && ccd->Left_Line_Old==200)||ccd->Direction_Error_New>15)
    {
      if(ccd->Direction_Error_New>15)            //���Ҽ���ʱ��ȫ���������Ա��ܵ�һ�����ߣ�������
      {
        ccd->Right_Line=200;
      }  
    }
    
    ccd->noleftflag=0; //û������
    if(ccd->shiziflag==2 || ccd->norightflag == 1)              //���������ʮ�֣�������������Ч
      ccd->Right_Line =200;
  }
  
  if(ccd->Right_Line==200 && ccd->Left_Line==200)           //��������
  {
      if(ccd->PixelAverageValue > ccd-> ccd_threshold) //ȫ��
      {
        if(ccd->Right_Line_Old!=200 && ccd->Left_Line_Old==200)                                //�������ʮ�� 
            ccd->shiziflag=1;
        if(ccd->Right_Line_Old==200 && ccd->Left_Line_Old!=200)                                //�������ʮ��
            ccd->shiziflag=2;
        if(ccd->Right_Line_Old!=200 && ccd->Left_Line_Old!=200)                                //ֱ����ʮ��
            ccd->shiziflag=3;
      } 
        
  } 
  
  if(ccd->Right_Line!=200 && ccd->Left_Line!=200)        //������
  {
    ccd->shiziflag = 0;  //���ʮ�ֱ�־
    if(ccd->Right_Line<ccd->Left_Line ) //����С�����ߣ���������������б��ʮ�ֻ�ȫ�ڽ�����
    {                                        //һ���ǿ������ܵ��͸����ܵ����ʮ�ֿ������
      if(ccd->Right_Line_Old!=200 && ccd->Left_Line_Old!=200) //�ϳ�û�ж��ߣ�б��ʮ��
        {
          if((ccd->Right_Line + ccd->Left_Line) < 128) //
          {
            ccd->Right_Line=200;  //�ж�Ϊ������
            ccd->norightflag=1;  
          }
          else            //С��128
          {
            ccd->Left_Line=200;
            ccd->noleftflag=1;
          }
        }
        else  //�ϳ������ˣ�ȫ�ڽ�����
        {
          if(ccd->Direction_Error_New>0) //������
            ccd->Right_Line=200;
          
          if(ccd->Direction_Error_New<0) //������
            ccd->Left_Line=200;
        }
    }
//    else if(ccd->RLine-ccd->LLine>70)
//    {
//      ccd->RightLine = (ccd->RLine-ccd->LLine)/2 +63;
//      ccd->LeftLine = 63 - (ccd->RLine-ccd->LLine)/2;
//     
//      if(ccd->RightLine<93)
//        ccd->RightLine=93;  //ǰհȷ����ȷ��һ�β�Ҫ�� 63+60/2=93     
//      
//      if(ccd->LeftLine>33)//ǰհȷ����ȷ��һ�β�Ҫ�� 63- 60/2=33
//        ccd->LeftLine=33;
//   
//    }
  }
}

/****************************************************************************
*		
* �������ܣ����㵱ǰƫ��
*
****************************************************************************/

void Direction_Calculate(struct CCD *ccd)
{
 //    int q=0;    //�߿�
     int i=0;
    // s16 p ;
    // u8 q ;
    // STEER.LAST_ERROR=STEER.CURRENT_ERROR;
    // Calculatexielv();
   if(ccd->Right_Line!=200 && ccd->Left_Line!=200)              //��������
   {           
      ccd->Direction_Error_New=(ccd->Right_Line+ccd->Left_Line)/2-63;        //��¼ʵ������λ��,�����ֵ  
      ccd->shiziflag=0;
      ccd->noleftflag=0;
      ccd->norightflag=0;
      uint8 l;
    if(ccd-> Left_Line<ccd-> Right_Line)
    {      
      for(l=ccd-> Left_Line+5;l<=ccd-> Right_Line-10;l++) //�������
      {
        if(ccd->CCD_BUFF[l]-ccd->CCD_BUFF[l+5]>28)
        down_count++;
        if(ccd->CCD_BUFF[l]-ccd->CCD_BUFF[l+5]<-28)
        up_count++;
      }
      up_num=up_count;
      down_num=down_count;
      if((up_count>=1||down_count>=1)&&start_time>19000) //���������� ��ͣ��
    {
      stop_flag=1;
    }
      
    }
      
   }
  if(ccd->Right_Line!=200 &&ccd->Left_Line==200)            //ֻ������
    {
      ccd->Direction_Error_New=ccd->Right_Line-ccd->Right_Position;//-LineWidthError; 
      if(ccd->Direction_Error_New>-1)
        ccd->Direction_Error_New=-1;
    }
  
  if(ccd->Right_Line==200 && ccd->Left_Line!=200)             //ֻ������
    {
      ccd->Direction_Error_New=ccd->Left_Line-ccd->Left_Position;//+LineWidthError;
      if(ccd->Direction_Error_New<1)
       ccd->Direction_Error_New=1;
    }
   
  if(ccd->Right_Line==200 && ccd->Left_Line==200)           //��ʧ������
    { 
      ccd->Direction_Error_New=ccd->Direction_Error_Old[0];
      if(ccd->shiziflag==0)//��ʮ�֣������й��� ������ȫ�� 
     {
        if(ccd->Direction_Error_New<0 && ccd->Direction_Error_New>-70)//ʮ������ʱ��һ���ߵ���������ƫ����С������Ʊ��ȥ
        {
//          ccd->Direction_Error_New--;
//          ccd->Direction_Error_New--;
//          ccd->Direction_Error_New--;
//          ccd->Direction_Error_New--;
         ccd->Direction_Error_New=-70;
        }
       if(ccd->Direction_Error_New>0 && ccd->Direction_Error_New<70)
       {
//        ccd->Direction_Error_New++;
//        ccd->Direction_Error_New++;
//        ccd->Direction_Error_New++;
//        ccd->Direction_Error_New++;
        ccd->Direction_Error_New=70;
       }
     }
    }    
   
// if(ABS(ccd->CURRENT_ERROR - ccd->LAST_ERROR[0])>60) //����������ͻ��,������---��������������˵���ǿ�
//    { 
//     ccd->CURRENT_ERROR=ccd->LAST_ERROR[0];
//  }
//  ccd->CURRENT_ERROR = (0.1*100*(ccd->LAST_ERROR[0]*1.2+ccd->LAST_ERROR[1]*1 + ccd->LAST_ERROR[2]*0.8)/3 + 0.9*100*ccd->CURRENT_ERROR)/100;
   for(i=9;i>=1;i--)                          //����ƫ��
   {
    ccd-> Direction_Error_Old[i] = ccd->Direction_Error_Old[i-1];
   }
   ccd->Direction_Error_Old[0] = ccd->Direction_Error_New;
  
}
void Direction_Calculate2(struct CCD *ccd)
{
   int i=0;
   if(ccd->Right_Line!=200 && ccd->Left_Line!=200)              //��������
    {           
      ccd->Direction_Error_New =(ccd->Right_Line+ccd->Left_Line)/2-63;        //��¼ʵ������λ��,�����ֵ  
      ccd->shiziflag=0;  //�����б�־λ
      ccd->noleftflag=0;
      ccd->norightflag=0;
       
    }

  if(ccd->Right_Line!=200 && ccd->Left_Line==200)            //ֻ�����ߣ���ա�ƫ��С��0
    {
      ccd->Direction_Error_New=ccd->Right_Line-ccd->Right_Position;//-LineWidthError; 
      if(ccd->Direction_Error_New>-1)                     //�޷�
        ccd->Direction_Error_New=-1;
    }
  
  if(ccd->Right_Line==200 && ccd->Left_Line!=200)             //ֻ�����ߣ�ƫ�����0
    {
      ccd->Direction_Error_New=ccd->Left_Line-ccd->Left_Position;//+LineWidthError;
      if(ccd->Direction_Error_New<1)                        //�޷�
        ccd->Direction_Error_New=1;
    }
   
  if(ccd->Right_Line==200 && ccd->Left_Line==200)           //��ʧ������
    { 
      ccd->Direction_Error_New=ccd->Direction_Error_Old[0];  //������һ��ƫ���ȫ���߱��ִ��
//      if(ccd->shiziflag==0)                   //��ʮ�֣������й��� ������ȫ��  
//      {
//        if(ccd->CURRENT_ERROR<0 && ccd->CURRENT_ERROR>-80)  //��bug����ǻ᲻����̫���ǲ�����
//        {
//          ccd->CURRENT_ERROR--;
//          ccd->CURRENT_ERROR--;
////          ccd->CURRENT_ERROR--;
////          ccd->CURRENT_ERROR--;
//        }
//        if(ccd->CURRENT_ERROR>0 && ccd->CURRENT_ERROR<80)
//        {
//          ccd->CURRENT_ERROR++;
//          ccd->CURRENT_ERROR++;
////          ccd->CURRENT_ERROR++;
////          ccd->CURRENT_ERROR++;
//        }
//      }
    }    
   
// if(ABS(ccd->CURRENT_ERROR - ccd->LAST_ERROR[0])>60) //����������ͻ��,������---��������������˵���ǿ�
//    { 
//     ccd->CURRENT_ERROR=ccd->LAST_ERROR[0];
//  }
   
   
 // ccd->CURRENT_ERROR = (s16)(0.2*100*(ccd->LAST_ERROR[0]*1.2+ccd->LAST_ERROR[1]*1 + ccd->LAST_ERROR[2]*0.8)/3 + 0.8*100*ccd->CURRENT_ERROR)/100;

   
   //  if(ccd->CURRENT_ERROR>50)
//    ccd->CURRENT_ERROR=50;
//  if(ccd->CURRENT_ERROR<-50)
//    ccd->CURRENT_ERROR=-50;
//  
   for(i=9;i>=1;i--)                          //��¼ƫ��
   {
     ccd->Direction_Error_Old[i] = ccd->Direction_Error_Old[i-1];
   }
   ccd->Direction_Error_Old[0] = ccd->Direction_Error_New;
  
   
//       if(ccd->LAST_ERROR[0]*ccd->LAST_ERROR[1]<0&&fabs(ccd->LAST_ERROR[0])<2)//ֱ������
//   ccd->CURRENT_ERROR=0;
//   if(ccd->LAST_ERROR[0]<15&&ccd->LAST_ERROR[1]>ccd->LAST_ERROR[2]&&ccd->LAST_ERROR[0]>0&&ccd->LAST_ERROR[2]>ccd->LAST_ERROR[3])
//   {if(ccd->LAST_ERROR[0]<ccd->LAST_ERROR[1])
//     ccd->CURRENT_ERROR=ccd->LAST_ERROR[1]+1;  
//   }
//   else if(ccd->LAST_ERROR[0]>-15&&ccd->LAST_ERROR[1]<ccd->LAST_ERROR[2]&&ccd->LAST_ERROR[0]<0&&ccd->LAST_ERROR[2]<ccd->LAST_ERROR[3])
//   {
//      if(ccd->LAST_ERROR[0]>ccd->LAST_ERROR[1])
//        ccd->CURRENT_ERROR=ccd->LAST_ERROR[1]-1;
//   }
//   else if(ccd->LAST_ERROR[0]<15&&ccd->LAST_ERROR[1]<ccd->LAST_ERROR[2]&&ccd->LAST_ERROR[0]>0&&ccd->LAST_ERROR[2]<ccd->LAST_ERROR[3])
//   {
//   if(ccd->LAST_ERROR[0]>ccd->LAST_ERROR[1])
//     ccd->CURRENT_ERROR=ccd->LAST_ERROR[1]-1;
//   }
//   else if(ccd->LAST_ERROR[0]>-15&&ccd->LAST_ERROR[1]>ccd->LAST_ERROR[2]&&ccd->LAST_ERROR[0]<0&&ccd->LAST_ERROR[2]>ccd->LAST_ERROR[3])
//     {if(ccd->LAST_ERROR[0]<ccd->LAST_ERROR[1])
//     ccd->CURRENT_ERROR=ccd->LAST_ERROR[1]+1;  
//   }

}
void Direction_Control()
 {      

//   if(ABS(ccd1.Direction_Error_New)>4 && ABS(ccd1.Direction_Error_New- ccd1.Direction_Error_Old[1])<3)
//      ccd1.Direction_Error_New= ccd1.Direction_Error_Old[1];
   
   if(ABS(ccd1.Direction_Error_New)>=10)
     SteeringD =12;
   else
      SteeringD =0;
     
      
     DirectionPianCha[2] = DirectionPianCha[1];        
     DirectionPianCha[1] = DirectionPianCha[0];
     DirectionPianCha[0] = ccd1.Direction_Error_New;     
    
     
     //AutoP();  
    // if(ABS(Direction_Error_New)<=10)
       
     Steer_PWM_New = 1535-(s16)((SteeringP_1*DirectionPianCha[0])+SteeringD*(DirectionPianCha[0]-DirectionPianCha[1]));
     
   //  if(ABS(Direction_Error_New)<=20 && ABS(Direction_Error_New)>10)   
       
   //  Steer_PWM_New = 1535-(s16)(SteeringP_1 *10 + (SteeringP_2*(DirectionPianCha[0]-10))+SteeringD*(DirectionPianCha[0]-DirectionPianCha[1]));
     
   //  if(ABS(Direction_Error_New)<=30 && ABS(Direction_Error_New)>20)
       
   //  Steer_PWM_New = 1535-(s16)(SteeringP_1 *10 + SteeringP_2 *10 + (SteeringP_3*(DirectionPianCha[0]-20))+SteeringD*(DirectionPianCha[0]-DirectionPianCha[1]));
     
   //  if(ABS(Direction_Error_New)<=40 && ABS(Direction_Error_New)>30)
      
   //  Steer_PWM_New = 1535-(s16)(SteeringP_1 *10 + SteeringP_2 *10 + SteeringP_3 *10 +(SteeringP_4*(DirectionPianCha[0]-30))+SteeringD*(DirectionPianCha[0]-DirectionPianCha[1]));
     
     
    //SteeringZkb = S3010_Duty_MID-(s16)(SteeringP*DirectionPianCha[0]);
//     if(ABS(Steer_PWM_New-Steer_PWM_Old)<5)
//        Steer_PWM_New=Steer_PWM_Old;
     if(Steer_PWM_New>1760)  //zuo
     {
        Steer_PWM_New = 1760;
     }
     if(Steer_PWM_New<1310)   // you
     {
       Steer_PWM_New = 1310;
     }  
    ftm_pwm_duty(FTM3,FTM_CH2,Steer_PWM_New); 
    Steer_PWM_Old=Steer_PWM_New;
 }



