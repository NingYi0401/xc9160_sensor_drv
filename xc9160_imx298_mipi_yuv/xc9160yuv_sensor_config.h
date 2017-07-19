#undef IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH
#undef IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT
#define IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH       4208 //1920 //4608    //640
#define IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT      3120 //1080     //480
#undef IMAGE_SENSOR_PV_WIDTH
#undef IMAGE_SENSOR_PV_WIDTH
#define IMAGE_SENSOR_PV_WIDTH					  2080
#define IMAGE_SENSOR_PV_HEIGHT					  1536

#define IMX298MIPIYUV_DEBUG
#ifdef IMX298MIPIYUV_DEBUG
#define IMX298MIPISENSORDB(a,arg... ) printk("[XC9160]" a,##arg)
#define TV_SWITCH_PRINT(a,arg... ) printk("[TV_SWITCH/XC9160]" a,##arg)
#else
#define IMX298MIPISENSORDB(x,...)
#define TV_SWITCH_PRINT(x,...)
#endif

static DEFINE_SPINLOCK(IMX298mipi_drv_lock);
static MSDK_SCENARIO_ID_ENUM CurrentScenarioId = MSDK_SCENARIO_ID_CAMERA_PREVIEW;
extern int iReadReg(u16 a_u2Addr , u8 * a_puBuff , u16 i2cId);
extern int iWriteReg(u16 a_u2Addr , u32 a_u4Data , u32 a_u4Bytes , u16 i2cId);

extern int iReadRegI2C(u8 *a_pSendData, u16 a_sizeSendData, u8 *a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData, u16 a_sizeSendData, u16 i2cId);

#define IMX298MIPI_write_cmos_sensor_8(addr, para) iWriteReg((u16) addr , (u32) para ,1,IMX298MIPI_WRITE_ID)
#define IMX298MIPI_write_cmos_sensor(addr, para) iWriteReg((u16) addr , (u32) para ,2,IMX298MIPI_WRITE_ID)
#define OV5675MIPI_write_cmos_sensor(addr, para) iWriteReg((u16) addr , (u32) para ,1,OV5675MIPI_WRITE_ID)

static void XC9160InitialSetting(void);
extern	int Af_VCOMA2_Power_On(void);

static void XC9160_write_cmos_sensor(kal_uint16 addr, kal_uint32 para)
{		
	iWriteReg((u16) addr , (u32) para ,1,XC9160_WRITE_ID); 
}

static kal_uint16 XC9160_read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte=0;
	
    iReadReg((u16) addr ,(u8*)&get_byte,XC9160_WRITE_ID);

    return get_byte;
}

kal_uint16 IMX298MIPIYUV_read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte=0;

	

    iReadReg((u16) addr ,(u8*)&get_byte,IMX298MIPI_WRITE_ID);
	
	//IMX298MIPISENSORDB("IMX298MIPIYUV_read_cmos_sensor addr=%x, get_byte=%x\n", addr, get_byte);	 // by zhangxueping
	
    return get_byte;
}

#define mDELAY(ms)  mdelay(ms)

 static void XC9160InitialSetting(void)
 {
 	IMX298MIPISENSORDB("[mccree]XC9160_ISP_InitialSetting start \n");


 	mDELAY(10);

	IMX298MIPISENSORDB("XC9160_ISP_InitialSetting end\n");
 }

void MainSensorMIPIInitialSetting(void)
 {
 	IMX298MIPISENSORDB("[mccree]MainSensorMIPIInitialSetting start \n");

 	IMX298MIPISENSORDB("[mccree]MainSensorMIPIInitialSetting end \n");
 }

void OV5675MIPIInitialSetting(void)
 {
 	IMX298MIPISENSORDB("[mccree]OV5675MIPIInitialSetting start \n");

 	IMX298MIPISENSORDB("[mccree]OV5675MIPIInitialSetting end \n");
 }


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
