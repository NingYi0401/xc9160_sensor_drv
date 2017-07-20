#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <asm/io.h>

#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"
#include "xc9160yuv_Sensor.h"
#include "xc9160yuv_Camera_Sensor_para.h"
#include "xc9160yuv_CameraCustomized.h" 

#define XC9160YUV_DEBUG
#ifdef XC9160YUV_DEBUG
#define XC9160DB(a,arg... ) printk("[XC9160]" a,##arg)
#else
#define XC9160DB(x,...)
#endif

MUINT16 xc9160_capture_width = 0;
MUINT16 xc9160_capture_height = 0;
MUINT16 xc9160_preview_width = 0;
MUINT16 xc9160_preview_height = 0;

extern int iReadReg(u16 a_u2Addr , u8 * a_puBuff , u16 i2cId);
extern int iWriteReg(u16 a_u2Addr , u32 a_u4Data , u32 a_u4Bytes , u16 i2cId);

extern int iReadRegI2C(u8 *a_pSendData, u16 a_sizeSendData, u8 *a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData, u16 a_sizeSendData, u16 i2cId);

#define mainsensor_write_cmos_sensor(addr, para) iWriteReg((u16) addr , (u32) para ,2,S5K3L8MIPI_WRITE_ID)
#define subsensor_write_cmos_sensor(addr, para) iWriteReg((u16) addr , (u32) para ,1,OV5675MIPI_WRITE_ID)

void XC9160_write_cmos_sensor(kal_uint16 addr, kal_uint32 para)
{		
	iWriteReg((u16) addr , (u32) para ,1,XC9160_WRITE_ID); 
}

kal_uint16 XC9160_read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte=0;	
    iReadReg((u16) addr ,(u8*)&get_byte,XC9160_WRITE_ID);
    return get_byte;
}

kal_uint16 mainsensor_read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte=0;	
    iReadReg((u16) addr ,(u8*)&get_byte,S5K3L8MIPI_WRITE_ID);	
    return get_byte;
}

kal_uint16 subsensor_read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte=0;
    iReadReg((u16) addr ,(u8*)&get_byte,OV5675MIPI_READ_ID);
	return get_byte;
}

#define mDELAY(ms)  mdelay(ms)

//=====================================================================//
//==========================open init==================================//
 void XC9160InitialSetting(void)
 {
 	XC9160DB("[xchip]XC9160_ISP_InitialSetting start \n");
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
	XC9160DB("[xchip]XC9160_ISP_InitialSetting end\n");
 }

void MainSensorInitialSetting(void)
 {
 	XC9160DB("[xchip]MainSensorMIPIInitialSetting start \n");

 	XC9160DB("[xchip]MainSensorMIPIInitialSetting end \n");
 }

void SubSensorInitialSetting(void)
 {
 	XC9160DB("[xchip]SubSensorMIPIInitialSetting start \n");

 	XC9160DB("[xchip]SubSensorMIPIInitialSetting end \n");
 }
//==================================================================//
//=============================preview==============================//

void XC9160PreviewSetting(void)
{
	XC9160DB("[xchip]XC9160PreviewSetting start \n");
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
	XC9160_write_cmos_sensor(0x0090,0x3a);
	
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
	XC9160DB("[xchip]XC9160PreviewSetting end \n");
} 

void MainSensorPreviewSetting(void)
{
    XC9160DB("[xchip]MainSensorPreviewSetting start\n ");

    XC9160DB("[xchip]MainSensorPreviewSetting end\n ");    
}  

void SubSensorPreviewSetting(void)
{
	XC9160DB("[xchip]SubSensorPreviewSetting start\n ");

    XC9160DB("[xchip]SubSensorPreviewSetting end\n "); 
}
//================================================================//
//=====================capture====================================//
void XC9160CaptureSetting(void)
{
	XC9160DB("[xchip]XC9160CaptureSetting start\n ");

    XC9160DB("[xchip]XC9160CaptureSetting end\n ");   
}

void MainSensorCaptureSetting(void)
{
	XC9160DB("[xchip]MainSensorCaptureSetting start\n ");

    XC9160DB("[xchip]MainSensorCaptureSetting end\n ");   
}

void SubSensorCaptureSetting(void)
{
	XC9160DB("[xchip]SubSensorCaptureSetting start\n ");

    XC9160DB("[xchip]SubSensorCaptureSetting end\n ");   
}

//===================================================================//
//=============================dualmode==============================//
//===================================================================//

////=====================================================================//
//==========================open init==================================//
 void Dual_XC9160InitialSetting(void)
 {
 	XC9160DB("[xchip]Dual_XC9160_ISP_InitialSetting start \n");
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
	
	XC9160_write_cmos_sensor(0x0030,0x44);
	XC9160_write_cmos_sensor(0x0031,0x58);
	XC9160_write_cmos_sensor(0x0032,0x34);  
	XC9160_write_cmos_sensor(0x0033,0x70);   
	
	XC9160_write_cmos_sensor(0x0020,0x03);
	XC9160_write_cmos_sensor(0x0021,0x0d);
	XC9160_write_cmos_sensor(0x0022,0x01);
	XC9160_write_cmos_sensor(0x0023,0x86);
	XC9160_write_cmos_sensor(0x0024,0x20);  //14
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
	
	XC9160_write_cmos_sensor(0xfffe,0x50);
	XC9160_write_cmos_sensor(0x00bc,0x19);
	XC9160_write_cmos_sensor(0x0090,0x38);
	
	XC9160_write_cmos_sensor(0x0200,0x0f);   //mipi_rx1_pad_en
	XC9160_write_cmos_sensor(0x0201,0x00);
	XC9160_write_cmos_sensor(0x0202,0x80);
	XC9160_write_cmos_sensor(0x0203,0x00);
	
	XC9160_write_cmos_sensor(0xfffe,0x26);
	XC9160_write_cmos_sensor(0x8000,0x1d);   //colorbar0
	XC9160_write_cmos_sensor(0x8001,0x38);
	XC9160_write_cmos_sensor(0x8002,0x08);
	XC9160_write_cmos_sensor(0x8003,0x18);
	XC9160_write_cmos_sensor(0x8004,0x06);
	XC9160_write_cmos_sensor(0x8005,0x40);
	XC9160_write_cmos_sensor(0x8006,0x40);
	XC9160_write_cmos_sensor(0x8007,0x10);  //30
	XC9160_write_cmos_sensor(0x8008,0xf0);  //60
	XC9160_write_cmos_sensor(0x8009,0x00);
	XC9160_write_cmos_sensor(0x800b,0x00);
	
	XC9160_write_cmos_sensor(0xfffe,0x26); 
	XC9160_write_cmos_sensor(0x8010,0x05); 
	XC9160_write_cmos_sensor(0x8012,0x20);   //crop0
	XC9160_write_cmos_sensor(0x8013,0x08);
	XC9160_write_cmos_sensor(0x8014,0x00);
	XC9160_write_cmos_sensor(0x8015,0x06);
	XC9160_write_cmos_sensor(0x8016,0x00);
	XC9160_write_cmos_sensor(0x8017,0x00);
	XC9160_write_cmos_sensor(0x8018,0x01);
	XC9160_write_cmos_sensor(0x8019,0x00);
	
	
	XC9160_write_cmos_sensor(0xfffe,0x30);  //isp0
	XC9160_write_cmos_sensor(0x0000,0x01);
	XC9160_write_cmos_sensor(0x0001,0x00);
	XC9160_write_cmos_sensor(0x0002,0x10);
	XC9160_write_cmos_sensor(0x0003,0x20);
	XC9160_write_cmos_sensor(0x0004,0x10);
	
	XC9160_write_cmos_sensor(0x0019,0x01);
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
	
	XC9160_write_cmos_sensor(0x2015,0x80);
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
	
	//XC9160_write_cmos_sensor(0xfffe,0x2e);  //retiming
	//XC9160_write_cmos_sensor(0x0001,0xee);
	//XC9160_write_cmos_sensor(0x0003,0x00);
	//XC9160_write_cmos_sensor(0x0004,0xa0);
	
	//isp top
	XC9160_write_cmos_sensor(0xfffe,0x30);
	XC9160_write_cmos_sensor(0x0000,0x57); //47
	XC9160_write_cmos_sensor(0x0001,0x82); //82                                              
	XC9160_write_cmos_sensor(0x0002,0x96); //90  satuation
	XC9160_write_cmos_sensor(0x0003,0x31); //35
	XC9160_write_cmos_sensor(0x0004,0x10);
	
	XC9160_write_cmos_sensor(0x0019,0x09);
	XC9160_write_cmos_sensor(0x0051,0x03);//ae 统计数据选择
	XC9160_write_cmos_sensor(0x1700,0x09);
	XC9160_write_cmos_sensor(0x1701,0x44);
	XC9160_write_cmos_sensor(0x1702,0x48);
	XC9160_write_cmos_sensor(0x1704,0x24);

	XC9160DB("[xchip]Dual_XC9160_ISP_InitialSetting end\n");
 }

void Dual_MainSensorInitialSetting(void)
 {
 	XC9160DB("[xchip]Dual_MainSensorMIPIInitialSetting start \n");
	mainsensor_write_cmos_sensor(0x6028,0x4000);
	mainsensor_write_cmos_sensor(0x6214,0xFFFF);
	mainsensor_write_cmos_sensor(0x6216,0xFFFF);
	mainsensor_write_cmos_sensor(0x6218,0x0000);
	mainsensor_write_cmos_sensor(0x621A,0x0000);
	mainsensor_write_cmos_sensor(0x6028,0x2000);
	mainsensor_write_cmos_sensor(0x602A,0x2450);
	mainsensor_write_cmos_sensor(0x6F12,0x0448);
	mainsensor_write_cmos_sensor(0x6F12,0x0349);
	mainsensor_write_cmos_sensor(0x6F12,0x0160);
	mainsensor_write_cmos_sensor(0x6F12,0xC26A);
	mainsensor_write_cmos_sensor(0x6F12,0x511A);
	mainsensor_write_cmos_sensor(0x6F12,0x8180);
	mainsensor_write_cmos_sensor(0x6F12,0x00F0);
	mainsensor_write_cmos_sensor(0x6F12,0x48B8);
	mainsensor_write_cmos_sensor(0x6F12,0x2000);
	mainsensor_write_cmos_sensor(0x6F12,0x2588);
	mainsensor_write_cmos_sensor(0x6F12,0x2000);
	mainsensor_write_cmos_sensor(0x6F12,0x16C0);
	mainsensor_write_cmos_sensor(0x6F12,0x0000);
	mainsensor_write_cmos_sensor(0x6F12,0x0000);
	mainsensor_write_cmos_sensor(0x6F12,0x0000);
	mainsensor_write_cmos_sensor(0x6F12,0x0000);
	mainsensor_write_cmos_sensor(0x6F12,0x10B5);
	mainsensor_write_cmos_sensor(0x6F12,0x00F0);
	mainsensor_write_cmos_sensor(0x6F12,0x5DF8);
	mainsensor_write_cmos_sensor(0x6F12,0x2748);
	mainsensor_write_cmos_sensor(0x6F12,0x4078);
	mainsensor_write_cmos_sensor(0x6F12,0x0028);
	mainsensor_write_cmos_sensor(0x6F12,0x0AD0);
	mainsensor_write_cmos_sensor(0x6F12,0x00F0);
	mainsensor_write_cmos_sensor(0x6F12,0x5CF8);
	mainsensor_write_cmos_sensor(0x6F12,0x2549);
	mainsensor_write_cmos_sensor(0x6F12,0xB1F8);
	mainsensor_write_cmos_sensor(0x6F12,0x1403);
	mainsensor_write_cmos_sensor(0x6F12,0x4200);
	mainsensor_write_cmos_sensor(0x6F12,0x2448);
	mainsensor_write_cmos_sensor(0x6F12,0x4282);
	mainsensor_write_cmos_sensor(0x6F12,0x91F8);
	mainsensor_write_cmos_sensor(0x6F12,0x9610);
	mainsensor_write_cmos_sensor(0x6F12,0x4187);
	mainsensor_write_cmos_sensor(0x6F12,0x10BD);
	mainsensor_write_cmos_sensor(0x6F12,0x70B5);
	mainsensor_write_cmos_sensor(0x6F12,0x0446);
	mainsensor_write_cmos_sensor(0x6F12,0x2148);
	mainsensor_write_cmos_sensor(0x6F12,0x0022);
	mainsensor_write_cmos_sensor(0x6F12,0x4068);
	mainsensor_write_cmos_sensor(0x6F12,0x86B2);
	mainsensor_write_cmos_sensor(0x6F12,0x050C);
	mainsensor_write_cmos_sensor(0x6F12,0x3146);
	mainsensor_write_cmos_sensor(0x6F12,0x2846);
	mainsensor_write_cmos_sensor(0x6F12,0x00F0);
	mainsensor_write_cmos_sensor(0x6F12,0x4CF8);
	mainsensor_write_cmos_sensor(0x6F12,0x2046);
	mainsensor_write_cmos_sensor(0x6F12,0x00F0);
	mainsensor_write_cmos_sensor(0x6F12,0x4EF8);
	mainsensor_write_cmos_sensor(0x6F12,0x14F8);
	mainsensor_write_cmos_sensor(0x6F12,0x680F);
	mainsensor_write_cmos_sensor(0x6F12,0x6178);
	mainsensor_write_cmos_sensor(0x6F12,0x40EA);
	mainsensor_write_cmos_sensor(0x6F12,0x4100);
	mainsensor_write_cmos_sensor(0x6F12,0x1749);
	mainsensor_write_cmos_sensor(0x6F12,0xC886);
	mainsensor_write_cmos_sensor(0x6F12,0x1848);
	mainsensor_write_cmos_sensor(0x6F12,0x2278);
	mainsensor_write_cmos_sensor(0x6F12,0x007C);
	mainsensor_write_cmos_sensor(0x6F12,0x4240);
	mainsensor_write_cmos_sensor(0x6F12,0x1348);
	mainsensor_write_cmos_sensor(0x6F12,0xA230);
	mainsensor_write_cmos_sensor(0x6F12,0x8378);
	mainsensor_write_cmos_sensor(0x6F12,0x43EA);
	mainsensor_write_cmos_sensor(0x6F12,0xC202);
	mainsensor_write_cmos_sensor(0x6F12,0x0378);
	mainsensor_write_cmos_sensor(0x6F12,0x4078);
	mainsensor_write_cmos_sensor(0x6F12,0x9B00);
	mainsensor_write_cmos_sensor(0x6F12,0x43EA);
	mainsensor_write_cmos_sensor(0x6F12,0x4000);
	mainsensor_write_cmos_sensor(0x6F12,0x0243);
	mainsensor_write_cmos_sensor(0x6F12,0xD0B2);
	mainsensor_write_cmos_sensor(0x6F12,0x0882);
	mainsensor_write_cmos_sensor(0x6F12,0x3146);
	mainsensor_write_cmos_sensor(0x6F12,0x2846);
	mainsensor_write_cmos_sensor(0x6F12,0xBDE8);
	mainsensor_write_cmos_sensor(0x6F12,0x7040);
	mainsensor_write_cmos_sensor(0x6F12,0x0122);
	mainsensor_write_cmos_sensor(0x6F12,0x00F0);
	mainsensor_write_cmos_sensor(0x6F12,0x2AB8);
	mainsensor_write_cmos_sensor(0x6F12,0x10B5);
	mainsensor_write_cmos_sensor(0x6F12,0x0022);
	mainsensor_write_cmos_sensor(0x6F12,0xAFF2);
	mainsensor_write_cmos_sensor(0x6F12,0x8701);
	mainsensor_write_cmos_sensor(0x6F12,0x0B48);
	mainsensor_write_cmos_sensor(0x6F12,0x00F0);
	mainsensor_write_cmos_sensor(0x6F12,0x2DF8);
	mainsensor_write_cmos_sensor(0x6F12,0x084C);
	mainsensor_write_cmos_sensor(0x6F12,0x0022);
	mainsensor_write_cmos_sensor(0x6F12,0xAFF2);
	mainsensor_write_cmos_sensor(0x6F12,0x6D01);
	mainsensor_write_cmos_sensor(0x6F12,0x2060);
	mainsensor_write_cmos_sensor(0x6F12,0x0848);
	mainsensor_write_cmos_sensor(0x6F12,0x00F0);
	mainsensor_write_cmos_sensor(0x6F12,0x25F8);
	mainsensor_write_cmos_sensor(0x6F12,0x6060);
	mainsensor_write_cmos_sensor(0x6F12,0x10BD);
	mainsensor_write_cmos_sensor(0x6F12,0x0000);
	mainsensor_write_cmos_sensor(0x6F12,0x2000);
	mainsensor_write_cmos_sensor(0x6F12,0x0550);
	mainsensor_write_cmos_sensor(0x6F12,0x2000);
	mainsensor_write_cmos_sensor(0x6F12,0x0C60);
	mainsensor_write_cmos_sensor(0x6F12,0x4000);
	mainsensor_write_cmos_sensor(0x6F12,0xD000);
	mainsensor_write_cmos_sensor(0x6F12,0x2000);
	mainsensor_write_cmos_sensor(0x6F12,0x2580);
	mainsensor_write_cmos_sensor(0x6F12,0x2000);
	mainsensor_write_cmos_sensor(0x6F12,0x16F0);
	mainsensor_write_cmos_sensor(0x6F12,0x0000);
	mainsensor_write_cmos_sensor(0x6F12,0x2221);
	mainsensor_write_cmos_sensor(0x6F12,0x0000);
	mainsensor_write_cmos_sensor(0x6F12,0x2249);
	mainsensor_write_cmos_sensor(0x6F12,0x42F2);
	mainsensor_write_cmos_sensor(0x6F12,0x351C);
	mainsensor_write_cmos_sensor(0x6F12,0xC0F2);
	mainsensor_write_cmos_sensor(0x6F12,0x000C);
	mainsensor_write_cmos_sensor(0x6F12,0x6047);
	mainsensor_write_cmos_sensor(0x6F12,0x42F2);
	mainsensor_write_cmos_sensor(0x6F12,0xE11C);
	mainsensor_write_cmos_sensor(0x6F12,0xC0F2);
	mainsensor_write_cmos_sensor(0x6F12,0x000C);
	mainsensor_write_cmos_sensor(0x6F12,0x6047);
	mainsensor_write_cmos_sensor(0x6F12,0x40F2);
	mainsensor_write_cmos_sensor(0x6F12,0x077C);
	mainsensor_write_cmos_sensor(0x6F12,0xC0F2);
	mainsensor_write_cmos_sensor(0x6F12,0x000C);
	mainsensor_write_cmos_sensor(0x6F12,0x6047);
	mainsensor_write_cmos_sensor(0x6F12,0x42F2);
	mainsensor_write_cmos_sensor(0x6F12,0x492C);
	mainsensor_write_cmos_sensor(0x6F12,0xC0F2);
	mainsensor_write_cmos_sensor(0x6F12,0x000C);
	mainsensor_write_cmos_sensor(0x6F12,0x6047);
	mainsensor_write_cmos_sensor(0x6F12,0x4BF2);
	mainsensor_write_cmos_sensor(0x6F12,0x453C);
	mainsensor_write_cmos_sensor(0x6F12,0xC0F2);
	mainsensor_write_cmos_sensor(0x6F12,0x000C);
	mainsensor_write_cmos_sensor(0x6F12,0x6047);
	mainsensor_write_cmos_sensor(0x6F12,0x0000);
	mainsensor_write_cmos_sensor(0x6F12,0x0000);
	mainsensor_write_cmos_sensor(0x6F12,0x0000);
	mainsensor_write_cmos_sensor(0x6F12,0x0000);
	mainsensor_write_cmos_sensor(0x6F12,0x0000);
	mainsensor_write_cmos_sensor(0x6F12,0x0000);
	mainsensor_write_cmos_sensor(0x6F12,0x0000);
	mainsensor_write_cmos_sensor(0x6F12,0x30C8);
	mainsensor_write_cmos_sensor(0x6F12,0x0157);
	mainsensor_write_cmos_sensor(0x6F12,0x0000);
	mainsensor_write_cmos_sensor(0x6F12,0x0003);
	mainsensor_write_cmos_sensor(0x6028,0x2000);
	mainsensor_write_cmos_sensor(0x602A,0x1082);
	mainsensor_write_cmos_sensor(0x6F12,0x8010);
	mainsensor_write_cmos_sensor(0x6028,0x4000);
	mainsensor_write_cmos_sensor(0x31CE,0x0001);
	mainsensor_write_cmos_sensor(0x0200,0x00C6);
	mainsensor_write_cmos_sensor(0x3734,0x0010);
	mainsensor_write_cmos_sensor(0x3736,0x0001);
	mainsensor_write_cmos_sensor(0x3738,0x0001);
	mainsensor_write_cmos_sensor(0x37CC,0x0000);
	mainsensor_write_cmos_sensor(0x3744,0x0100);
	mainsensor_write_cmos_sensor(0x3762,0x0105);
	mainsensor_write_cmos_sensor(0x3764,0x0105);
	mainsensor_write_cmos_sensor(0x376A,0x00F0);
	mainsensor_write_cmos_sensor(0x344A,0x000F);
	mainsensor_write_cmos_sensor(0x344C,0x003D);
	mainsensor_write_cmos_sensor(0xF460,0x0020);
	mainsensor_write_cmos_sensor(0xF414,0x24C2);
	mainsensor_write_cmos_sensor(0xF416,0x0183);
	mainsensor_write_cmos_sensor(0xF468,0x0405);
	mainsensor_write_cmos_sensor(0x3424,0x0807);
	mainsensor_write_cmos_sensor(0x3426,0x0F07);
	mainsensor_write_cmos_sensor(0x3428,0x0F07);
	mainsensor_write_cmos_sensor(0x341E,0x0804);
	mainsensor_write_cmos_sensor(0x3420,0x0C0C);
	mainsensor_write_cmos_sensor(0x3422,0x2D2D);
	mainsensor_write_cmos_sensor(0xF462,0x003A);
	mainsensor_write_cmos_sensor(0x3450,0x0010);
	mainsensor_write_cmos_sensor(0x3452,0x0010);
	mainsensor_write_cmos_sensor(0xF446,0x0020);
	mainsensor_write_cmos_sensor(0xF44E,0x000C);
	mainsensor_write_cmos_sensor(0x31FA,0x0007);
	mainsensor_write_cmos_sensor(0x31FC,0x0161);
	mainsensor_write_cmos_sensor(0x31FE,0x0009);
	mainsensor_write_cmos_sensor(0x3200,0x000C);
	mainsensor_write_cmos_sensor(0x3202,0x007F);
	mainsensor_write_cmos_sensor(0x3204,0x00A2);
	mainsensor_write_cmos_sensor(0x3206,0x007D);
	mainsensor_write_cmos_sensor(0x3208,0x00A4);
	mainsensor_write_cmos_sensor(0x3334,0x00A7);
	mainsensor_write_cmos_sensor(0x3336,0x00A5);
	mainsensor_write_cmos_sensor(0x3338,0x0033);
	mainsensor_write_cmos_sensor(0x333A,0x0006);
	mainsensor_write_cmos_sensor(0x333C,0x009F);
	mainsensor_write_cmos_sensor(0x333E,0x008C);
	mainsensor_write_cmos_sensor(0x3340,0x002D);
	mainsensor_write_cmos_sensor(0x3342,0x000A);
	mainsensor_write_cmos_sensor(0x3344,0x002F);
	mainsensor_write_cmos_sensor(0x3346,0x0008);
	mainsensor_write_cmos_sensor(0x3348,0x009F);
	mainsensor_write_cmos_sensor(0x334A,0x008C);
	mainsensor_write_cmos_sensor(0x334C,0x002D);
	mainsensor_write_cmos_sensor(0x334E,0x000A);
	mainsensor_write_cmos_sensor(0x3350,0x000A);
	mainsensor_write_cmos_sensor(0x320A,0x007B);
	mainsensor_write_cmos_sensor(0x320C,0x0161);
	mainsensor_write_cmos_sensor(0x320E,0x007F);
	mainsensor_write_cmos_sensor(0x3210,0x015F);
	mainsensor_write_cmos_sensor(0x3212,0x007B);
	mainsensor_write_cmos_sensor(0x3214,0x00B0);
	mainsensor_write_cmos_sensor(0x3216,0x0009);
	mainsensor_write_cmos_sensor(0x3218,0x0038);
	mainsensor_write_cmos_sensor(0x321A,0x0009);
	mainsensor_write_cmos_sensor(0x321C,0x0031);
	mainsensor_write_cmos_sensor(0x321E,0x0009);
	mainsensor_write_cmos_sensor(0x3220,0x0038);
	mainsensor_write_cmos_sensor(0x3222,0x0009);
	mainsensor_write_cmos_sensor(0x3224,0x007B);
	mainsensor_write_cmos_sensor(0x3226,0x0001);
	mainsensor_write_cmos_sensor(0x3228,0x0010);
	mainsensor_write_cmos_sensor(0x322A,0x00A2);
	mainsensor_write_cmos_sensor(0x322C,0x00B1);
	mainsensor_write_cmos_sensor(0x322E,0x0002);
	mainsensor_write_cmos_sensor(0x3230,0x015D);
	mainsensor_write_cmos_sensor(0x3232,0x0001);
	mainsensor_write_cmos_sensor(0x3234,0x015D);
	mainsensor_write_cmos_sensor(0x3236,0x0001);
	mainsensor_write_cmos_sensor(0x3238,0x000B);
	mainsensor_write_cmos_sensor(0x323A,0x0016);
	mainsensor_write_cmos_sensor(0x323C,0x000D);
	mainsensor_write_cmos_sensor(0x323E,0x001C);
	mainsensor_write_cmos_sensor(0x3240,0x000D);
	mainsensor_write_cmos_sensor(0x3242,0x0054);
	mainsensor_write_cmos_sensor(0x3244,0x007B);
	mainsensor_write_cmos_sensor(0x3246,0x00CC);
	mainsensor_write_cmos_sensor(0x3248,0x015D);
	mainsensor_write_cmos_sensor(0x324A,0x007E);
	mainsensor_write_cmos_sensor(0x324C,0x0095);
	mainsensor_write_cmos_sensor(0x324E,0x0085);
	mainsensor_write_cmos_sensor(0x3250,0x009D);
	mainsensor_write_cmos_sensor(0x3252,0x008D);
	mainsensor_write_cmos_sensor(0x3254,0x009D);
	mainsensor_write_cmos_sensor(0x3256,0x007E);
	mainsensor_write_cmos_sensor(0x3258,0x0080);
	mainsensor_write_cmos_sensor(0x325A,0x0001);
	mainsensor_write_cmos_sensor(0x325C,0x0005);
	mainsensor_write_cmos_sensor(0x325E,0x0085);
	mainsensor_write_cmos_sensor(0x3260,0x009D);
	mainsensor_write_cmos_sensor(0x3262,0x0001);
	mainsensor_write_cmos_sensor(0x3264,0x0005);
	mainsensor_write_cmos_sensor(0x3266,0x007E);
	mainsensor_write_cmos_sensor(0x3268,0x0080);
	mainsensor_write_cmos_sensor(0x326A,0x0053);
	mainsensor_write_cmos_sensor(0x326C,0x007D);
	mainsensor_write_cmos_sensor(0x326E,0x00CB);
	mainsensor_write_cmos_sensor(0x3270,0x015E);
	mainsensor_write_cmos_sensor(0x3272,0x0001);
	mainsensor_write_cmos_sensor(0x3274,0x0005);
	mainsensor_write_cmos_sensor(0x3276,0x0009);
	mainsensor_write_cmos_sensor(0x3278,0x000C);
	mainsensor_write_cmos_sensor(0x327A,0x007E);
	mainsensor_write_cmos_sensor(0x327C,0x0098);
	mainsensor_write_cmos_sensor(0x327E,0x0009);
	mainsensor_write_cmos_sensor(0x3280,0x000C);
	mainsensor_write_cmos_sensor(0x3282,0x007E);
	mainsensor_write_cmos_sensor(0x3284,0x0080);
	mainsensor_write_cmos_sensor(0x3286,0x0044);
	mainsensor_write_cmos_sensor(0x3288,0x0163);
	mainsensor_write_cmos_sensor(0x328A,0x0045);
	mainsensor_write_cmos_sensor(0x328C,0x0047);
	mainsensor_write_cmos_sensor(0x328E,0x007D);
	mainsensor_write_cmos_sensor(0x3290,0x0080);
	mainsensor_write_cmos_sensor(0x3292,0x015F);
	mainsensor_write_cmos_sensor(0x3294,0x0162);
	mainsensor_write_cmos_sensor(0x3296,0x007D);
	mainsensor_write_cmos_sensor(0x3298,0x0000);
	mainsensor_write_cmos_sensor(0x329A,0x0000);
	mainsensor_write_cmos_sensor(0x329C,0x0000);
	mainsensor_write_cmos_sensor(0x329E,0x0000);
	mainsensor_write_cmos_sensor(0x32A0,0x0008);
	mainsensor_write_cmos_sensor(0x32A2,0x0010);
	mainsensor_write_cmos_sensor(0x32A4,0x0018);
	mainsensor_write_cmos_sensor(0x32A6,0x0020);
	mainsensor_write_cmos_sensor(0x32A8,0x0000);
	mainsensor_write_cmos_sensor(0x32AA,0x0008);
	mainsensor_write_cmos_sensor(0x32AC,0x0010);
	mainsensor_write_cmos_sensor(0x32AE,0x0018);
	mainsensor_write_cmos_sensor(0x32B0,0x0020);
	mainsensor_write_cmos_sensor(0x32B2,0x0020);
	mainsensor_write_cmos_sensor(0x32B4,0x0020);
	mainsensor_write_cmos_sensor(0x32B6,0x0020);
	mainsensor_write_cmos_sensor(0x32B8,0x0000);
	mainsensor_write_cmos_sensor(0x32BA,0x0000);
	mainsensor_write_cmos_sensor(0x32BC,0x0000);
	mainsensor_write_cmos_sensor(0x32BE,0x0000);
	mainsensor_write_cmos_sensor(0x32C0,0x0000);
	mainsensor_write_cmos_sensor(0x32C2,0x0000);
	mainsensor_write_cmos_sensor(0x32C4,0x0000);
	mainsensor_write_cmos_sensor(0x32C6,0x0000);
	mainsensor_write_cmos_sensor(0x32C8,0x0000);
	mainsensor_write_cmos_sensor(0x32CA,0x0000);
	mainsensor_write_cmos_sensor(0x32CC,0x0000);
	mainsensor_write_cmos_sensor(0x32CE,0x0000);
	mainsensor_write_cmos_sensor(0x32D0,0x0000);
	mainsensor_write_cmos_sensor(0x32D2,0x0000);
	mainsensor_write_cmos_sensor(0x32D4,0x0000);
	mainsensor_write_cmos_sensor(0x32D6,0x0000);
	mainsensor_write_cmos_sensor(0x32D8,0x0000);
	mainsensor_write_cmos_sensor(0x32DA,0x0000);
	mainsensor_write_cmos_sensor(0x32DC,0x0000);
	mainsensor_write_cmos_sensor(0x32DE,0x0000);
	mainsensor_write_cmos_sensor(0x32E0,0x0000);
	mainsensor_write_cmos_sensor(0x32E2,0x0000);
	mainsensor_write_cmos_sensor(0x32E4,0x0000);
	mainsensor_write_cmos_sensor(0x32E6,0x0000);
	mainsensor_write_cmos_sensor(0x32E8,0x0000);
	mainsensor_write_cmos_sensor(0x32EA,0x0000);
	mainsensor_write_cmos_sensor(0x32EC,0x0000);
	mainsensor_write_cmos_sensor(0x32EE,0x0000);
	mainsensor_write_cmos_sensor(0x32F0,0x0000);
	mainsensor_write_cmos_sensor(0x32F2,0x0000);
	mainsensor_write_cmos_sensor(0x32F4,0x000A);
	mainsensor_write_cmos_sensor(0x32F6,0x0002);
	mainsensor_write_cmos_sensor(0x32F8,0x0008);
	mainsensor_write_cmos_sensor(0x32FA,0x0010);
	mainsensor_write_cmos_sensor(0x32FC,0x0020);
	mainsensor_write_cmos_sensor(0x32FE,0x0028);
	mainsensor_write_cmos_sensor(0x3300,0x0038);
	mainsensor_write_cmos_sensor(0x3302,0x0040);
	mainsensor_write_cmos_sensor(0x3304,0x0050);
	mainsensor_write_cmos_sensor(0x3306,0x0058);
	mainsensor_write_cmos_sensor(0x3308,0x0068);
	mainsensor_write_cmos_sensor(0x330A,0x0070);
	mainsensor_write_cmos_sensor(0x330C,0x0080);
	mainsensor_write_cmos_sensor(0x330E,0x0088);
	mainsensor_write_cmos_sensor(0x3310,0x0098);
	mainsensor_write_cmos_sensor(0x3312,0x00A0);
	mainsensor_write_cmos_sensor(0x3314,0x00B0);
	mainsensor_write_cmos_sensor(0x3316,0x00B8);
	mainsensor_write_cmos_sensor(0x3318,0x00C8);
	mainsensor_write_cmos_sensor(0x331A,0x00D0);
	mainsensor_write_cmos_sensor(0x331C,0x00E0);
	mainsensor_write_cmos_sensor(0x331E,0x00E8);
	mainsensor_write_cmos_sensor(0x3320,0x0017);
	mainsensor_write_cmos_sensor(0x3322,0x002F);
	mainsensor_write_cmos_sensor(0x3324,0x0047);
	mainsensor_write_cmos_sensor(0x3326,0x005F);
	mainsensor_write_cmos_sensor(0x3328,0x0077);
	mainsensor_write_cmos_sensor(0x332A,0x008F);
	mainsensor_write_cmos_sensor(0x332C,0x00A7);
	mainsensor_write_cmos_sensor(0x332E,0x00BF);
	mainsensor_write_cmos_sensor(0x3330,0x00D7);
	mainsensor_write_cmos_sensor(0x3332,0x00EF);
	mainsensor_write_cmos_sensor(0x3352,0x00A5);
	mainsensor_write_cmos_sensor(0x3354,0x00AF);
	mainsensor_write_cmos_sensor(0x3356,0x0187);
	mainsensor_write_cmos_sensor(0x3358,0x0000);
	mainsensor_write_cmos_sensor(0x335A,0x009E);
	mainsensor_write_cmos_sensor(0x335C,0x016B);
	mainsensor_write_cmos_sensor(0x335E,0x0015);
	mainsensor_write_cmos_sensor(0x3360,0x00A5);
	mainsensor_write_cmos_sensor(0x3362,0x00AF);
	mainsensor_write_cmos_sensor(0x3364,0x01FB);
	mainsensor_write_cmos_sensor(0x3366,0x0000);
	mainsensor_write_cmos_sensor(0x3368,0x009E);
	mainsensor_write_cmos_sensor(0x336A,0x016B);
	mainsensor_write_cmos_sensor(0x336C,0x0015);
	mainsensor_write_cmos_sensor(0x336E,0x00A5);
	mainsensor_write_cmos_sensor(0x3370,0x00A6);
	mainsensor_write_cmos_sensor(0x3372,0x0187);
	mainsensor_write_cmos_sensor(0x3374,0x0000);
	mainsensor_write_cmos_sensor(0x3376,0x009E);
	mainsensor_write_cmos_sensor(0x3378,0x016B);
	mainsensor_write_cmos_sensor(0x337A,0x0015);
	mainsensor_write_cmos_sensor(0x337C,0x00A5);
	mainsensor_write_cmos_sensor(0x337E,0x00A6);
	mainsensor_write_cmos_sensor(0x3380,0x01FB);
	mainsensor_write_cmos_sensor(0x3382,0x0000);
	mainsensor_write_cmos_sensor(0x3384,0x009E);
	mainsensor_write_cmos_sensor(0x3386,0x016B);
	mainsensor_write_cmos_sensor(0x3388,0x0015);
	mainsensor_write_cmos_sensor(0x319A,0x0005);
	mainsensor_write_cmos_sensor(0x1006,0x0005);
	mainsensor_write_cmos_sensor(0x3416,0x0001);
	mainsensor_write_cmos_sensor(0x308C,0x0008);
	mainsensor_write_cmos_sensor(0x307C,0x0240);
	mainsensor_write_cmos_sensor(0x375E,0x0050);
	mainsensor_write_cmos_sensor(0x31CE,0x0101);
	mainsensor_write_cmos_sensor(0x374E,0x0007);
	mainsensor_write_cmos_sensor(0x3460,0x0001);
	mainsensor_write_cmos_sensor(0x3052,0x0002);
	mainsensor_write_cmos_sensor(0x3058,0x0001);
	mainsensor_write_cmos_sensor(0x6028,0x2000);
	mainsensor_write_cmos_sensor(0x602A,0x108A);
	mainsensor_write_cmos_sensor(0x6F12,0x0359);
	mainsensor_write_cmos_sensor(0x6F12,0x0100);
	
	//#ifdefNONCONTINUEMODE
	mainsensor_write_cmos_sensor(0xB0A0,0x7C);//noncontinuemode
	//#else
	//mainsensor_write_cmos_sensor(0xB0A0,0x7D);//continuemode
	//#endif
	
	
	
	mainsensor_write_cmos_sensor(0x0100,0x0000);
	mainsensor_write_cmos_sensor(0x6028,0x2000);
	mainsensor_write_cmos_sensor(0x602A,0x0F74);
	mainsensor_write_cmos_sensor(0x6F12,0x0040);
	mainsensor_write_cmos_sensor(0x6F12,0x0040);
	mainsensor_write_cmos_sensor(0x6028,0x4000);
	//mainsensor_write_cmos_sensor(0x30c0,0x0300);//openmastermode
	mainsensor_write_cmos_sensor(0x0344,0x0008);
	mainsensor_write_cmos_sensor(0x0346,0x0008);
	mainsensor_write_cmos_sensor(0x0348,0x1077);
	mainsensor_write_cmos_sensor(0x034A,0x0C37);
	mainsensor_write_cmos_sensor(0x034C,0x0840);
	mainsensor_write_cmos_sensor(0x034E,0x0620);
	mainsensor_write_cmos_sensor(0x0900,0x0112);
	mainsensor_write_cmos_sensor(0x0380,0x0001);
	mainsensor_write_cmos_sensor(0x0382,0x0001);
	mainsensor_write_cmos_sensor(0x0384,0x0001);
	mainsensor_write_cmos_sensor(0x0386,0x0003);
	mainsensor_write_cmos_sensor(0x0400,0x0001);
	mainsensor_write_cmos_sensor(0x0404,0x0020);
	mainsensor_write_cmos_sensor(0x0114,0x0300);
	mainsensor_write_cmos_sensor(0x0110,0x0002);
	
	
	mainsensor_write_cmos_sensor(0x0136,0x1800);
	mainsensor_write_cmos_sensor(0x0304,0x0006);
	mainsensor_write_cmos_sensor(0x0306,0x007d);
	mainsensor_write_cmos_sensor(0x0302,0x0001);
	mainsensor_write_cmos_sensor(0x0300,0x0005);
	mainsensor_write_cmos_sensor(0x030C,0x0006);
	mainsensor_write_cmos_sensor(0x030E,0x0096);
	mainsensor_write_cmos_sensor(0x030A,0x0001);
	mainsensor_write_cmos_sensor(0x0308,0x0008);
	
	mainsensor_write_cmos_sensor(0x0342,0x1f40);
	mainsensor_write_cmos_sensor(0x0340,0x0A00);//648
	mainsensor_write_cmos_sensor(0x0202,0x0700);
	mainsensor_write_cmos_sensor(0x0200,0x00C6);
	mainsensor_write_cmos_sensor(0x0B04,0x0101);
	mainsensor_write_cmos_sensor(0x0B08,0x0000);
	mainsensor_write_cmos_sensor(0x0B00,0x0007);
	mainsensor_write_cmos_sensor(0x316A,0x0007);
	
	//mainsensor_write_cmos_sensor(0x6028,0x4000);
	//mainsensor_write_cmos_sensor(0x3098,0x0002);
	//mainsensor_write_cmos_sensor(0x3166,0x000a);
	//mainsensor_write_cmos_sensor(0x3168,0x0023);
	mainsensor_write_cmos_sensor(0x0100,0x0100);

 	XC9160DB("[xchip]Dual_MainSensorMIPIInitialSetting end \n");
 }

void Dual_SubSensorInitialSetting(void)
 {
 	XC9160DB("[xchip]Dual_SubSensorMIPIInitialSetting start \n");

 	XC9160DB("[xchip]Dual_SubSensorMIPIInitialSetting end \n");
 }
//==================================================================//
//=============================preview==============================//

void Dual_XC9160PreviewSetting(void)
{
	XC9160DB("[xchip]Dual_XC9160PreviewSetting start \n");

	XC9160DB("[xchip]Dual_XC9160PreviewSetting end \n");
} 

void Dual_MainSensorPreviewSetting(void)
{
    XC9160DB("[xchip]Dual_MainSensorPreviewSetting start\n ");

    XC9160DB("[xchip]Dual_MainSensorPreviewSetting end\n ");    
}  

void Dual_SubSensorPreviewSetting(void)
{
	XC9160DB("[xchip]Dual_SubSensorPreviewSetting start\n ");

    XC9160DB("[xchip]Dual_SubSensorPreviewSetting end\n "); 
}
//================================================================//
//=====================capture====================================//
void Dual_XC9160CaptureSetting(void)
{
	XC9160DB("[xchip]Dual_XC9160CaptureSetting start\n ");

    XC9160DB("[xchip]Dual_XC9160CaptureSetting end\n ");   
}

void Dual_MainSensorCaptureSetting(void)
{
	XC9160DB("[xchip]Dual_MainSensorCaptureSetting start\n ");

    XC9160DB("[xchip]Dual_MainSensorCaptureSetting end\n ");   
}

void Dual_SubSensorCaptureSetting(void)
{
	XC9160DB("[xchip]Dual_SubSensorCaptureSetting start\n ");

    XC9160DB("[xchip]Dual_SubSensorCaptureSetting end\n ");   
}

//==============================================================================//
//==============================================================================//
//==========================struct pfunc========================================//
//==============================================================================//
//==============================================================================//
typedef void (*pfunc)(void);

struct CAMERA_FUNCTION_STRUCT
{
	pfunc xc9160init;
	pfunc xc9160preview;
	pfunc xc9160capture;
	pfunc mainsensorinit;
	pfunc mainsensorpreview;
	pfunc mainsensorcapture;
	pfunc subsensorinit;
	pfunc subsensorpreview;
	pfunc subsensorcapture;
};

struct CAMERA_FUNCTION_STRUCT *CurrentMode;

struct CAMERA_FUNCTION_STRUCT SingleModeFunction=
	{	
		XC9160InitialSetting,
		XC9160PreviewSetting,
		XC9160CaptureSetting,
		MainSensorInitialSetting,
		MainSensorPreviewSetting,
		MainSensorCaptureSetting,
		SubSensorInitialSetting,
		SubSensorPreviewSetting,
		SubSensorCaptureSetting
	};

struct CAMERA_FUNCTION_STRUCT DualModeFunction=
	{	
		Dual_XC9160InitialSetting,
		Dual_XC9160PreviewSetting,
		Dual_XC9160CaptureSetting,
		Dual_MainSensorInitialSetting,
		Dual_MainSensorPreviewSetting,
		Dual_MainSensorCaptureSetting,
		Dual_SubSensorInitialSetting,
		Dual_SubSensorPreviewSetting,
		Dual_SubSensorCaptureSetting
	};