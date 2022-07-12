/*
 * gdisp_lld_config.h
 *
 *  Created on: Dec 30, 2015
 *      Author: varcain
 */

#ifndef INC_UGFX_GDISP_LLD_CONFIG_H_
#define INC_UGFX_GDISP_LLD_CONFIG_H_

#if GFX_USE_GDISP

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

#define GDISP_HARDWARE_DRAWPIXEL			TRUE
#define GDISP_HARDWARE_FILLS				TRUE

#define GDISP_LLD_PIXELFORMAT				GDISP_PIXELFORMAT_MONO

#define GDISP_SCREEN_WIDTH 320
#define GDISP_SCREEN_HEIGHT 240

#endif	/* GFX_USE_GDISP */

#endif /* INC_UGFX_GDISP_LLD_CONFIG_H_ */
