#ifndef	_SPEED_H_
#define _SPEED_H_
#define pi 3.1415926
extern float  Motor_P;                          //���PID����ֵ
extern float  Motor_I;
extern float  Motor_D; 
extern int16 L_set_speed;                       //Ԥ����ٶ�
extern int16 R_set_speed;
extern int16 speed_Lnum;                          //������ʵ���ٶ�
extern int16 speed_Rnum;  
extern int16  L_Speed_err[3];		  //�����ٶ�ƫ��
extern int16  R_Speed_err[3];
extern int16 L_base_speed;                       //������ٶ�
extern int16 R_base_speed;
extern int16 L_set_speed_low;                       //Ԥ����ٶ�
extern int16 R_set_speed_low;
extern int16 L_set_speed_mid;                       //Ԥ����ٶ�
extern int16 R_set_speed_mid;
extern int16 L_set_speed_high;                       //Ԥ����ٶ�
extern int16 R_set_speed_high;
extern int speed_MAX;
extern u8 xiaoS;
extern int SUM_L;
extern int SUM_R;
extern uint8 KS;
extern uint8 stop_flag;
extern float  Angle;                           //�����ǽǶ�
extern float  tanAngle;      
void Set_Speed(int16 PWM_L,int16 PWM_R);
void Motor_PID_control();
void BMQ_read(void) ; //�����ҵ��������ֵ
void ffcountbmq_L(void);
void ffcountbmq_R(void);
void Speed_control(void) ;
#endif