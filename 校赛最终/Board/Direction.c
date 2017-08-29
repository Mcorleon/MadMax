#include "include.h"

uint8 zhixian=0;
uint8 up_count=0;
uint8 down_count=0;
uint8 up_num;
uint8 down_num;
float DirectionPianCha[3] = {0}; //取三次偏差数组
extern float DirectionPianCha[3];
float Steer_PWM_Old=0;
float Steer_PWM_New=0;
float SteeringP_1 = 6.5;    //比例
float SteeringP_2 = 5.8;
float SteeringP_3 = 7;
float SteeringP_4 = 7.5;
float SteeringD =0;//2.50;     //微分、I是积分



/**********************************************************************
*		
* 函数功能：寻黑线
*
**********************************************************************/

void Find_Black_Line(struct CCD *ccd)
{
  int i;
  ccd->Left_Line_Old=ccd->Left_Line;
  ccd->Right_Line_Old=ccd->Right_Line;
  ccd->Left_Line = 200;
  ccd->Right_Line = 200;
  if(ccd->ccd_threshold==0||ccd->ccd_threshold==255)   //全黑全白退出         
    return;
  /*从两边向中间扫线*/
  if(ccd->CCD_BUFF[10]>ccd->ccd_threshold &&ccd-> CCD_BUFF[115]>ccd->ccd_threshold)          //两边是白，从两边向中间扫
  {
      for(i=1; i<=105; i++)
      {
        if(ccd->CCD_BUFF[i+10] <ccd->ccd_threshold)  //寻找下一个跳变点
        {
          if(ccd->Right_Line == 200)
          ccd->Right_Line = i+10;         //到后记下左线
        }
        if(ccd->CCD_BUFF[115-i] < ccd->ccd_threshold)      //下一个跳变点
        {
          if(ccd->Left_Line == 200)
         ccd-> Left_Line = 115-i;        //到右线
        }
      }
  }
 else   //中间是黑，大弯时太外切会看到全黑，然后又马上看到一根线，此时左右线颠倒了
  {
  /*从中间扫线*/
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
*  函数功能：分析ccd1线条是否合理
*
************************************************************************/
void AnalyzeLine(struct CCD *ccd)
{ 
  if(ccd->Right_Line==200 && ccd->Left_Line !=200)         //只有左线，右拐，如果右拐，偏差是大于0的 
  {
    if((ccd->Right_Line_Old==200 &&ccd-> Left_Line_Old==200) || ccd->Direction_Error_New<-30)
    {
     if(ccd->Direction_Error_New<-30)          //打左急弯时，全丢到看到旁边跑道一根左线，不合理
      {
       ccd-> Left_Line =200;
      }
    }
    ccd->norightflag=0;//丢右线，0是丢，1是没丢 
   
  if(ccd->shiziflag == 1 ||ccd->noleftflag==1 )     //打左弯出十字，看到左线判无效
    ccd->Left_Line =200;

  }
  
  
  
  if(ccd->Right_Line!=200 && ccd->Left_Line ==200)        //只有右线 
  {
    if((ccd->Right_Line_Old==200 &&ccd->Left_Line_Old==200)||ccd-> Direction_Error_New>30)
    {
      if(ccd->Direction_Error_New>30)           //打右急弯时，全丢到看到旁边跑道一根右线，不合理
      {
        ccd->Right_Line=200;
      }  
    }
    ccd->noleftflag=0;
  
    if(ccd->shiziflag==2||ccd->norightflag==1 )              //打右弯进出十字，看到右线判无效
     ccd-> Right_Line =200;
  
  }
  if(ccd->Right_Line!=200 && ccd-> Left_Line!=200)        //两根线
  { 
   
    ccd->shiziflag = 0;
    if(ccd->Right_Line< ccd->Left_Line )//右线小于左线，不正常。可能是斜看十字或全黑进赛道
    {                                        //一般是看到本跑道和隔壁跑道或出十字看到尖角
     {
      if(ccd->Right_Line_Old!=200 && ccd->Left_Line_Old!=200)//上场没有丢线，斜看十字
        {
          
          if((ccd->Right_Line + ccd->Left_Line) < 128)
          {
           ccd->Right_Line=200;//判断为丢右线
            ccd->norightflag=1;
          }
          else     //小于128
          {
            ccd->Left_Line=200;
            ccd->noleftflag=1;
          }
        }
        else  //上场丢线了，全黑进赛道
        {
          if(ccd->Direction_Error_New>0)//丢右线
            ccd->Right_Line=200;
          
          if(ccd->Direction_Error_New<0)//丢左线
             ccd->Left_Line=200;
        }
    }
   }
  }

 if(ccd->Right_Line==200 &&ccd-> Left_Line ==200)           //十字全白丢两根线
  {
      if(ccd->PixelAverageValue >ccd-> ccd_threshold)
    {
     if(ccd->Right_Line_Old!=200 &&ccd-> Left_Line_Old==200)                                //打右弯进十字 
      ccd->shiziflag=1;
    if(ccd->Right_Line_Old==200 &&ccd-> Left_Line_Old!=200)                                //打左弯进十字
      ccd->shiziflag=2;
    if(ccd->Right_Line_Old!=200 && ccd->Left_Line_Old!=200)                                //直道进十字
     ccd-> shiziflag=3;
    }
  }

//  if(ccd->RLine==200 && ccd->LLine!=200)        //只有左线 
//  {
//    if(ccd->RLineOld==200 && ccd->LLineOld==200)
//    {
//      if(ccd->CURRENT_ERROR<-30)          //打左急弯时，全丢到看到旁边跑道一根左线，不合理
//      {
//        ccd->LLine=200;
//      }
//      if(flag_shizi == 1)                 //十字打左弯，看到右线判无效
//        ccd->LLine =200;
//        
//    }
//     }
//      if(ccd->RLine!=200 && ccd->LLine==200)        //只有右线 
//  {
//    if(ccd->RLineOld==200 && ccd->LLineOld==200)
//    {
//      if(ccd->CURRENT_ERROR>30)          //打左急弯时，全丢到看到旁边跑道一根左线，不合理
//      {
//        ccd->RLine=200;
//      }
//      if(flag_shizi == 2)                 //十字打左弯，看到右线判无效
//        ccd->RLine =200;
//        
//    }
//  }
 
   
}
void AnalyzeLine2(struct CCD *ccd)
{
   if(ccd->Right_Line==200 && ccd->Left_Line!=200)        //只有左线，右拐，如果右拐，偏差是大于0的
  {
    if((ccd->Right_Line_Old==200 && ccd->Left_Line_Old==200) ||ccd->Direction_Error_New<-15) //上一场丢左右线或上一场偏差小于-15，大弯
    {
      if(ccd->Direction_Error_New<-15)      //打左急弯时，全丢到看到旁边跑道一根左线，不合理，改小一点更好？
      {
        ccd->Left_Line=200;
      }
    } 
    
    ccd->norightflag=0;  //丢右线，0是丢，1是没丢
  if(ccd->shiziflag == 1 || ccd->noleftflag==1)                //打左弯出十字，看到左线判无效
    ccd->Left_Line =200;
  }  
  
  if(ccd->Right_Line!=200 &&ccd->Left_Line==200)        //只有右线 
  {
    if((ccd->Right_Line_Old==200 && ccd->Left_Line_Old==200)||ccd->Direction_Error_New>15)
    {
      if(ccd->Direction_Error_New>15)            //打右急弯时，全丢到看到旁边跑道一根右线，不合理
      {
        ccd->Right_Line=200;
      }  
    }
    
    ccd->noleftflag=0; //没有左线
    if(ccd->shiziflag==2 || ccd->norightflag == 1)              //打右弯进出十字，看到右线判无效
      ccd->Right_Line =200;
  }
  
  if(ccd->Right_Line==200 && ccd->Left_Line==200)           //丢两根线
  {
      if(ccd->PixelAverageValue > ccd-> ccd_threshold) //全白
      {
        if(ccd->Right_Line_Old!=200 && ccd->Left_Line_Old==200)                                //打右弯进十字 
            ccd->shiziflag=1;
        if(ccd->Right_Line_Old==200 && ccd->Left_Line_Old!=200)                                //打左弯进十字
            ccd->shiziflag=2;
        if(ccd->Right_Line_Old!=200 && ccd->Left_Line_Old!=200)                                //直道进十字
            ccd->shiziflag=3;
      } 
        
  } 
  
  if(ccd->Right_Line!=200 && ccd->Left_Line!=200)        //两根线
  {
    ccd->shiziflag = 0;  //清除十字标志
    if(ccd->Right_Line<ccd->Left_Line ) //右线小于左线，不正常。可能是斜看十字或全黑进赛道
    {                                        //一般是看到本跑道和隔壁跑道或出十字看到尖角
      if(ccd->Right_Line_Old!=200 && ccd->Left_Line_Old!=200) //上场没有丢线，斜看十字
        {
          if((ccd->Right_Line + ccd->Left_Line) < 128) //
          {
            ccd->Right_Line=200;  //判断为丢右线
            ccd->norightflag=1;  
          }
          else            //小于128
          {
            ccd->Left_Line=200;
            ccd->noleftflag=1;
          }
        }
        else  //上场丢线了，全黑进赛道
        {
          if(ccd->Direction_Error_New>0) //丢右线
            ccd->Right_Line=200;
          
          if(ccd->Direction_Error_New<0) //丢左线
            ccd->Left_Line=200;
        }
    }
//    else if(ccd->RLine-ccd->LLine>70)
//    {
//      ccd->RightLine = (ccd->RLine-ccd->LLine)/2 +63;
//      ccd->LeftLine = 63 - (ccd->RLine-ccd->LLine)/2;
//     
//      if(ccd->RightLine<93)
//        ccd->RightLine=93;  //前瞻确定，确定一次不要改 63+60/2=93     
//      
//      if(ccd->LeftLine>33)//前瞻确定，确定一次不要改 63- 60/2=33
//        ccd->LeftLine=33;
//   
//    }
  }
}

/****************************************************************************
*		
* 函数功能：计算当前偏差
*
****************************************************************************/

void Direction_Calculate(struct CCD *ccd)
{
 //    int q=0;    //线宽
     int i=0;
    // s16 p ;
    // u8 q ;
    // STEER.LAST_ERROR=STEER.CURRENT_ERROR;
    // Calculatexielv();
   if(ccd->Right_Line!=200 && ccd->Left_Line!=200)              //有左右线
   {           
      ccd->Direction_Error_New=(ccd->Right_Line+ccd->Left_Line)/2-63;        //记录实际中线位置,算误差值  
      ccd->shiziflag=0;
      ccd->noleftflag=0;
      ccd->norightflag=0;
      uint8 l;
    if(ccd-> Left_Line<ccd-> Right_Line)
    {      
      for(l=ccd-> Left_Line+5;l<=ccd-> Right_Line-10;l++) //检测跳变
      {
        if(ccd->CCD_BUFF[l]-ccd->CCD_BUFF[l+5]>28)
        down_count++;
        if(ccd->CCD_BUFF[l]-ccd->CCD_BUFF[l+5]<-28)
        up_count++;
      }
      up_num=up_count;
      down_num=down_count;
      if((up_count>=1||down_count>=1)&&start_time>19000) //看到发车线 ，停车
    {
      stop_flag=1;
    }
      
    }
      
   }
  if(ccd->Right_Line!=200 &&ccd->Left_Line==200)            //只有右线
    {
      ccd->Direction_Error_New=ccd->Right_Line-ccd->Right_Position;//-LineWidthError; 
      if(ccd->Direction_Error_New>-1)
        ccd->Direction_Error_New=-1;
    }
  
  if(ccd->Right_Line==200 && ccd->Left_Line!=200)             //只有左线
    {
      ccd->Direction_Error_New=ccd->Left_Line-ccd->Left_Position;//+LineWidthError;
      if(ccd->Direction_Error_New<1)
       ccd->Direction_Error_New=1;
    }
   
  if(ccd->Right_Line==200 && ccd->Left_Line==200)           //丢失两根线
    { 
      ccd->Direction_Error_New=ccd->Direction_Error_Old[0];
      if(ccd->shiziflag==0)//非十字，即外切过弯 ，看到全黑 
     {
        if(ccd->Direction_Error_New<0 && ccd->Direction_Error_New>-70)//十字外切时丢一根线到丢两根线偏差会很小，导致票出去
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
   
// if(ABS(ccd->CURRENT_ERROR - ccd->LAST_ERROR[0])>60) //防串道，滤突变,不靠谱---哈哈哈哈哈，别说我是坑
//    { 
//     ccd->CURRENT_ERROR=ccd->LAST_ERROR[0];
//  }
//  ccd->CURRENT_ERROR = (0.1*100*(ccd->LAST_ERROR[0]*1.2+ccd->LAST_ERROR[1]*1 + ccd->LAST_ERROR[2]*0.8)/3 + 0.9*100*ccd->CURRENT_ERROR)/100;
   for(i=9;i>=1;i--)                          //更新偏差
   {
    ccd-> Direction_Error_Old[i] = ccd->Direction_Error_Old[i-1];
   }
   ccd->Direction_Error_Old[0] = ccd->Direction_Error_New;
  
}
void Direction_Calculate2(struct CCD *ccd)
{
   int i=0;
   if(ccd->Right_Line!=200 && ccd->Left_Line!=200)              //有左右线
    {           
      ccd->Direction_Error_New =(ccd->Right_Line+ccd->Left_Line)/2-63;        //记录实际中线位置,算误差值  
      ccd->shiziflag=0;  //清所有标志位
      ccd->noleftflag=0;
      ccd->norightflag=0;
       
    }

  if(ccd->Right_Line!=200 && ccd->Left_Line==200)            //只有右线，左拐。偏差小于0
    {
      ccd->Direction_Error_New=ccd->Right_Line-ccd->Right_Position;//-LineWidthError; 
      if(ccd->Direction_Error_New>-1)                     //限幅
        ccd->Direction_Error_New=-1;
    }
  
  if(ccd->Right_Line==200 && ccd->Left_Line!=200)             //只有左线，偏差大于0
    {
      ccd->Direction_Error_New=ccd->Left_Line-ccd->Left_Position;//+LineWidthError;
      if(ccd->Direction_Error_New<1)                        //限幅
        ccd->Direction_Error_New=1;
    }
   
  if(ccd->Right_Line==200 && ccd->Left_Line==200)           //丢失两根线
    { 
      ccd->Direction_Error_New=ccd->Direction_Error_Old[0];  //保持上一次偏差，即全丢线保持打脚
//      if(ccd->shiziflag==0)                   //非十字，即外切过弯 ，看到全黑  
//      {
//        if(ccd->CURRENT_ERROR<0 && ccd->CURRENT_ERROR>-80)  //有bug，打角会不够，太大打角不连续
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
   
// if(ABS(ccd->CURRENT_ERROR - ccd->LAST_ERROR[0])>60) //防串道，滤突变,不靠谱---哈哈哈哈哈，别说我是坑
//    { 
//     ccd->CURRENT_ERROR=ccd->LAST_ERROR[0];
//  }
   
   
 // ccd->CURRENT_ERROR = (s16)(0.2*100*(ccd->LAST_ERROR[0]*1.2+ccd->LAST_ERROR[1]*1 + ccd->LAST_ERROR[2]*0.8)/3 + 0.8*100*ccd->CURRENT_ERROR)/100;

   
   //  if(ccd->CURRENT_ERROR>50)
//    ccd->CURRENT_ERROR=50;
//  if(ccd->CURRENT_ERROR<-50)
//    ccd->CURRENT_ERROR=-50;
//  
   for(i=9;i>=1;i--)                          //记录偏差
   {
     ccd->Direction_Error_Old[i] = ccd->Direction_Error_Old[i-1];
   }
   ccd->Direction_Error_Old[0] = ccd->Direction_Error_New;
  
   
//       if(ccd->LAST_ERROR[0]*ccd->LAST_ERROR[1]<0&&fabs(ccd->LAST_ERROR[0])<2)//直到消抖
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



