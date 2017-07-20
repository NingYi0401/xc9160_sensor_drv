
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
#include "xc9160yuv_Sensor.h"
#include "xc9160yuv_Camera_Sensor_para.h"
#include "xc9160yuv_CameraCustomized.h" 
#include "xc9160yuv_sensor_config.h"

unsigned int dual_mode_symbol;
#define CAMERA_MODE_DUAL 1

static MSDK_SCENARIO_ID_ENUM CurrentScenarioId = MSDK_SCENARIO_ID_CAMERA_PREVIEW;
MSDK_SENSOR_CONFIG_STRUCT IMX298MIPISensorConfigData;

int read_sensor_mode(MUINT8* dual_mode)
{
	XC9160DB("XCHIP:dual_mode = %d ", *dual_mode);
	dual_mode_symbol = *dual_mode;

	if(dual_mode_symbol == CAMERA_MODE_DUAL){
		xc9160_capture_width  = 6000;
		xc9160_capture_height = 3120;
		xc9160_preview_width  = 2080;
		xc9160_preview_height = 1536;
        CurrentMode = &DualModeFunction;
		}
	else{
		xc9160_capture_width  = 4160;
		xc9160_capture_height = 3120;
		xc9160_preview_width  = 2080;
		xc9160_preview_height = 1536;
        CurrentMode = &SingleModeFunction; 
	}
    return 0;
}

kal_uint32 XC9160_GetSensorID(kal_uint32 *sensorID)
{

    kal_uint32 isp_id[4]={0};

    XC9160DB("XC9160_GetSensorID-4: 150618\n ");
	
	isp_id[0]=XC9160_read_cmos_sensor(0xfffb);
    XC9160DB("xc9160 isp_id[0]=%x  should eq 0x71\n",isp_id[0]);
  
    XC9160_write_cmos_sensor(0xfffd ,0x80);
    XC9160_write_cmos_sensor(0xfffe,0x50);
    mDELAY(10);
  
	*sensorID=XC9160IMX298_SENSOR_ID;

	XC9160_write_cmos_sensor(0xfffe,0x50);
    XC9160_write_cmos_sensor(0x004d,0x0); // 
	
	XC9160DB("[xchip]exit XC9160_GetSensorID \n ");
   return ERROR_NONE;    

}   

UINT32 UnusedOpen(void)
{
	XC9160DB("xchip UnusedOpen\n ");
	return ERROR_NONE;
}

UINT32 Open(void)
{
    kal_uint32 isp_id[4]={0};  
		
	XC9160DB("xchip open start\n ");

    XC9160_write_cmos_sensor(0xfffd,0x80);
    XC9160_write_cmos_sensor(0xfffe,0x50);
    mDELAY(10);

	isp_id[0]=XC9160_read_cmos_sensor(0xfffb);
    XC9160DB("xchip xc9160 open isp_id[0]=%x  should eq 0x71\n",isp_id[0]);

        CurrentMode->xc9160init();//Dual_XC9160InitialSetting(); 
        XC9160_write_cmos_sensor(0xfffe,0x50);
        XC9160_write_cmos_sensor(0x0050,0x0f);  //  PWDN,GPIO1,GPIO0 en   by Nathan 20150719
        XC9160_write_cmos_sensor(0x0054,0x0f);  //  PWDN,GPIO1,ouput 控制sensorPWDN&reset ; GPIO0 被MTK6572来控制XC9160 PWDN
        XC9160_write_cmos_sensor(0x0058,0x00); //   将GPIO1（sensor reset）先拉低
        mDELAY(10);
        XC9160_write_cmos_sensor(0x0058,0x0a); //   将GPIO1（sensor reset）先拉低     
        
        XC9160_write_cmos_sensor(0xfffd ,0x80);
        XC9160_write_cmos_sensor(0xfffe,0x50);
        XC9160_write_cmos_sensor(0x004d ,0x01);

        CurrentMode->mainsensorinit();//Dual_MainSensorInitialSetting();

        mDELAY(10);

        XC9160_write_cmos_sensor(0xfffd,0x80);
        XC9160_write_cmos_sensor(0xfffe,0x50);
        XC9160_write_cmos_sensor(0x004d ,0x02);

        CurrentMode->subsensorinit();//Dual_SubSensorInitialSetting();

        XC9160_write_cmos_sensor(0xfffe,0x50);
        XC9160_write_cmos_sensor(0xfffe,0x50);
        XC9160_write_cmos_sensor(0x004d ,0x00); 
     
        XC9160DB("[xchip]exit IMX298MIPIOpen\n ");
    
        return ERROR_NONE;
}  

UINT32 Preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    XC9160DB("[xchip]enter Preview function:\n ");

        XC9160_write_cmos_sensor(0xfffd ,0x80);
        XC9160_write_cmos_sensor(0xfffe,0x50);
        XC9160_write_cmos_sensor(0x004d ,0x01);

        CurrentMode->mainsensorpreview();//Dual_MainSensorPreviewSetting();

        XC9160_write_cmos_sensor(0xfffd ,0x80);
        XC9160_write_cmos_sensor(0xfffe,0x50);
        XC9160_write_cmos_sensor(0x004d ,0x02);

        CurrentMode->subsensorpreview();//Dual_SubSensorPreviewSetting();
    
        XC9160_write_cmos_sensor(0xfffd ,0x80);
        XC9160_write_cmos_sensor(0xfffe,0x50);
        XC9160_write_cmos_sensor(0x004d ,0x00);

        CurrentMode->xc9160preview();//Dual_XC9160PreviewSetting();
    
        XC9160_write_cmos_sensor(0xfffe,0x50);
   
        XC9160DB("[xchip]exit Preview function:\n ");
        return ERROR_NONE ;
}

UINT32 Capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    XC9160DB("[xchip]Capture\n ");


        image_window->GrabStartX = IMX298MIPI_FULL_GRAB_START_X;
        image_window->GrabStartY = IMX298MIPI_FULL_GRAB_START_Y;
        image_window->ExposureWindowWidth= xc9160_capture_width;
        image_window->ExposureWindowHeight = xc9160_capture_height;
    
        XC9160_write_cmos_sensor(0xfffd ,0x80);
        XC9160_write_cmos_sensor(0xfffe,0x50);
        XC9160_write_cmos_sensor(0x004d ,0x01);
    
        CurrentMode->mainsensorcapture();

        XC9160_write_cmos_sensor(0xfffd ,0x80);
        XC9160_write_cmos_sensor(0xfffe,0x50);
        XC9160_write_cmos_sensor(0x004d ,0x02);

        CurrentMode->subsensorcapture();
        
        XC9160_write_cmos_sensor(0xfffd ,0x80);
        XC9160_write_cmos_sensor(0xfffe,0x50);
        XC9160_write_cmos_sensor(0x004d ,0x02);

        CurrentMode->xc9160capture();
          
        XC9160_write_cmos_sensor(0xfffe,0x50);

        XC9160DB("[xchip]exit Capture\n ");
        return ERROR_NONE;
}

UINT32 Close(void)
{
    XC9160DB("[xchip] Close\n ");
    return ERROR_NONE;
} 

UINT32 GetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
    pSensorResolution->SensorPreviewWidth= (xc9160_preview_width - 48);   //xc9160_capture_width-2*IMX298MIPI_PV_GRAB_START_X;
    pSensorResolution->SensorPreviewHeight= (xc9160_preview_height - 36);   //xc9160_capture_height-2*IMX298MIPI_PV_GRAB_START_Y;
    pSensorResolution->SensorFullWidth= (xc9160_capture_width - 48);    //xc9160_capture_width-2*IMX298MIPI_FULL_GRAB_START_X; 
    pSensorResolution->SensorFullHeight= (xc9160_capture_height - 36);   //xc9160_capture_height-2*IMX298MIPI_FULL_GRAB_START_Y;
    pSensorResolution->SensorVideoWidth= (xc9160_capture_width - 48);   //xc9160_capture_width-2*IMX298MIPI_PV_GRAB_START_X; 
    pSensorResolution->SensorVideoHeight= (xc9160_capture_height - 36);   //xc9160_capture_height-2*IMX298MIPI_PV_GRAB_START_Y;;
    XC9160DB("[xchip]exit GetResolution function:\n ");
    return ERROR_NONE;
}

UINT32 GetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,MSDK_SENSOR_INFO_STRUCT *pSensorInfo,MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
    XC9160DB("[xchip]GetInfo-619: ScenarioId=%d %d, %d\n ", ScenarioId, xc9160_capture_width,xc9160_capture_height);
   
    pSensorInfo->SensorPreviewResolutionX=(xc9160_preview_width - 32);//IMX298MIPI_IMAGE_SENSOR_SVGA_WIDTH-2*IMX298MIPI_PV_GRAB_START_X; ;
    pSensorInfo->SensorPreviewResolutionY=(xc9160_preview_height - 24);//IMX298MIPI_IMAGE_SENSOR_SVGA_HEIGHT-2*IMX298MIPI_PV_GRAB_START_Y;
    pSensorInfo->SensorCameraPreviewFrameRate=30;           
    pSensorInfo->SensorFullResolutionX= (xc9160_capture_width - 32);//xc9160_capture_width-2*IMX298MIPI_FULL_GRAB_START_X;
    pSensorInfo->SensorFullResolutionY= (xc9160_capture_height  - 24);//xc9160_capture_height-2*IMX298MIPI_FULL_GRAB_START_Y;
    pSensorInfo->SensorCameraPreviewFrameRate=30;
    pSensorInfo->SensorVideoFrameRate=30;
    pSensorInfo->SensorStillCaptureFrameRate=5;
    pSensorInfo->SensorWebCamCaptureFrameRate=15;
    pSensorInfo->SensorResetActiveHigh=FALSE;
    pSensorInfo->SensorResetDelayCount=4;
    pSensorInfo->SensorOutputDataFormat=SENSOR_OUTPUT_FORMAT_YUYV;
	
    pSensorInfo->SensorClockPolarity=SENSOR_CLOCK_POLARITY_LOW; 
    pSensorInfo->SensorClockFallingPolarity=SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;  
	
    pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	
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

	pSensorInfo->MIPIsensorType = MIPI_OPHY_NCSI2;      
	
	pSensorInfo->SettleDelayMode = MIPI_SETTLEDELAY_AUTO;
	//pSensorInfo->SettleDelayMode = MIPI_SETTLEDELAY_MANUAL;  

	pSensorInfo->HighSpeedVideoDelayFrame = 4;    
	
    switch (ScenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		
            pSensorInfo->SensorClockFreq=24;
			
            pSensorInfo->SensorClockDividCount= 5;
            pSensorInfo->SensorClockRisingCount= 0;
            pSensorInfo->SensorClockFallingCount= 2;
            pSensorInfo->SensorPixelClockCount= 3;
            pSensorInfo->SensorDataLatchCount= 2;
			
            pSensorInfo->SensorGrabStartX = IMX298MIPI_PV_GRAB_START_X; 
            pSensorInfo->SensorGrabStartY = IMX298MIPI_PV_GRAB_START_Y;   
            pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_2_LANE; //SENSOR_MIPI_1_LANE;    

			
            pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0;

			//pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 4; 
            pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 14; // 130;  // 13;  // 17; // 4; // 14;    // 4; 
            
            pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;				
			
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
    XC9160DB("[xchip]exit GetInfo function:\n ");  
    return ERROR_NONE;
} 

UINT32 Control(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
      XC9160DB("[xchip]enter Control function:\n ");
     // spin_lock(&IMX298mipi_drv_lock);
      CurrentScenarioId = ScenarioId;
      //spin_unlock(&IMX298mipi_drv_lock);
      switch (ScenarioId)
      {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        	 Preview(pImageWindow, pSensorConfigData);
             break;
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        	
             Capture(pImageWindow, pSensorConfigData);
             break;
        default:
            return ERROR_INVALID_SCENARIO_ID;
    }
    XC9160DB("[xchip]exit Control function:\n ");
    return ERROR_NONE;
} 

UINT32 XC9160YUVSensorSetting(FEATURE_ID iCmd, UINT32 iPara)
{

    XC9160DB("[xchip]enter XC9160YUVSensorSetting function:\n ");
    switch (iCmd) {
        case FID_SCENE_MODE:
            break;      
        case FID_AWB_MODE:
              break;
        case FID_COLOR_EFFECT:              
              break;
        case FID_AE_EV:   
            break;
        case FID_AE_FLICKER:                    
              break;
        case FID_AE_SCENE_MODE: 
                if (iPara == AE_MODE_OFF) 
                {
                    spin_lock(&IMX298mipi_drv_lock);
                    spin_unlock(&IMX298mipi_drv_lock);
        }
        else 
        {
                    spin_lock(&IMX298mipi_drv_lock);
                    spin_unlock(&IMX298mipi_drv_lock);
            }
        break; 
        case FID_ISP_CONTRAST:
            break;
        case FID_ISP_BRIGHT:
            break;
        case FID_ISP_SAT:
        break; 
    case FID_ZOOM_FACTOR:
            XC9160DB("FID_ZOOM_FACTOR:%d\n", iPara);      
                    spin_lock(&IMX298mipi_drv_lock);
                    spin_unlock(&IMX298mipi_drv_lock);
            break; 
        case FID_AE_ISO:
            break;        
      default:
                  break;
    }
    XC9160DB("[xchip]exit XC9160YUVSensorSetting function:\n ");

      return TRUE;
}

UINT32 XC9160GetDefaultFramerateByScenario(
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


UINT32 FeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
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
    XC9160DB("[xchip][FeatureControl]feature id=%d \n",FeatureId);
    switch (FeatureId)
    {
        case SENSOR_FEATURE_GET_RESOLUTION:
            *pFeatureReturnPara16++=xc9160_capture_width;
            *pFeatureReturnPara16=xc9160_capture_height;
            *pFeatureParaLen=4;
            break;
		case SENSOR_FEATURE_SET_DUAL_MODE:
			read_sensor_mode(pFeaturePara);
			open();
			break;	
        case SENSOR_FEATURE_GET_PERIOD:


			*pFeatureReturnPara16++=xc9160_preview_width;
			*pFeatureReturnPara16=xc9160_preview_height;
			*pFeatureParaLen=4;

						
		
            break;

        case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:

			*pFeatureReturnPara32 = 48000000;	 //unit: Hz
			*pFeatureParaLen=4;
	
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
				XC9160DB("SENSOR_FEATURE_SET_REGISTER(XC91601): addr:%x  value:%x\n",pSensorRegData->RegAddr,pSensorRegData->RegData);
				XC9160_write_cmos_sensor(0xfffd ,0x80);
				XC9160_write_cmos_sensor(0xfffe,0x50);
				XC9160_write_cmos_sensor(0x004d ,0x01);
				mainsensor_write_cmos_sensor(pSensorRegData->RegAddr&0xffff, pSensorRegData->RegData);
				XC9160_write_cmos_sensor(0x004d ,0x00);
            }
			else
			{
				XC9160DB("SENSOR_FEATURE_SET_REGISTER(XC9160): addr:%x  value:%x\n",pSensorRegData->RegAddr,pSensorRegData->RegData);
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
				XC9160DB("SENSOR_FEATURE_GET_REGISTER(XC91601): addr:0x%x  value:0x%x\n",pSensorRegData->RegAddr,pSensorRegData->RegData);
            }
			else
			{	
				pSensorRegData->RegData = XC9160_read_cmos_sensor(pSensorRegData->RegAddr&0xffff);
				XC9160DB("SENSOR_FEATURE_GET_REGISTER(XC9160): addr:0x%x  value:0x%x\n",pSensorRegData->RegAddr,pSensorRegData->RegData);
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
            //IMX298SetTestPatternMode((BOOL)*pFeatureData16);            
            break;
        case SENSOR_FEATURE_CHECK_SENSOR_ID:
            XC9160_GetSensorID(pFeatureData32);
            break;
        case SENSOR_FEATURE_SET_YUV_CMD:
            XC9160YUVSensorSetting((FEATURE_ID)*feature_data, *(feature_data+1));
            break;

		case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
			XC9160GetDefaultFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*feature_data, (MUINT32 *)(uintptr_t)(*(feature_data+1)));
			break;

        default:
            XC9160DB("FeatureControl:default \n");
            break;          
    }
    XC9160DB("[xchip]exit FeatureControl function:\n ");
    return ERROR_NONE;
}   /* IMX298MIPIFeatureControl() */

SENSOR_FUNCTION_STRUCT  SensorFuncIMX298MIPI=
{
    UnusedOpen,
    GetInfo,
    GetResolution,
    FeatureControl,
    Control,
    Close
};

UINT32 XC9160IMX298MIPISensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
    if (pfFunc!=NULL)
        *pfFunc=&SensorFuncIMX298MIPI;
    return ERROR_NONE;
}
	

