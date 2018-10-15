/*!
 * @file buffers.h
 * @definitions for ALL buffer memory space regions used by sdk drivers
 */
#define IPU_DEFAULT_WORK_CLOCK      264000000
#define IPU_DMA_MEMORY_START	    0x40000000
#define IPU_DMA_MEMORY_END		    0x43FFFFFF

#define HDMI_AUDIO_BUF_START        0x4fff0000
#define HDMI_AUDIO_BUF_END          0x4fff4000

#define CH23_EBA0 	                (IPU_DMA_MEMORY_START + 0x00000000)
#define CH23_EBA1 	                (IPU_DMA_MEMORY_START + 0x00400000)
#define CH27_EBA0 	                (IPU_DMA_MEMORY_START + 0x00800000)
#define CH27_EBA1 	                (IPU_DMA_MEMORY_START + 0x00C00000)
#define CH28_EBA0 	                (IPU_DMA_MEMORY_START + 0x01000000)
#define CH28_EBA1 	                (IPU_DMA_MEMORY_START + 0x01400000)
#define CH0_EBA0 	                (IPU_DMA_MEMORY_START + 0x01800000)
#define CH0_EBA1 	                (IPU_DMA_MEMORY_START + 0x01C00000)
/*
 * for dual video playback
 */
#define IPU1_CH23_EBA0	            CH23_EBA0
#define IPU1_CH23_EBA1	            CH23_EBA1
#define IPU2_CH23_EBA0	            CH27_EBA0
#define IPU2_CH23_EBA1	            CH27_EBA1
/*
 * for video playback after resizing&rotation
 */
#define CH22_EBA0 	                (IPU_DMA_MEMORY_START + 0x01800000)
#define CH22_EBA1	                (IPU_DMA_MEMORY_START + 0x01C00000)
#define CH21_EBA0 	                (IPU_DMA_MEMORY_START + 0x02000000)
#define CH21_EBA1 	                (IPU_DMA_MEMORY_START + 0x02400000)
#define CH20_EBA0 	                (IPU_DMA_MEMORY_START + 0x02800000)
#define CH20_EBA1 	                (IPU_DMA_MEMORY_START + 0x02C00000)
/*
 * put the TWO video instance on different CS to
 * improve the performance.
 */
#define VPU_WORK_BUFFERS            (0x44100000)
#define VIDEO_BUFFERS_START         (0x48000000)
#define VIDEO_BUFFERS_END           (0x4FFFFFFF)
/*
 * OCRAM partition table
 */
#define VPU_SEC_AXI_START	        0x00910000
#define VPU_SEC_AXI_END		        0x0091FFFF
/*
 * OCRAM ADMA buffer
 */
#define USDHC_ADMA_BUFFER1          0x00907000
#define USDHC_ADMA_BUFFER2          0x00908000
#define USDHC_ADMA_BUFFER3          0x00909000
#define USDHC_ADMA_BUFFER4          0x0090A000

// USB buffers
#define QH_BUFFER                   0x00908000  // internal RAM
#define TD_BUFFER                   0x00908200  // internal RAM

#define SATA_PROTOCOL_BUFFER_BASE   0x0090a000
#define SATA_PROTOCOL_BUFFER_SIZE   0x1000
#define SATA_TRANSFER_BUFFER_BASE   0x0090c000
/*********************************************************************************************************
   END
*********************************************************************************************************/
