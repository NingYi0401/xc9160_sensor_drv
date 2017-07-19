
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <asm/io.h>
//#include <asm/system.h>  
#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"
#include "imx298mipiyuv_Sensor.h"
#include "imx298mipiyuv_Camera_Sensor_para.h"
#include "imx298mipiyuv_CameraCustomized.h" 
#include "xc9160yuv_sensor_config.h"
#define IMX298MIPIYUV_DEBUG
#ifdef IMX298MIPIYUV_DEBUG
//#define IMX298MIPISENSORDB printk
#define IMX298MIPISENSORDB(a,arg... ) printk("[XC9160]" a,##arg)
#define TV_SWITCH_PRINT(a,arg... ) printk("[TV_SWITCH/XC9160]" a,##arg)


#else
#define IMX298MIPISENSORDB(x,...)
#define TV_SWITCH_PRINT(x,...)

#endif



// GM7150, by zhangxueping
//extern UINT32 GM7150_On(bool force);
//extern struct mutex TV_switch_lock;

#define CAMERA_INIT_NONE              0
#define CAMERA_INIT_ONCE              1
#define CAMERA_INIT_ALWAYS           2





// VGA size, by zhangxueping
#undef IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH
#undef IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT
#define IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH       4208 //1920 //4608    //640
#define IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT      3120 //1080     //480
#undef IMAGE_SENSOR_PV_WIDTH
#undef IMAGE_SENSOR_PV_WIDTH
#define IMAGE_SENSOR_PV_WIDTH					  2080
#define IMAGE_SENSOR_PV_HEIGHT					  1536
//#define IMX298MIPI_SENSOR_ID IMX298MIPI_SENSOR_ID

//#define mutex_lock(...)
//#define mutex_unlock(...)





//bool is_KERN_EMERG = false;

bool isCameraOpen = false;

static DEFINE_SPINLOCK(IMX298mipi_drv_lock);
static MSDK_SCENARIO_ID_ENUM CurrentScenarioId = MSDK_SCENARIO_ID_CAMERA_PREVIEW;
extern int iReadReg(u16 a_u2Addr , u8 * a_puBuff , u16 i2cId);
extern int iWriteReg(u16 a_u2Addr , u32 a_u4Data , u32 a_u4Bytes , u16 i2cId);

extern int iReadRegI2C(u8 *a_pSendData, u16 a_sizeSendData, u8 *a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData, u16 a_sizeSendData, u16 i2cId);

#define IMX298MIPI_write_cmos_sensor_8(addr, para) iWriteReg((u16) addr , (u32) para ,1,IMX298MIPI_WRITE_ID)
#define IMX298MIPI_write_cmos_sensor(addr, para) iWriteReg((u16) addr , (u32) para ,2,IMX298MIPI_WRITE_ID)
#define OV5675MIPI_write_cmos_sensor(addr, para) iWriteReg((u16) addr , (u32) para ,1,OV5675MIPI_WRITE_ID)
//#define IMX298MIPI_write_cmos_sensor(addr, para) do {iWriteReg((u16) addr , (u32) para ,1,IMX298MIPI_WRITE_ID); IMX298MIPISENSORDB("IMX298MIPI_write, addr=%x\n", addr);	}while(0)


static void XC9160InitialSetting(void);
extern	int Af_VCOMA2_Power_On(void);

static void XC9160_write_cmos_sensor(kal_uint16 addr, kal_uint32 para)
{
	//IMX298MIPISENSORDB("[XC9160RegWrite,0411.1.3] addr:%d  value:%x\n",addr,para);  		
	iWriteReg((u16) addr , (u32) para ,1,XC9160_WRITE_ID); 
}

static kal_uint16 XC9160_read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte=0;
	
    iReadReg((u16) addr ,(u8*)&get_byte,XC9160_WRITE_ID);

	//IMX298MIPISENSORDB("XC9160_read_cmos_sensor addr=%x, get_byte=%x\n", addr,get_byte);		 // by zhangxueping
	
    return get_byte;
}
/*
static kal_uint16 OV5675_read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte=0;
    iReadReg((u16) addr ,(u8*)&get_byte,OV5675MIPI_READ_ID);
	return get_byte;
}
*/
#define mDELAY(ms)  mdelay(ms)

typedef enum
{
    PRV_W=8416,
    PRV_H=3120
}PREVIEW_VIEW_SIZE;
kal_uint16 IMX298MIPIYUV_read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte=0;

	

    iReadReg((u16) addr ,(u8*)&get_byte,IMX298MIPI_WRITE_ID);
	
	//IMX298MIPISENSORDB("IMX298MIPIYUV_read_cmos_sensor addr=%x, get_byte=%x\n", addr, get_byte);	 // by zhangxueping
	
    return get_byte;
}


int read_sensor_mode(MUINT8* dual_mode)
{
	IMX298MIPISENSORDB("MCCREE:dual_mode = %d ", *dual_mode);

	return 0;
}
#if 0
static struct
{
    //kal_uint8   Banding;
    kal_bool      NightMode;
    kal_bool      VideoMode;
    kal_uint16  Fps;
    kal_uint16  ShutterStep;
    kal_uint8   IsPVmode;
    kal_uint32  PreviewDummyPixels;
    kal_uint32  PreviewDummyLines;
    kal_uint32  CaptureDummyPixels;
    kal_uint32  CaptureDummyLines;
    kal_uint32  PreviewPclk;
    kal_uint32  CapturePclk;
    kal_uint32  ZsdturePclk;
    kal_uint32  PreviewShutter;
    kal_uint32  PreviewExtraShutter;
    kal_uint32  SensorGain;
    kal_bool        manualAEStart;
    kal_bool        userAskAeLock;
       kal_bool     userAskAwbLock;
    kal_uint32      currentExposureTime;
       kal_uint32      currentShutter;
    kal_uint32      currentextshutter;
       kal_uint32      currentAxDGain;
    kal_uint32      sceneMode;
    unsigned char isoSpeed;
    unsigned char zsd_flag;
    IMX298MIPI_SENSOR_MODE SensorMode;
} IMX298MIPISensor;

#endif

/* Global Valuable */
MSDK_SENSOR_CONFIG_STRUCT IMX298MIPISensorConfigData;

#define IMX298_TEST_PATTERN_CHECKSUM (0x39ff61ce)  
//#define IMX298_TEST_PATTERN_CHECKSUM (0x3b969447)  

//CRC_accum 0x3b969447


// by zhangxueping
//#define IMX298MIPI_SENSOR_ID   IMX298_SENSOR_ID 		   

//static int saturation_level=ISP_SAT_MIDDLE;          // by zhangxueping


void IMX298MIPIGetExifInfo(UINT32 exifAddr)
{
#if 0 //brat
     IMX298MIPISENSORDB("[IMX298MIPI]enter IMX298MIPIGetExifInfo function\n");
    SENSOR_EXIF_INFO_STRUCT* pExifInfo = (SENSOR_EXIF_INFO_STRUCT*)(uintptr_t)exifAddr;
    pExifInfo->FNumber = 20;
    //pExifInfo->AEISOSpeed = IMX298MIPISensor.isoSpeed;
    pExifInfo->FlashLightTimeus = 0;
    //pExifInfo->RealISOValue = IMX298MIPISensor.isoSpeed;
    IMX298MIPISENSORDB("[IMX298MIPI]exit IMX298MIPIGetExifInfo function\n");
#endif
}

// add by zhangxueping
UINT32 IMX298SetTestPatternMode(kal_bool bEnable)
{
    IMX298MIPISENSORDB("[IMX298MIPI]enter IMX298SetTestPatternMode 0331\n");
    
#if 0
    if(bEnable)
    {
		XC9160_write_cmos_sensor(0xfffd ,0x80);
		XC9160_write_cmos_sensor(0xfffe,0x50);
		XC9160_write_cmos_sensor(0x004d ,0x01);
		IMX298MIPI_write_cmos_sensor(0x503D, 0xa0);
		XC9160_write_cmos_sensor(0x004d ,0x00);
    }
    else
    {
		XC9160_write_cmos_sensor(0xfffd ,0x80);
		XC9160_write_cmos_sensor(0xfffe,0x50);
		XC9160_write_cmos_sensor(0x004d ,0x01);
		IMX298MIPI_write_cmos_sensor(0x503D, 0x00);
		XC9160_write_cmos_sensor(0x004d ,0x00);

    } 
#else

	if(bEnable)
	{
		XC9160_write_cmos_sensor(0xfffd ,0x80);
		XC9160_write_cmos_sensor(0xfffE ,0x06); 
		XC9160_write_cmos_sensor(0x3022 ,0x82);
		XC9160_write_cmos_sensor(0xfffE ,0x21); 
		XC9160_write_cmos_sensor(0x0000 ,0x01);
		XC9160_write_cmos_sensor(0x0001 ,0x00);
		XC9160_write_cmos_sensor(0x0003 ,0x20);
		XC9160_write_cmos_sensor(0xfffe,0x50);
		XC9160_write_cmos_sensor(0x004d ,0x01);
		IMX298MIPI_write_cmos_sensor(0x503D, 0xa0);
		XC9160_write_cmos_sensor(0x004d ,0x00);
	}
	else
	{
		XC9160_write_cmos_sensor(0xfffd ,0x80);
		XC9160_write_cmos_sensor(0xfffE ,0x06); 
		XC9160_write_cmos_sensor(0x3022 ,0x80);
		XC9160_write_cmos_sensor(0xfffE ,0x21); 
		XC9160_write_cmos_sensor(0x0000 ,0x47);
		XC9160_write_cmos_sensor(0x0001 ,0x82);
		XC9160_write_cmos_sensor(0x0003 ,0xb1);
		XC9160_write_cmos_sensor(0xfffe,0x50);
		XC9160_write_cmos_sensor(0x004d ,0x01);
		IMX298MIPI_write_cmos_sensor(0x503D, 0x00);
		XC9160_write_cmos_sensor(0x004d ,0x00);

	}


#endif
    return ERROR_NONE;
}


//#define IMX298_REG_READBACK

#if defined(IMX298_REG_READBACK)

#define READ_PERIOD_S       2
    // 2 second

static struct hrtimer imx298_readback_timer;
static   bool isIMX298HrtimerStart= KAL_FALSE;

static struct work_struct readback_work;
static struct workqueue_struct * readback_workqueue = NULL;
enum hrtimer_restart imx298_read_hrtimer_func(struct hrtimer *timer);

static void reg_readback(void);

static void imx298_read_detect_hrtimer_init(void)
{
    ktime_t ktime;


    IMX298MIPISENSORDB("imx298_read_detect_hrtimer_init\n" );

	if(!readback_workqueue)
	{
		readback_workqueue = create_singlethread_workqueue("imx298_read_det");
		if(!readback_workqueue) {
			IMX298MIPISENSORDB("Unable to create workqueue\n");
			return -ENODATA;
		}
		INIT_WORK(&readback_work, reg_readback);
	}	

    ktime = ktime_set(READ_PERIOD_S, 0);	// 3s, 10* 1000 ms
    hrtimer_init(&imx298_readback_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    imx298_readback_timer.function = imx298_read_hrtimer_func;    
    hrtimer_start(&imx298_readback_timer, ktime, HRTIMER_MODE_REL);

	
}

enum hrtimer_restart imx298_read_hrtimer_func(struct hrtimer *timer)
{
    ktime_t ktime;

    IMX298MIPISENSORDB("imx298_read_hrtimer_func, by zxp \n" );
	
	queue_work(readback_workqueue, &readback_work);
	
	ktime = ktime_set(READ_PERIOD_S, 0);	
	hrtimer_start(&imx298_readback_timer, ktime, HRTIMER_MODE_REL);
	
    return HRTIMER_NORESTART;		

}
#endif  //  IMX298_REG_READBACK

void reg_readback(void)
{
	//int ret=0;
	//int var1,var2,var3,var4;
	//int gain = 0;
  //int night_mode = 0;
	//printk(KERN_WARNING"[XC9160_REG]reg_read_back\n ");  

	/*
    XC9160_write_cmos_sensor(0xfffe ,0x14);
	  ret=XC9160_read_cmos_sensor(0x0002);
    printk(KERN_WARNING"[XC9160_REG]AE_ON,0x0002=%x\n ",ret);      //AE, on/off
    
	ret=XC9160_read_cmos_sensor(0x0026);
    printk(KERN_WARNING"[XC9160_REG]LINE,0x0026=%x\n ",ret);      // exposure line, max 0x3c
    
	 gain = XC9160_read_cmos_sensor(0x002b);
	printk(KERN_WARNING"[XC9160_REG]GAIN,0x002b=%x\n ",ret);     //gain 0x10~0x78(0x0037)
    
	ret=XC9160_read_cmos_sensor(0x003c);
    printk(KERN_WARNING"[XC9160_REG]LUM,0x003c=%x\n ",ret);     // lum value
    
	ret=XC9160_read_cmos_sensor(0x0022);
    printk(KERN_WARNING"[XC9160_REG]0x0022=%x\n ",ret);    // 0x00/0x01
   
   if((gain > 0x70) && (night_mode == 0))
   	{    
   		printk(KERN_WARNING"[XC9160_REG]------ inter night mode" );    // 0x00/0x01

   		XC9160_write_cmos_sensor(0xfffe,0x14); 
      XC9160_write_cmos_sensor(0x003d,0x50); 
      XC9160_write_cmos_sensor(0xfffe,0x21); 
      XC9160_write_cmos_sensor(0x031a,0x00); 
      XC9160_write_cmos_sensor(0x031b,0x10); 
      XC9160_write_cmos_sensor(0x031c,0x00); 
      XC9160_write_cmos_sensor(0x031d,0x00); 
      XC9160_write_cmos_sensor(0x031e,0x14); 
      XC9160_write_cmos_sensor(0x031f,0x08); 
      XC9160_write_cmos_sensor(0x0320,0x06); 
      XC9160_write_cmos_sensor(0x0321,0x08); 

   		
   		 night_mode = 1;
   		}
   else if(gain < 0x40 && night_mode == 1)
   	{
   		printk(KERN_WARNING"[XC9160_REG]------ inter day mode" );    // 0x00/0x01

      XC9160_write_cmos_sensor(0xfffe,0x14); 
      XC9160_write_cmos_sensor(0x003d,0x60); 
      XC9160_write_cmos_sensor(0xfffe,0x21); 
      XC9160_write_cmos_sensor(0x031a,0x00); 
      XC9160_write_cmos_sensor(0x031b,0x00); 
      XC9160_write_cmos_sensor(0x031c,0x00); 
      XC9160_write_cmos_sensor(0x031d,0x00); 
      XC9160_write_cmos_sensor(0x031e,0x10); 
      XC9160_write_cmos_sensor(0x031f,0x0f); 
      XC9160_write_cmos_sensor(0x0320,0x06); 
      XC9160_write_cmos_sensor(0x0321,0x08); 

   		
   		night_mode = 0;
   		}
   */
 	//mutex_unlock(&TV_switch_lock);
	//is_KERN_EMERG = false;
	
    //return ERROR_NONE ;
    
}	

//#endif  //  IMX298_REG_READBACK


/*************************************************************************
* FUNCTION
*   IMX298MIPI_GetSensorID
*
* DESCRIPTION
*   This function get the sensor ID
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
#if 0
kal_uint32 IMX298MIPI_GetSensorID_dummy(kal_uint32 *sensorID)
{
	IMX298MIPISENSORDB("IMX298MIPI_GetSensorID: FAIL, ONLY FOR DEBUG");
	*sensorID = 0xFFFFFFFF;
	return ERROR_SENSOR_CONNECT_FAIL;

    //*sensorID=IMX298MIPI_SENSOR_ID;
	//IMX298MIPISENSORDB("[IMX298MIPI] IMX298MIPI_GetSensorID: OK, only for test");
   //return ERROR_NONE; 	
}
#endif
//static kal_uint32 gSensorID = 0;

kal_uint32 IMX298MIPI_GetSensorID(kal_uint32 *sensorID)
{
    //volatile signed char i;
   // kal_uint32 sensor_id=0;
    //kal_uint8 temp_sccb_addr = 0;
    kal_uint32 isp_id[4]={0};
    //kal_uint32 j;

    //IMX298MIPISENSORDB("IMX298MIPI_GetSensorID: ZHENGFENG_0320\n ");
    IMX298MIPISENSORDB("IMX298MIPI_GetSensorID-4: 150618\n ");
	
	isp_id[0]=XC9160_read_cmos_sensor(0xfffb);
    printk("xc9160 isp_id[0]=%x  should eq 0x71\n",isp_id[0]);
    //mDELAY(10);
    XC9160_write_cmos_sensor(0xfffd ,0x80);
    XC9160_write_cmos_sensor(0xfffe,0x50);
    //XC9160_write_cmos_sensor(0x004D ,0x00);
    mDELAY(10);


   #if 0

	for (j=0;j<4;j++)
    {
        isp_id[j]=XC9160_read_cmos_sensor(j);
        IMX298MIPISENSORDB("HZTCT : XC9160_ID[%d]=0x%x \n",j,isp_id[j]);       // 0x01, 0x11, 0x43, 0x58
    }
   
	if ( (isp_id[0]==0x01) && (isp_id[1]==0x11) && (isp_id[2]==0x43) && (isp_id[3]==0x58) )
	{		
		 IMX298MIPISENSORDB("HZTCT : start get img sensor id ");  
	    mDELAY(10);
		XC9160InitialSetting();


	  	XC9160_write_cmos_sensor(0xfffe,0x50);
		//XC9160_write_cmos_sensor(0x004d,0x1); //  I2C bypass
		XC9160_write_cmos_sensor(0x0050,0x0f);  //  PWDN,GPIO1,GPIO0 en   
		XC9160_write_cmos_sensor(0x0054,0x0f);  //  PWDN,GPIO1,ouput 控制sensorPWDN&reset ; GPIO0 被MTK6572来控制XC9160 PWDN

		XC9160_write_cmos_sensor(0x0058,0x00); //   将GPIO1（sensor reset）先拉低
		mDELAY(5);
		XC9160_write_cmos_sensor(0x0058,0x0a); //    将GPIO1（sensor reset）再拉高 PWDN?
		mDELAY(5);

    XC9160_write_cmos_sensor(0xfffd ,0x80);
    XC9160_write_cmos_sensor(0xfffe,0x50);
    XC9160_write_cmos_sensor(0x004d ,0x01);   //by Nathan 20150719
		//IMX298MIPISENSORDB("IMX298MIPI_GetSensorID\n");    // by zhangxueping
	    //for(i=0;i<3;i++)
	    {
	    	// by zhangxueping
	        sensor_id = (IMX298MIPIYUV_read_cmos_sensor(0x0016) << 8) | IMX298MIPIYUV_read_cmos_sensor(0x0017);
			
	        IMX298MIPISENSORDB("HZTCT : IMX298MIPI_GetSensorID: ID=%x\n",sensor_id);
			
			//sensor_id = IMX298MIPI_SENSOR_ID;   // by zhangxueping
				
	        if(sensor_id != IMX298MIPI_SENSOR_ID)
	        {   
	            *sensorID =0xffffffff;
				IMX298MIPISENSORDB("IMX298MIPI_GetSensorID: FAIL");
	            //return ERROR_SENSOR_CONNECT_FAIL;
				
	        }
	        else
	        {
	            *sensorID=IMX298MIPI_SENSOR_ID;
	                //break;
	        }
	    }

	}
	#else
	*sensorID=XC9160IMX298_SENSOR_ID;
	#endif

	XC9160_write_cmos_sensor(0xfffe,0x50);
    XC9160_write_cmos_sensor(0x004d,0x0); // 

	// add by zhangxueping
	 
	// GM7150, by zhangxueping
	//gIMX298_has_SensorID = 1;
	
	IMX298MIPISENSORDB("HZTCT444 : [IMX298MIPI]exit IMX298MIPI_GetSensorID \n ");
   return ERROR_NONE;    

}   


/*************************************************************************
* FUNCTION
*    IMX298MIPIInitialSetting
*
* DESCRIPTION
*    This function initialize the registers of CMOS sensor.
*
* PARAMETERS
*    None
*
* RETURNS
*    None
*
* LOCAL AFFECTED
*
*************************************************************************/
 static    void XC9160InitialSetting(void)
{
    //mipi_2lane, MCLK=24M , mipi=768M
    IMX298MIPISENSORDB("XC9160_ISP_InitialSetting start \n");

XC9160_write_cmos_sensor(0xfffd,0x80);
XC9160_write_cmos_sensor(0xfffe,0x50);
XC9160_write_cmos_sensor(0x001c,0xff);       
XC9160_write_cmos_sensor(0x001d,0xff);  
XC9160_write_cmos_sensor(0x001e,0xff);  
XC9160_write_cmos_sensor(0x001f,0xff);  
XC9160_write_cmos_sensor(0x0018,0x00);         
XC9160_write_cmos_sensor(0x0019,0x00);  
XC9160_write_cmos_sensor(0x001a,0x00);  
XC9160_write_cmos_sensor(0x001b,0x00);
XC9160_write_cmos_sensor(0x00bc,0x19);

XC9160_write_cmos_sensor(0x0030,0x44);
XC9160_write_cmos_sensor(0x0031,0x58);
XC9160_write_cmos_sensor(0x0032,0x34);  
XC9160_write_cmos_sensor(0x0033,0x70);   

XC9160_write_cmos_sensor(0x0020,0x02);
XC9160_write_cmos_sensor(0x0021,0x0d);
XC9160_write_cmos_sensor(0x0022,0x01);
XC9160_write_cmos_sensor(0x0023,0x86);
XC9160_write_cmos_sensor(0x0024,0x0e);
XC9160_write_cmos_sensor(0x0025,0x05); 
XC9160_write_cmos_sensor(0x0026,0x01);
XC9160_write_cmos_sensor(0x0027,0x06);
XC9160_write_cmos_sensor(0x0028,0x01);
XC9160_write_cmos_sensor(0x0029,0x00);
XC9160_write_cmos_sensor(0x002a,0x02);
XC9160_write_cmos_sensor(0x002b,0x05);

XC9160_write_cmos_sensor(0xfffe,0x50);
XC9160_write_cmos_sensor(0x0050,0x0f);  
XC9160_write_cmos_sensor(0x0054,0x0f);  
XC9160_write_cmos_sensor(0x0058,0x00);
XC9160_write_cmos_sensor(0x0058,0x0a);   


//BLC
XC9160_write_cmos_sensor(0xfffe,0x30);                     
XC9160_write_cmos_sensor(0x0013,0x40);   //bias               
XC9160_write_cmos_sensor(0x0014,0x00);                   
XC9160_write_cmos_sensor(0x071b,0xff);  //blc 

//AE  V2
XC9160_write_cmos_sensor(0xfffe,0x30);                                                                                                     
XC9160_write_cmos_sensor(0x1f00,0x00);   // WIN start X                                                                                    
XC9160_write_cmos_sensor(0x1f01,0x00);                                                                                                     
XC9160_write_cmos_sensor(0x1f02,0x00);   // WIN stat Y                                                                                     
XC9160_write_cmos_sensor(0x1f03,0x00);                                                                                                     
XC9160_write_cmos_sensor(0x1f04,0x08);   // WIN width                                                                                      
XC9160_write_cmos_sensor(0x1f05,0x20);                                                                                                     
XC9160_write_cmos_sensor(0x1f06,0x06);   // WIN height                                                                                     
XC9160_write_cmos_sensor(0x1f07,0x00);                                                                                                     
XC9160_write_cmos_sensor(0x1f08,0x03);                                                                                                     
                                                                                                                 
XC9160_write_cmos_sensor(0x0051,0x03);     //统计数据在gamma之后

XC9160_write_cmos_sensor(0xfffe,0x25);
XC9160_write_cmos_sensor(0x0002,0xff);    //i2c速度调整，速度过快AE闪烁 change 2017年3月30日  保证i2c速度在100k-400k之间
XC9160_write_cmos_sensor(0xfffe,0x14);
XC9160_write_cmos_sensor(0x0022,0x1e);
                                                                            
/////////////////////////////////////////////////////                                                            
/////////////////////    SENSOR     ///////////////                                                              
/////////////////////////////////////////////////////                                                            
XC9160_write_cmos_sensor(0xfffe,0x14);                                                                                                     
XC9160_write_cmos_sensor(0x000e,0x00);   //Isp0 Used I2c
XC9160_write_cmos_sensor(0x010e,0x5a);  	//camera i2c id                                                                                  
XC9160_write_cmos_sensor(0x010f,0x01);  	//camera i2c bits                                                                                
XC9160_write_cmos_sensor(0x0110,0x00);  	//sensor type gain 
XC9160_write_cmos_sensor(0x00f3,0x03);    // gain offset                                                                              
XC9160_write_cmos_sensor(0x0111,0x02);  	//sensor type exposure                                                                           
                                                                                                                 
//exposure                                                                                                                                 
XC9160_write_cmos_sensor(0x0114,0x02);  //write camera line exposure[7:0]                                                            
XC9160_write_cmos_sensor(0x0115,0x02);
XC9160_write_cmos_sensor(0x0116,0x02);
XC9160_write_cmos_sensor(0x0117,0x03);
                                                                                                                                       
XC9160_write_cmos_sensor(0x011c,0x00);  //camera exposure addr mask 1                                                                      
XC9160_write_cmos_sensor(0x011d,0xff);
XC9160_write_cmos_sensor(0x011e,0x00);
XC9160_write_cmos_sensor(0x011f,0xff);
                                                                          
//gain                                                                                                           
XC9160_write_cmos_sensor(0x0134,0x02);  //camera gain addr                                                                                 
XC9160_write_cmos_sensor(0x0135,0x04);
XC9160_write_cmos_sensor(0x0136,0x02);
XC9160_write_cmos_sensor(0x0137,0x05);
                                                                                                                                                                                            
XC9160_write_cmos_sensor(0x013c,0x00);  //camera gain addr mask 1                                                                          
XC9160_write_cmos_sensor(0x013d,0xff);
XC9160_write_cmos_sensor(0x013e,0x00);
XC9160_write_cmos_sensor(0x013f,0xff);
                                                                                                                                                                                                                   
/////////////////////////////////////////////////////                                                            
//////////////////////   AE START     ///////////////                                                            
/////////////////////////////////////////////////////                                                            
XC9160_write_cmos_sensor(0x004c,0x00);                                                                                                        
XC9160_write_cmos_sensor(0x002b,0x01);    //  ae enable                                                                                       
XC9160_write_cmos_sensor(0x004d,0x01);    //  ae Force write                                                                                  
                                                                                                                 
XC9160_write_cmos_sensor(0x00fa,0x02);                                                                                                       
XC9160_write_cmos_sensor(0x00fb,0x00);   //max gain                                                                                          
                                                                                                                 
XC9160_write_cmos_sensor(0x00fc,0x00);                                                                                                       
XC9160_write_cmos_sensor(0x00fd,0x20);   //min gain                                                                                          
                                                                                                                 
XC9160_write_cmos_sensor(0x00e2,0x38);   //max exp  10fps                                                                                    
XC9160_write_cmos_sensor(0x00e3,0x00);                                                                                                       
                                                                                                                 
XC9160_write_cmos_sensor(0x00de,0x00);                                                                                                       
XC9160_write_cmos_sensor(0x00df,0x10);   //min exp                                                                                           
                                                                                                                 
XC9160_write_cmos_sensor(0x00a0,0x01);  //01   //day target                                                                                 
XC9160_write_cmos_sensor(0x00a1,0xc0);  //C0                                                                                                 
                                                                                                                 
XC9160_write_cmos_sensor(0x0104,0x02);                                                                                                     
XC9160_write_cmos_sensor(0x0105,0x01);                                                                                                     
XC9160_write_cmos_sensor(0x0108,0x08);                                                                                                     
XC9160_write_cmos_sensor(0x0109,0x00);   //flicker                                                                                         
                                                                                                                 
XC9160_write_cmos_sensor(0x0145,0x00);    //gain delay                                                                                      
                                                                                                                 
XC9160_write_cmos_sensor(0x0055,0x04);                                                                                                       
XC9160_write_cmos_sensor(0x0056,0x04);                                                                                                       
XC9160_write_cmos_sensor(0x0057,0x08);                                                                                                       
XC9160_write_cmos_sensor(0x0058,0x04);                                                                                                       
XC9160_write_cmos_sensor(0x0059,0x04);                                                                                                       
                                                                                                                 
XC9160_write_cmos_sensor(0x005a,0x04);                                                                                                       
XC9160_write_cmos_sensor(0x005b,0x08);                                                                                                       
XC9160_write_cmos_sensor(0x005c,0x10);                                                                                                      
XC9160_write_cmos_sensor(0x005d,0x08);                                                                                                       
XC9160_write_cmos_sensor(0x005e,0x04);                                                                                                       
                                                                                                                 
XC9160_write_cmos_sensor(0x005f,0x08);                                                                                                       
XC9160_write_cmos_sensor(0x0060,0x10);                                                                                                      
XC9160_write_cmos_sensor(0x0061,0x20);                                                                                                      
XC9160_write_cmos_sensor(0x0062,0x10);                                                                                                      
XC9160_write_cmos_sensor(0x0063,0x08);                                                                                                       
                                                                                                                 
XC9160_write_cmos_sensor(0x0064,0x04);                                                                                                       
XC9160_write_cmos_sensor(0x0065,0x08);                                                                                                       
XC9160_write_cmos_sensor(0x0066,0x10);                                                                                                      
XC9160_write_cmos_sensor(0x0067,0x08);                                                                                                       
XC9160_write_cmos_sensor(0x0068,0x04);                                                                                                       
                                                                                                                 
XC9160_write_cmos_sensor(0x0069,0x04);                                                                                                       
XC9160_write_cmos_sensor(0x006a,0x04);                                                                                                       
XC9160_write_cmos_sensor(0x006b,0x08);                                                                                                       
XC9160_write_cmos_sensor(0x006c,0x04);                                                                                                       
XC9160_write_cmos_sensor(0x006d,0x04);                                                                                                       
                                                                                                                 
XC9160_write_cmos_sensor(0x0088,0x00);                                                                                                       
XC9160_write_cmos_sensor(0x0089,0x07);                                                                                                       
XC9160_write_cmos_sensor(0x008a,0x7d);                                                                                                      
XC9160_write_cmos_sensor(0x008b,0xc0);   //weight                                                                                           
                                                                                                                 
XC9160_write_cmos_sensor(0x0050,0x01);   //refresh                                                                                          
/////////////////////////////////////////////////////                                                            
///////////////////    AE SPEED       ///////////////                                                            
/////////////////////////////////////////////////////                                                            
XC9160_write_cmos_sensor(0x00c6,0x01);     // delay frame                                                                                     
                                                                                                                 
XC9160_write_cmos_sensor(0x00ca,0x00);    //1. threshold low                                                                                 
XC9160_write_cmos_sensor(0x00cb,0x60);                                                                                                       
                                                                                                                 
XC9160_write_cmos_sensor(0x01bc,0x00);                                                                                                      
XC9160_write_cmos_sensor(0x01bd,0x60);   //thr_l_all                                                                                        
                                                                                                                 
XC9160_write_cmos_sensor(0x01be,0x00);                                                                                                      
XC9160_write_cmos_sensor(0x01bf,0x40);   //thr_l_avg                                                                                        
                                                                                                                 
XC9160_write_cmos_sensor(0x00cc,0x00);    //2. threshold high                                                                                
XC9160_write_cmos_sensor(0x00cd,0x80);                                                                                                       
                                                                                                                 
XC9160_write_cmos_sensor(0x00c7,0x18);    //3. finally thr                                                                                   
                                                                                                                 
XC9160_write_cmos_sensor(0x00d8,0x80);    //4.stable thr_h                                                                                   
                                                                                                                 
XC9160_write_cmos_sensor(0x00c8,0x01);     //total speed                                                                                    
XC9160_write_cmos_sensor(0x0208,0x01);    //limint speed                                                                                    
                                                                                                                 
XC9160_write_cmos_sensor(0x00da,0x00);                                                                                                       
XC9160_write_cmos_sensor(0x00db,0x80);    //lumadiffthrlow                                                                                   
                                                                                                                 
XC9160_write_cmos_sensor(0x00dc,0x03);                                                                                                       
XC9160_write_cmos_sensor(0x00dd,0x00);    //lumadiffthrhigh                                                                                  
                                                                                                                 
/////////////////////////////////////////////////////                                                            
///////////smart ae//////////////////////////////////                                                            
/////////////////////////////////////////////////////                                                            
XC9160_write_cmos_sensor(0x0092,0x01);                                                                                                       
XC9160_write_cmos_sensor(0x0093,0x3f);                                                                                                       
XC9160_write_cmos_sensor(0x0094,0x01);                                                                                                       
XC9160_write_cmos_sensor(0x0095,0x01);                                                                                                       
XC9160_write_cmos_sensor(0x00ad,0x04);   //ATT block Cnt                                                                                     
XC9160_write_cmos_sensor(0x01c0,0x06);   //PDF_H used cnt to calc     //liu                                                                        
                                                                                                                 
XC9160_write_cmos_sensor(0x01d0,0x01);  //overExpRefBL                                                                                      
//////////////////// Table   ////////////////////////                                                            
//Exposure value   (BASE)                                                                                        
XC9160_write_cmos_sensor(0x0022,0x1e);  //fps                                                                                                
                                                                                                                 
XC9160_write_cmos_sensor(0x01e4,0x00);   // Exp value Table                                                                                
XC9160_write_cmos_sensor(0x01e5,0x00);                                                                                                     
XC9160_write_cmos_sensor(0x01e6,0x50);                                                                                                     
XC9160_write_cmos_sensor(0x01e7,0x00);  //table0                                                                                           
                                                                                                                 
XC9160_write_cmos_sensor(0x01e8,0x00);                                                                                                      
XC9160_write_cmos_sensor(0x01e9,0x00);                                                                                                      
XC9160_write_cmos_sensor(0x01ea,0xa8);                                                                                                      
XC9160_write_cmos_sensor(0x01eb,0x00);   //table1                                                                                           
                                                                                                                 
XC9160_write_cmos_sensor(0x01ec,0x00);                                                                                                      
XC9160_write_cmos_sensor(0x01ed,0x01);                                                                                                      
XC9160_write_cmos_sensor(0x01ee,0x08);                                                                                                      
XC9160_write_cmos_sensor(0x01ef,0x00);    //table2                                                                                          
                                                                                                                 
XC9160_write_cmos_sensor(0x01f0,0x00);                                                                                                      
XC9160_write_cmos_sensor(0x01f1,0x03);                                                                                                      
XC9160_write_cmos_sensor(0x01f2,0xc0);                                                                                                      
XC9160_write_cmos_sensor(0x01f3,0x00);    //table3                                                                                          
                                                                                                                 
XC9160_write_cmos_sensor(0x01f4,0x00);                                                                                                      
XC9160_write_cmos_sensor(0x01f5,0x0f);                                                                                                      
XC9160_write_cmos_sensor(0x01f6,0x00);                                                                                                      
XC9160_write_cmos_sensor(0x01f7,0x00);   //table4                                                                                           
                                                                                                                 
XC9160_write_cmos_sensor(0x01f8,0x00);                                                                                                      
XC9160_write_cmos_sensor(0x01f9,0x3c);                                                                                                      
XC9160_write_cmos_sensor(0x01fa,0x00);                                                                                                      
XC9160_write_cmos_sensor(0x01fb,0x00);  //table5 
          
//ref target                                                                                                     
XC9160_write_cmos_sensor(0x00b2,0x02);   // ref target table  0                                                                             
XC9160_write_cmos_sensor(0x00b3,0x20);                                                                                                      
XC9160_write_cmos_sensor(0x00b4,0x02);   // ref target table  1                                                                             
XC9160_write_cmos_sensor(0x00b5,0x60);                                                                                                      
XC9160_write_cmos_sensor(0x00b6,0x02);   // ref target table  2                                                                             
XC9160_write_cmos_sensor(0x00b7,0x20);                                                                                                      
XC9160_write_cmos_sensor(0x00b8,0x02);   // ref target table  3                                                                             
XC9160_write_cmos_sensor(0x00b9,0x00);                                                                                                      
XC9160_write_cmos_sensor(0x00ba,0x01);   // ref target table  4                                                                             
XC9160_write_cmos_sensor(0x00bb,0xd0);                                                                                                      
XC9160_write_cmos_sensor(0x00bc,0x01);   // ref target table  5                                                                             
XC9160_write_cmos_sensor(0x00bd,0x00); 
                                                                                                                 
//over exposure offset table                                                                                     
XC9160_write_cmos_sensor(0x01d6,0x06);                                                                                                     
XC9160_write_cmos_sensor(0x01d7,0x0a);                                                                                                     
XC9160_write_cmos_sensor(0x01d8,0x14);                                                                                                     
XC9160_write_cmos_sensor(0x01d9,0x20);                                                                                                     
XC9160_write_cmos_sensor(0x01da,0x28);                                                                                                     
                                                                                                                                                                                           
XC9160_write_cmos_sensor(0x01dc,0x40);                                                                                                     
XC9160_write_cmos_sensor(0x01dd,0x30);                                                                                                     
XC9160_write_cmos_sensor(0x01de,0x20);                                                                                                     
XC9160_write_cmos_sensor(0x01df,0x10);                                                                                                     
XC9160_write_cmos_sensor(0x01e0,0x00);                                                                                                      
XC9160_write_cmos_sensor(0x01e1,0x00);                                                                                                      
////////////////////////  AVG   //////////////////                                                               
XC9160_write_cmos_sensor(0x01cd,0x28);  // avg affect val                                                                                  
XC9160_write_cmos_sensor(0x01cb,0x00);  // avg thr_l                                                                                       
XC9160_write_cmos_sensor(0x01cc,0x60);  // avg thr_h                                                                                       
///////////////////  /Variance  /////////////////                                                                
XC9160_write_cmos_sensor(0x01b1,0x30);  // variance affect                                                                                 
                                                                                                                 
XC9160_write_cmos_sensor(0x01b2,0x01);  // thr l                                                                                           
XC9160_write_cmos_sensor(0x01b3,0x00);                                                                                                     
XC9160_write_cmos_sensor(0x01b4,0x08);  // thr h                                                                                           
XC9160_write_cmos_sensor(0x01b5,0x00);                                                                                                     
///////////////////////  PDFH ///////////////////                                                                
XC9160_write_cmos_sensor(0x01c7,0x06);   // PDFH_MAX                                                                                       
XC9160_write_cmos_sensor(0x01c9,0x02);   // PDFH_Target                                                                                    
                                                                                                                 
XC9160_write_cmos_sensor(0x01d1,0x60);   // OE affect 0         //liu                                                                       
XC9160_write_cmos_sensor(0x01ca,0x60);   // OE affect 1        //                                                                            
//////////////////  BRIGHT RATIO /////////////////                                                               
XC9160_write_cmos_sensor(0x021d,0x04);   //PDFH_Brighten_thr                                                                                
XC9160_write_cmos_sensor(0x01b7,0x00);   //bright_pdfh_diff thr_l                                                                           
XC9160_write_cmos_sensor(0x01b8,0x30);  //bright_pdfh_diff thr_m                                                                           
XC9160_write_cmos_sensor(0x01ba,0x10);  //bright affect ratio                                                                              
                                                                                                                 
XC9160_write_cmos_sensor(0x00a6,0x32); // CDF_H                                                                                            
XC9160_write_cmos_sensor(0x00a7,0x20); // CDF_L                                                                                            
XC9160_write_cmos_sensor(0x01c6,0x30); // nPDF_L Target                                                                                    
XC9160_write_cmos_sensor(0x01d2,0x08); //hist under exp affect ratio                                                                       
XC9160_write_cmos_sensor(0x01b0,0x40); //PDF_L brighten max                                                                                
///////////////////  ATT BLOCK LIMIT /////////////////                                                           
XC9160_write_cmos_sensor(0x016e,0x02);                                                                                                     
XC9160_write_cmos_sensor(0x016f,0x80);  //Max ATT thrshold
XC9160_write_cmos_sensor(0x016c,0x00);  //add liu
XC9160_write_cmos_sensor(0x016d,0xc0);  //add liu                                                                              
                                                                                                                 
XC9160_write_cmos_sensor(0x01d3,0x00);   // Att limit ratio L  addliu                                                                             
XC9160_write_cmos_sensor(0x01d4,0x08);   // Att limit ratio H                                                                               
//////////////////// BLOCK LIMIT /////////////////////                                                           
XC9160_write_cmos_sensor(0x016a,0x03);  //MAX avg thrshold                                                                                 
XC9160_write_cmos_sensor(0x016b,0x80);                                                                                                     
                                                                                                                 
XC9160_write_cmos_sensor(0x01d5,0x10);  // global limit ratio                                                                              
/////////////// ADJUST STABE THR IN AF ///////////////                                                           
XC9160_write_cmos_sensor(0x0212,0x01);   // enable switch avg stable thr                                                                    
XC9160_write_cmos_sensor(0x0215,0x28);  // global avg diff thr normal                                                                      
XC9160_write_cmos_sensor(0x0217,0x48);  // global avg diff thr for AF                                                                      
XC9160_write_cmos_sensor(0x0219,0x80);  // global all diff thr normal,look in d8                                                         
XC9160_write_cmos_sensor(0x021b,0xff);  // global all diff thr for AF,look in d8                                                         
//XC9160_write_cmos_sensor(0x01b6,0x28);  //avgdiff_en  enable AE threshold                                                                 
//////////////////// DEBUG   /////////////////////                                                               
//XC9160_write_cmos_sensor(0x0209,0x01);   //b2d speed limit                                                                               
XC9160_write_cmos_sensor(0x020b,0x40);   //b2d stable buff                                                                                 
XC9160_write_cmos_sensor(0x020d,0x03);    //b2d speed limit max cnt,0 means disable                                                        
                                                                                                                 
XC9160_write_cmos_sensor(0x0220,0x00);                                                                                                      
XC9160_write_cmos_sensor(0x0221,0xff);   //HistLimitDarkenMaxVal,according 27(15%)                                                       
/////////////////////////////////////////////////////                                                            
///////////////////   AE START  END   ///////////////                                                            
///////////////////////////////////////////////////// 

//AF
XC9160_write_cmos_sensor(0xfffe,0x30);
//XC9160_write_cmos_sensor(0x0019,0x88);
XC9160_write_cmos_sensor(0x1e00,0x90);

XC9160_write_cmos_sensor(0x1e01,0x03);    
XC9160_write_cmos_sensor(0x1e02,0xf0);

XC9160_write_cmos_sensor(0x1e03,0x03);    
XC9160_write_cmos_sensor(0x1e04,0xf0);

XC9160_write_cmos_sensor(0x1e0f,0x02);     
XC9160_write_cmos_sensor(0x1e10,0x96);

XC9160_write_cmos_sensor(0x1e11,0x01);    
XC9160_write_cmos_sensor(0x1e12,0x86);

XC9160_write_cmos_sensor(0x1e0a,0x00);
XC9160_write_cmos_sensor(0x1e0b,0x20);
XC9160_write_cmos_sensor(0x1e0c,0xff);

XC9160_write_cmos_sensor(0x1e21,0xff);
/////////////////////////////////////////////////////
//////////////////////SW/////////////////////////////
XC9160_write_cmos_sensor(0xfffe,0x14);
XC9160_write_cmos_sensor(0x0024,0x00);     //bDebugEnable; /**< BOOL: Debug enable */
XC9160_write_cmos_sensor(0x0025,0x00);
XC9160_write_cmos_sensor(0x0027,0x07);    //nDebugAFMode; /**< BOOL: AF Debug enable */
XC9160_write_cmos_sensor(0x002d,0x01);   //bISP0AFEnable;  /**< BOOL: AF enable */

XC9160_write_cmos_sensor(0x02e0,0x01);   //nAFMode; /**< 0. AF_SNAPSHOT); 1); AF_VIDEOSERVO */
XC9160_write_cmos_sensor(0x02e1,0x00);   // bAutoContrastThr;
XC9160_write_cmos_sensor(0x02e2,0x01);   //bAFActive; /**< current AF status is active or not  */

/////////////////////////////////////////////////////
/////////////////////motor driver////////////////////
XC9160_write_cmos_sensor(0x02e7,0x1e);   //frame rate  

XC9160_write_cmos_sensor(0x045b,0x04);   //specialmode
XC9160_write_cmos_sensor(0x0475,0x18);   // slave ID
XC9160_write_cmos_sensor(0x0476,0x03);   // motor set : addr

XC9160_write_cmos_sensor(0x047c,0x00);    // DAC MIN
XC9160_write_cmos_sensor(0x047d,0xb0);    // DAC MIN

XC9160_write_cmos_sensor(0x047e,0x03);    // DAC MAX
XC9160_write_cmos_sensor(0x047f,0x20);

XC9160_write_cmos_sensor(0x0466,0x00);  
XC9160_write_cmos_sensor(0x0467,0x80);   // motor used time ms

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
XC9160_write_cmos_sensor(0x0368,0x01);    // previous move
//XC9160_write_cmos_sensor(0x0369,0xff);

XC9160_write_cmos_sensor(0x030c,0x08);    //over peak ratio

XC9160_write_cmos_sensor(0x02e8,0x01);
XC9160_write_cmos_sensor(0x02e9,0x00);    //base step

XC9160_write_cmos_sensor(0x02ea,0x10);     //coarse min step ratio

XC9160_write_cmos_sensor(0x02eb,0x18);    //coarse step ratio

XC9160_write_cmos_sensor(0x02ec,0x0c);     //FINE step
XC9160_write_cmos_sensor(0x02ed,0x0A);     //FINE step

XC9160_write_cmos_sensor(0x02fe,0x00);	//compare-coarse
XC9160_write_cmos_sensor(0x02ff,0x40);

XC9160_write_cmos_sensor(0x0300,0x00);	//compare-coarse
XC9160_write_cmos_sensor(0x0301,0x40);

XC9160_write_cmos_sensor(0x0302,0x00);	//compare-fine
XC9160_write_cmos_sensor(0x0303,0x28);

XC9160_write_cmos_sensor(0x0304,0x00);	//compare-fine
XC9160_write_cmos_sensor(0x0305,0x30);

XC9160_write_cmos_sensor(0x030e,0x02);     //failure choose

XC9160_write_cmos_sensor(0x05f0,0x00);    //HW max low thrshold
XC9160_write_cmos_sensor(0x05f1,0x10);

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

// contrast change ratio  U.6
XC9160_write_cmos_sensor(0x02ee,0x00);
XC9160_write_cmos_sensor(0x02ef,0x20);

XC9160_write_cmos_sensor(0x02f0,0x00);
XC9160_write_cmos_sensor(0x02f1,0x28);

// last hist change ratio U.6
XC9160_write_cmos_sensor(0x02f2,0x00);
XC9160_write_cmos_sensor(0x02f3,0x10);

XC9160_write_cmos_sensor(0x02f4,0x00);
XC9160_write_cmos_sensor(0x02f5,0x16);

// previous hist change ratio U.6
XC9160_write_cmos_sensor(0x02f6,0x00);
XC9160_write_cmos_sensor(0x02f7,0x06);

XC9160_write_cmos_sensor(0x02f8,0x00);
XC9160_write_cmos_sensor(0x02f9,0x08);

// hist diff-value change ratio U.6
XC9160_write_cmos_sensor(0x02fa,0x00);
XC9160_write_cmos_sensor(0x02fb,0x00);

XC9160_write_cmos_sensor(0x02fc,0x00);
XC9160_write_cmos_sensor(0x02fd,0x00);

// hist mean change ratio U.6
XC9160_write_cmos_sensor(0x0308,0x1b);

//gain
XC9160_write_cmos_sensor(0x0306,0x00);
XC9160_write_cmos_sensor(0x0307,0x28);

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

XC9160_write_cmos_sensor(0x05a8,0x01);          //detect
XC9160_write_cmos_sensor(0x05a9,0x00);

XC9160_write_cmos_sensor(0x0310,0x20);         // detect last mean change 
XC9160_write_cmos_sensor(0x0311,0x18);         // detect last mean diff-value change

XC9160_write_cmos_sensor(0x0312,0x22);          //detect histogram difference

XC9160_write_cmos_sensor(0x030f,0xA0);          // contrast change ratio  U.6
XC9160_write_cmos_sensor(0x0314,0xF0);

//ISP0 AutoLenc 2017_05_23_141734

//atuo lenc
XC9160_write_cmos_sensor(0xfffe,0x30);
//XC9160_write_cmos_sensor(0x0002,0x80);      //bit[7]:lenc_en
	
XC9160_write_cmos_sensor(0x03ca,0x05);	
XC9160_write_cmos_sensor(0x03cb,0xD7);	
XC9160_write_cmos_sensor(0x03cc,0x07);	
XC9160_write_cmos_sensor(0x03cd,0xE0);	
XC9160_write_cmos_sensor(0x03ce,0x05);	
XC9160_write_cmos_sensor(0x03cf,0xD7);	
XC9160_write_cmos_sensor(0x03d0,0x03);	
XC9160_write_cmos_sensor(0x03d1,0xF0);	        

XC9160_write_cmos_sensor(0xfffe,0x14);
XC9160_write_cmos_sensor(0x002c,0x01);
XC9160_write_cmos_sensor(0x0030,0x01);
XC9160_write_cmos_sensor(0x0620,0x01);	
XC9160_write_cmos_sensor(0x0621,0x01);	

XC9160_write_cmos_sensor(0xfffe,0x14);	
XC9160_write_cmos_sensor(0x0928,0x00);	
XC9160_write_cmos_sensor(0x0929,0x81);  //A	
XC9160_write_cmos_sensor(0x092a,0x00);	
XC9160_write_cmos_sensor(0x092b,0xdb);  //C	
XC9160_write_cmos_sensor(0x092c,0x01);	
XC9160_write_cmos_sensor(0x092d,0x7c);	//D

XC9160_write_cmos_sensor(0x06e5,0x3f);  //a_92% 20170704   4208x3120
XC9160_write_cmos_sensor(0x06e6,0x34);  
XC9160_write_cmos_sensor(0x06e7,0x2d);  
XC9160_write_cmos_sensor(0x06e8,0x26);  
XC9160_write_cmos_sensor(0x06e9,0x26);  
XC9160_write_cmos_sensor(0x06ea,0x2c);  
XC9160_write_cmos_sensor(0x06eb,0x36);  
XC9160_write_cmos_sensor(0x06ec,0x3f);  
XC9160_write_cmos_sensor(0x06ed,0x2a);  
XC9160_write_cmos_sensor(0x06ee,0x1e);  
XC9160_write_cmos_sensor(0x06ef,0x16);  
XC9160_write_cmos_sensor(0x06f0,0x13);  
XC9160_write_cmos_sensor(0x06f1,0x13);  
XC9160_write_cmos_sensor(0x06f2,0x15);  
XC9160_write_cmos_sensor(0x06f3,0x1d);  
XC9160_write_cmos_sensor(0x06f4,0x2a);  
XC9160_write_cmos_sensor(0x06f5,0x1b);  
XC9160_write_cmos_sensor(0x06f6,0x10);  
XC9160_write_cmos_sensor(0x06f7,0x0a);  
XC9160_write_cmos_sensor(0x06f8,0x06);  
XC9160_write_cmos_sensor(0x06f9,0x06);  
XC9160_write_cmos_sensor(0x06fa,0x0a);  
XC9160_write_cmos_sensor(0x06fb,0x10);  
XC9160_write_cmos_sensor(0x06fc,0x19);  
XC9160_write_cmos_sensor(0x06fd,0x14);  
XC9160_write_cmos_sensor(0x06fe,0x0b);  
XC9160_write_cmos_sensor(0x06ff,0x03);  
XC9160_write_cmos_sensor(0x0700,0x00);  
XC9160_write_cmos_sensor(0x0701,0x00);  
XC9160_write_cmos_sensor(0x0702,0x03);  
XC9160_write_cmos_sensor(0x0703,0x0a);  
XC9160_write_cmos_sensor(0x0704,0x13);  
XC9160_write_cmos_sensor(0x0705,0x14);  
XC9160_write_cmos_sensor(0x0706,0x0b);  
XC9160_write_cmos_sensor(0x0707,0x03);  
XC9160_write_cmos_sensor(0x0708,0x00);  
XC9160_write_cmos_sensor(0x0709,0x00);  
XC9160_write_cmos_sensor(0x070a,0x03);  
XC9160_write_cmos_sensor(0x070b,0x0a);  
XC9160_write_cmos_sensor(0x070c,0x13);  
XC9160_write_cmos_sensor(0x070d,0x1b);  
XC9160_write_cmos_sensor(0x070e,0x10);  
XC9160_write_cmos_sensor(0x070f,0x0a);  
XC9160_write_cmos_sensor(0x0710,0x06);  
XC9160_write_cmos_sensor(0x0711,0x06);  
XC9160_write_cmos_sensor(0x0712,0x09);  
XC9160_write_cmos_sensor(0x0713,0x10);  
XC9160_write_cmos_sensor(0x0714,0x19);  
XC9160_write_cmos_sensor(0x0715,0x2a);  
XC9160_write_cmos_sensor(0x0716,0x1d);  
XC9160_write_cmos_sensor(0x0717,0x15);  
XC9160_write_cmos_sensor(0x0718,0x12);  
XC9160_write_cmos_sensor(0x0719,0x12);  
XC9160_write_cmos_sensor(0x071a,0x15);  
XC9160_write_cmos_sensor(0x071b,0x1d);  
XC9160_write_cmos_sensor(0x071c,0x2a);  
XC9160_write_cmos_sensor(0x071d,0x3f);  
XC9160_write_cmos_sensor(0x071e,0x36);  
XC9160_write_cmos_sensor(0x071f,0x2e);  
XC9160_write_cmos_sensor(0x0720,0x27);  
XC9160_write_cmos_sensor(0x0721,0x27);  
XC9160_write_cmos_sensor(0x0722,0x2d);  
XC9160_write_cmos_sensor(0x0723,0x37);  
XC9160_write_cmos_sensor(0x0724,0x3f);  
XC9160_write_cmos_sensor(0x0725,0x25);  
XC9160_write_cmos_sensor(0x0726,0x21);  
XC9160_write_cmos_sensor(0x0727,0x22);  
XC9160_write_cmos_sensor(0x0728,0x20);  
XC9160_write_cmos_sensor(0x0729,0x21);  
XC9160_write_cmos_sensor(0x072a,0x21);  
XC9160_write_cmos_sensor(0x072b,0x22);  
XC9160_write_cmos_sensor(0x072c,0x23);  
XC9160_write_cmos_sensor(0x072d,0x20);  
XC9160_write_cmos_sensor(0x072e,0x1f);  
XC9160_write_cmos_sensor(0x072f,0x20);  
XC9160_write_cmos_sensor(0x0730,0x20);  
XC9160_write_cmos_sensor(0x0731,0x20);  
XC9160_write_cmos_sensor(0x0732,0x20);  
XC9160_write_cmos_sensor(0x0733,0x20);  
XC9160_write_cmos_sensor(0x0734,0x21);  
XC9160_write_cmos_sensor(0x0735,0x21);  
XC9160_write_cmos_sensor(0x0736,0x20);  
XC9160_write_cmos_sensor(0x0737,0x20);  
XC9160_write_cmos_sensor(0x0738,0x21);  
XC9160_write_cmos_sensor(0x0739,0x21);  
XC9160_write_cmos_sensor(0x073a,0x21);  
XC9160_write_cmos_sensor(0x073b,0x20);  
XC9160_write_cmos_sensor(0x073c,0x22);  
XC9160_write_cmos_sensor(0x073d,0x20);  
XC9160_write_cmos_sensor(0x073e,0x1f);  
XC9160_write_cmos_sensor(0x073f,0x21);  
XC9160_write_cmos_sensor(0x0740,0x23);  
XC9160_write_cmos_sensor(0x0741,0x23);  
XC9160_write_cmos_sensor(0x0742,0x22);  
XC9160_write_cmos_sensor(0x0743,0x20);  
XC9160_write_cmos_sensor(0x0744,0x22);  
XC9160_write_cmos_sensor(0x0745,0x20);  
XC9160_write_cmos_sensor(0x0746,0x1f);  
XC9160_write_cmos_sensor(0x0747,0x21);  
XC9160_write_cmos_sensor(0x0748,0x23);  
XC9160_write_cmos_sensor(0x0749,0x23);  
XC9160_write_cmos_sensor(0x074a,0x22);  
XC9160_write_cmos_sensor(0x074b,0x20);  
XC9160_write_cmos_sensor(0x074c,0x22);  
XC9160_write_cmos_sensor(0x074d,0x20);  
XC9160_write_cmos_sensor(0x074e,0x1f);  
XC9160_write_cmos_sensor(0x074f,0x1f);  
XC9160_write_cmos_sensor(0x0750,0x20);  
XC9160_write_cmos_sensor(0x0751,0x20);  
XC9160_write_cmos_sensor(0x0752,0x20);  
XC9160_write_cmos_sensor(0x0753,0x20);  
XC9160_write_cmos_sensor(0x0754,0x21);  
XC9160_write_cmos_sensor(0x0755,0x20);  
XC9160_write_cmos_sensor(0x0756,0x1e);  
XC9160_write_cmos_sensor(0x0757,0x1f);  
XC9160_write_cmos_sensor(0x0758,0x1f);  
XC9160_write_cmos_sensor(0x0759,0x1f);  
XC9160_write_cmos_sensor(0x075a,0x1f);  
XC9160_write_cmos_sensor(0x075b,0x20);  
XC9160_write_cmos_sensor(0x075c,0x21);  
XC9160_write_cmos_sensor(0x075d,0x1d);  
XC9160_write_cmos_sensor(0x075e,0x21);  
XC9160_write_cmos_sensor(0x075f,0x1f);  
XC9160_write_cmos_sensor(0x0760,0x1f);  
XC9160_write_cmos_sensor(0x0761,0x1f);  
XC9160_write_cmos_sensor(0x0762,0x20);  
XC9160_write_cmos_sensor(0x0763,0x20);  
XC9160_write_cmos_sensor(0x0764,0x22);  
XC9160_write_cmos_sensor(0x0765,0x28);  
XC9160_write_cmos_sensor(0x0766,0x2d);  
XC9160_write_cmos_sensor(0x0767,0x2d);  
XC9160_write_cmos_sensor(0x0768,0x2e);  
XC9160_write_cmos_sensor(0x0769,0x2f);  
XC9160_write_cmos_sensor(0x076a,0x2d);  
XC9160_write_cmos_sensor(0x076b,0x2e);  
XC9160_write_cmos_sensor(0x076c,0x2c);  
XC9160_write_cmos_sensor(0x076d,0x2d);  
XC9160_write_cmos_sensor(0x076e,0x2e);  
XC9160_write_cmos_sensor(0x076f,0x2e);  
XC9160_write_cmos_sensor(0x0770,0x2d);  
XC9160_write_cmos_sensor(0x0771,0x2e);  
XC9160_write_cmos_sensor(0x0772,0x2e);  
XC9160_write_cmos_sensor(0x0773,0x2e);  
XC9160_write_cmos_sensor(0x0774,0x30);  
XC9160_write_cmos_sensor(0x0775,0x2c);  
XC9160_write_cmos_sensor(0x0776,0x2d);  
XC9160_write_cmos_sensor(0x0777,0x2a);  
XC9160_write_cmos_sensor(0x0778,0x26);  
XC9160_write_cmos_sensor(0x0779,0x26);  
XC9160_write_cmos_sensor(0x077a,0x2a);  
XC9160_write_cmos_sensor(0x077b,0x2e);  
XC9160_write_cmos_sensor(0x077c,0x2f);  
XC9160_write_cmos_sensor(0x077d,0x2d);  
XC9160_write_cmos_sensor(0x077e,0x28);  
XC9160_write_cmos_sensor(0x077f,0x23);  
XC9160_write_cmos_sensor(0x0780,0x20);  
XC9160_write_cmos_sensor(0x0781,0x20);  
XC9160_write_cmos_sensor(0x0782,0x23);  
XC9160_write_cmos_sensor(0x0783,0x28);  
XC9160_write_cmos_sensor(0x0784,0x30);  
XC9160_write_cmos_sensor(0x0785,0x2d);  
XC9160_write_cmos_sensor(0x0786,0x29);  
XC9160_write_cmos_sensor(0x0787,0x24);  
XC9160_write_cmos_sensor(0x0788,0x20);  
XC9160_write_cmos_sensor(0x0789,0x20);  
XC9160_write_cmos_sensor(0x078a,0x23);  
XC9160_write_cmos_sensor(0x078b,0x29);  
XC9160_write_cmos_sensor(0x078c,0x30);  
XC9160_write_cmos_sensor(0x078d,0x2c);  
XC9160_write_cmos_sensor(0x078e,0x2d);  
XC9160_write_cmos_sensor(0x078f,0x29);  
XC9160_write_cmos_sensor(0x0790,0x25);  
XC9160_write_cmos_sensor(0x0791,0x25);  
XC9160_write_cmos_sensor(0x0792,0x28);  
XC9160_write_cmos_sensor(0x0793,0x2d);  
XC9160_write_cmos_sensor(0x0794,0x2f);  
XC9160_write_cmos_sensor(0x0795,0x2d);  
XC9160_write_cmos_sensor(0x0796,0x2d);  
XC9160_write_cmos_sensor(0x0797,0x2d);  
XC9160_write_cmos_sensor(0x0798,0x2d);  
XC9160_write_cmos_sensor(0x0799,0x2c);  
XC9160_write_cmos_sensor(0x079a,0x2d);  
XC9160_write_cmos_sensor(0x079b,0x2d);  
XC9160_write_cmos_sensor(0x079c,0x2f);  
XC9160_write_cmos_sensor(0x079d,0x28);  
XC9160_write_cmos_sensor(0x079e,0x2b);  
XC9160_write_cmos_sensor(0x079f,0x2a);  
XC9160_write_cmos_sensor(0x07a0,0x2b);  
XC9160_write_cmos_sensor(0x07a1,0x2b);  
XC9160_write_cmos_sensor(0x07a2,0x2b);  
XC9160_write_cmos_sensor(0x07a3,0x2b);  
XC9160_write_cmos_sensor(0x07a4,0x2a);  

XC9160_write_cmos_sensor(0x07a5,0x3f);        //c_93% 20170704   4208x3120
XC9160_write_cmos_sensor(0x07a6,0x31);
XC9160_write_cmos_sensor(0x07a7,0x2a);
XC9160_write_cmos_sensor(0x07a8,0x23);
XC9160_write_cmos_sensor(0x07a9,0x24);
XC9160_write_cmos_sensor(0x07aa,0x2a);
XC9160_write_cmos_sensor(0x07ab,0x33);
XC9160_write_cmos_sensor(0x07ac,0x3f);
XC9160_write_cmos_sensor(0x07ad,0x28);
XC9160_write_cmos_sensor(0x07ae,0x1c);
XC9160_write_cmos_sensor(0x07af,0x14);
XC9160_write_cmos_sensor(0x07b0,0x11);
XC9160_write_cmos_sensor(0x07b1,0x11);
XC9160_write_cmos_sensor(0x07b2,0x14);
XC9160_write_cmos_sensor(0x07b3,0x1c);
XC9160_write_cmos_sensor(0x07b4,0x28);
XC9160_write_cmos_sensor(0x07b5,0x19);
XC9160_write_cmos_sensor(0x07b6,0x0f);
XC9160_write_cmos_sensor(0x07b7,0x09);
XC9160_write_cmos_sensor(0x07b8,0x06);
XC9160_write_cmos_sensor(0x07b9,0x06);
XC9160_write_cmos_sensor(0x07ba,0x09);
XC9160_write_cmos_sensor(0x07bb,0x0f);
XC9160_write_cmos_sensor(0x07bc,0x17);
XC9160_write_cmos_sensor(0x07bd,0x13);
XC9160_write_cmos_sensor(0x07be,0x0a);
XC9160_write_cmos_sensor(0x07bf,0x03);
XC9160_write_cmos_sensor(0x07c0,0x00);
XC9160_write_cmos_sensor(0x07c1,0x00);
XC9160_write_cmos_sensor(0x07c2,0x03);
XC9160_write_cmos_sensor(0x07c3,0x09);
XC9160_write_cmos_sensor(0x07c4,0x11);
XC9160_write_cmos_sensor(0x07c5,0x12);
XC9160_write_cmos_sensor(0x07c6,0x0a);
XC9160_write_cmos_sensor(0x07c7,0x03);
XC9160_write_cmos_sensor(0x07c8,0x00);
XC9160_write_cmos_sensor(0x07c9,0x00);
XC9160_write_cmos_sensor(0x07ca,0x02);
XC9160_write_cmos_sensor(0x07cb,0x09);
XC9160_write_cmos_sensor(0x07cc,0x11);
XC9160_write_cmos_sensor(0x07cd,0x19);
XC9160_write_cmos_sensor(0x07ce,0x0f);
XC9160_write_cmos_sensor(0x07cf,0x09);
XC9160_write_cmos_sensor(0x07d0,0x05);
XC9160_write_cmos_sensor(0x07d1,0x05);
XC9160_write_cmos_sensor(0x07d2,0x09);
XC9160_write_cmos_sensor(0x07d3,0x0f);
XC9160_write_cmos_sensor(0x07d4,0x17);
XC9160_write_cmos_sensor(0x07d5,0x28);
XC9160_write_cmos_sensor(0x07d6,0x1c);
XC9160_write_cmos_sensor(0x07d7,0x14);
XC9160_write_cmos_sensor(0x07d8,0x11);
XC9160_write_cmos_sensor(0x07d9,0x11);
XC9160_write_cmos_sensor(0x07da,0x14);
XC9160_write_cmos_sensor(0x07db,0x1b);
XC9160_write_cmos_sensor(0x07dc,0x27);
XC9160_write_cmos_sensor(0x07dd,0x3f);
XC9160_write_cmos_sensor(0x07de,0x34);
XC9160_write_cmos_sensor(0x07df,0x2c);
XC9160_write_cmos_sensor(0x07e0,0x25);
XC9160_write_cmos_sensor(0x07e1,0x25);
XC9160_write_cmos_sensor(0x07e2,0x2b);
XC9160_write_cmos_sensor(0x07e3,0x34);
XC9160_write_cmos_sensor(0x07e4,0x3f);
XC9160_write_cmos_sensor(0x07e5,0x20);
XC9160_write_cmos_sensor(0x07e6,0x1f);
XC9160_write_cmos_sensor(0x07e7,0x20);
XC9160_write_cmos_sensor(0x07e8,0x1f);
XC9160_write_cmos_sensor(0x07e9,0x1f);
XC9160_write_cmos_sensor(0x07ea,0x20);
XC9160_write_cmos_sensor(0x07eb,0x1f);
XC9160_write_cmos_sensor(0x07ec,0x21);
XC9160_write_cmos_sensor(0x07ed,0x1e);
XC9160_write_cmos_sensor(0x07ee,0x1e);
XC9160_write_cmos_sensor(0x07ef,0x1e);
XC9160_write_cmos_sensor(0x07f0,0x1e);
XC9160_write_cmos_sensor(0x07f1,0x1e);
XC9160_write_cmos_sensor(0x07f2,0x1e);
XC9160_write_cmos_sensor(0x07f3,0x1e);
XC9160_write_cmos_sensor(0x07f4,0x1f);
XC9160_write_cmos_sensor(0x07f5,0x1f);
XC9160_write_cmos_sensor(0x07f6,0x1e);
XC9160_write_cmos_sensor(0x07f7,0x1e);
XC9160_write_cmos_sensor(0x07f8,0x1f);
XC9160_write_cmos_sensor(0x07f9,0x1f);
XC9160_write_cmos_sensor(0x07fa,0x1f);
XC9160_write_cmos_sensor(0x07fb,0x1e);
XC9160_write_cmos_sensor(0x07fc,0x20);
XC9160_write_cmos_sensor(0x07fd,0x1e);
XC9160_write_cmos_sensor(0x07fe,0x1e);
XC9160_write_cmos_sensor(0x07ff,0x1f);
XC9160_write_cmos_sensor(0x0800,0x21);
XC9160_write_cmos_sensor(0x0801,0x22);
XC9160_write_cmos_sensor(0x0802,0x20);
XC9160_write_cmos_sensor(0x0803,0x1e);
XC9160_write_cmos_sensor(0x0804,0x20);
XC9160_write_cmos_sensor(0x0805,0x1e);
XC9160_write_cmos_sensor(0x0806,0x1d);
XC9160_write_cmos_sensor(0x0807,0x1f);
XC9160_write_cmos_sensor(0x0808,0x21);
XC9160_write_cmos_sensor(0x0809,0x21);
XC9160_write_cmos_sensor(0x080a,0x20);
XC9160_write_cmos_sensor(0x080b,0x1e);
XC9160_write_cmos_sensor(0x080c,0x1f);
XC9160_write_cmos_sensor(0x080d,0x1e);
XC9160_write_cmos_sensor(0x080e,0x1d);
XC9160_write_cmos_sensor(0x080f,0x1d);
XC9160_write_cmos_sensor(0x0810,0x1e);
XC9160_write_cmos_sensor(0x0811,0x1e);
XC9160_write_cmos_sensor(0x0812,0x1e);
XC9160_write_cmos_sensor(0x0813,0x1e);
XC9160_write_cmos_sensor(0x0814,0x1f);
XC9160_write_cmos_sensor(0x0815,0x1e);
XC9160_write_cmos_sensor(0x0816,0x1d);
XC9160_write_cmos_sensor(0x0817,0x1d);
XC9160_write_cmos_sensor(0x0818,0x1d);
XC9160_write_cmos_sensor(0x0819,0x1d);
XC9160_write_cmos_sensor(0x081a,0x1d);
XC9160_write_cmos_sensor(0x081b,0x1e);
XC9160_write_cmos_sensor(0x081c,0x1f);
XC9160_write_cmos_sensor(0x081d,0x1b);
XC9160_write_cmos_sensor(0x081e,0x1e);
XC9160_write_cmos_sensor(0x081f,0x1e);
XC9160_write_cmos_sensor(0x0820,0x1e);
XC9160_write_cmos_sensor(0x0821,0x1e);
XC9160_write_cmos_sensor(0x0822,0x1e);
XC9160_write_cmos_sensor(0x0823,0x1e);
XC9160_write_cmos_sensor(0x0824,0x21);
XC9160_write_cmos_sensor(0x0825,0x20);
XC9160_write_cmos_sensor(0x0826,0x24);
XC9160_write_cmos_sensor(0x0827,0x24);
XC9160_write_cmos_sensor(0x0828,0x24);
XC9160_write_cmos_sensor(0x0829,0x25);
XC9160_write_cmos_sensor(0x082a,0x24);
XC9160_write_cmos_sensor(0x082b,0x24);
XC9160_write_cmos_sensor(0x082c,0x25);
XC9160_write_cmos_sensor(0x082d,0x23);
XC9160_write_cmos_sensor(0x082e,0x23);
XC9160_write_cmos_sensor(0x082f,0x22);
XC9160_write_cmos_sensor(0x0830,0x22);
XC9160_write_cmos_sensor(0x0831,0x22);
XC9160_write_cmos_sensor(0x0832,0x22);
XC9160_write_cmos_sensor(0x0833,0x23);
XC9160_write_cmos_sensor(0x0834,0x26);
XC9160_write_cmos_sensor(0x0835,0x21);
XC9160_write_cmos_sensor(0x0836,0x22);
XC9160_write_cmos_sensor(0x0837,0x22);
XC9160_write_cmos_sensor(0x0838,0x21);
XC9160_write_cmos_sensor(0x0839,0x21);
XC9160_write_cmos_sensor(0x083a,0x22);
XC9160_write_cmos_sensor(0x083b,0x23);
XC9160_write_cmos_sensor(0x083c,0x24);
XC9160_write_cmos_sensor(0x083d,0x21);
XC9160_write_cmos_sensor(0x083e,0x20);
XC9160_write_cmos_sensor(0x083f,0x20);
XC9160_write_cmos_sensor(0x0840,0x20);
XC9160_write_cmos_sensor(0x0841,0x20);
XC9160_write_cmos_sensor(0x0842,0x20);
XC9160_write_cmos_sensor(0x0843,0x21);
XC9160_write_cmos_sensor(0x0844,0x24);
XC9160_write_cmos_sensor(0x0845,0x21);
XC9160_write_cmos_sensor(0x0846,0x20);
XC9160_write_cmos_sensor(0x0847,0x20);
XC9160_write_cmos_sensor(0x0848,0x20);
XC9160_write_cmos_sensor(0x0849,0x20);
XC9160_write_cmos_sensor(0x084a,0x20);
XC9160_write_cmos_sensor(0x084b,0x21);
XC9160_write_cmos_sensor(0x084c,0x23);
XC9160_write_cmos_sensor(0x084d,0x21);
XC9160_write_cmos_sensor(0x084e,0x22);
XC9160_write_cmos_sensor(0x084f,0x21);
XC9160_write_cmos_sensor(0x0850,0x20);
XC9160_write_cmos_sensor(0x0851,0x20);
XC9160_write_cmos_sensor(0x0852,0x21);
XC9160_write_cmos_sensor(0x0853,0x22);
XC9160_write_cmos_sensor(0x0854,0x24);
XC9160_write_cmos_sensor(0x0855,0x23);
XC9160_write_cmos_sensor(0x0856,0x22);
XC9160_write_cmos_sensor(0x0857,0x21);
XC9160_write_cmos_sensor(0x0858,0x20);
XC9160_write_cmos_sensor(0x0859,0x20);
XC9160_write_cmos_sensor(0x085a,0x21);
XC9160_write_cmos_sensor(0x085b,0x22);
XC9160_write_cmos_sensor(0x085c,0x24);
XC9160_write_cmos_sensor(0x085d,0x21);
XC9160_write_cmos_sensor(0x085e,0x21);
XC9160_write_cmos_sensor(0x085f,0x21);
XC9160_write_cmos_sensor(0x0860,0x21);
XC9160_write_cmos_sensor(0x0861,0x21);
XC9160_write_cmos_sensor(0x0862,0x21);
XC9160_write_cmos_sensor(0x0863,0x21);
XC9160_write_cmos_sensor(0x0864,0x22);

XC9160_write_cmos_sensor(0x0865,0x3f);         //d_93% 20170704  4208x3120
XC9160_write_cmos_sensor(0x0866,0x33); 
XC9160_write_cmos_sensor(0x0867,0x2b); 
XC9160_write_cmos_sensor(0x0868,0x24); 
XC9160_write_cmos_sensor(0x0869,0x25); 
XC9160_write_cmos_sensor(0x086a,0x2b); 
XC9160_write_cmos_sensor(0x086b,0x34); 
XC9160_write_cmos_sensor(0x086c,0x3f); 
XC9160_write_cmos_sensor(0x086d,0x29); 
XC9160_write_cmos_sensor(0x086e,0x1d); 
XC9160_write_cmos_sensor(0x086f,0x15); 
XC9160_write_cmos_sensor(0x0870,0x12); 
XC9160_write_cmos_sensor(0x0871,0x12); 
XC9160_write_cmos_sensor(0x0872,0x14); 
XC9160_write_cmos_sensor(0x0873,0x1c); 
XC9160_write_cmos_sensor(0x0874,0x29); 
XC9160_write_cmos_sensor(0x0875,0x1a); 
XC9160_write_cmos_sensor(0x0876,0x0f); 
XC9160_write_cmos_sensor(0x0877,0x0a); 
XC9160_write_cmos_sensor(0x0878,0x06); 
XC9160_write_cmos_sensor(0x0879,0x06); 
XC9160_write_cmos_sensor(0x087a,0x09); 
XC9160_write_cmos_sensor(0x087b,0x0f); 
XC9160_write_cmos_sensor(0x087c,0x18); 
XC9160_write_cmos_sensor(0x087d,0x13); 
XC9160_write_cmos_sensor(0x087e,0x0a); 
XC9160_write_cmos_sensor(0x087f,0x03); 
XC9160_write_cmos_sensor(0x0880,0x00); 
XC9160_write_cmos_sensor(0x0881,0x00); 
XC9160_write_cmos_sensor(0x0882,0x03); 
XC9160_write_cmos_sensor(0x0883,0x0a); 
XC9160_write_cmos_sensor(0x0884,0x12); 
XC9160_write_cmos_sensor(0x0885,0x13); 
XC9160_write_cmos_sensor(0x0886,0x0a); 
XC9160_write_cmos_sensor(0x0887,0x03); 
XC9160_write_cmos_sensor(0x0888,0x00); 
XC9160_write_cmos_sensor(0x0889,0x00); 
XC9160_write_cmos_sensor(0x088a,0x03); 
XC9160_write_cmos_sensor(0x088b,0x09); 
XC9160_write_cmos_sensor(0x088c,0x12); 
XC9160_write_cmos_sensor(0x088d,0x1a); 
XC9160_write_cmos_sensor(0x088e,0x0f); 
XC9160_write_cmos_sensor(0x088f,0x09); 
XC9160_write_cmos_sensor(0x0890,0x06); 
XC9160_write_cmos_sensor(0x0891,0x06); 
XC9160_write_cmos_sensor(0x0892,0x09); 
XC9160_write_cmos_sensor(0x0893,0x0f); 
XC9160_write_cmos_sensor(0x0894,0x18); 
XC9160_write_cmos_sensor(0x0895,0x29); 
XC9160_write_cmos_sensor(0x0896,0x1d); 
XC9160_write_cmos_sensor(0x0897,0x14); 
XC9160_write_cmos_sensor(0x0898,0x11); 
XC9160_write_cmos_sensor(0x0899,0x11); 
XC9160_write_cmos_sensor(0x089a,0x14); 
XC9160_write_cmos_sensor(0x089b,0x1c); 
XC9160_write_cmos_sensor(0x089c,0x29); 
XC9160_write_cmos_sensor(0x089d,0x3f); 
XC9160_write_cmos_sensor(0x089e,0x35); 
XC9160_write_cmos_sensor(0x089f,0x2d); 
XC9160_write_cmos_sensor(0x08a0,0x26); 
XC9160_write_cmos_sensor(0x08a1,0x26); 
XC9160_write_cmos_sensor(0x08a2,0x2c); 
XC9160_write_cmos_sensor(0x08a3,0x35); 
XC9160_write_cmos_sensor(0x08a4,0x3f); 
XC9160_write_cmos_sensor(0x08a5,0x1f); 
XC9160_write_cmos_sensor(0x08a6,0x1d); 
XC9160_write_cmos_sensor(0x08a7,0x1e); 
XC9160_write_cmos_sensor(0x08a8,0x1d); 
XC9160_write_cmos_sensor(0x08a9,0x1d); 
XC9160_write_cmos_sensor(0x08aa,0x1e); 
XC9160_write_cmos_sensor(0x08ab,0x1e); 
XC9160_write_cmos_sensor(0x08ac,0x20); 
XC9160_write_cmos_sensor(0x08ad,0x1d); 
XC9160_write_cmos_sensor(0x08ae,0x1e); 
XC9160_write_cmos_sensor(0x08af,0x1d); 
XC9160_write_cmos_sensor(0x08b0,0x1d); 
XC9160_write_cmos_sensor(0x08b1,0x1e); 
XC9160_write_cmos_sensor(0x08b2,0x1d); 
XC9160_write_cmos_sensor(0x08b3,0x1e); 
XC9160_write_cmos_sensor(0x08b4,0x1e); 
XC9160_write_cmos_sensor(0x08b5,0x1e); 
XC9160_write_cmos_sensor(0x08b6,0x1e); 
XC9160_write_cmos_sensor(0x08b7,0x1e); 
XC9160_write_cmos_sensor(0x08b8,0x1e); 
XC9160_write_cmos_sensor(0x08b9,0x1e); 
XC9160_write_cmos_sensor(0x08ba,0x1e); 
XC9160_write_cmos_sensor(0x08bb,0x1e); 
XC9160_write_cmos_sensor(0x08bc,0x1e); 
XC9160_write_cmos_sensor(0x08bd,0x1e); 
XC9160_write_cmos_sensor(0x08be,0x1d); 
XC9160_write_cmos_sensor(0x08bf,0x1f); 
XC9160_write_cmos_sensor(0x08c0,0x20); 
XC9160_write_cmos_sensor(0x08c1,0x20); 
XC9160_write_cmos_sensor(0x08c2,0x1f); 
XC9160_write_cmos_sensor(0x08c3,0x1d); 
XC9160_write_cmos_sensor(0x08c4,0x1f); 
XC9160_write_cmos_sensor(0x08c5,0x1e); 
XC9160_write_cmos_sensor(0x08c6,0x1d); 
XC9160_write_cmos_sensor(0x08c7,0x1e); 
XC9160_write_cmos_sensor(0x08c8,0x20); 
XC9160_write_cmos_sensor(0x08c9,0x20); 
XC9160_write_cmos_sensor(0x08ca,0x1f); 
XC9160_write_cmos_sensor(0x08cb,0x1e); 
XC9160_write_cmos_sensor(0x08cc,0x1e); 
XC9160_write_cmos_sensor(0x08cd,0x1e); 
XC9160_write_cmos_sensor(0x08ce,0x1e); 
XC9160_write_cmos_sensor(0x08cf,0x1d); 
XC9160_write_cmos_sensor(0x08d0,0x1e); 
XC9160_write_cmos_sensor(0x08d1,0x1e); 
XC9160_write_cmos_sensor(0x08d2,0x1e); 
XC9160_write_cmos_sensor(0x08d3,0x1e); 
XC9160_write_cmos_sensor(0x08d4,0x1e); 
XC9160_write_cmos_sensor(0x08d5,0x1e); 
XC9160_write_cmos_sensor(0x08d6,0x1d); 
XC9160_write_cmos_sensor(0x08d7,0x1d); 
XC9160_write_cmos_sensor(0x08d8,0x1d); 
XC9160_write_cmos_sensor(0x08d9,0x1d); 
XC9160_write_cmos_sensor(0x08da,0x1d); 
XC9160_write_cmos_sensor(0x08db,0x1e); 
XC9160_write_cmos_sensor(0x08dc,0x1e); 
XC9160_write_cmos_sensor(0x08dd,0x1b); 
XC9160_write_cmos_sensor(0x08de,0x1f); 
XC9160_write_cmos_sensor(0x08df,0x1d); 
XC9160_write_cmos_sensor(0x08e0,0x1d); 
XC9160_write_cmos_sensor(0x08e1,0x1d); 
XC9160_write_cmos_sensor(0x08e2,0x1e); 
XC9160_write_cmos_sensor(0x08e3,0x1e); 
XC9160_write_cmos_sensor(0x08e4,0x20); 
XC9160_write_cmos_sensor(0x08e5,0x23); 
XC9160_write_cmos_sensor(0x08e6,0x29); 
XC9160_write_cmos_sensor(0x08e7,0x28); 
XC9160_write_cmos_sensor(0x08e8,0x29); 
XC9160_write_cmos_sensor(0x08e9,0x29); 
XC9160_write_cmos_sensor(0x08ea,0x28); 
XC9160_write_cmos_sensor(0x08eb,0x29); 
XC9160_write_cmos_sensor(0x08ec,0x27); 
XC9160_write_cmos_sensor(0x08ed,0x28); 
XC9160_write_cmos_sensor(0x08ee,0x28); 
XC9160_write_cmos_sensor(0x08ef,0x28); 
XC9160_write_cmos_sensor(0x08f0,0x27); 
XC9160_write_cmos_sensor(0x08f1,0x27); 
XC9160_write_cmos_sensor(0x08f2,0x28); 
XC9160_write_cmos_sensor(0x08f3,0x28); 
XC9160_write_cmos_sensor(0x08f4,0x2a); 
XC9160_write_cmos_sensor(0x08f5,0x27); 
XC9160_write_cmos_sensor(0x08f6,0x27); 
XC9160_write_cmos_sensor(0x08f7,0x26); 
XC9160_write_cmos_sensor(0x08f8,0x24); 
XC9160_write_cmos_sensor(0x08f9,0x24); 
XC9160_write_cmos_sensor(0x08fa,0x26); 
XC9160_write_cmos_sensor(0x08fb,0x28); 
XC9160_write_cmos_sensor(0x08fc,0x29); 
XC9160_write_cmos_sensor(0x08fd,0x27); 
XC9160_write_cmos_sensor(0x08fe,0x24); 
XC9160_write_cmos_sensor(0x08ff,0x22); 
XC9160_write_cmos_sensor(0x0900,0x20); 
XC9160_write_cmos_sensor(0x0901,0x20); 
XC9160_write_cmos_sensor(0x0902,0x22); 
XC9160_write_cmos_sensor(0x0903,0x25); 
XC9160_write_cmos_sensor(0x0904,0x29); 
XC9160_write_cmos_sensor(0x0905,0x26); 
XC9160_write_cmos_sensor(0x0906,0x24); 
XC9160_write_cmos_sensor(0x0907,0x22); 
XC9160_write_cmos_sensor(0x0908,0x20); 
XC9160_write_cmos_sensor(0x0909,0x20); 
XC9160_write_cmos_sensor(0x090a,0x22); 
XC9160_write_cmos_sensor(0x090b,0x25); 
XC9160_write_cmos_sensor(0x090c,0x29); 
XC9160_write_cmos_sensor(0x090d,0x26); 
XC9160_write_cmos_sensor(0x090e,0x27); 
XC9160_write_cmos_sensor(0x090f,0x25); 
XC9160_write_cmos_sensor(0x0910,0x23); 
XC9160_write_cmos_sensor(0x0911,0x23); 
XC9160_write_cmos_sensor(0x0912,0x25); 
XC9160_write_cmos_sensor(0x0913,0x27); 
XC9160_write_cmos_sensor(0x0914,0x29); 
XC9160_write_cmos_sensor(0x0915,0x28); 
XC9160_write_cmos_sensor(0x0916,0x27); 
XC9160_write_cmos_sensor(0x0917,0x27); 
XC9160_write_cmos_sensor(0x0918,0x26); 
XC9160_write_cmos_sensor(0x0919,0x26); 
XC9160_write_cmos_sensor(0x091a,0x26); 
XC9160_write_cmos_sensor(0x091b,0x27); 
XC9160_write_cmos_sensor(0x091c,0x28); 
XC9160_write_cmos_sensor(0x091d,0x24); 
XC9160_write_cmos_sensor(0x091e,0x26); 
XC9160_write_cmos_sensor(0x091f,0x25); 
XC9160_write_cmos_sensor(0x0920,0x26); 
XC9160_write_cmos_sensor(0x0921,0x26); 
XC9160_write_cmos_sensor(0x0922,0x26); 
XC9160_write_cmos_sensor(0x0923,0x25); 
XC9160_write_cmos_sensor(0x0924,0x26); 

XC9160_write_cmos_sensor(0x03c4,0x07);//auto_g mode disable//6 add by diana_yang 11.16//7 changed by diana_yang 1116
XC9160_write_cmos_sensor(0x03c6,0x01);
XC9160_write_cmos_sensor(0x03c7,0x00);// auto_g max gain changed by diana_yang 1116
XC9160_write_cmos_sensor(0x03c8,0x00);
XC9160_write_cmos_sensor(0x03c9,0x90);//auto_g min gain changed by diana_yang 1116
XC9160_write_cmos_sensor(0x03c2,0x20);//minq
XC9160_write_cmos_sensor(0x03c3,0x40);//maxq

XC9160_write_cmos_sensor(0xfffe,0x30);    //lenc skip
XC9160_write_cmos_sensor(0x0012,0x95);            
XC9160_write_cmos_sensor(0x000e,0x00);            
XC9160_write_cmos_sensor(0x000f,0x04);            
XC9160_write_cmos_sensor(0x0010,0x00);           
XC9160_write_cmos_sensor(0x0011,0x00);    

//ISP0 AWB	0627
XC9160_write_cmos_sensor(0xfffe,0x14); 
XC9160_write_cmos_sensor(0x0248,0x02);//0. AWB_ARITH_ORIGIN21 1. AWB_ARITH_SW_PRO 2. AWB_ARITH_M
XC9160_write_cmos_sensor(0x0282,0x06);   //int B gain
XC9160_write_cmos_sensor(0x0283,0x00);  
XC9160_write_cmos_sensor(0x0286,0x04);   //int Gb gain
XC9160_write_cmos_sensor(0x0287,0x00); 
XC9160_write_cmos_sensor(0x028a,0x04);   //int Gr gain
XC9160_write_cmos_sensor(0x028b,0x00);
XC9160_write_cmos_sensor(0x028e,0x04);   //int R gain
XC9160_write_cmos_sensor(0x028f,0x04);
XC9160_write_cmos_sensor(0x02b6,0x06);    //B_temp      
XC9160_write_cmos_sensor(0x02b7,0x00);           
XC9160_write_cmos_sensor(0x02ba,0x04);    //G_ temp          
XC9160_write_cmos_sensor(0x02bb,0x00);      
XC9160_write_cmos_sensor(0x02be,0x04);   //R_temp      
XC9160_write_cmos_sensor(0x02bf,0x04);

XC9160_write_cmos_sensor(0xfffe,0x14);
XC9160_write_cmos_sensor(0x0248,0x01);//0. AWB_ARITH_ORIGIN21 1. AWB_ARITH_SW_PRO 2. AWB_ARITH_MANUAL
XC9160_write_cmos_sensor(0x0249,0x01);//AWBFlexiMap_en
XC9160_write_cmos_sensor(0x024a,0x00);//AWBMove_en
XC9160_write_cmos_sensor(0x027a,0x08);//nCTBasedMinNum    //20
XC9160_write_cmos_sensor(0x027b,0x00);
XC9160_write_cmos_sensor(0x027c,0x0f);
XC9160_write_cmos_sensor(0x027d,0xff);//nMaxAWBGain

XC9160_write_cmos_sensor(0xfffe,0x30);
XC9160_write_cmos_sensor(0x0708,0x02); //pixel max value[9:8]  //03
XC9160_write_cmos_sensor(0x0709,0xa0); //pixel max value[7:0]  //f0
XC9160_write_cmos_sensor(0x070a,0x00); //pixel min value[9:8] 
XC9160_write_cmos_sensor(0x070b,0x10); //pixel min value[7:0] 
XC9160_write_cmos_sensor(0x071c,0x0a); //simple awb
//XC9160_write_cmos_sensor(0x0003,0x11);   //bit[4]:awb_en  bit[0]:awb_gain_en

XC9160_write_cmos_sensor(0xfffe,0x30);  //3l8_1_0630
XC9160_write_cmos_sensor(0x0730,0x95);  // win1 startx
XC9160_write_cmos_sensor(0x0731,0xb5);  // win1 endx
XC9160_write_cmos_sensor(0x0732,0x49);  // win1 starty
XC9160_write_cmos_sensor(0x0733,0x58);  // win1 endy
XC9160_write_cmos_sensor(0x0734,0x8a);  // win2 startx
XC9160_write_cmos_sensor(0x0735,0x96);  // win2 endx
XC9160_write_cmos_sensor(0x0736,0x70);  // win2 starty
XC9160_write_cmos_sensor(0x0737,0x7e);  // win2 endy
XC9160_write_cmos_sensor(0x0738,0x52);  // win3 startx
XC9160_write_cmos_sensor(0x0739,0x63);  // win3 endx
XC9160_write_cmos_sensor(0x073a,0x7d);  // win3 starty
XC9160_write_cmos_sensor(0x073b,0x9b);  // win3 endy
XC9160_write_cmos_sensor(0x073c,0x78);  // win4 startx
XC9160_write_cmos_sensor(0x073d,0x91);  // win4 endx
XC9160_write_cmos_sensor(0x073e,0x64);  // win4 starty
XC9160_write_cmos_sensor(0x073f,0x71);  // win4 endy
XC9160_write_cmos_sensor(0x0740,0x60);  // win5 startx
XC9160_write_cmos_sensor(0x0741,0x72);  // win5 endx
XC9160_write_cmos_sensor(0x0742,0x77);  // win5 starty
XC9160_write_cmos_sensor(0x0743,0x9a);  // win5 endy
XC9160_write_cmos_sensor(0x0744,0x51);  // win6 startx
XC9160_write_cmos_sensor(0x0745,0x59);  // win6 endx
XC9160_write_cmos_sensor(0x0746,0x9a);  // win6 starty
XC9160_write_cmos_sensor(0x0747,0xaa);  // win6 endy
XC9160_write_cmos_sensor(0x0748,0x65);  // win7 startx
XC9160_write_cmos_sensor(0x0749,0x8c);  // win7 endx
XC9160_write_cmos_sensor(0x074a,0x70);  // win7 starty
XC9160_write_cmos_sensor(0x074b,0x78);  // win7 endy
XC9160_write_cmos_sensor(0x074c,0x8c);  // win8 startx
XC9160_write_cmos_sensor(0x074d,0xa0);  // win8 endx
XC9160_write_cmos_sensor(0x074e,0x56);  // win8 starty
XC9160_write_cmos_sensor(0x074f,0x66);  // win8 endy
XC9160_write_cmos_sensor(0x0750,0x9f);  // win9 startx
XC9160_write_cmos_sensor(0x0751,0xb4);  // win9 endx
XC9160_write_cmos_sensor(0x0752,0x56);  // win9 starty
XC9160_write_cmos_sensor(0x0753,0x5d);  // win9 endy
XC9160_write_cmos_sensor(0x0754,0x70);  // win10 startx
XC9160_write_cmos_sensor(0x0755,0x78);  // win10 endx
XC9160_write_cmos_sensor(0x0756,0x77);  // win10 starty
XC9160_write_cmos_sensor(0x0757,0x84);  // win10 endy
XC9160_write_cmos_sensor(0x0758,0x00);  // win11 startx
XC9160_write_cmos_sensor(0x0759,0x00);  // win11 endx
XC9160_write_cmos_sensor(0x075a,0x00);  // win11 starty
XC9160_write_cmos_sensor(0x075b,0x00);  // win11 endy
XC9160_write_cmos_sensor(0x075c,0x00);  // win12 startx
XC9160_write_cmos_sensor(0x075d,0x00);  // win12 endx
XC9160_write_cmos_sensor(0x075e,0x00);  // win12 starty
XC9160_write_cmos_sensor(0x075f,0x00);  // win12 endy
XC9160_write_cmos_sensor(0x0760,0x00);  // win13 startx
XC9160_write_cmos_sensor(0x0761,0x00);  // win13 endx
XC9160_write_cmos_sensor(0x0762,0x00);  // win13 starty
XC9160_write_cmos_sensor(0x0763,0x00);  // win13 endy
XC9160_write_cmos_sensor(0x0764,0x00);  // win14 startx
XC9160_write_cmos_sensor(0x0765,0x00);  // win14 endx
XC9160_write_cmos_sensor(0x0766,0x00);  // win14 starty
XC9160_write_cmos_sensor(0x0767,0x00);  // win14 endy
XC9160_write_cmos_sensor(0x0768,0x00);  // win15 startx
XC9160_write_cmos_sensor(0x0769,0x00);  // win15 endx
XC9160_write_cmos_sensor(0x076a,0x00);  // win15 starty
XC9160_write_cmos_sensor(0x076b,0x00);  // win15 endy
XC9160_write_cmos_sensor(0x076c,0x00);  // win16 startx
XC9160_write_cmos_sensor(0x076d,0x00);  // win16 endx
XC9160_write_cmos_sensor(0x076e,0x00);  // win16 starty
XC9160_write_cmos_sensor(0x076f,0x00);  // win16 endy
XC9160_write_cmos_sensor(0x0770,0x21);  // wt1 wt2
XC9160_write_cmos_sensor(0x0771,0x31);  // wt3 wt4
XC9160_write_cmos_sensor(0x0772,0x31);  // wt5 wt6
XC9160_write_cmos_sensor(0x0773,0x11);  // wt7 wt8
XC9160_write_cmos_sensor(0x0774,0x11);  // wt9 wt10
XC9160_write_cmos_sensor(0x0775,0x00);  // wt11 wt12
XC9160_write_cmos_sensor(0x0776,0x00);  // wt13 wt14
XC9160_write_cmos_sensor(0x0777,0x00);  // wt15 wt16


//Gamma  0628
XC9160_write_cmos_sensor(0xfffe,0x30);  //3l8_33
//XC9160_write_cmos_sensor(0x0000,0x40);   //bit[6]:rgb_gamma_en
XC9160_write_cmos_sensor(0x1400,0x00);
XC9160_write_cmos_sensor(0x1401,0x00);
XC9160_write_cmos_sensor(0x1402,0x00);
XC9160_write_cmos_sensor(0x1403,0x00);
XC9160_write_cmos_sensor(0x1404,0x00);
XC9160_write_cmos_sensor(0x1405,0x00);
XC9160_write_cmos_sensor(0x1406,0x00);
XC9160_write_cmos_sensor(0x1407,0x00);
XC9160_write_cmos_sensor(0x1408,0x00);
XC9160_write_cmos_sensor(0x1409,0x06);
XC9160_write_cmos_sensor(0x140a,0x0c);
XC9160_write_cmos_sensor(0x140b,0x12);
XC9160_write_cmos_sensor(0x140c,0x18);
XC9160_write_cmos_sensor(0x140d,0x1e);
XC9160_write_cmos_sensor(0x140e,0x24);
XC9160_write_cmos_sensor(0x140f,0x29);
XC9160_write_cmos_sensor(0x1410,0x2f);
XC9160_write_cmos_sensor(0x1411,0x35);
XC9160_write_cmos_sensor(0x1412,0x3a);
XC9160_write_cmos_sensor(0x1413,0x3f);
XC9160_write_cmos_sensor(0x1414,0x44);
XC9160_write_cmos_sensor(0x1415,0x49);
XC9160_write_cmos_sensor(0x1416,0x4e);
XC9160_write_cmos_sensor(0x1417,0x53);
XC9160_write_cmos_sensor(0x1418,0x57);
XC9160_write_cmos_sensor(0x1419,0x5c);
XC9160_write_cmos_sensor(0x141a,0x60);
XC9160_write_cmos_sensor(0x141b,0x64);
XC9160_write_cmos_sensor(0x141c,0x68);
XC9160_write_cmos_sensor(0x141d,0x6c);
XC9160_write_cmos_sensor(0x141e,0x70);
XC9160_write_cmos_sensor(0x141f,0x73);
XC9160_write_cmos_sensor(0x1420,0x77);
XC9160_write_cmos_sensor(0x1421,0x7e);
XC9160_write_cmos_sensor(0x1422,0x84);
XC9160_write_cmos_sensor(0x1423,0x8a);
XC9160_write_cmos_sensor(0x1424,0x8f);
XC9160_write_cmos_sensor(0x1425,0x95);
XC9160_write_cmos_sensor(0x1426,0x9a);
XC9160_write_cmos_sensor(0x1427,0x9f);
XC9160_write_cmos_sensor(0x1428,0xa4);
XC9160_write_cmos_sensor(0x1429,0xa8);
XC9160_write_cmos_sensor(0x142a,0xad);
XC9160_write_cmos_sensor(0x142b,0xb1);
XC9160_write_cmos_sensor(0x142c,0xb6);
XC9160_write_cmos_sensor(0x142d,0xba);
XC9160_write_cmos_sensor(0x142e,0xbe);
XC9160_write_cmos_sensor(0x142f,0xc2);
XC9160_write_cmos_sensor(0x1430,0xc6);
XC9160_write_cmos_sensor(0x1431,0xcd);
XC9160_write_cmos_sensor(0x1432,0xd3);
XC9160_write_cmos_sensor(0x1433,0xd9);
XC9160_write_cmos_sensor(0x1434,0xde);
XC9160_write_cmos_sensor(0x1435,0xe3);
XC9160_write_cmos_sensor(0x1436,0xe7);
XC9160_write_cmos_sensor(0x1437,0xeb);
XC9160_write_cmos_sensor(0x1438,0xef);
XC9160_write_cmos_sensor(0x1439,0xf2);
XC9160_write_cmos_sensor(0x143a,0xf4);
XC9160_write_cmos_sensor(0x143b,0xf7);
XC9160_write_cmos_sensor(0x143c,0xf9);
XC9160_write_cmos_sensor(0x143d,0xfb);
XC9160_write_cmos_sensor(0x143e,0xfc);
XC9160_write_cmos_sensor(0x143f,0xfe);
XC9160_write_cmos_sensor(0x1440,0xff);

XC9160_write_cmos_sensor(0x1450,0xff);
XC9160_write_cmos_sensor(0x1451,0x03);
XC9160_write_cmos_sensor(0x1452,0x80);
XC9160_write_cmos_sensor(0x1453,0x80);

//ISP0 CMX  0620 
XC9160_write_cmos_sensor(0xfffe,0x30);  //CMX_0623_ct
//XC9160_write_cmos_sensor(0x0001,0x02);   //bit[1]:cmx_en
XC9160_write_cmos_sensor(0x1200,0x02);	
XC9160_write_cmos_sensor(0x1201,0x78);	
XC9160_write_cmos_sensor(0x1202,0x00);	
XC9160_write_cmos_sensor(0x1203,0x71);	
XC9160_write_cmos_sensor(0x1204,0x02);	
XC9160_write_cmos_sensor(0x1205,0x18);	
XC9160_write_cmos_sensor(0x1206,0x01);	
XC9160_write_cmos_sensor(0x1207,0xD6);	
XC9160_write_cmos_sensor(0x1208,0x00);	
XC9160_write_cmos_sensor(0x1209,0xA7);	
XC9160_write_cmos_sensor(0x120A,0x03);	
XC9160_write_cmos_sensor(0x120B,0x34);	
XC9160_write_cmos_sensor(0x120C,0x05);	
XC9160_write_cmos_sensor(0x120D,0xD7);	
XC9160_write_cmos_sensor(0x120E,0x00);	
XC9160_write_cmos_sensor(0x120F,0x7A);	
XC9160_write_cmos_sensor(0x1210,0x01);	
XC9160_write_cmos_sensor(0x1211,0x11);	
XC9160_write_cmos_sensor(0x1212,0x02);	
XC9160_write_cmos_sensor(0x1213,0x54);	
XC9160_write_cmos_sensor(0x1214,0x00);	
XC9160_write_cmos_sensor(0x1215,0x50);	
XC9160_write_cmos_sensor(0x1216,0x01);	
XC9160_write_cmos_sensor(0x1217,0x66);	
XC9160_write_cmos_sensor(0x1218,0x03);	
XC9160_write_cmos_sensor(0x1219,0x97);	
XC9160_write_cmos_sensor(0x121A,0x00);	
XC9160_write_cmos_sensor(0x121B,0x68);	
XC9160_write_cmos_sensor(0x121C,0x00);	
XC9160_write_cmos_sensor(0x121D,0xCC);	
XC9160_write_cmos_sensor(0x121E,0x02);	
XC9160_write_cmos_sensor(0x121F,0x4B);	
XC9160_write_cmos_sensor(0x1220,0x00);	
XC9160_write_cmos_sensor(0x1221,0x58);	
XC9160_write_cmos_sensor(0x1222,0x04);	
XC9160_write_cmos_sensor(0x1223,0x56);	
XC9160_write_cmos_sensor(0x122e,0x00);	
XC9160_write_cmos_sensor(0x122F,0x02);	
XC9160_write_cmos_sensor(0x1230,0x00);	
XC9160_write_cmos_sensor(0x1228,0x00);	
XC9160_write_cmos_sensor(0x1229,0x4E);	
XC9160_write_cmos_sensor(0x122A,0x00);	
XC9160_write_cmos_sensor(0x122B,0x8C);	
XC9160_write_cmos_sensor(0x122C,0x00);	
XC9160_write_cmos_sensor(0x122D,0xF6);	

XC9160_write_cmos_sensor(0x1231,0x02);
//XC9160_write_cmos_sensor(0x1232,0x50);
//dns&cip
XC9160_write_cmos_sensor(0xfffe,0x30);                                                                                            
//XC9160_write_cmos_sensor(0x0002,0x02);   //bit[1]advance_raw_dns_en                                                             
                                                                                                        
XC9160_write_cmos_sensor(0x2000,0x06);  //s0_sigma_0                                                                              
XC9160_write_cmos_sensor(0x2001,0x08);                                                                                            
XC9160_write_cmos_sensor(0x2002,0x0c);                                                                                            
XC9160_write_cmos_sensor(0x2003,0x10);                                                                                            
XC9160_write_cmos_sensor(0x2004,0x14);                                                                                            
XC9160_write_cmos_sensor(0x2005,0x18);  //s0_sigma_5                                                                              
                                                                                                        
XC9160_write_cmos_sensor(0x2006,0x01);  //s0_Gsl_0                                                                                
XC9160_write_cmos_sensor(0x2007,0x01);                                                                                            
XC9160_write_cmos_sensor(0x2008,0x01);                                                                                            
XC9160_write_cmos_sensor(0x2009,0x01);                                                                                            
XC9160_write_cmos_sensor(0x200a,0x01);                                                                                            
XC9160_write_cmos_sensor(0x200b,0x01);  //s0_Gsl_5                                                                                
                                                                                                        
XC9160_write_cmos_sensor(0x200c,0x06);  //s0_RBsl_0                                                                               
XC9160_write_cmos_sensor(0x200d,0x04);                                                                                            
XC9160_write_cmos_sensor(0x200e,0x02);                                                                                            
XC9160_write_cmos_sensor(0x200f,0x01);                                                                                            
XC9160_write_cmos_sensor(0x2010,0x01);                                                                                            
XC9160_write_cmos_sensor(0x2011,0x01);   //s0_RBsl_5                                                                              
                                                                                                        
XC9160_write_cmos_sensor(0x2012,0x04);   //s0_ps00    //sigma_scale 大                                                            
XC9160_write_cmos_sensor(0x2013,0x06);                                                                                            
XC9160_write_cmos_sensor(0x2014,0x10);                                                                                                
XC9160_write_cmos_sensor(0x2015,0x18);                                                                                            
XC9160_write_cmos_sensor(0x2016,0x20);                                                                                            
XC9160_write_cmos_sensor(0x2017,0x24);   //s0_ps05                                                                                
                                                                                                        
XC9160_write_cmos_sensor(0x2018,0x14);   //s0_ps10                                                                                
XC9160_write_cmos_sensor(0x2019,0x1a);                                                                                            
XC9160_write_cmos_sensor(0x201a,0x24);                                                                                            
XC9160_write_cmos_sensor(0x201b,0x24);                                                                                            
XC9160_write_cmos_sensor(0x201c,0x24);                                                                                            
XC9160_write_cmos_sensor(0x201d,0x24);  //s0_ps15                                                                                 
                                                                                                        
XC9160_write_cmos_sensor(0x201e,0x24);  //s0_ps20                                                                                 
XC9160_write_cmos_sensor(0x201f,0x24);                                                                                            
XC9160_write_cmos_sensor(0x2020,0x22);                                                                                            
XC9160_write_cmos_sensor(0x2021,0x22);                                                                                            
XC9160_write_cmos_sensor(0x2022,0x22);                                                                                            
XC9160_write_cmos_sensor(0x2023,0x22);   //s0_ps25                                                                                
                                                                                                        
XC9160_write_cmos_sensor(0x2024,0x38);   //s0_ps30                                                                                
XC9160_write_cmos_sensor(0x2025,0x28);                                                                                            
XC9160_write_cmos_sensor(0x2026,0x24);                                                                                            
XC9160_write_cmos_sensor(0x2027,0x22);                                                                                            
XC9160_write_cmos_sensor(0x2028,0x20);                                                                                            
XC9160_write_cmos_sensor(0x2029,0x20);   //s0_ps35                                                                                
                                                                                                        
XC9160_write_cmos_sensor(0x202a,0x40);    //s0_pl00     //h2_scale 小                                                             
XC9160_write_cmos_sensor(0x202b,0x40);                                                                                            
XC9160_write_cmos_sensor(0x202c,0x30);                                                                                            
XC9160_write_cmos_sensor(0x202d,0x20);                                                                                            
XC9160_write_cmos_sensor(0x202e,0x20);                                                                                            
XC9160_write_cmos_sensor(0x202f,0x20);   //s0_pl05                                                                                
                                                                                                        
XC9160_write_cmos_sensor(0x2030,0x20);   //s0_pl10                                                                                
XC9160_write_cmos_sensor(0x2031,0x20);                                                                                            
XC9160_write_cmos_sensor(0x2032,0x28);                                                                                            
XC9160_write_cmos_sensor(0x2033,0x20);                                                                                            
XC9160_write_cmos_sensor(0x2034,0x20);                                                                                            
XC9160_write_cmos_sensor(0x2035,0x20);   //s0_pl15                                                                                
                                                                                                        
XC9160_write_cmos_sensor(0x2036,0x18);  //s0_pl20                                                                                 
XC9160_write_cmos_sensor(0x2037,0x18);                                                                                            
XC9160_write_cmos_sensor(0x2038,0x20);                                                                                            
XC9160_write_cmos_sensor(0x2039,0x20);                                                                                            
XC9160_write_cmos_sensor(0x203a,0x20);                                                                                            
XC9160_write_cmos_sensor(0x203b,0x20);   //s0_pl25                                                                                
                                                                                                        
XC9160_write_cmos_sensor(0x203c,0x10);  //s0_pl30                                                                                 
XC9160_write_cmos_sensor(0x203d,0x10);                                                                                            
XC9160_write_cmos_sensor(0x203e,0x10);                                                                                            
XC9160_write_cmos_sensor(0x203f,0x20);                                                                                            
XC9160_write_cmos_sensor(0x2040,0x20);                                                                                            
XC9160_write_cmos_sensor(0x2041,0x20);   //s0_pl35                                                                                
                                                                                                        
XC9160_write_cmos_sensor(0x2044,0x10);   //bit[4:0]thre_y                                                                               
                                                                                                        
//UV DNS                                                                                                
XC9160_write_cmos_sensor(0xfffe,0x30);                                                                                            
//XC9160_write_cmos_sensor(0x0002,0x04);   //bit[2]uv_dns_en                                                                      
XC9160_write_cmos_sensor(0x2100,0x02);   //l_noise_list0                                                                          
XC9160_write_cmos_sensor(0x2101,0x08);                                                                                            
XC9160_write_cmos_sensor(0x2102,0x0a);                                                                                            
XC9160_write_cmos_sensor(0x2103,0x10);                                                                                            
XC9160_write_cmos_sensor(0x2104,0x30);                                                                                            
XC9160_write_cmos_sensor(0x2105,0x50);   //l_noise_list5                                                                          
XC9160_write_cmos_sensor(0x2106,0x5f);   //bit[7]l_noise_man_en; bit[6]l_sdw_ext_en; bit[4:0]l_sdw_ext_ns                         
XC9160_write_cmos_sensor(0x2107,0x08);   //l_noise_man                                                                            
                                                                                                        
//CIP                                                                                                   
XC9160_write_cmos_sensor(0xfffe,0x30);                                                                                            
//XC9160_write_cmos_sensor(0x0000,0x01);   //bit[0] cip_en                                                                        
XC9160_write_cmos_sensor(0x0f00,0x17);   //bit[7:4] noise_y_slp  bit[3:0] Lsharp                                                  
XC9160_write_cmos_sensor(0x0f02,0x00);  //noise_list_0                                                                            
XC9160_write_cmos_sensor(0x0f03,0x00);                                                                                            
XC9160_write_cmos_sensor(0x0f04,0x00);                                                                                            
XC9160_write_cmos_sensor(0x0f05,0x00);                                                                                            
XC9160_write_cmos_sensor(0x0f06,0x00);                                                                                            
XC9160_write_cmos_sensor(0x0f07,0x00);                                                                                            
XC9160_write_cmos_sensor(0x0f08,0x00);                                                                                            
XC9160_write_cmos_sensor(0x0f09,0x00);  //noise_list_7                                                                            
XC9160_write_cmos_sensor(0x0f0a,0x24);   //min_shp                                                                                
XC9160_write_cmos_sensor(0x0f0b,0x28);   //max_shp                                                                                
XC9160_write_cmos_sensor(0x0f0c,0x08);   //min_detail                                                                             
XC9160_write_cmos_sensor(0x0f0d,0x0f);   //max_detail                                                                             
XC9160_write_cmos_sensor(0x0f0e,0x08);   //min_shp_gain                                                                           
XC9160_write_cmos_sensor(0x0f0f,0x10);   //max_shp_gain         

//!defog		 		 
XC9160_write_cmos_sensor(0xfffe,0x30);
//XC9160_write_cmos_sensor(0x0003,0x04);   //bit[2] defog_en
XC9160_write_cmos_sensor(0x2b04,0x08);   //bit[3:0]
XC9160_write_cmos_sensor(0x2b16,0x06);  //bit[3]pdf_avg_mode; bit[2]detail_enhance_en; bit[1]wdr_en; bit[0]:jdark_en

//ISP1 Firmware
XC9160_write_cmos_sensor(0xfffe,0x14);
XC9160_write_cmos_sensor(0x002f,0x01);
XC9160_write_cmos_sensor(0x09ab,0x02);   //Gmax
XC9160_write_cmos_sensor(0x09ac,0x06);   //map_rang[0]//10
XC9160_write_cmos_sensor(0x09ad,0xb4);   //map_rang[1]
XC9160_write_cmos_sensor(0x09ae,0x41);   //nzPDF64_num_thd
XC9160_write_cmos_sensor(0x09af,0xFF);   //grayhigh
XC9160_write_cmos_sensor(0x09b2,0x02);
XC9160_write_cmos_sensor(0x09b3,0x00);   //gain_thd[7:0]
XC9160_write_cmos_sensor(0x09b4,0x10);   //Extratio
XC9160_write_cmos_sensor(0x09b5,0x14);   //Extratio max
XC9160_write_cmos_sensor(0x09b6,0x00);  //weak_adj_on  00:强均衡；01:弱均衡

XC9160_write_cmos_sensor(0xfffe,0x30);
XC9160_write_cmos_sensor(0x2b9a,0x00);  //gain0
XC9160_write_cmos_sensor(0x2b9b,0x00);
XC9160_write_cmos_sensor(0x2b9c,0x10);  //gain1
XC9160_write_cmos_sensor(0x2b9d,0x00);
XC9160_write_cmos_sensor(0x2b9e,0x20);  //gain2
XC9160_write_cmos_sensor(0x2b9f,0x00);
XC9160_write_cmos_sensor(0x2ba0,0x40);  //gain3
XC9160_write_cmos_sensor(0x2ba1,0x00);
XC9160_write_cmos_sensor(0x2ba2,0x80);  //gain4
XC9160_write_cmos_sensor(0x2ba3,0x00);
XC9160_write_cmos_sensor(0x2ba4,0x00);  //gain5
XC9160_write_cmos_sensor(0x2ba5,0x01);  
XC9160_write_cmos_sensor(0x2ba6,0x00);  //gain6
XC9160_write_cmos_sensor(0x2ba7,0x02);  
XC9160_write_cmos_sensor(0x2ba8,0x00);   //gain7
XC9160_write_cmos_sensor(0x2ba9,0x04);  
XC9160_write_cmos_sensor(0x2bac,0x04);  //thd0
XC9160_write_cmos_sensor(0x2bad,0x08); 
XC9160_write_cmos_sensor(0x2bae,0x0c); 
XC9160_write_cmos_sensor(0x2baf,0x10); 
XC9160_write_cmos_sensor(0x2bb0,0x18); 
XC9160_write_cmos_sensor(0x2bb1,0x20); 
XC9160_write_cmos_sensor(0x2bb2,0x24); 
XC9160_write_cmos_sensor(0x2bb3,0x28);  //thd7
XC9160_write_cmos_sensor(0x2bb4,0x7f);   //thd0_max
XC9160_write_cmos_sensor(0x2bb5,0x7f); 
XC9160_write_cmos_sensor(0x2bb6,0x7f); 
XC9160_write_cmos_sensor(0x2bb7,0x7f); 
XC9160_write_cmos_sensor(0x2bb8,0x7f); 
XC9160_write_cmos_sensor(0x2bb9,0x7f); 
XC9160_write_cmos_sensor(0x2bba,0x7f); 
XC9160_write_cmos_sensor(0x2bbb,0x7f);   //thd7_max
XC9160_write_cmos_sensor(0x2bbc,0x40);  //enhance_ratio0 
XC9160_write_cmos_sensor(0x2bbd,0x30);
XC9160_write_cmos_sensor(0x2bbe,0x2a);
XC9160_write_cmos_sensor(0x2bbf,0x28);
XC9160_write_cmos_sensor(0x2bc0,0x24);
XC9160_write_cmos_sensor(0x2bc1,0x20);
XC9160_write_cmos_sensor(0x2bc2,0x10);
XC9160_write_cmos_sensor(0x2bc3,0x08);  //enhance_ratio7 
//defog end

//ISP0 TOP         
XC9160_write_cmos_sensor(0xfffe,0x14);   //isp     
XC9160_write_cmos_sensor(0x002c,0x01);              
XC9160_write_cmos_sensor(0x002b,0x01);              
XC9160_write_cmos_sensor(0x002d,0x01);              
XC9160_write_cmos_sensor(0x002f,0x01);              
XC9160_write_cmos_sensor(0x0030,0x01); 
XC9160_write_cmos_sensor(0x0620,0x01);
XC9160_write_cmos_sensor(0x0621,0x01);    
     
XC9160_write_cmos_sensor(0xfffe,0x30);   //isp   
XC9160_write_cmos_sensor(0x0000,0x57);            
XC9160_write_cmos_sensor(0x0001,0x82);            
XC9160_write_cmos_sensor(0x0002,0x96);            
XC9160_write_cmos_sensor(0x0003,0x35);            
XC9160_write_cmos_sensor(0x0004,0x10);    
XC9160_write_cmos_sensor(0x0013,0x40);
XC9160_write_cmos_sensor(0x071b,0xff);
XC9160_write_cmos_sensor(0x0019,0x0b);
XC9160_write_cmos_sensor(0x071c,0x0a);
XC9160_write_cmos_sensor(0x1700,0x09);
XC9160_write_cmos_sensor(0x1701,0x40);
XC9160_write_cmos_sensor(0x1702,0x40);
XC9160_write_cmos_sensor(0x1704,0x21);
XC9160_write_cmos_sensor(0x2b16,0x06);

	mDELAY(10);

	IMX298MIPISENSORDB("XC9160_ISP_InitialSetting end\n");
    
}


   
void MainSensorMIPIInitialSetting(void)
{
    printk("[IMX298MIPI]enter MainSensorMIPIInitialSetting function:\n ");

//S5K3L8_SensorSetting

IMX298MIPI_write_cmos_sensor(0x6028,0x4000);
IMX298MIPI_write_cmos_sensor(0x6214,0xFFFF);
IMX298MIPI_write_cmos_sensor(0x6216,0xFFFF);
IMX298MIPI_write_cmos_sensor(0x6218,0x0000);
IMX298MIPI_write_cmos_sensor(0x621A,0x0000);
IMX298MIPI_write_cmos_sensor(0x6028,0x2000);
IMX298MIPI_write_cmos_sensor(0x602A,0x2450);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0448);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0349);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0160);
IMX298MIPI_write_cmos_sensor(0x6F12,0xC26A);
IMX298MIPI_write_cmos_sensor(0x6F12,0x511A);
IMX298MIPI_write_cmos_sensor(0x6F12,0x8180);
IMX298MIPI_write_cmos_sensor(0x6F12,0x00F0);
IMX298MIPI_write_cmos_sensor(0x6F12,0x48B8);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2588);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x16C0);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x10B5);
IMX298MIPI_write_cmos_sensor(0x6F12,0x00F0);
IMX298MIPI_write_cmos_sensor(0x6F12,0x5DF8);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2748);
IMX298MIPI_write_cmos_sensor(0x6F12,0x4078);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0028);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0AD0);
IMX298MIPI_write_cmos_sensor(0x6F12,0x00F0);
IMX298MIPI_write_cmos_sensor(0x6F12,0x5CF8);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2549);
IMX298MIPI_write_cmos_sensor(0x6F12,0xB1F8);
IMX298MIPI_write_cmos_sensor(0x6F12,0x1403);
IMX298MIPI_write_cmos_sensor(0x6F12,0x4200);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2448);
IMX298MIPI_write_cmos_sensor(0x6F12,0x4282);
IMX298MIPI_write_cmos_sensor(0x6F12,0x91F8);
IMX298MIPI_write_cmos_sensor(0x6F12,0x9610);
IMX298MIPI_write_cmos_sensor(0x6F12,0x4187);
IMX298MIPI_write_cmos_sensor(0x6F12,0x10BD);
IMX298MIPI_write_cmos_sensor(0x6F12,0x70B5);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0446);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2148);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0022);
IMX298MIPI_write_cmos_sensor(0x6F12,0x4068);
IMX298MIPI_write_cmos_sensor(0x6F12,0x86B2);
IMX298MIPI_write_cmos_sensor(0x6F12,0x050C);
IMX298MIPI_write_cmos_sensor(0x6F12,0x3146);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2846);
IMX298MIPI_write_cmos_sensor(0x6F12,0x00F0);
IMX298MIPI_write_cmos_sensor(0x6F12,0x4CF8);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2046);
IMX298MIPI_write_cmos_sensor(0x6F12,0x00F0);
IMX298MIPI_write_cmos_sensor(0x6F12,0x4EF8);
IMX298MIPI_write_cmos_sensor(0x6F12,0x14F8);
IMX298MIPI_write_cmos_sensor(0x6F12,0x680F);
IMX298MIPI_write_cmos_sensor(0x6F12,0x6178);
IMX298MIPI_write_cmos_sensor(0x6F12,0x40EA);
IMX298MIPI_write_cmos_sensor(0x6F12,0x4100);
IMX298MIPI_write_cmos_sensor(0x6F12,0x1749);
IMX298MIPI_write_cmos_sensor(0x6F12,0xC886);
IMX298MIPI_write_cmos_sensor(0x6F12,0x1848);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2278);
IMX298MIPI_write_cmos_sensor(0x6F12,0x007C);
IMX298MIPI_write_cmos_sensor(0x6F12,0x4240);
IMX298MIPI_write_cmos_sensor(0x6F12,0x1348);
IMX298MIPI_write_cmos_sensor(0x6F12,0xA230);
IMX298MIPI_write_cmos_sensor(0x6F12,0x8378);
IMX298MIPI_write_cmos_sensor(0x6F12,0x43EA);
IMX298MIPI_write_cmos_sensor(0x6F12,0xC202);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0378);
IMX298MIPI_write_cmos_sensor(0x6F12,0x4078);
IMX298MIPI_write_cmos_sensor(0x6F12,0x9B00);
IMX298MIPI_write_cmos_sensor(0x6F12,0x43EA);
IMX298MIPI_write_cmos_sensor(0x6F12,0x4000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0243);
IMX298MIPI_write_cmos_sensor(0x6F12,0xD0B2);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0882);
IMX298MIPI_write_cmos_sensor(0x6F12,0x3146);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2846);
IMX298MIPI_write_cmos_sensor(0x6F12,0xBDE8);
IMX298MIPI_write_cmos_sensor(0x6F12,0x7040);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0122);
IMX298MIPI_write_cmos_sensor(0x6F12,0x00F0);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2AB8);
IMX298MIPI_write_cmos_sensor(0x6F12,0x10B5);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0022);
IMX298MIPI_write_cmos_sensor(0x6F12,0xAFF2);
IMX298MIPI_write_cmos_sensor(0x6F12,0x8701);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0B48);
IMX298MIPI_write_cmos_sensor(0x6F12,0x00F0);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2DF8);
IMX298MIPI_write_cmos_sensor(0x6F12,0x084C);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0022);
IMX298MIPI_write_cmos_sensor(0x6F12,0xAFF2);
IMX298MIPI_write_cmos_sensor(0x6F12,0x6D01);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2060);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0848);
IMX298MIPI_write_cmos_sensor(0x6F12,0x00F0);
IMX298MIPI_write_cmos_sensor(0x6F12,0x25F8);
IMX298MIPI_write_cmos_sensor(0x6F12,0x6060);
IMX298MIPI_write_cmos_sensor(0x6F12,0x10BD);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0550);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0C60);
IMX298MIPI_write_cmos_sensor(0x6F12,0x4000);
IMX298MIPI_write_cmos_sensor(0x6F12,0xD000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2580);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x16F0);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2221);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x2249);
IMX298MIPI_write_cmos_sensor(0x6F12,0x42F2);
IMX298MIPI_write_cmos_sensor(0x6F12,0x351C);
IMX298MIPI_write_cmos_sensor(0x6F12,0xC0F2);
IMX298MIPI_write_cmos_sensor(0x6F12,0x000C);
IMX298MIPI_write_cmos_sensor(0x6F12,0x6047);
IMX298MIPI_write_cmos_sensor(0x6F12,0x42F2);
IMX298MIPI_write_cmos_sensor(0x6F12,0xE11C);
IMX298MIPI_write_cmos_sensor(0x6F12,0xC0F2);
IMX298MIPI_write_cmos_sensor(0x6F12,0x000C);
IMX298MIPI_write_cmos_sensor(0x6F12,0x6047);
IMX298MIPI_write_cmos_sensor(0x6F12,0x40F2);
IMX298MIPI_write_cmos_sensor(0x6F12,0x077C);
IMX298MIPI_write_cmos_sensor(0x6F12,0xC0F2);
IMX298MIPI_write_cmos_sensor(0x6F12,0x000C);
IMX298MIPI_write_cmos_sensor(0x6F12,0x6047);
IMX298MIPI_write_cmos_sensor(0x6F12,0x42F2);
IMX298MIPI_write_cmos_sensor(0x6F12,0x492C);
IMX298MIPI_write_cmos_sensor(0x6F12,0xC0F2);
IMX298MIPI_write_cmos_sensor(0x6F12,0x000C);
IMX298MIPI_write_cmos_sensor(0x6F12,0x6047);
IMX298MIPI_write_cmos_sensor(0x6F12,0x4BF2);
IMX298MIPI_write_cmos_sensor(0x6F12,0x453C);
IMX298MIPI_write_cmos_sensor(0x6F12,0xC0F2);
IMX298MIPI_write_cmos_sensor(0x6F12,0x000C);
IMX298MIPI_write_cmos_sensor(0x6F12,0x6047);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x30C8);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0157);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0000);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0003);
IMX298MIPI_write_cmos_sensor(0x6028,0x2000);
IMX298MIPI_write_cmos_sensor(0x602A,0x1082);
IMX298MIPI_write_cmos_sensor(0x6F12,0x8010);
IMX298MIPI_write_cmos_sensor(0x6028,0x4000);
IMX298MIPI_write_cmos_sensor(0x31CE,0x0001);
IMX298MIPI_write_cmos_sensor(0x0200,0x00C6);
IMX298MIPI_write_cmos_sensor(0x3734,0x0010);
IMX298MIPI_write_cmos_sensor(0x3736,0x0001);
IMX298MIPI_write_cmos_sensor(0x3738,0x0001);
IMX298MIPI_write_cmos_sensor(0x37CC,0x0000);
IMX298MIPI_write_cmos_sensor(0x3744,0x0100);
IMX298MIPI_write_cmos_sensor(0x3762,0x0105);
IMX298MIPI_write_cmos_sensor(0x3764,0x0105);
IMX298MIPI_write_cmos_sensor(0x376A,0x00F0);
IMX298MIPI_write_cmos_sensor(0x344A,0x000F);
IMX298MIPI_write_cmos_sensor(0x344C,0x003D);
IMX298MIPI_write_cmos_sensor(0xF460,0x0020);
IMX298MIPI_write_cmos_sensor(0xF414,0x24C2);
IMX298MIPI_write_cmos_sensor(0xF416,0x0183);
IMX298MIPI_write_cmos_sensor(0xF468,0x0405);
IMX298MIPI_write_cmos_sensor(0x3424,0x0807);
IMX298MIPI_write_cmos_sensor(0x3426,0x0F07);
IMX298MIPI_write_cmos_sensor(0x3428,0x0F07);
IMX298MIPI_write_cmos_sensor(0x341E,0x0804);
IMX298MIPI_write_cmos_sensor(0x3420,0x0C0C);
IMX298MIPI_write_cmos_sensor(0x3422,0x2D2D);
IMX298MIPI_write_cmos_sensor(0xF462,0x003A);
IMX298MIPI_write_cmos_sensor(0x3450,0x0010);
IMX298MIPI_write_cmos_sensor(0x3452,0x0010);
IMX298MIPI_write_cmos_sensor(0xF446,0x0020);
IMX298MIPI_write_cmos_sensor(0xF44E,0x000C);
IMX298MIPI_write_cmos_sensor(0x31FA,0x0007);
IMX298MIPI_write_cmos_sensor(0x31FC,0x0161);
IMX298MIPI_write_cmos_sensor(0x31FE,0x0009);
IMX298MIPI_write_cmos_sensor(0x3200,0x000C);
IMX298MIPI_write_cmos_sensor(0x3202,0x007F);
IMX298MIPI_write_cmos_sensor(0x3204,0x00A2);
IMX298MIPI_write_cmos_sensor(0x3206,0x007D);
IMX298MIPI_write_cmos_sensor(0x3208,0x00A4);
IMX298MIPI_write_cmos_sensor(0x3334,0x00A7);
IMX298MIPI_write_cmos_sensor(0x3336,0x00A5);
IMX298MIPI_write_cmos_sensor(0x3338,0x0033);
IMX298MIPI_write_cmos_sensor(0x333A,0x0006);
IMX298MIPI_write_cmos_sensor(0x333C,0x009F);
IMX298MIPI_write_cmos_sensor(0x333E,0x008C);
IMX298MIPI_write_cmos_sensor(0x3340,0x002D);
IMX298MIPI_write_cmos_sensor(0x3342,0x000A);
IMX298MIPI_write_cmos_sensor(0x3344,0x002F);
IMX298MIPI_write_cmos_sensor(0x3346,0x0008);
IMX298MIPI_write_cmos_sensor(0x3348,0x009F);
IMX298MIPI_write_cmos_sensor(0x334A,0x008C);
IMX298MIPI_write_cmos_sensor(0x334C,0x002D);
IMX298MIPI_write_cmos_sensor(0x334E,0x000A);
IMX298MIPI_write_cmos_sensor(0x3350,0x000A);
IMX298MIPI_write_cmos_sensor(0x320A,0x007B);
IMX298MIPI_write_cmos_sensor(0x320C,0x0161);
IMX298MIPI_write_cmos_sensor(0x320E,0x007F);
IMX298MIPI_write_cmos_sensor(0x3210,0x015F);
IMX298MIPI_write_cmos_sensor(0x3212,0x007B);
IMX298MIPI_write_cmos_sensor(0x3214,0x00B0);
IMX298MIPI_write_cmos_sensor(0x3216,0x0009);
IMX298MIPI_write_cmos_sensor(0x3218,0x0038);
IMX298MIPI_write_cmos_sensor(0x321A,0x0009);
IMX298MIPI_write_cmos_sensor(0x321C,0x0031);
IMX298MIPI_write_cmos_sensor(0x321E,0x0009);
IMX298MIPI_write_cmos_sensor(0x3220,0x0038);
IMX298MIPI_write_cmos_sensor(0x3222,0x0009);
IMX298MIPI_write_cmos_sensor(0x3224,0x007B);
IMX298MIPI_write_cmos_sensor(0x3226,0x0001);
IMX298MIPI_write_cmos_sensor(0x3228,0x0010);
IMX298MIPI_write_cmos_sensor(0x322A,0x00A2);
IMX298MIPI_write_cmos_sensor(0x322C,0x00B1);
IMX298MIPI_write_cmos_sensor(0x322E,0x0002);
IMX298MIPI_write_cmos_sensor(0x3230,0x015D);
IMX298MIPI_write_cmos_sensor(0x3232,0x0001);
IMX298MIPI_write_cmos_sensor(0x3234,0x015D);
IMX298MIPI_write_cmos_sensor(0x3236,0x0001);
IMX298MIPI_write_cmos_sensor(0x3238,0x000B);
IMX298MIPI_write_cmos_sensor(0x323A,0x0016);
IMX298MIPI_write_cmos_sensor(0x323C,0x000D);
IMX298MIPI_write_cmos_sensor(0x323E,0x001C);
IMX298MIPI_write_cmos_sensor(0x3240,0x000D);
IMX298MIPI_write_cmos_sensor(0x3242,0x0054);
IMX298MIPI_write_cmos_sensor(0x3244,0x007B);
IMX298MIPI_write_cmos_sensor(0x3246,0x00CC);
IMX298MIPI_write_cmos_sensor(0x3248,0x015D);
IMX298MIPI_write_cmos_sensor(0x324A,0x007E);
IMX298MIPI_write_cmos_sensor(0x324C,0x0095);
IMX298MIPI_write_cmos_sensor(0x324E,0x0085);
IMX298MIPI_write_cmos_sensor(0x3250,0x009D);
IMX298MIPI_write_cmos_sensor(0x3252,0x008D);
IMX298MIPI_write_cmos_sensor(0x3254,0x009D);
IMX298MIPI_write_cmos_sensor(0x3256,0x007E);
IMX298MIPI_write_cmos_sensor(0x3258,0x0080);
IMX298MIPI_write_cmos_sensor(0x325A,0x0001);
IMX298MIPI_write_cmos_sensor(0x325C,0x0005);
IMX298MIPI_write_cmos_sensor(0x325E,0x0085);
IMX298MIPI_write_cmos_sensor(0x3260,0x009D);
IMX298MIPI_write_cmos_sensor(0x3262,0x0001);
IMX298MIPI_write_cmos_sensor(0x3264,0x0005);
IMX298MIPI_write_cmos_sensor(0x3266,0x007E);
IMX298MIPI_write_cmos_sensor(0x3268,0x0080);
IMX298MIPI_write_cmos_sensor(0x326A,0x0053);
IMX298MIPI_write_cmos_sensor(0x326C,0x007D);
IMX298MIPI_write_cmos_sensor(0x326E,0x00CB);
IMX298MIPI_write_cmos_sensor(0x3270,0x015E);
IMX298MIPI_write_cmos_sensor(0x3272,0x0001);
IMX298MIPI_write_cmos_sensor(0x3274,0x0005);
IMX298MIPI_write_cmos_sensor(0x3276,0x0009);
IMX298MIPI_write_cmos_sensor(0x3278,0x000C);
IMX298MIPI_write_cmos_sensor(0x327A,0x007E);
IMX298MIPI_write_cmos_sensor(0x327C,0x0098);
IMX298MIPI_write_cmos_sensor(0x327E,0x0009);
IMX298MIPI_write_cmos_sensor(0x3280,0x000C);
IMX298MIPI_write_cmos_sensor(0x3282,0x007E);
IMX298MIPI_write_cmos_sensor(0x3284,0x0080);
IMX298MIPI_write_cmos_sensor(0x3286,0x0044);
IMX298MIPI_write_cmos_sensor(0x3288,0x0163);
IMX298MIPI_write_cmos_sensor(0x328A,0x0045);
IMX298MIPI_write_cmos_sensor(0x328C,0x0047);
IMX298MIPI_write_cmos_sensor(0x328E,0x007D);
IMX298MIPI_write_cmos_sensor(0x3290,0x0080);
IMX298MIPI_write_cmos_sensor(0x3292,0x015F);
IMX298MIPI_write_cmos_sensor(0x3294,0x0162);
IMX298MIPI_write_cmos_sensor(0x3296,0x007D);
IMX298MIPI_write_cmos_sensor(0x3298,0x0000);
IMX298MIPI_write_cmos_sensor(0x329A,0x0000);
IMX298MIPI_write_cmos_sensor(0x329C,0x0000);
IMX298MIPI_write_cmos_sensor(0x329E,0x0000);
IMX298MIPI_write_cmos_sensor(0x32A0,0x0008);
IMX298MIPI_write_cmos_sensor(0x32A2,0x0010);
IMX298MIPI_write_cmos_sensor(0x32A4,0x0018);
IMX298MIPI_write_cmos_sensor(0x32A6,0x0020);
IMX298MIPI_write_cmos_sensor(0x32A8,0x0000);
IMX298MIPI_write_cmos_sensor(0x32AA,0x0008);
IMX298MIPI_write_cmos_sensor(0x32AC,0x0010);
IMX298MIPI_write_cmos_sensor(0x32AE,0x0018);
IMX298MIPI_write_cmos_sensor(0x32B0,0x0020);
IMX298MIPI_write_cmos_sensor(0x32B2,0x0020);
IMX298MIPI_write_cmos_sensor(0x32B4,0x0020);
IMX298MIPI_write_cmos_sensor(0x32B6,0x0020);
IMX298MIPI_write_cmos_sensor(0x32B8,0x0000);
IMX298MIPI_write_cmos_sensor(0x32BA,0x0000);
IMX298MIPI_write_cmos_sensor(0x32BC,0x0000);
IMX298MIPI_write_cmos_sensor(0x32BE,0x0000);
IMX298MIPI_write_cmos_sensor(0x32C0,0x0000);
IMX298MIPI_write_cmos_sensor(0x32C2,0x0000);
IMX298MIPI_write_cmos_sensor(0x32C4,0x0000);
IMX298MIPI_write_cmos_sensor(0x32C6,0x0000);
IMX298MIPI_write_cmos_sensor(0x32C8,0x0000);
IMX298MIPI_write_cmos_sensor(0x32CA,0x0000);
IMX298MIPI_write_cmos_sensor(0x32CC,0x0000);
IMX298MIPI_write_cmos_sensor(0x32CE,0x0000);
IMX298MIPI_write_cmos_sensor(0x32D0,0x0000);
IMX298MIPI_write_cmos_sensor(0x32D2,0x0000);
IMX298MIPI_write_cmos_sensor(0x32D4,0x0000);
IMX298MIPI_write_cmos_sensor(0x32D6,0x0000);
IMX298MIPI_write_cmos_sensor(0x32D8,0x0000);
IMX298MIPI_write_cmos_sensor(0x32DA,0x0000);
IMX298MIPI_write_cmos_sensor(0x32DC,0x0000);
IMX298MIPI_write_cmos_sensor(0x32DE,0x0000);
IMX298MIPI_write_cmos_sensor(0x32E0,0x0000);
IMX298MIPI_write_cmos_sensor(0x32E2,0x0000);
IMX298MIPI_write_cmos_sensor(0x32E4,0x0000);
IMX298MIPI_write_cmos_sensor(0x32E6,0x0000);
IMX298MIPI_write_cmos_sensor(0x32E8,0x0000);
IMX298MIPI_write_cmos_sensor(0x32EA,0x0000);
IMX298MIPI_write_cmos_sensor(0x32EC,0x0000);
IMX298MIPI_write_cmos_sensor(0x32EE,0x0000);
IMX298MIPI_write_cmos_sensor(0x32F0,0x0000);
IMX298MIPI_write_cmos_sensor(0x32F2,0x0000);
IMX298MIPI_write_cmos_sensor(0x32F4,0x000A);
IMX298MIPI_write_cmos_sensor(0x32F6,0x0002);
IMX298MIPI_write_cmos_sensor(0x32F8,0x0008);
IMX298MIPI_write_cmos_sensor(0x32FA,0x0010);
IMX298MIPI_write_cmos_sensor(0x32FC,0x0020);
IMX298MIPI_write_cmos_sensor(0x32FE,0x0028);
IMX298MIPI_write_cmos_sensor(0x3300,0x0038);
IMX298MIPI_write_cmos_sensor(0x3302,0x0040);
IMX298MIPI_write_cmos_sensor(0x3304,0x0050);
IMX298MIPI_write_cmos_sensor(0x3306,0x0058);
IMX298MIPI_write_cmos_sensor(0x3308,0x0068);
IMX298MIPI_write_cmos_sensor(0x330A,0x0070);
IMX298MIPI_write_cmos_sensor(0x330C,0x0080);
IMX298MIPI_write_cmos_sensor(0x330E,0x0088);
IMX298MIPI_write_cmos_sensor(0x3310,0x0098);
IMX298MIPI_write_cmos_sensor(0x3312,0x00A0);
IMX298MIPI_write_cmos_sensor(0x3314,0x00B0);
IMX298MIPI_write_cmos_sensor(0x3316,0x00B8);
IMX298MIPI_write_cmos_sensor(0x3318,0x00C8);
IMX298MIPI_write_cmos_sensor(0x331A,0x00D0);
IMX298MIPI_write_cmos_sensor(0x331C,0x00E0);
IMX298MIPI_write_cmos_sensor(0x331E,0x00E8);
IMX298MIPI_write_cmos_sensor(0x3320,0x0017);
IMX298MIPI_write_cmos_sensor(0x3322,0x002F);
IMX298MIPI_write_cmos_sensor(0x3324,0x0047);
IMX298MIPI_write_cmos_sensor(0x3326,0x005F);
IMX298MIPI_write_cmos_sensor(0x3328,0x0077);
IMX298MIPI_write_cmos_sensor(0x332A,0x008F);
IMX298MIPI_write_cmos_sensor(0x332C,0x00A7);
IMX298MIPI_write_cmos_sensor(0x332E,0x00BF);
IMX298MIPI_write_cmos_sensor(0x3330,0x00D7);
IMX298MIPI_write_cmos_sensor(0x3332,0x00EF);
IMX298MIPI_write_cmos_sensor(0x3352,0x00A5);
IMX298MIPI_write_cmos_sensor(0x3354,0x00AF);
IMX298MIPI_write_cmos_sensor(0x3356,0x0187);
IMX298MIPI_write_cmos_sensor(0x3358,0x0000);
IMX298MIPI_write_cmos_sensor(0x335A,0x009E);
IMX298MIPI_write_cmos_sensor(0x335C,0x016B);
IMX298MIPI_write_cmos_sensor(0x335E,0x0015);
IMX298MIPI_write_cmos_sensor(0x3360,0x00A5);
IMX298MIPI_write_cmos_sensor(0x3362,0x00AF);
IMX298MIPI_write_cmos_sensor(0x3364,0x01FB);
IMX298MIPI_write_cmos_sensor(0x3366,0x0000);
IMX298MIPI_write_cmos_sensor(0x3368,0x009E);
IMX298MIPI_write_cmos_sensor(0x336A,0x016B);
IMX298MIPI_write_cmos_sensor(0x336C,0x0015);
IMX298MIPI_write_cmos_sensor(0x336E,0x00A5);
IMX298MIPI_write_cmos_sensor(0x3370,0x00A6);
IMX298MIPI_write_cmos_sensor(0x3372,0x0187);
IMX298MIPI_write_cmos_sensor(0x3374,0x0000);
IMX298MIPI_write_cmos_sensor(0x3376,0x009E);
IMX298MIPI_write_cmos_sensor(0x3378,0x016B);
IMX298MIPI_write_cmos_sensor(0x337A,0x0015);
IMX298MIPI_write_cmos_sensor(0x337C,0x00A5);
IMX298MIPI_write_cmos_sensor(0x337E,0x00A6);
IMX298MIPI_write_cmos_sensor(0x3380,0x01FB);
IMX298MIPI_write_cmos_sensor(0x3382,0x0000);
IMX298MIPI_write_cmos_sensor(0x3384,0x009E);
IMX298MIPI_write_cmos_sensor(0x3386,0x016B);
IMX298MIPI_write_cmos_sensor(0x3388,0x0015);
IMX298MIPI_write_cmos_sensor(0x319A,0x0005);
IMX298MIPI_write_cmos_sensor(0x1006,0x0005);
IMX298MIPI_write_cmos_sensor(0x3416,0x0001);
IMX298MIPI_write_cmos_sensor(0x308C,0x0008);
IMX298MIPI_write_cmos_sensor(0x307C,0x0240);
IMX298MIPI_write_cmos_sensor(0x375E,0x0050);
IMX298MIPI_write_cmos_sensor(0x31CE,0x0101);
IMX298MIPI_write_cmos_sensor(0x374E,0x0007);
IMX298MIPI_write_cmos_sensor(0x3460,0x0001);
IMX298MIPI_write_cmos_sensor(0x3052,0x0002);
IMX298MIPI_write_cmos_sensor(0x3058,0x0001);
IMX298MIPI_write_cmos_sensor(0x6028,0x2000);
IMX298MIPI_write_cmos_sensor(0x602A,0x108A);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0359);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0100);

//#ifdefNONCONTINUEMODE
IMX298MIPI_write_cmos_sensor(0xB0A0,0x007C);//noncontinuemode
//#else
//IMX298MIPI_write_cmos_sensor(0xB0A0,0x007D);//continuemode
//#endif
        
IMX298MIPISENSORDB("[IMX298MIPI]exit IMX298MIPIInitialSetting function:\n ");
} 


void OV5675MIPIInitialSetting(void)
{
    IMX298MIPISENSORDB("[OV5675MIPI]enter OV5675MIPIInitialSetting function:\n ");
    
OV5675MIPI_write_cmos_sensor(0x0100,0x0);  
OV5675MIPI_write_cmos_sensor(0x0103,0x1);  

    
IMX298MIPISENSORDB("[OV5675MIPI]exit OV5675MIPIInitialSetting function:\n ");
 
}

/*****************************************************************
* FUNCTION
*    IMX298MIPIPreviewSetting
*
* DESCRIPTION
*    This function config Preview setting related registers of CMOS sensor.
*
* PARAMETERS
*    None
*
* RETURNS
*    None
*
* LOCAL AFFECTED
*
*************************************************************************/

// 1952x1092 -> active array size: 1920x1080,  by zhangxueping

// SVGA 800 * 600,  by zhangxueping 

void IMX298MIPIPreviewSetting_SVGA(void)
{
    
    IMX298MIPISENSORDB("[IMX298MIPI]enter IMX298MIPIPreviewSetting_SVGA function:\n ");

#if defined(IME_IMX298_MIRROR)
	
#endif

    IMX298MIPISENSORDB("[IMX298MIPI]exit IMX298MIPIPreviewSetting_SVGA function:\n ");
}
/*static void XC9160PreviewSetting_SVGA(void)
{
     IMX298MIPISENSORDB("[XC9160]ENTER XC9160PreviewSetting_SVGA function:\n ");
    mDELAY(5);

#if defined(IME_IMX298_MIRROR)
	
#endif

  IMX298MIPISENSORDB("[XC9160]exit XC9160PreviewSetting_SVGA function end :\n ");
}
*/
/*************************************************************************
* FUNCTION
*     XC9160 ISP FullSizeCaptureSetting
*
* DESCRIPTION
*    This function config full size capture setting by XC9160.
*
* PARAMETERS
*    None
*
* RETURNS
*    None
*
* LOCAL AFFECTED
*
*************************************************************************/
#if 0
static void XC9160FullSizeCaptureSetting(void)
{
IMX298MIPISENSORDB("XC9160_ISP_5MCaptureSetting start \n");



IMX298MIPISENSORDB("XC9160_ISP_5MCaptureSetting  end \n");

}
#endif



void  IMX298MIPI_Preview_Init(void); // by zhangxueping

/*************************************************************************
* FUNCTION
*   IMX298MIPIOpen
*
* DESCRIPTION
*   This function initialize the registers of CMOS sensor
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 IMX298MIPIOpen(void)
{
    kal_uint32 isp_id[4]={0};
    //kal_uint32 j;

    
    //volatile signed int i;
    //kal_uint16 isp_id = 0;
    // kal_uint16 sensor_id = 0;    
     kal_uint16 ret;
		
	//TV_SWITCH_PRINT("[IMX298MIPI]IMX298MIPIOpen: ZHENGFENG_0320\n ");
	TV_SWITCH_PRINT("mccree [IMX298MIPI]IMX298MIPIOpen-4: 150618\n ");

	//if(isCameraOpen)
	//{
	//	TV_SWITCH_PRINT("[IMX298MIPI]IMX298MIPIOpen, has opened\n ");
	//	return ERROR_NONE;
	//}

	//is_KERN_EMERG = true;
	//mutex_lock(&TV_switch_lock);
    XC9160_write_cmos_sensor(0xfffd,0x80);
    XC9160_write_cmos_sensor(0xfffe,0x50);
        mDELAY(10);

	#if 0
    for (j=0;j<4;j++)
    {
        isp_id[j]=XC9160_read_cmos_sensor(j);
        IMX298MIPISENSORDB("[IMX298MIPI_ISP]ISP_id[%d]=0x%x \n",j,isp_id[j]);
    }
	#else
	isp_id[0]=XC9160_read_cmos_sensor(0xfffb);
    printk("mccree xc9160 open isp_id[0]=%x  should eq 0x71\n",isp_id[0]);
	#endif
    
    XC9160InitialSetting(); 
    XC9160_write_cmos_sensor(0xfffe,0x50);
		XC9160_write_cmos_sensor(0x0050,0x0f);  //  PWDN,GPIO1,GPIO0 en   by Nathan 20150719
		XC9160_write_cmos_sensor(0x0054,0x0f);  //  PWDN,GPIO1,ouput 控制sensorPWDN&reset ; GPIO0 被MTK6572来控制XC9160 PWDN
		XC9160_write_cmos_sensor(0x0058,0x00); //   将GPIO1（sensor reset）先拉低
		mDELAY(10);
		XC9160_write_cmos_sensor(0x0058,0x0a); //   将GPIO1（sensor reset）先拉低		
		
    XC9160_write_cmos_sensor(0xfffd ,0x80);
    XC9160_write_cmos_sensor(0xfffe,0x50);
    XC9160_write_cmos_sensor(0x004d ,0x01);

    MainSensorMIPIInitialSetting();

    mDELAY(10);

  //  IMX298MIPIInitialSetting();

    ret=XC9160_read_cmos_sensor(0x004D);
    IMX298MIPISENSORDB("[XC9160]XC9160 Preview ret:0x004D_first---%x\n ",ret);

    XC9160_write_cmos_sensor(0xfffd,0x80);
    XC9160_write_cmos_sensor(0xfffe,0x50);
    XC9160_write_cmos_sensor(0x004d ,0x02);


    ret=XC9160_read_cmos_sensor(0x004D);
    IMX298MIPISENSORDB("[XC9160]XC9160 Preview ret:0x004D---%x\n ",ret);
 //	OV5675MIPIInitialSetting();
   /* 
       ret=IMX298MIPIYUV_read_cmos_sensor(0x300f);
    IMX298MIPISENSORDB("[IMX298]IMX298MIPIPreview ret:0x300f---%x\n ",ret);
       ret=IMX298MIPIYUV_read_cmos_sensor(0x3010);
    IMX298MIPISENSORDB("[IMX298]IMX298MIPIPreview ret:0x3010---%x\n ",ret);
       ret=IMX298MIPIYUV_read_cmos_sensor(0x3011);
    IMX298MIPISENSORDB("[IMX298]IMX298MIPIPreview ret:0x3011---%x\n ",ret);
    ret=IMX298MIPIYUV_read_cmos_sensor(0x3012);
    IMX298MIPISENSORDB("[IMX298]IMX298MIPIPreview ret:0x3012---%x\n ",ret);
*/

    XC9160_write_cmos_sensor(0xfffe,0x50);
    XC9160_write_cmos_sensor(0xfffe,0x50);
    XC9160_write_cmos_sensor(0x004d ,0x00);
   
	//IMX298MIPI_Preview_Init(); // by zhangxueping
	 
    TV_SWITCH_PRINT("[mccree]exit IMX298MIPIOpen\n ");

	
    return ERROR_NONE;
}   /* IMX298MIPIOpen() */

/*************************************************************************
* FUNCTION
*   IMX298MIPIClose
*
* DESCRIPTION
*   This function is to turn off sensor module power.
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 IMX298MIPIClose(void)
{
	TV_SWITCH_PRINT("IMX298MIPIClose\n ");

#ifdef IMX298_REG_READBACK
		if(isIMX298HrtimerStart)
		{
			IMX298MIPISENSORDB("Cancel hrtimer\n ");
			hrtimer_cancel(&imx298_readback_timer);
			isIMX298HrtimerStart = KAL_FALSE;
		}	  
#endif	

	
  //CISModulePowerOn(FALSE);
    return ERROR_NONE;
}   /* IMX298MIPIClose() */

/*************************************************************************
* FUNCTION
*   IMX298MIPIPreview
*
* DESCRIPTION
*   This function start the sensor preview.
*
* PARAMETERS
*   *image_window : address pointer of pixel numbers in one period of HSYNC
*  *sensor_config_data : address pointer of line numbers in one period of VSYNC
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/

static void XC9160PreviewSetting(void)
{
XC9160_write_cmos_sensor(0xfffe,0x50);	
XC9160_write_cmos_sensor(0x0030,0x44);
XC9160_write_cmos_sensor(0x0031,0x58);
XC9160_write_cmos_sensor(0x0032,0x34);  
XC9160_write_cmos_sensor(0x0033,0x70);   

XC9160_write_cmos_sensor(0x0020,0x02);
XC9160_write_cmos_sensor(0x0021,0x0d);
XC9160_write_cmos_sensor(0x0022,0x01);
XC9160_write_cmos_sensor(0x0023,0x86);
XC9160_write_cmos_sensor(0x0024,0x0e);
XC9160_write_cmos_sensor(0x0025,0x05); 
XC9160_write_cmos_sensor(0x0026,0x01);
XC9160_write_cmos_sensor(0x0027,0x06);
XC9160_write_cmos_sensor(0x0028,0x01);
XC9160_write_cmos_sensor(0x0029,0x00);
XC9160_write_cmos_sensor(0x002a,0x02);
XC9160_write_cmos_sensor(0x002b,0x05);	

XC9160_write_cmos_sensor(0xfffe,0x14);
XC9160_write_cmos_sensor(0x002b,0x01); 
XC9160_write_cmos_sensor(0x002c,0x01);  
XC9160_write_cmos_sensor(0x002d,0x01);  
		
XC9160_write_cmos_sensor(0xfffe,0x50);
XC9160_write_cmos_sensor(0x00bc,0x19);
XC9160_write_cmos_sensor(0x0090,0x38);

XC9160_write_cmos_sensor(0x0200,0x0f);   //mipi_rx1_pad_en
XC9160_write_cmos_sensor(0x0201,0x00);
XC9160_write_cmos_sensor(0x0202,0x80);
XC9160_write_cmos_sensor(0x0203,0x00);

XC9160_write_cmos_sensor(0xfffe,0x26);

XC9160_write_cmos_sensor(0x8001,0x20);      //colorbar0
XC9160_write_cmos_sensor(0x8002,0x08);
XC9160_write_cmos_sensor(0x8003,0x00);
XC9160_write_cmos_sensor(0x8004,0x06);
XC9160_write_cmos_sensor(0x8005,0x40);
XC9160_write_cmos_sensor(0x8006,0x40);
XC9160_write_cmos_sensor(0x8007,0x10);  //30
XC9160_write_cmos_sensor(0x8008,0xf0);  //60
XC9160_write_cmos_sensor(0x8009,0x00);
XC9160_write_cmos_sensor(0x800b,0x00);
XC9160_write_cmos_sensor(0x8000,0x1d);

XC9160_write_cmos_sensor(0xfffe,0x26); 
XC9160_write_cmos_sensor(0x8010,0x05);   //crop0
XC9160_write_cmos_sensor(0x8012,0x20);   
XC9160_write_cmos_sensor(0x8013,0x08);
XC9160_write_cmos_sensor(0x8014,0x00);
XC9160_write_cmos_sensor(0x8015,0x06);
XC9160_write_cmos_sensor(0x8016,0x01);
XC9160_write_cmos_sensor(0x8017,0x00);
XC9160_write_cmos_sensor(0x8018,0x00);
XC9160_write_cmos_sensor(0x8019,0x00);


XC9160_write_cmos_sensor(0xfffe,0x30);  //isp0
//XC9160_write_cmos_sensor(0x0000,0x01);
//XC9160_write_cmos_sensor(0x0001,0x00);
//XC9160_write_cmos_sensor(0x0002,0x10);
//XC9160_write_cmos_sensor(0x0003,0x20);
//XC9160_write_cmos_sensor(0x0004,0x10);

//XC9160_write_cmos_sensor(0x0019,0x01);
XC9160_write_cmos_sensor(0x0050,0x20);
 
XC9160_write_cmos_sensor(0x005e,0x1f);      
XC9160_write_cmos_sensor(0x005f,0x08);
XC9160_write_cmos_sensor(0x0060,0xff);
XC9160_write_cmos_sensor(0x0061,0x05);
XC9160_write_cmos_sensor(0x0064,0x20);
XC9160_write_cmos_sensor(0x0065,0x08);
XC9160_write_cmos_sensor(0x0066,0x00);
XC9160_write_cmos_sensor(0x0067,0x06);

XC9160_write_cmos_sensor(0x0006,0x08);
XC9160_write_cmos_sensor(0x0007,0x20);
XC9160_write_cmos_sensor(0x0008,0x06);
XC9160_write_cmos_sensor(0x0009,0x00);
XC9160_write_cmos_sensor(0x000a,0x08);
XC9160_write_cmos_sensor(0x000b,0x20);
XC9160_write_cmos_sensor(0x000c,0x06);
XC9160_write_cmos_sensor(0x000d,0x00); 

XC9160_write_cmos_sensor(0xfffe,0x26);
XC9160_write_cmos_sensor(0x0000,0x60);
XC9160_write_cmos_sensor(0x0009,0xc4);   //mipi_rx_set

XC9160_write_cmos_sensor(0xfffe,0x26);
XC9160_write_cmos_sensor(0x2019,0x08);   //mipi_tx
XC9160_write_cmos_sensor(0x201a,0x20);
XC9160_write_cmos_sensor(0x201b,0x06);
XC9160_write_cmos_sensor(0x201c,0x00);
XC9160_write_cmos_sensor(0x201d,0x00);
XC9160_write_cmos_sensor(0x201e,0x00);
XC9160_write_cmos_sensor(0x201f,0x00);
XC9160_write_cmos_sensor(0x2020,0x00);

XC9160_write_cmos_sensor(0x2015,0x83);
XC9160_write_cmos_sensor(0x2017,0x1e);
XC9160_write_cmos_sensor(0x2018,0x00);
XC9160_write_cmos_sensor(0x2023,0x03);   //mipi_tx_set

XC9160_write_cmos_sensor(0xfffe,0x2c);  //stitch
XC9160_write_cmos_sensor(0x0000,0x00);
XC9160_write_cmos_sensor(0x0001,0x08);
XC9160_write_cmos_sensor(0x0002,0x20);
XC9160_write_cmos_sensor(0x0004,0x06);
XC9160_write_cmos_sensor(0x0005,0x00);
XC9160_write_cmos_sensor(0x0008,0x10);
XC9160_write_cmos_sensor(0x0044,0x08);  //fifo0
XC9160_write_cmos_sensor(0x0045,0x04);
XC9160_write_cmos_sensor(0x0048,0x10);
XC9160_write_cmos_sensor(0x0049,0x30);

XC9160_write_cmos_sensor(0xfffe,0x2e);  //retiming
//XC9160_write_cmos_sensor(0x0000,0x42);
XC9160_write_cmos_sensor(0x0001,0xcc);
XC9160_write_cmos_sensor(0x0003,0x00);
XC9160_write_cmos_sensor(0x0004,0x01);

XC9160_write_cmos_sensor(0xfffe,0x30);    //lenc skip
XC9160_write_cmos_sensor(0x0012,0x95);            
XC9160_write_cmos_sensor(0x000e,0x00);            
XC9160_write_cmos_sensor(0x000f,0x04);            
XC9160_write_cmos_sensor(0x0010,0x00);           
XC9160_write_cmos_sensor(0x0011,0x00);    

//isp top
XC9160_write_cmos_sensor(0xfffe,0x30);   //isp   
XC9160_write_cmos_sensor(0x0000,0x57);            
XC9160_write_cmos_sensor(0x0001,0x82);            
XC9160_write_cmos_sensor(0x0002,0x96);            
XC9160_write_cmos_sensor(0x0003,0x35);            
XC9160_write_cmos_sensor(0x0004,0x10);    
XC9160_write_cmos_sensor(0x0019,0x0b);

IMX298MIPISENSORDB("[mccree]exit XC9160_write_cmos_sensorpreview function:\n ");  
}


static void MainSensorMIPIPreviewSetting(void)
{
//2080*1536

IMX298MIPI_write_cmos_sensor(0x0100,0x0000);
IMX298MIPI_write_cmos_sensor(0x6028,0x2000);
IMX298MIPI_write_cmos_sensor(0x602A,0x0F74);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0040);
IMX298MIPI_write_cmos_sensor(0x6F12,0x0040);
IMX298MIPI_write_cmos_sensor(0x6028,0x4000);
//IMX298MIPI_write_cmos_sensor(0x30c0,0x0300);//openmastermode
IMX298MIPI_write_cmos_sensor(0x0344,0x0008);
IMX298MIPI_write_cmos_sensor(0x0346,0x0008);
IMX298MIPI_write_cmos_sensor(0x0348,0x1077);
IMX298MIPI_write_cmos_sensor(0x034A,0x0C37);
IMX298MIPI_write_cmos_sensor(0x034C,0x0840);
IMX298MIPI_write_cmos_sensor(0x034E,0x0620);
IMX298MIPI_write_cmos_sensor(0x0900,0x0112);
IMX298MIPI_write_cmos_sensor(0x0380,0x0001);
IMX298MIPI_write_cmos_sensor(0x0382,0x0001);
IMX298MIPI_write_cmos_sensor(0x0384,0x0001);
IMX298MIPI_write_cmos_sensor(0x0386,0x0003);
IMX298MIPI_write_cmos_sensor(0x0400,0x0001);
IMX298MIPI_write_cmos_sensor(0x0404,0x0020);
IMX298MIPI_write_cmos_sensor(0x0114,0x0300);
IMX298MIPI_write_cmos_sensor(0x0110,0x0002);

IMX298MIPI_write_cmos_sensor(0x0136,0x1800);
IMX298MIPI_write_cmos_sensor(0x0304,0x0006);
IMX298MIPI_write_cmos_sensor(0x0306,0x007d);
IMX298MIPI_write_cmos_sensor(0x0302,0x0001);
IMX298MIPI_write_cmos_sensor(0x0300,0x0005);
IMX298MIPI_write_cmos_sensor(0x030C,0x0006);
IMX298MIPI_write_cmos_sensor(0x030E,0x0064);
IMX298MIPI_write_cmos_sensor(0x030A,0x0001);
IMX298MIPI_write_cmos_sensor(0x0308,0x0008);

IMX298MIPI_write_cmos_sensor(0x0342,0x1640);
IMX298MIPI_write_cmos_sensor(0x0340,0x0680);//648
//IMX298MIPI_write_cmos_sensor(0x0202,0x0700);
IMX298MIPI_write_cmos_sensor(0x0200,0x00C6);
IMX298MIPI_write_cmos_sensor(0x0B04,0x0101);
IMX298MIPI_write_cmos_sensor(0x0B08,0x0001);
IMX298MIPI_write_cmos_sensor(0x0B00,0x0007);
IMX298MIPI_write_cmos_sensor(0x316A,0x0007);

//IMX298MIPI_write_cmos_sensor(0x6028,0x4000);
//IMX298MIPI_write_cmos_sensor(0x3098,0x0002);
//IMX298MIPI_write_cmos_sensor(0x3166,0x000a);
//IMX298MIPI_write_cmos_sensor(0x3168,0x0023);
IMX298MIPI_write_cmos_sensor(0x0100,0x0100);
IMX298MIPI_write_cmos_sensor(0x0101,0x0300);
    IMX298MIPISENSORDB("[mccree]exit MainSensorMIPIPreviewSetting function:\n ");   

    //return ERROR_NONE ;
    
}   /* IMX298MIPIPreview() */

UINT32 IMX298MIPIPreview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    //kal_uint32 zsdshutter = 0;
	int ret = 0;
    IMX298MIPISENSORDB("[mccree]enter IMX298MIPIPreview function:\n ");

    XC9160_write_cmos_sensor(0xfffd ,0x80);
    XC9160_write_cmos_sensor(0xfffe,0x50);
    XC9160_write_cmos_sensor(0x004d ,0x01);

	MainSensorMIPIPreviewSetting();	

	XC9160_write_cmos_sensor(0xfffd ,0x80);
    XC9160_write_cmos_sensor(0xfffe,0x50);
    XC9160_write_cmos_sensor(0x004d ,0x00);

	XC9160PreviewSetting();
	
	 XC9160_write_cmos_sensor(0xfffe,0x50);
	 ret=XC9160_read_cmos_sensor(0x004D);
    IMX298MIPISENSORDB("Colin_[XC9160]XC9160 Preview ret:0x004D_first---%x\n ",ret);

   
	 IMX298MIPISENSORDB("[mccree]exit IMX298MIPIPreview function:\n ");
	return ERROR_NONE ;
}



static void XC9160FullSizeCaptureSetting(void)
{
XC9160_write_cmos_sensor(0xfffe,0x50);	
XC9160_write_cmos_sensor(0x0030,0x44);
XC9160_write_cmos_sensor(0x0031,0x94);
XC9160_write_cmos_sensor(0x0032,0x70);  
XC9160_write_cmos_sensor(0x0033,0x70);   

XC9160_write_cmos_sensor(0x0020,0x01);
XC9160_write_cmos_sensor(0x0021,0x0d);
XC9160_write_cmos_sensor(0x0022,0x01);
XC9160_write_cmos_sensor(0x0023,0x86);
XC9160_write_cmos_sensor(0x0024,0x0e);
XC9160_write_cmos_sensor(0x0025,0x05); 
XC9160_write_cmos_sensor(0x0026,0x01);
XC9160_write_cmos_sensor(0x0027,0x06);
XC9160_write_cmos_sensor(0x0028,0x01);
XC9160_write_cmos_sensor(0x0029,0x00);
XC9160_write_cmos_sensor(0x002a,0x02);
XC9160_write_cmos_sensor(0x002b,0x05);	

XC9160_write_cmos_sensor(0xfffe,0x14);
XC9160_write_cmos_sensor(0x002b,0x00); 
XC9160_write_cmos_sensor(0x002c,0x00);  
XC9160_write_cmos_sensor(0x002d,0x00);  	
		
XC9160_write_cmos_sensor(0xfffe,0x50);
XC9160_write_cmos_sensor(0x00bc,0x19);
XC9160_write_cmos_sensor(0x0090,0x38);

XC9160_write_cmos_sensor(0x0200,0x0f);   //mipi_rx1_pad_en
XC9160_write_cmos_sensor(0x0201,0x00);
XC9160_write_cmos_sensor(0x0202,0x80);
XC9160_write_cmos_sensor(0x0203,0x00);

XC9160_write_cmos_sensor(0xfffe,0x26);

XC9160_write_cmos_sensor(0x8001,0x70);    //colorbar0
XC9160_write_cmos_sensor(0x8002,0x10);
XC9160_write_cmos_sensor(0x8003,0x30);
XC9160_write_cmos_sensor(0x8004,0x0c);
XC9160_write_cmos_sensor(0x8005,0x40);
XC9160_write_cmos_sensor(0x8006,0x40);
XC9160_write_cmos_sensor(0x8007,0x10);  //30
XC9160_write_cmos_sensor(0x8008,0xf0);  //60
XC9160_write_cmos_sensor(0x8009,0x00);
XC9160_write_cmos_sensor(0x800b,0x00);
XC9160_write_cmos_sensor(0x8000,0x1d);  

XC9160_write_cmos_sensor(0xfffe,0x26);  
XC9160_write_cmos_sensor(0x8010,0x05);  //crop0
XC9160_write_cmos_sensor(0x8012,0x70);   
XC9160_write_cmos_sensor(0x8013,0x10);
XC9160_write_cmos_sensor(0x8014,0x30);
XC9160_write_cmos_sensor(0x8015,0x0c);
XC9160_write_cmos_sensor(0x8016,0x01);
XC9160_write_cmos_sensor(0x8017,0x00);
XC9160_write_cmos_sensor(0x8018,0x00);
XC9160_write_cmos_sensor(0x8019,0x00);


XC9160_write_cmos_sensor(0xfffe,0x30);  //isp0
//XC9160_write_cmos_sensor(0x0000,0x01);
//XC9160_write_cmos_sensor(0x0001,0x00);
//XC9160_write_cmos_sensor(0x0002,0x10);
//XC9160_write_cmos_sensor(0x0003,0x20);
//XC9160_write_cmos_sensor(0x0004,0x10);

//XC9160_write_cmos_sensor(0x0019,0x01);
XC9160_write_cmos_sensor(0x0050,0x20);
 
XC9160_write_cmos_sensor(0x005e,0x6f);      
XC9160_write_cmos_sensor(0x005f,0x10);
XC9160_write_cmos_sensor(0x0060,0x2f);
XC9160_write_cmos_sensor(0x0061,0x0c);
XC9160_write_cmos_sensor(0x0064,0x70);
XC9160_write_cmos_sensor(0x0065,0x10);
XC9160_write_cmos_sensor(0x0066,0x30);
XC9160_write_cmos_sensor(0x0067,0x0c);

XC9160_write_cmos_sensor(0x0006,0x10);
XC9160_write_cmos_sensor(0x0007,0x70);
XC9160_write_cmos_sensor(0x0008,0x0c);
XC9160_write_cmos_sensor(0x0009,0x30);
XC9160_write_cmos_sensor(0x000a,0x10);
XC9160_write_cmos_sensor(0x000b,0x70);
XC9160_write_cmos_sensor(0x000c,0x0c);
XC9160_write_cmos_sensor(0x000d,0x30); 

XC9160_write_cmos_sensor(0xfffe,0x26);
XC9160_write_cmos_sensor(0x0000,0x60);
XC9160_write_cmos_sensor(0x0009,0xc4);   //mipi_rx_set

XC9160_write_cmos_sensor(0xfffe,0x26);
XC9160_write_cmos_sensor(0x2019,0x10);    //mipi_tx
XC9160_write_cmos_sensor(0x201a,0x70);
XC9160_write_cmos_sensor(0x201b,0x0c);
XC9160_write_cmos_sensor(0x201c,0x30);
XC9160_write_cmos_sensor(0x201d,0x00);
XC9160_write_cmos_sensor(0x201e,0x00);
XC9160_write_cmos_sensor(0x201f,0x00);
XC9160_write_cmos_sensor(0x2020,0x00);

XC9160_write_cmos_sensor(0x2015,0x83);
XC9160_write_cmos_sensor(0x2017,0x1e);
XC9160_write_cmos_sensor(0x2018,0x00);
XC9160_write_cmos_sensor(0x2023,0x03);   //mipi_tx_set

XC9160_write_cmos_sensor(0xfffe,0x2c);  //stitch
XC9160_write_cmos_sensor(0x0000,0x00);
XC9160_write_cmos_sensor(0x0001,0x10);
XC9160_write_cmos_sensor(0x0002,0x70);
XC9160_write_cmos_sensor(0x0004,0x0c);
XC9160_write_cmos_sensor(0x0005,0x30);
XC9160_write_cmos_sensor(0x0008,0x10);
XC9160_write_cmos_sensor(0x0044,0x08);  //fifo0
XC9160_write_cmos_sensor(0x0045,0x04);
XC9160_write_cmos_sensor(0x0048,0x20);   
XC9160_write_cmos_sensor(0x0049,0xd0);   

XC9160_write_cmos_sensor(0xfffe,0x2e);  //retiming
//XC9160_write_cmos_sensor(0x0000,0x42);
XC9160_write_cmos_sensor(0x0001,0xcc);
XC9160_write_cmos_sensor(0x0003,0x01);
XC9160_write_cmos_sensor(0x0004,0x00);

XC9160_write_cmos_sensor(0xfffe,0x30);    //lenc skip
XC9160_write_cmos_sensor(0x0012,0x80);            
XC9160_write_cmos_sensor(0x000e,0x00);            
XC9160_write_cmos_sensor(0x000f,0x00);            
XC9160_write_cmos_sensor(0x0010,0x00);           
XC9160_write_cmos_sensor(0x0011,0x00);    

//isp top
XC9160_write_cmos_sensor(0xfffe,0x30);   //isp   
XC9160_write_cmos_sensor(0x0000,0x57);            
XC9160_write_cmos_sensor(0x0001,0x82);            
XC9160_write_cmos_sensor(0x0002,0x96);            
XC9160_write_cmos_sensor(0x0003,0x35);            
XC9160_write_cmos_sensor(0x0004,0x10);    
XC9160_write_cmos_sensor(0x0019,0x0b);

IMX298MIPISENSORDB("mccree :Capture OK");
}

static void MainSensorMIPIFullSizeCaptureSetting(void)
{
//4160*3120

IMX298MIPI_write_cmos_sensor(0x0100,0x0000);
IMX298MIPI_write_cmos_sensor(0x6028,0x2000);	
IMX298MIPI_write_cmos_sensor(0x602A,0x0F74);	
IMX298MIPI_write_cmos_sensor(0x6F12,0x0040);	 // 64
IMX298MIPI_write_cmos_sensor(0x6F12,0x0040);	 // 64
IMX298MIPI_write_cmos_sensor(0x6028,0x4000);	
IMX298MIPI_write_cmos_sensor(0x0344,0x0008);
IMX298MIPI_write_cmos_sensor(0x0346,0x0008);
IMX298MIPI_write_cmos_sensor(0x0348,0x1077);
IMX298MIPI_write_cmos_sensor(0x034A,0x0C37);
IMX298MIPI_write_cmos_sensor(0x034C,0x1074);
IMX298MIPI_write_cmos_sensor(0x034E,0x0C34);
IMX298MIPI_write_cmos_sensor(0x0900,0x0011);	
IMX298MIPI_write_cmos_sensor(0x0380,0x0001);	
IMX298MIPI_write_cmos_sensor(0x0382,0x0001);	
IMX298MIPI_write_cmos_sensor(0x0384,0x0001);	
IMX298MIPI_write_cmos_sensor(0x0386,0x0001);	
IMX298MIPI_write_cmos_sensor(0x0400,0x0000);	
IMX298MIPI_write_cmos_sensor(0x0404,0x0010);	
IMX298MIPI_write_cmos_sensor(0x0114,0x0300);	
IMX298MIPI_write_cmos_sensor(0x0110,0x0002);	 

IMX298MIPI_write_cmos_sensor(0x0136,0x1800);	 
IMX298MIPI_write_cmos_sensor(0x0304,0x0006);	 
IMX298MIPI_write_cmos_sensor(0x0306,0x00bf);	
IMX298MIPI_write_cmos_sensor(0x0302,0x0001);	
IMX298MIPI_write_cmos_sensor(0x0300,0x0005);	 
IMX298MIPI_write_cmos_sensor(0x030C,0x0006);	 
IMX298MIPI_write_cmos_sensor(0x030E,0x00c8);	
IMX298MIPI_write_cmos_sensor(0x030A,0x0001);	
IMX298MIPI_write_cmos_sensor(0x0308,0x0008);	 

IMX298MIPI_write_cmos_sensor(0x0342,0x2180);
IMX298MIPI_write_cmos_sensor(0x0340,0x0c66);	 
//IMX298MIPI_write_cmos_sensor(0x0202,0x0200);	
IMX298MIPI_write_cmos_sensor(0x0200,0x00C6);	 
IMX298MIPI_write_cmos_sensor(0x0B04,0x0101);	//M.BPC_On
IMX298MIPI_write_cmos_sensor(0x0B08,0x0000);	//D.BPC_Off
IMX298MIPI_write_cmos_sensor(0x0B00,0x0007);	//LSC_Off
IMX298MIPI_write_cmos_sensor(0x316A,0x00A0);	// OUTIF threshold

IMX298MIPI_write_cmos_sensor(0x0100,0x0100);
IMX298MIPI_write_cmos_sensor(0x0101,0x0300);

IMX298MIPISENSORDB("mccree mainsensor capture OK");
}


UINT32 IMX298MIPICapture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	 int ret = 0;
    TV_SWITCH_PRINT("[mccree]IMX298MIPICapture\n ");
 
	image_window->GrabStartX = IMX298MIPI_FULL_GRAB_START_X;
    image_window->GrabStartY = IMX298MIPI_FULL_GRAB_START_Y;
    image_window->ExposureWindowWidth= IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH;
    image_window->ExposureWindowHeight = IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT;
   
	
	XC9160_write_cmos_sensor(0xfffd ,0x80);
    XC9160_write_cmos_sensor(0xfffe,0x50);
    XC9160_write_cmos_sensor(0x004d ,0x01);
	
    MainSensorMIPIFullSizeCaptureSetting();

    	XC9160_write_cmos_sensor(0xfffd ,0x80);
    XC9160_write_cmos_sensor(0xfffe,0x50);
    XC9160_write_cmos_sensor(0x004d ,0x00);

     XC9160FullSizeCaptureSetting();
     
     
   XC9160_write_cmos_sensor(0xfffe,0x50);
	 ret=XC9160_read_cmos_sensor(0x004D);
    IMX298MIPISENSORDB("Colin_[XC9160]XC9160 Preview ret:0x004D_first---%x\n ",ret);

    TV_SWITCH_PRINT("[mccree]exit IMX298MIPICapture\n ");
    return ERROR_NONE; 
}/* IMX298MIPICapture() */



UINT32 IMX298MIPIGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
   // IMX298MIPISENSORDB("[IMX298MIPI]enter IMX298MIPIGetResolution:%d,%d\n ", IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH, IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT);

    pSensorResolution->SensorPreviewWidth= (IMAGE_SENSOR_PV_WIDTH - 48);   //IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH-2*IMX298MIPI_PV_GRAB_START_X;
    pSensorResolution->SensorPreviewHeight= (IMAGE_SENSOR_PV_HEIGHT - 36);   //IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT-2*IMX298MIPI_PV_GRAB_START_Y;
    pSensorResolution->SensorFullWidth= (IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH - 48);    //IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH-2*IMX298MIPI_FULL_GRAB_START_X; 
    pSensorResolution->SensorFullHeight= (IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT - 36);   //IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT-2*IMX298MIPI_FULL_GRAB_START_Y;
    pSensorResolution->SensorVideoWidth= (IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH - 48);   //IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH-2*IMX298MIPI_PV_GRAB_START_X; 
    pSensorResolution->SensorVideoHeight= (IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT - 36);   //IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT-2*IMX298MIPI_PV_GRAB_START_Y;;
    IMX298MIPISENSORDB("mccree - >>>>>[IMX298MIPI]exit IMX298MIPIGetResolution function:\n ");
    return ERROR_NONE;
}   /* IMX298MIPIGetResolution() */

UINT32 IMX298MIPIGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,MSDK_SENSOR_INFO_STRUCT *pSensorInfo,MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
    IMX298MIPISENSORDB("[mccree]IMX298MIPIGetInfo-619: ScenarioId=%d %d, %d\n ", ScenarioId, IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH,IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT);
    //switch (ScenarioId)
    //{
        //case MSDK_SCENARIO_ID_CAMERA_ZSD:
        //  pSensorInfo->SensorPreviewResolutionX=IMX298MIPI_IMAGE_SENSOR_SVGA_WIDTH-2*IMX298MIPI_FULL_GRAB_START_X;//IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH ;
        //  pSensorInfo->SensorPreviewResolutionY=IMX298MIPI_IMAGE_SENSOR_SVGA_HEIGHT-2*IMX298MIPI_FULL_GRAB_START_Y;//IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT ;
        //  pSensorInfo->SensorCameraPreviewFrameRate=15;
        //  break;
        //default:
            pSensorInfo->SensorPreviewResolutionX=(IMAGE_SENSOR_PV_WIDTH - 32);//IMX298MIPI_IMAGE_SENSOR_SVGA_WIDTH-2*IMX298MIPI_PV_GRAB_START_X; ;
            pSensorInfo->SensorPreviewResolutionY=(IMAGE_SENSOR_PV_HEIGHT - 24);//IMX298MIPI_IMAGE_SENSOR_SVGA_HEIGHT-2*IMX298MIPI_PV_GRAB_START_Y;
            pSensorInfo->SensorCameraPreviewFrameRate=30;
            //break;
    //}             
    pSensorInfo->SensorFullResolutionX= (IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH - 32);//IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH-2*IMX298MIPI_FULL_GRAB_START_X;
    pSensorInfo->SensorFullResolutionY= (IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT  - 24);//IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT-2*IMX298MIPI_FULL_GRAB_START_Y;
    pSensorInfo->SensorCameraPreviewFrameRate=30;
    pSensorInfo->SensorVideoFrameRate=30;
    pSensorInfo->SensorStillCaptureFrameRate=5;
    pSensorInfo->SensorWebCamCaptureFrameRate=15;
    pSensorInfo->SensorResetActiveHigh=FALSE;
    pSensorInfo->SensorResetDelayCount=4;
    pSensorInfo->SensorOutputDataFormat=SENSOR_OUTPUT_FORMAT_YUYV;
	
    pSensorInfo->SensorClockPolarity=SENSOR_CLOCK_POLARITY_LOW; 
    pSensorInfo->SensorClockFallingPolarity=SENSOR_CLOCK_POLARITY_LOW;

	//by zhangxueping
    //pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_HIGH;  
	pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;  
	
    pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
    //pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_HIGH;
	
    pSensorInfo->SensorInterruptDelayLines = 2;
    pSensorInfo->SensroInterfaceType=SENSOR_INTERFACE_TYPE_MIPI;

    pSensorInfo->CaptureDelayFrame = 3;
    pSensorInfo->PreviewDelayFrame = 3; 
	
    pSensorInfo->VideoDelayFrame = 3;       
    pSensorInfo->SensorMasterClockSwitch = 0; 
    pSensorInfo->YUVAwbDelayFrame = 5;
    pSensorInfo->YUVEffectDelayFrame= 3; 
    pSensorInfo->AEShutDelayFrame= 0;
    pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_8MA;        


	// imx179, add by zhangxueping
	pSensorInfo->MIPIsensorType = MIPI_OPHY_NCSI2;       // MIPI_OPHY_CSI2; 
	
	pSensorInfo->SettleDelayMode = MIPI_SETTLEDELAY_AUTO;
	//pSensorInfo->SettleDelayMode = MIPI_SETTLEDELAY_MANUAL;  

	pSensorInfo->HighSpeedVideoDelayFrame = 4;    
	//pSensorInfo->SlimVideoDelayFrame = 4;    
	//pSensorInfo->SensorModeNum = 5;

	
    switch (ScenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:

#if 1			
            pSensorInfo->SensorClockFreq=24;
			
            pSensorInfo->SensorClockDividCount= 5;
            pSensorInfo->SensorClockRisingCount= 0;
            pSensorInfo->SensorClockFallingCount= 2;
            pSensorInfo->SensorPixelClockCount= 3;
            pSensorInfo->SensorDataLatchCount= 2;
#endif
			
            pSensorInfo->SensorGrabStartX = IMX298MIPI_PV_GRAB_START_X; 
            pSensorInfo->SensorGrabStartY = IMX298MIPI_PV_GRAB_START_Y;   
            pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_2_LANE; //SENSOR_MIPI_1_LANE;    

			
            pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0;

			//pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 4; 
            pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 14; // 130;  // 13;  // 17; // 4; // 14;    // 4; 
            
            pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;		
			
            


#if 0

MUINT8 MIPIDataLowPwr2HighSpeedTermDelayCount;
MUINT8 MIPIDataLowPwr2HighSpeedSettleDelayCount;
MUINT8 MIPICLKLowPwr2HighSpeedTermDelayCount;

#endif
			
			
            pSensorInfo->SensorWidthSampling = 0; 
            pSensorInfo->SensorHightSampling = 0;   
            pSensorInfo->SensorPacketECCOrder = 1;      
            break;

			
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
            pSensorInfo->SensorClockFreq=24;
            pSensorInfo->SensorClockDividCount= 5;
            pSensorInfo->SensorClockRisingCount= 0;
            pSensorInfo->SensorClockFallingCount= 2;
            pSensorInfo->SensorPixelClockCount= 3;
            pSensorInfo->SensorDataLatchCount= 2;
            pSensorInfo->SensorGrabStartX = IMX298MIPI_FULL_GRAB_START_X; 
            pSensorInfo->SensorGrabStartY = IMX298MIPI_FULL_GRAB_START_Y;             
            pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_2_LANE; //SENSOR_MIPI_1_LANE;         
            pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
            pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount =4; 
            pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0; 
            pSensorInfo->SensorWidthSampling = 0; 
            pSensorInfo->SensorHightSampling = 0;
            pSensorInfo->SensorPacketECCOrder = 1;
            break;
        default:
            pSensorInfo->SensorClockFreq=24;
            pSensorInfo->SensorClockDividCount=5;
            pSensorInfo->SensorClockRisingCount=0;
            pSensorInfo->SensorClockFallingCount=2;
            pSensorInfo->SensorPixelClockCount=3;
            pSensorInfo->SensorDataLatchCount=2;
            pSensorInfo->SensorGrabStartX = IMX298MIPI_PV_GRAB_START_X; 
            pSensorInfo->SensorGrabStartY = IMX298MIPI_PV_GRAB_START_Y;             
            pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_2_LANE; //SENSOR_MIPI_1_LANE;         
            pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
            pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 4; 
            pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
            pSensorInfo->SensorWidthSampling = 0;
            pSensorInfo->SensorHightSampling = 0;   
            pSensorInfo->SensorPacketECCOrder = 1;
          break;


    }
    memcpy(pSensorConfigData, &IMX298MIPISensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));  
    IMX298MIPISENSORDB("[mccree]exit IMX298MIPIGetInfo function:\n ");  
    return ERROR_NONE;
}   /* IMX298MIPIGetInfo() */

UINT32 IMX298MIPIControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
      IMX298MIPISENSORDB("[mccree]enter IMX298MIPIControl function:\n ");
      spin_lock(&IMX298mipi_drv_lock);
      CurrentScenarioId = ScenarioId;
      spin_unlock(&IMX298mipi_drv_lock);
      switch (ScenarioId)
      {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        	 IMX298MIPIPreview(pImageWindow, pSensorConfigData);
             break;
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
             //IMX298MIPIPreview(pImageWindow, pSensorConfigData);
            // break;
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        	
             IMX298MIPICapture(pImageWindow, pSensorConfigData);
             break;
        default:
            return ERROR_INVALID_SCENARIO_ID;
    }
    IMX298MIPISENSORDB("[mccree]exit IMX298MIPIControl function:\n ");
    return ERROR_NONE;
}   /* IMX298MIPIControl() */

UINT32 IMX298MIPIYUVSensorSetting(FEATURE_ID iCmd, UINT32 iPara)
{

   // IMX298MIPISENSORDB("IMX298MIPIYUVSensorSetting:iCmd=%d,iPara=%d, %d \n",iCmd, iPara);
    IMX298MIPISENSORDB("[mccree]enter IMX298MIPIYUVSensorSetting function:\n ");
    switch (iCmd) {
        case FID_SCENE_MODE:
            //IMX298MIPI_set_scene_mode(iPara);
            break;      
        case FID_AWB_MODE:
                //IMX298MIPI_set_param_wb(iPara);
              break;
        case FID_COLOR_EFFECT:              
                //IMX298MIPI_set_param_effect(iPara);
              break;
        case FID_AE_EV:   
                //IMX298MIPI_set_param_exposure(iPara);
            break;
        case FID_AE_FLICKER:                    
                //IMX298MIPI_set_param_banding(iPara);
              break;
        case FID_AE_SCENE_MODE: 
                if (iPara == AE_MODE_OFF) 
                {
                    spin_lock(&IMX298mipi_drv_lock);
                    //IMX298MIPI_AE_ENABLE = KAL_FALSE; 
                    spin_unlock(&IMX298mipi_drv_lock);
        }
        else 
        {
                    spin_lock(&IMX298mipi_drv_lock);
                    //IMX298MIPI_AE_ENABLE = KAL_TRUE; 
                    spin_unlock(&IMX298mipi_drv_lock);
            }
                //IMX298MIPI_set_AE_mode(IMX298MIPI_AE_ENABLE);
        break; 
        case FID_ISP_CONTRAST:
            //IMX298MIPI_set_contrast(iPara);
            break;
        case FID_ISP_BRIGHT:
            //IMX298MIPI_set_brightness(iPara);
            break;
        case FID_ISP_SAT:
            //IMX298MIPI_set_saturation(iPara);
        break; 
    case FID_ZOOM_FACTOR:
            IMX298MIPISENSORDB("FID_ZOOM_FACTOR:%d\n", iPara);      
                    spin_lock(&IMX298mipi_drv_lock);
            //zoom_factor = iPara; 
                    spin_unlock(&IMX298mipi_drv_lock);
            break; 
        case FID_AE_ISO:
            //IMX298MIPI_set_iso(iPara);
            break;
#if 0 //afc
        case FID_AF_MODE:
             IMX298MIPI_set_param_afmode(iPara);
                    break;     
#endif            
      default:
                  break;
    }
    IMX298MIPISENSORDB("[mccree]exit IMX298MIPIYUVSensorSetting function:\n ");

      return TRUE;
}   /* IMX298MIPIYUVSensorSetting */

UINT32 IMX298MIPIGetDefaultFramerateByScenario(
  MSDK_SCENARIO_ID_ENUM scenarioId, MUINT32 *pframeRate)
{
    switch (scenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
             *pframeRate = 300;
             break;
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
             *pframeRate = 300;
             break;
        case MSDK_SCENARIO_ID_CAMERA_3D_PREVIEW: //added
        case MSDK_SCENARIO_ID_CAMERA_3D_VIDEO:
        case MSDK_SCENARIO_ID_CAMERA_3D_CAPTURE: //added
             *pframeRate = 300;
             break;
        default:
             *pframeRate = 300;
          break;
    }

  return ERROR_NONE;
}


UINT32 IMX298MIPIFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{
    UINT16 *pFeatureReturnPara16=(UINT16 *) pFeaturePara;
    UINT16 *pFeatureData16=(UINT16 *) pFeaturePara;
    UINT32 *pFeatureReturnPara32=(UINT32 *) pFeaturePara;
    UINT32 *pFeatureData32=(UINT32 *) pFeaturePara;
	unsigned long long *feature_data=(unsigned long long *) pFeaturePara;
    MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData=(MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
    MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData=(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;
    UINT32 Tony_Temp1 = 0;
    UINT32 Tony_Temp2 = 0;
    Tony_Temp1 = pFeaturePara[0];
    Tony_Temp2 = pFeaturePara[1];
    IMX298MIPISENSORDB("[mccree][IMX298MIPIFeatureControl]feature id=%d \n",FeatureId);
    switch (FeatureId)
    {
        case SENSOR_FEATURE_GET_RESOLUTION:
            *pFeatureReturnPara16++=IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH;
            *pFeatureReturnPara16=IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT;
            *pFeatureParaLen=4;
            break;
		case SENSOR_FEATURE_SET_DUAL_MODE:
			read_sensor_mode(pFeaturePara);
			break;	
        case SENSOR_FEATURE_GET_PERIOD:

#if 0 			
            switch(CurrentScenarioId)
            {
                case MSDK_SCENARIO_ID_CAMERA_ZSD:
                    *pFeatureReturnPara16++=IMX298MIPI_FULL_PERIOD_PIXEL_NUMS + IMX298MIPISensor.CaptureDummyPixels;
                    *pFeatureReturnPara16=IMX298MIPI_FULL_PERIOD_LINE_NUMS + IMX298MIPISensor.CaptureDummyLines;
                    *pFeatureParaLen=4;
                    break;
                default:
                    *pFeatureReturnPara16++=IMX298MIPI_PV_PERIOD_PIXEL_NUMS + IMX298MIPISensor.PreviewDummyPixels;
                    *pFeatureReturnPara16=IMX298MIPI_PV_PERIOD_LINE_NUMS + IMX298MIPISensor.PreviewDummyLines;
                    *pFeatureParaLen=4;
                    break;
            }
#else
			*pFeatureReturnPara16++=IMAGE_SENSOR_PV_WIDTH;
			*pFeatureReturnPara16=IMAGE_SENSOR_PV_HEIGHT;
			*pFeatureParaLen=4;

						
#endif			
            break;

        case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:

#if 0   
            switch(CurrentScenarioId)
            {

         
                case MSDK_SCENARIO_ID_CAMERA_ZSD:
                    *pFeatureReturnPara32 = IMX298MIPISensor.ZsdturePclk * 1000 *100;    //unit: Hz             
                    *pFeatureParaLen=4;
                    break;
                default:
                    *pFeatureReturnPara32 = IMX298MIPISensor.PreviewPclk * 1000 *100;    //unit: Hz
                    *pFeatureParaLen=4;
					
                    break;
            }

#else
			*pFeatureReturnPara32 = 48000000;	 //unit: Hz
			*pFeatureParaLen=4;
			
#endif
			
            break;

        case SENSOR_FEATURE_SET_ESHUTTER:
            break;
        case SENSOR_FEATURE_GET_EXIF_INFO:
            IMX298MIPIGetExifInfo(*feature_data);
            break;
        case SENSOR_FEATURE_SET_NIGHTMODE:
            //IMX298MIPI_night_mode((BOOL) *pFeatureData16);
            break;
        case SENSOR_FEATURE_SET_GAIN:
            break;
        case SENSOR_FEATURE_SET_FLASHLIGHT:
            break;
        case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
            break;
        case SENSOR_FEATURE_SET_REGISTER:
			
			//mutex_lock(&TV_switch_lock);
			
            if(pSensorRegData->RegAddr & 0x10000)            
            {            	
				IMX298MIPISENSORDB("SENSOR_FEATURE_SET_REGISTER(IMX298): addr:%x  value:%x\n",pSensorRegData->RegAddr,pSensorRegData->RegData);
				XC9160_write_cmos_sensor(0xfffd ,0x80);
				XC9160_write_cmos_sensor(0xfffe,0x50);
				XC9160_write_cmos_sensor(0x004d ,0x01);
				IMX298MIPI_write_cmos_sensor(pSensorRegData->RegAddr&0xffff, pSensorRegData->RegData);
				XC9160_write_cmos_sensor(0x004d ,0x00);
            }
			else
			{
				IMX298MIPISENSORDB("SENSOR_FEATURE_SET_REGISTER(XC9160): addr:%x  value:%x\n",pSensorRegData->RegAddr,pSensorRegData->RegData);
				XC9160_write_cmos_sensor(pSensorRegData->RegAddr&0xffff, pSensorRegData->RegData);
			}
			//mutex_unlock(&TV_switch_lock);
			
            break;
        case SENSOR_FEATURE_GET_REGISTER:

			//mutex_lock(&TV_switch_lock);										
            if(pSensorRegData->RegAddr & 0x10000)            
            {
				XC9160_write_cmos_sensor(0xfffd ,0x80);
				XC9160_write_cmos_sensor(0xfffe,0x50);
				XC9160_write_cmos_sensor(0x004d ,0x01);
				pSensorRegData->RegData = IMX298MIPIYUV_read_cmos_sensor(pSensorRegData->RegAddr&0xffff);				
				XC9160_write_cmos_sensor(0x004d ,0x00);				
				IMX298MIPISENSORDB("SENSOR_FEATURE_GET_REGISTER(IMX298): addr:0x%x  value:0x%x\n",pSensorRegData->RegAddr,pSensorRegData->RegData);
            }
			else
			{	
				pSensorRegData->RegData = XC9160_read_cmos_sensor(pSensorRegData->RegAddr&0xffff);
				IMX298MIPISENSORDB("SENSOR_FEATURE_GET_REGISTER(XC9160): addr:0x%x  value:0x%x\n",pSensorRegData->RegAddr,pSensorRegData->RegData);
    		}
			//mutex_unlock(&TV_switch_lock);	
			
            break;
        case SENSOR_FEATURE_GET_CONFIG_PARA:
            memcpy(pSensorConfigData, &IMX298MIPISensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
            *pFeatureParaLen=sizeof(MSDK_SENSOR_CONFIG_STRUCT);
            break;
        case SENSOR_FEATURE_SET_CCT_REGISTER:
        case SENSOR_FEATURE_GET_CCT_REGISTER:
        case SENSOR_FEATURE_SET_ENG_REGISTER:
        case SENSOR_FEATURE_GET_ENG_REGISTER:
        case SENSOR_FEATURE_GET_REGISTER_DEFAULT:
        case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
        case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
        case SENSOR_FEATURE_GET_GROUP_INFO:
        case SENSOR_FEATURE_GET_ITEM_INFO:
        case SENSOR_FEATURE_SET_ITEM_INFO:
        case SENSOR_FEATURE_GET_ENG_INFO:
            break;
        case SENSOR_FEATURE_GET_GROUP_COUNT:
            *pFeatureReturnPara32++=0;
            *pFeatureParaLen=4;    
            break; 
        case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
            *pFeatureReturnPara32=LENS_DRIVER_ID_DO_NOT_CARE;
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_SET_TEST_PATTERN:            
            IMX298SetTestPatternMode((BOOL)*pFeatureData16);            
            break;
        case SENSOR_FEATURE_CHECK_SENSOR_ID:
            IMX298MIPI_GetSensorID(pFeatureData32);
            break;
        case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE:
            *pFeatureReturnPara32=IMX298_TEST_PATTERN_CHECKSUM;
            *pFeatureParaLen=4;
            break;

			
        case SENSOR_FEATURE_SET_YUV_CMD:
            IMX298MIPIYUVSensorSetting((FEATURE_ID)*feature_data, *(feature_data+1));
            break;

		case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
			IMX298MIPIGetDefaultFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*feature_data, (MUINT32 *)(uintptr_t)(*(feature_data+1)));
			break;

#if 0

			
        case SENSOR_FEATURE_SET_YUV_3A_CMD:
            IMX298MIPI_3ACtrl((ACDK_SENSOR_3A_LOCK_ENUM)*pFeatureData32);
            break;
        case SENSOR_FEATURE_SET_VIDEO_MODE:
            IMX298MIPIYUVSetVideoMode(*pFeatureData16);
            break;
        case SENSOR_FEATURE_GET_EV_AWB_REF:
            IMX298MIPIGetEvAwbRef(*pFeatureData32);
            break;
        case SENSOR_FEATURE_GET_SHUTTER_GAIN_AWB_GAIN:
            IMX298MIPIGetCurAeAwbInfo(*pFeatureData32);         
            break;
        case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
            IMX298MIPIMaxFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*pFeatureData32,*(pFeatureData32+1));
            break;
        case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
            IMX298MIPIGetDefaultFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*pFeatureData32,(MUINT32 *)*(pFeatureData32+1));
            break;
        case SENSOR_FEATURE_GET_AE_AWB_LOCK_INFO:
            //IMX298MIPI_get_AEAWB_lock(*pFeatureData32, *(pFeatureData32+1));  // by zhangxueping
            break;
        case SENSOR_FEATURE_GET_DELAY_INFO:
            IMX298MIPISENSORDB("SENSOR_FEATURE_GET_DELAY_INFO\n");
            //IMX298MIPI_GetDelayInfo(*pFeatureData32);         // by zhangxueping
            break;
        case SENSOR_FEATURE_AUTOTEST_CMD:
            IMX298MIPISENSORDB("SENSOR_FEATURE_AUTOTEST_CMD\n");
            IMX298MIPI_AutoTestCmd(*feature_data,*(feature_data+1));
            break;
        case SENSOR_FEATURE_INITIALIZE_AF:  
	     // IMX298_FOCUS_OVT_AFC_Init();         
             break;
        case SENSOR_FEATURE_MOVE_FOCUS_LENS:
            IMX298_FOCUS_Move_to(*pFeatureData16);
            break;
        case SENSOR_FEATURE_GET_AF_STATUS:
            IMX298_FOCUS_OVT_AFC_Get_AF_Status(pFeatureReturnPara32);            
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_GET_AF_INF:
            IMX298_FOCUS_Get_AF_Inf(pFeatureReturnPara32);
            *pFeatureParaLen=4;            
            break;
        case SENSOR_FEATURE_GET_AF_MACRO:
            IMX298_FOCUS_Get_AF_Macro(pFeatureReturnPara32);
            *pFeatureParaLen=4;            
            break;
        case SENSOR_FEATURE_CONSTANT_AF:
            //IMX298_FOCUS_OVT_AFC_Constant_Focus();
             break;
        case SENSOR_FEATURE_SET_AF_WINDOW:       
            IMX298_FOCUS_Set_AF_Window(*feature_data);
            break;
        case SENSOR_FEATURE_SINGLE_FOCUS_MODE:
            //IMX298_FOCUS_OVT_AFC_Single_Focus();
            break;  
        case SENSOR_FEATURE_CANCEL_AF:
            IMX298_FOCUS_OVT_AFC_Cancel_Focus();
            break;                  
        case SENSOR_FEATURE_GET_AF_MAX_NUM_FOCUS_AREAS:
            IMX298_FOCUS_Get_AF_Max_Num_Focus_Areas(pFeatureReturnPara32);            
            *pFeatureParaLen=4;
            break;        
        case SENSOR_FEATURE_GET_AE_MAX_NUM_METERING_AREAS:
            IMX298_FOCUS_Get_AE_Max_Num_Metering_Areas(pFeatureReturnPara32);            
            *pFeatureParaLen=4;
            break;        
        case SENSOR_FEATURE_SET_AE_WINDOW:
//	            IMX298MIPISENSORDB("AE zone addr = 0x%x\n",*feature_data);            
            IMX298_FOCUS_Set_AE_Window(*pFeatureData32);
            break; 
#endif

        default:
            IMX298MIPISENSORDB("IMX298MIPIFeatureControl:default \n");
            break;          
    }
    IMX298MIPISENSORDB("[IMX298MIPI]exit IMX298MIPIFeatureControl function:\n ");
    return ERROR_NONE;
}   /* IMX298MIPIFeatureControl() */

SENSOR_FUNCTION_STRUCT  SensorFuncIMX298MIPI=
{
    IMX298MIPIOpen,
    IMX298MIPIGetInfo,
    IMX298MIPIGetResolution,
    IMX298MIPIFeatureControl,
    IMX298MIPIControl,
    IMX298MIPIClose
};

UINT32 XC9160IMX298MIPISensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
    /* To Do : Check Sensor status here */
    if (pfFunc!=NULL)
        *pfFunc=&SensorFuncIMX298MIPI;
    return ERROR_NONE;
}/* SensorInit() */
	

