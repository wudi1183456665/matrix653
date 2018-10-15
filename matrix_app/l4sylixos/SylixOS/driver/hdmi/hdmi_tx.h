/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: hdmi_tx.h
**
** 创   建   人: zhang.xu (张旭)
**
** 文件创建日期: 2015 年 12 月 13 日
**
** 描        述: imx6q hdmi 设备驱动函数及相关宏定义
*********************************************************************************************************/
#ifndef __HDMI_COMMON_H__
#define __HDMI_COMMON_H__

#include "sdk.h"
/*********************************************************************************************************
  相关宏定义
*********************************************************************************************************/
/*
 * Enumeration of variable pixel format for HDMI transmitter
 */
enum hdmi_datamap {
    RGB444_8B = 0x01,
    RGB444_10B = 0x03,
    RGB444_12B = 0x05,
    RGB444_16B = 0x07,
    YCbCr444_8B = 0x09,
    YCbCr444_10B = 0x0B,
    YCbCr444_12B = 0x0D,
    YCbCr444_16B = 0x0F,
    YCbCr422_8B = 0x16,
    YCbCr422_10B = 0x14,
    YCbCr422_12B = 0x12,
};
/*
 * Enumeration of HDMI CSC encode format
 */
enum hdmi_csc_enc_format {
    eRGB = 0x0,
    eYCC444 = 0x01,
    eYCC422 = 0x2,
    eExtended = 0x3,
};
/*
 * HDMI colorimetry defined by ITU
 */
enum hdmi_colorimetry {
    eITU601,
    eITU709,
};
/*
 * HDMI video mode structure. this is used to set the timming of HDMI output must follow the VESA
 * standard or EDID information embedded in the display
 */
typedef struct hdmi_vmode {
    unsigned int mCode;
    unsigned int mHdmiDviSel;
    unsigned int mRVBlankInOSC;
    unsigned int mRefreshRate;
    unsigned int mHImageSize;
    unsigned int mVImageSize;
    unsigned int mHActive;
    unsigned int mVActive;
    unsigned int mHBlanking;
    unsigned int mVBlanking;
    unsigned int mHSyncOffset;
    unsigned int mVSyncOffset;
    unsigned int mHSyncPulseWidth;
    unsigned int mVSyncPulseWidth;
    unsigned int mHSyncPolarity;
    unsigned int mVSyncPolarity;
    unsigned int mDataEnablePolarity;
    unsigned int mInterlaced;
    unsigned int mPixelClock;
    unsigned int mHBorder;
    unsigned int mVBorder;
    unsigned int mPixelRepetitionInput;
} hdmi_vmode_s;

/*
 * HDMI input and output data information
 */
typedef struct hdmi_data_info {
    unsigned int enc_in_format;
    unsigned int enc_out_format;
    unsigned int enc_color_depth;
    unsigned int colorimetry;
    unsigned int pix_repet_factor;
    unsigned int hdcp_enable;
    hdmi_vmode_s *video_mode;
} hdmi_data_info_s;

/*
 * HDMI audio params
 */
typedef struct hdmi_AudioParam {
    unsigned char IecCgmsA;
    int IecCopyright;
    unsigned char IecCategoryCode;
    unsigned char IecPcmMode;
    unsigned char IecSourceNumber;
    unsigned char IecClockAccuracy;
    unsigned int OriginalSamplingFrequency;
    unsigned char ChannelAllocation;
    unsigned int SamplingFrequency;
    unsigned char SampleSize;
} hdmi_audioparam_s;

/*
 * Enumeration of HDMI audio data codec format
 */
typedef enum {
    PCM = 1,
    AC3,
    MPEG1,
    MP3,
    MPEG2,
    AAC,
    DTS,
    ATRAC,
    ONE_BIT_AUDIO,
    DOLBY_DIGITAL_PLUS,
    DTS_HD,
    MAT,
    DST,
    WMAPRO
} codingType_t;

/*
 * Enumeration of HDMI data packet type
 */
typedef enum {
    AUDIO_SAMPLE = 1,
    HBR_STREAM
} packet_t;

/*
 * HDMI DMA access type
 */
typedef enum {
    DMA_4_BEAT_INCREMENT = 0,
    DMA_8_BEAT_INCREMENT,
    DMA_16_BEAT_INCREMENT,
    DMA_UNUSED_BEAT_INCREMENT,
    DMA_UNSPECIFIED_INCREMENT
} dmaIncrement_t;

/*
 * HDMI audio params
 */
typedef struct {
    codingType_t   mCodingType;        /* (audioParams_t *params, see InfoFrame)                        */
    unsigned char  mChannelAllocation; /* channel allocation (audioParams_t *params,  see InfoFrame)    */
    unsigned char  mSampleSize;        /* sample size (audioParams_t *params, 16 to 24)                 */
    unsigned int   mSamplingFrequency; /* sampling frequency (audioParams_t *params, Hz)                */
    unsigned char  mLevelShiftValue;   /* level shift value (audioParams_t *params, see InfoFrame)      */
    unsigned char  mDownMixInhibitFlag;/* down-mix inhibit flag (audioParams_t *params, see InfoFrame)  */
    unsigned int   mOriginalSamplingFrequency; /* Original sampling frequency  */
    unsigned char  mIecCopyright;              /* IEC copyright                */
    unsigned char  mIecCgmsA;                  /* IEC CGMS-A                   */
    unsigned char  mIecPcmMode;                /* IEC PCM mode                 */
    unsigned char  mIecCategoryCode;           /* IEC category code            */
    unsigned char  mIecSourceNumber;           /* IEC source number            */
    unsigned char  mIecClockAccuracy;          /* IEC clock accuracy           */
    packet_t       mPacketType; /** packet type. currently only Audio Sample (AUDS) and High Bit Rate (HBR) are supported */
    unsigned short mClockFsFactor; /** Input audio clock Fs factor used at the audop 
						packetizer to calculate the CTS value and ACR packet insertion rate */
    dmaIncrement_t mDmaBeatIncrement; /** Incremental burst modes: unspecified
									lengths (upper limit is 1kB boundary) and INCR4, INCR8, and INCR16 fixed-beat burst */
    unsigned char  mDmaThreshold; /** When the number of samples in the Audio FIFO is lower
						than the threshold, the DMA engine requests a new burst
						request to the AHB master interface */
    unsigned char  mDmaHlock; /** Master burst lock mechanism */
} audioParams_t;

/*********************************************************************************************************
  hdmi 设备驱动函数
*********************************************************************************************************/
void hdmi_set_video_mode(hdmi_vmode_s * vmode);
int isColorSpaceConversion(hdmi_data_info_s hdmi_instance);
int isColorSpaceDecimation(hdmi_data_info_s hdmi_instance);
int isColorSpaceInterpolation(hdmi_data_info_s hdmi_instance);
int isPixelRepetition(hdmi_data_info_s hdmi_instance);
void hdmi_config_input_source(uint32_t mux_value);
void hdmi_video_sample(hdmi_data_info_s hdmi_instance);
void update_csc_coeffs(hdmi_data_info_s hdmi_instance);
void hdmi_video_csc(hdmi_data_info_s hdmi_instance);
void hdmi_video_packetize(hdmi_data_info_s hdmi_instance);
void preamble_filter_set(uint8_t value, uint8_t channel);
void hdmi_av_frame_composer(hdmi_data_info_s * hdmi_instance);
uint32_t hdmi_audio_mute(uint32_t en);
void hdmi_tx_hdcp_config(uint32_t de);
void hdmi_phy_init(uint8_t de, uint16_t pclk);
void hdmi_config_force_video(uint8_t force);
void hdmi_config_forced_pixels(uint8_t red, uint8_t green, uint8_t blue);
void hdmi_clock_set(int ipu_index, uint32_t pclk);

#endif
/*********************************************************************************************************
  END
*********************************************************************************************************/
