#ifndef _CCD_H
#define _CCD_H

extern unsigned char TimerFlag20ms,IntegrationTime;

extern unsigned int CCD_MAX,CCD_MIN,CCD_AVG;

extern unsigned char PixelAverageVoltage;/* 128�����ص��ƽ����ѹֵ��10�� */
extern int TargetPixelAverageVoltage;/* �趨Ŀ��ƽ����ѹֵ��ʵ�ʵ�ѹ��10�� */
extern int PixelAverageVoltageError;/* Ŀ��ƽ����ѹֵ��ʵ��ֵ��ƫ�ʵ�ʵ�ѹ��10�� */
extern int TargetPixelAverageVoltageAllowError;/* �趨Ŀ��ƽ����ѹֵ�����ƫ�ʵ�ʵ�ѹ��10�� */
typedef struct CCD
{
    uint8 Left_Line;                //�������������
    uint8 Right_Line;
    u8 Left_Line_Old;
    u8 Right_Line_Old;
    uint8 Left_Position;
    uint8 Right_Position;
    u8 CCD_Left;             //��ʼʹ�õĵڼ�����
    u8 CCD_Right;
    u16 ccd_threshold;               //��ֵ
    s16 Direction_Error_New;
    uint8 PixelAverageValue;    //128�����ص��ƽ��ADֵ
    uint8 CCD_BUFF[128];
    s16 Direction_Error_Old[10];	//����ƫ����ʷ;
    u8 CCD_Pixelcnt;   // s32 CCDDmax;
    u8 CCDD ;
    u8 noleftflag;
    u8 norightflag;
    u8 shiziflag;
    uint8 Track_Width; //������ȣ���ǰհ�й�

}CCD;

extern struct CCD ccd1 ;
extern struct CCD ccd2 ;

extern void Integration(void);   //�ع⺯��
extern void CCD_TSL1401(uint8 NO);
extern void CalculateIntegrationTime(void); 
extern void Dly_us(unsigned int us);
extern unsigned char PixelAverage(unsigned char len, unsigned char *data);

void Threshold_Calculation(struct CCD *ccd) ;
void ImageFilter(struct CCD *ccd);
void buxian_init(void);
void SamplingDelay(void);
void SendImageData(uint8 NO) ;
void SendHex(uint8 hex);








#endif 



