#ifndef _CCD_H
#define _CCD_H

extern unsigned char TimerFlag20ms,IntegrationTime;

extern unsigned int CCD_MAX,CCD_MIN,CCD_AVG;

extern unsigned char PixelAverageVoltage;/* 128个像素点的平均电压值的10倍 */
extern int TargetPixelAverageVoltage;/* 设定目标平均电压值，实际电压的10倍 */
extern int PixelAverageVoltageError;/* 目标平均电压值与实际值的偏差，实际电压的10倍 */
extern int TargetPixelAverageVoltageAllowError;/* 设定目标平均电压值允许的偏差，实际电压的10倍 */
typedef struct CCD
{
    uint8 Left_Line;                //计算出来的左线
    uint8 Right_Line;
    u8 Left_Line_Old;
    u8 Right_Line_Old;
    uint8 Left_Position;
    uint8 Right_Position;
    u8 CCD_Left;             //开始使用的第几个点
    u8 CCD_Right;
    u16 ccd_threshold;               //阈值
    s16 Direction_Error_New;
    uint8 PixelAverageValue;    //128个像素点的平均AD值
    uint8 CCD_BUFF[128];
    s16 Direction_Error_Old[10];	//方向偏差历史;
    u8 CCD_Pixelcnt;   // s32 CCDDmax;
    u8 CCDD ;
    u8 noleftflag;
    u8 norightflag;
    u8 shiziflag;
    uint8 Track_Width; //赛道宽度，与前瞻有关

}CCD;

extern struct CCD ccd1 ;
extern struct CCD ccd2 ;

extern void Integration(void);   //曝光函数
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



