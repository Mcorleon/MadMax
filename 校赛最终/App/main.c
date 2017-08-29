

#include "include.h"



void  main(void)
{
         
     init() ;//È«²¿³õÊ¼»¯
     set_vector_handler(PIT0_VECTORn ,PIT0_IRQHandler);  
      EnableInterrupts;
      enable_irq (PIT0_IRQn);//Æô¶¯pit0ÖĞ¶Ï
     

     while(1)
     {  
      
       Board_Interaction();//¼ì²â¼üÅÌ
      
     //  SendImageData(1); //À¶Öæ
       
//       vcan_sendccd((uint8 *)&CCD_BUFF,128); //É½Íâ
//       if(L_base_speed>0)
//       {
      // printf("±àÂëÆ÷Âö³å£º ×ó  %d               ÓÒ  %d\n ",speed_Lnum,speed_Rnum);
      // printf("Õ¼¿Õ±È£º     ×ó  %d            ÓÒ  %d\n",L_base_speed,R_base_speed);
      //   printf("±àÂëÆ÷Âö³å£º ×ó  %d               ÓÒ  %d\n ",SUM_L,SUM_R);
//       }    
     }
}






