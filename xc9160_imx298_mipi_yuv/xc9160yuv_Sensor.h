/*****************************************************************************
 *
 * Filename:
 * ---------
 *   sensor.h
 *
 * Project:
 * --------
 *   DUMA
 *
 * Description:
 * ------------
 *   Header file of Sensor driver
 *
 *
 * Author:
 * -------
 *   PC Huang (MTK02204)
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision:$
 * $Modtime:$
 * $Log:$
 *
 * 09 10 2010 jackie.su
 * [ALPS00002279] [Need Patch] [Volunteer Patch] ALPS.Wxx.xx Volunteer patch for
 * .10y dual sensor
 *
 * 09 02 2010 jackie.su
 * [ALPS00002279] [Need Patch] [Volunteer Patch] ALPS.Wxx.xx Volunteer patch for
 * .roll back dual sensor
 *
 * Mar 4 2010 mtk70508
 * [DUMA00154792] Sensor driver
 * 
 *
 * Feb 24 2010 mtk01118
 * [DUMA00025869] [Camera][YUV I/F & Query feature] check in camera code
 * 
 *
 * Aug 5 2009 mtk01051
 * [DUMA00009217] [Camera Driver] CCAP First Check In
 * 
 *
 * Apr 7 2009 mtk02204
 * [DUMA00004012] [Camera] Restructure and rename camera related custom folders and folder name of came
 * 
 *
 * Mar 26 2009 mtk02204
 * [DUMA00003515] [PC_Lint] Remove PC_Lint check warnings of camera related drivers.
 * 
 *
 * Mar 2 2009 mtk02204
 * [DUMA00001084] First Check in of MT6516 multimedia drivers
 * 
 *
 * Feb 24 2009 mtk02204
 * [DUMA00001084] First Check in of MT6516 multimedia drivers
 * 
 *
 * Dec 27 2008 MTK01813
 * DUMA_MBJ CheckIn Files
 * created by clearfsimport
 *
 * Dec 10 2008 mtk02204
 * [DUMA00001084] First Check in of MT6516 multimedia drivers
 * 
 *
 * Oct 27 2008 mtk01051
 * [DUMA00000851] Camera related drivers check in
 * Modify Copyright Header
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
/* SENSOR FULL SIZE */
#ifndef __SENSOR_H
#define __SENSOR_H


typedef enum {
    SENSOR_MODE_INIT = 0,
    SENSOR_MODE_PREVIEW,
    SENSOR_MODE_CAPTURE
} IMX298MIPI_SENSOR_MODE;

typedef enum _IMX298MIPI_OP_TYPE_ {
        IMX298MIPI_MODE_NONE,
        IMX298MIPI_MODE_PREVIEW,
        IMX298MIPI_MODE_CAPTURE,
        IMX298MIPI_MODE_QCIF_VIDEO,
        IMX298MIPI_MODE_CIF_VIDEO,
        IMX298MIPI_MODE_QVGA_VIDEO
    } IMX298MIPI_OP_TYPE;

extern IMX298MIPI_OP_TYPE IMX298MIPI_g_iIMX298MIPI_Mode;

#define IMX298MIPI_ID_REG                          (0x0016)
#define IMX298MIPI_INFO_REG                        (0x0017)
 
/* sensor size */
#define IMX298MIPI_IMAGE_SENSOR_SVGA_WIDTH          (1888)
#define IMX298MIPI_IMAGE_SENSOR_SVGA_HEIGHT         (1256)
#define IMX298MIPI_IMAGE_SENSOR_QSXGA_WITDH         (1888) 
#define IMX298MIPI_IMAGE_SENSOR_QSXGA_HEIGHT        (1256)
#define IMX298MIPI_IMAGE_SENSOR_VIDEO_WITDH         (1888) 
#define IMX298MIPI_IMAGE_SENSOR_VIDEO_HEIGHT        (1256)

#define IMX298MIPI_IMAGE_SENSOR_720P_WIDTH          (1280)
#define IMX298MIPI_IMAGE_SENSOR_720P_HEIGHT         (720)


/* Sesnor Pixel/Line Numbers in One Period */	
#define IMX298MIPI_PV_PERIOD_PIXEL_NUMS    		(1896)  	/* Default preview line length HTS*/
#define IMX298MIPI_PV_PERIOD_LINE_NUMS     		(984)   	/* Default preview frame length  VTS*/
#define IMX298MIPI_FULL_PERIOD_PIXEL_NUMS  		(2844)  	/* Default full size line length */
#define IMX298MIPI_FULL_PERIOD_LINE_NUMS   		(1968)  	/* Default full size frame length */

/* Sensor Exposure Line Limitation */
#define IMX298MIPI_PV_EXPOSURE_LIMITATION      	(984-4)
#define IMX298MIPI_FULL_EXPOSURE_LIMITATION    	(1968-4)

/* Config the ISP grab start x & start y, Config the ISP grab width & height */
#define IMX298MIPI_PV_GRAB_START_X 				   (0)
#define IMX298MIPI_PV_GRAB_START_Y  			(1)
#define IMX298MIPI_FULL_GRAB_START_X   			(0)
#define IMX298MIPI_FULL_GRAB_START_Y	  		(1)

/*50Hz,60Hz*/
#define IMX298MIPI_NUM_50HZ                        (50 * 2)
#define IMX298MIPI_NUM_60HZ                        (60 * 2)

/* FRAME RATE UNIT */
#define IMX298MIPI_FRAME_RATE_UNIT                 (10)

/* MAX CAMERA FRAME RATE */
#define IMX298MIPI_MAX_CAMERA_FPS                  (IMX298MIPI_FRAME_RATE_UNIT * 30)

#define IMX298_PREVIEW_MODE             0
#define IMX298_VIDEO_MODE               1
#define IMX298_PREVIEW_FULLSIZE_MODE    2


/* SENSOR READ/WRITE ID */
#define S5K3L8MIPI_WRITE_ID						    0x5a   //0x6c
#define S5K3L8MIPI_READ_ID							0x5b     //0x6d

#define OV5675MIPI_WRITE_ID						    0x6c  
#define OV5675MIPI_READ_ID							0x6d

#define OTP_WRITE_ID						0xA0  
#define OTP_READ_ID							0xA0      

/* XC9160 READ/WRITE ID */
#define XC9160_WRITE_ID						    0x36
#define XC9160_READ_ID							0x37

#define XC9160_ISP_ID							0x7160

static UINT32 UnusedOpen(void);
static UINT32 GetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution);
static UINT32 GetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_INFO_STRUCT *pSensorInfo, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
static UINT32 Control(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
static UINT32 FeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId, UINT8 *pFeaturePara,UINT32 *pFeatureParaLen);
static UINT32 Close(void);
//UINT32 XC9160IMX298MIPISensorInit(PSENSOR_FUNCTION_STRUCT pfFunc);
#endif /* __SENSOR_H */
