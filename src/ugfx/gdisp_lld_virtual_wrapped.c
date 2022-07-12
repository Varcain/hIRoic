/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GDISP

#define GDISP_DRIVER_VMT			GDISPVMT_VIRTUAL_WRAPPED
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

#include "serial_wrapper.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local routines.                                                    */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

LLDSPEC bool_t gdisp_lld_init(GDisplay* g)
{
	g->board = 0;
	g->priv = 0;

	/* Initialise the GDISP structure to match */
	g->g.Orientation = GDISP_ROTATE_0;
	g->g.Powermode = powerOn;
	g->g.Width = GDISP_SCREEN_WIDTH;
	g->g.Height = GDISP_SCREEN_HEIGHT;

	return (TRUE);
}

LLDSPEC void gdisp_lld_draw_pixel(GDisplay* g)
{
	unsigned char data[5];
	uint8_t c;

	c = (gdispColor2Native(g->p.color) == 0 ? 0 : 1);
	data[0] = (g->p.x & 0xFF);
	data[1] = ((g->p.x >> 8) & 0xFF);
	data[2] = (g->p.y & 0xFF);
	data[3] = ((g->p.y >> 8) & 0xFF);
	data[4] = c;
	serial_comm(E_ID_SET_PIXEL, data, sizeof(data));
}

LLDSPEC	void gdisp_lld_fill_area(GDisplay *g)
{
	unsigned char data[9];
	uint8_t c;

	c = (gdispColor2Native(g->p.color) == 0 ? 0 : 1);
	data[0] = (g->p.x & 0xFF);
	data[1] = ((g->p.x >> 8) & 0xFF);
	data[2] = (g->p.y & 0xFF);
	data[3] = ((g->p.y >> 8) & 0xFF);
	data[4] = (g->p.cx & 0xFF);
	data[5] = ((g->p.cx >> 8) & 0xFF);
	data[6] = (g->p.cy & 0xFF);
	data[7] = ((g->p.cy >> 8) & 0xFF);
	data[8] = c;
	serial_comm(E_ID_FILL_AREA, data, sizeof(data));
}

#endif /* GFX_USE_GDISP */
