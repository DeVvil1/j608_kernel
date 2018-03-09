/*
 * Module author : Stain Colton
*/

#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
#endif

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH                                         (720)
#define FRAME_HEIGHT                                        (1280)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

static unsigned int lcm_esd_test = FALSE;

#define LCM_DSI_CMD_MODE	0

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n)                                           (lcm_util.mdelay(n))

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)

  
// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS * util)
{
  memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS * params)
{
    memset(params, 0, sizeof(LCM_PARAMS));
	
    params->type   								= 2;  
    params->width  							= FRAME_WIDTH;
    params->height 							= FRAME_HEIGHT;

    params->dsi.mode   = BURST_VDO_MODE;	

    // DSI
    /* Command mode setting */
    params->dsi.LANE_NUM						= LCM_FOUR_LANE;
    //The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.color_order 			= LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq   			= LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding     			= LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format      			= LCM_DSI_FORMAT_RGB888;

   // Highly depends on LCD driver capability.
   // Not support in MT6573
    params->dsi.packet_size=256;
  
   //video mode timing
    params->dsi.intermediat_buffer_num = 0;	
    params->dsi.PS								= LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.vertical_sync_active				= 2;
    params->dsi.vertical_backporch				= 14;
    params->dsi.vertical_frontporch				= 16;
    params->dsi.vertical_active_line				= FRAME_HEIGHT;

    params->dsi.horizontal_sync_active			= 2;
    params->dsi.horizontal_backporch			= 42;
    params->dsi.horizontal_frontporch			= 44;
    params->dsi.horizontal_active_pixel			= FRAME_WIDTH;
  
    //improve clk quality
    params->dsi.PLL_CLOCK = 198; //this value must be in MTK suggested table
    params->dsi.ssc_disable = 1;
	
}
	
static void init_lcm_registers(void)
{
  unsigned int data_array[16];
  
  data_array[0] = 0x00043902;
  data_array[1] = 0x9483FFB9;
  dsi_set_cmdq(data_array, 2, 1);
  MDELAY(5);

  data_array[0] = 0x00033902;
  data_array[1] = 0x008373BA;
  dsi_set_cmdq(data_array, 2, 1);
  MDELAY(1);

  data_array[0] = 0x00103902;
  data_array[1] = 0x12126AB1;
  data_array[2] = 0xF111E424;
  data_array[3] = 0x239DE480;
  data_array[4] = 0x58D2C080;
  dsi_set_cmdq(data_array, 5, 1);
  MDELAY(1);

  data_array[0] = 0x000C3902;
  data_array[1] = 0x106400B2;
  data_array[2] = 0x081C1207;
  data_array[3] = 0x004D1C08;
  dsi_set_cmdq(data_array, 4, 1);
  MDELAY(1);

  data_array[0] = 0x000D3902;
  data_array[1] = 0x03FF00B4;
  data_array[2] = 0x035A035A;
  data_array[3] = 0x016A015A;
  data_array[4] = 0x0000006A;
  dsi_set_cmdq(data_array, 5, 1);
  MDELAY(1);

  data_array[1] = 0x000055D2;
  data_array[0] = 0x00023902;
  dsi_set_cmdq(data_array, 2, 1);
  MDELAY(1);

  data_array[0] = 0x00043902;
  data_array[1] = 0x010E41BF;
  dsi_set_cmdq(data_array, 2, 1);
  MDELAY(1);

  data_array[0] = 0x00263902;
  data_array[1] = 0x000600D3;
  data_array[2] = 0x00081A40;
  data_array[3] = 0x00071032;
  data_array[4] = 0x0F155407;
  data_array[5] = 0x12020405;
  data_array[6] = 0x33070510;
  data_array[7] = 0x370B0B33;
  data_array[8] = 0x08070710;
  data_array[10] = 0x00000100;
  data_array[9] = 0x0A000000;
  dsi_set_cmdq(data_array, 11, 1);
  MDELAY(1);

  data_array[2] = 0x1B1A1A18;
  data_array[4] = 0x02010007;
  data_array[6] = 0x18181818;
  data_array[0] = 0x002D3902;
  data_array[12] = 0x00000018;
  data_array[1] = 0x181919D5;
  data_array[3] = 0x0605041B;
  data_array[5] = 0x18212003;
  data_array[7] = 0x18181818;
  data_array[8] = 0x22181818;
  data_array[10] = 0x18181818;
  data_array[9] = 0x18181823;
  data_array[11] = 0x18181818;
  dsi_set_cmdq(data_array, 13, 1);
  MDELAY(1);

  data_array[2] = 0x1B1A1A19;
  data_array[4] = 0x05060700;
  data_array[6] = 0x18181818;
  data_array[0] = 0x002D3902;
  data_array[12] = 0x00000018;
  data_array[1] = 0x191818D6;
  data_array[3] = 0x0102031B;
  data_array[5] = 0x18222304;
  data_array[7] = 0x18181818;
  data_array[8] = 0x21181818;
  data_array[10] = 0x18181818;
  data_array[9] = 0x18181820;
  data_array[11] = 0x18181818;
  dsi_set_cmdq(data_array, 13, 1);
  MDELAY(1);

  data_array[0] = 0x002C3902;
  data_array[1] = 0x171204E0;
  data_array[2] = 0x213E3330;
  data_array[3] = 0x0C0A073C;
  data_array[4] = 0x13110D17;
  data_array[5] = 0x12081311;
  data_array[6] = 0x12041A16;
  data_array[7] = 0x3E333017;
  data_array[8] = 0x0A073C21;
  data_array[9] = 0x110D170C;
  data_array[10] = 0x08131113;
  data_array[11] = 0x001A1612;
  dsi_set_cmdq(data_array, 12, 1);
  MDELAY(1);

  data_array[0] = 0x00033902;
  data_array[1] = 0x006565B6;
  dsi_set_cmdq(data_array, 2, 1);
  MDELAY(1);

  data_array[1] = 0x000009CC;
  data_array[0] = 0x00023902;
  dsi_set_cmdq(data_array, 2, 1);
  MDELAY(1);

  data_array[0] = 0x00053902;
  data_array[1] = 0x40C000C7;
  data_array[2] = 0x000000C0;
  dsi_set_cmdq(data_array, 3, 1);
  MDELAY(1);

  data_array[1] = 0x000087DF;
  data_array[0] = 0x00023902;
  dsi_set_cmdq(data_array, 2, 1);
  MDELAY(1);

  data_array[0] = 0x00110500;
  dsi_set_cmdq(data_array, 1, 1);
  MDELAY(150);

  data_array[0] = 0x00290500;
  dsi_set_cmdq(data_array, 1, 1);
  MDELAY(20);
 } 	

static void lcm_init(void)
{
  SET_RESET_PIN(1);
  MDELAY(5);
  SET_RESET_PIN(0);
  MDELAY(10);
  SET_RESET_PIN(1);
  MDELAY(180);
  
  init_lcm_registers();
}

static void lcm_suspend(void)
{
  unsigned int data_array[16];

  data_array[0] = 0x00280500;
  dsi_set_cmdq(data_array, 1, 1);
  MDELAY(10);
  data_array[0] = 0x00100500;
  dsi_set_cmdq(data_array, 1, 1);
  MDELAY(120);
  
  SET_RESET_PIN(1);
  MDELAY(10);
  SET_RESET_PIN(0);
  MDELAY(10);
  SET_RESET_PIN(1);
  MDELAY(120);
}

static void lcm_resume(void)
{
  lcm_init();
}

static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);

}

static unsigned int lcm_compare_id(void)
{
  return 1;
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------

LCM_DRIVER hx8394d_hd720_dsi_vdo_p34_lcm_drv =
{
	.name           = "hx8394d_hd720_dsi_vdo_p34",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
#if (LCM_DSI_CMD_MODE)
        .update.        = lcm_update,
#endif
};
   
