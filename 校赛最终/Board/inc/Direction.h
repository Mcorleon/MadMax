#ifndef _DIRECTION_H
#define _DIRECTION_H

extern float Steer_PWM_Old;
extern float Steer_PWM_New;
extern float DirectionPianCha[3];
extern float Steer_PWM_Old;
extern float Steer_PWM_New;
extern uint8 zhixian;
extern uint8 up_count;
extern uint8 down_count;
extern uint8 up_num;
extern uint8 down_num;
extern float SteeringP_1;    //比例
extern float SteeringP_2;
extern float SteeringP_3;
extern float SteeringP_4;
extern float SteeringD ;//2.50;     //微分、I是积分
extern float SteeringP2;   
extern void Find_Black_Line(struct CCD *ccd);
extern void AnalyzeLine2(struct CCD *ccd);
extern void AnalyzeLine(struct CCD *ccd);
extern void Direction_Calculate(struct CCD *ccd);
extern void Direction_Calculate2(struct CCD *ccd);
extern void Direction_Control(void);
extern void Steer(void);  //舵机打脚


#endif