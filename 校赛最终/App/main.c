

#include "include.h"



void  main(void)
{
         
     init() ;//ȫ����ʼ��
     set_vector_handler(PIT0_VECTORn ,PIT0_IRQHandler);  
      EnableInterrupts;
      enable_irq (PIT0_IRQn);//����pit0�ж�
     

     while(1)
     {  
      
       Board_Interaction();//������
      
     //  SendImageData(1); //����
       
//       vcan_sendccd((uint8 *)&CCD_BUFF,128); //ɽ��
//       if(L_base_speed>0)
//       {
      // printf("���������壺 ��  %d               ��  %d\n ",speed_Lnum,speed_Rnum);
      // printf("ռ�ձȣ�     ��  %d            ��  %d\n",L_base_speed,R_base_speed);
      //   printf("���������壺 ��  %d               ��  %d\n ",SUM_L,SUM_R);
//       }    
     }
}






