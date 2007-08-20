/*
 * Copyright © 2007 Daniel Stone
 * Copyright © 2007 Matthew Garrett
 * Copyright © 2007 Jerome Glisse
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * A copy of the General Public License is included with the source
 * distribution of this driver, as COPYING.
 *
 * Authors: Daniel Stone <daniel@fooishbar.org>
 *          Matthew Garrett <mjg59@srcf.ucam.org>
 *          Jerome Glisse <glisse@freedesktop.org>
 */
/*
 * avivo output handling functions. 
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
/* DPMS */
#define DPMS_SERVER
#include <X11/extensions/dpms.h>
#include <unistd.h>

#include "avivo.h"
#include "radeon_reg.h"

static void
avivo_output_dac1_setup(xf86OutputPtr output)
{
    struct avivo_output_private *avivo_output = output->driver_private;
    struct avivo_info *avivo = avivo_get_info(output->scrn);

    if (output->crtc) {
        struct avivo_crtc_private *avivo_crtc = output->crtc->driver_private;

        xf86DrvMsg(output->scrn->scrnIndex, X_INFO,
                   "(DAC1) %s connector associated to crtc(%d)\n",
                   xf86ConnectorGetName(avivo_output->type),
                   avivo_crtc->crtc_number);
        OUTREG(AVIVO_DAC1_CRTC_SOURCE, avivo_crtc->crtc_number);
    }
    OUTREG(AVIVO_DAC1_MYSTERY1, 0);
    OUTREG(AVIVO_DAC1_MYSTERY2, 0);
    OUTREG(AVIVO_DAC1_CNTL, AVIVO_DAC_EN);
}

static void
avivo_output_dac2_setup(xf86OutputPtr output)
{
    struct avivo_output_private *avivo_output = output->driver_private;
    struct avivo_info *avivo = avivo_get_info(output->scrn);

    if (output->crtc) {
        struct avivo_crtc_private *avivo_crtc = output->crtc->driver_private;

        xf86DrvMsg(output->scrn->scrnIndex, X_INFO,
                   "(DAC2) %s connector associated to crtc(%d)\n",
                   xf86ConnectorGetName(avivo_output->type),
                   avivo_crtc->crtc_number);
        OUTREG(AVIVO_DAC2_CRTC_SOURCE, avivo_crtc->crtc_number);
    }
    OUTREG(AVIVO_DAC2_MYSTERY1, 0);
    OUTREG(AVIVO_DAC2_MYSTERY2, 0);
    OUTREG(AVIVO_DAC2_CNTL, AVIVO_DAC_EN);
}

static void
avivo_output_tmds1_setup(xf86OutputPtr output)
{
    struct avivo_output_private *avivo_output = output->driver_private;
    struct avivo_info *avivo = avivo_get_info(output->scrn);
    unsigned int tmp;

    if (output->crtc) {
        struct avivo_crtc_private *avivo_crtc = output->crtc->driver_private;

        xf86DrvMsg(output->scrn->scrnIndex, X_INFO,
                   "(TMDS1) %s connector associated to crtc(%d)\n",
                   xf86ConnectorGetName(avivo_output->type),
                   avivo_crtc->crtc_number);
        OUTREG(AVIVO_TMDS1_CRTC_SOURCE, avivo_crtc->crtc_number);
    }
    tmp = INREG(AVIVO_TMDS1_MYSTERY3);
    tmp = (tmp & ~0x3) | 1;
    OUTREG(AVIVO_TMDS1_MYSTERY3, tmp);
    OUTREG(AVIVO_TMDS1_CLOCK_CNTL, 0x1F);
    OUTREG(AVIVO_TMDS1_CNTL, 0x1001);
    OUTREG(0x78D0, 0x1);
    OUTREG(AVIVO_TMDS1_MYSTERY3, tmp);
    OUTREG(AVIVO_TMDS1_MYSTERY3, tmp | 0x3);
    OUTREG(AVIVO_TMDS1_MYSTERY3, tmp);
    OUTREG(AVIVO_TMDS1_MYSTERY2, 0x1);
    OUTREG(AVIVO_TMDS1_MYSTERY2, 0x101);
    OUTREG(AVIVO_TMDS1_MYSTERY2, 0x1);
}

static void
avivo_output_tmds2_setup(xf86OutputPtr output)
{
    struct avivo_output_private *avivo_output = output->driver_private;
    struct avivo_info *avivo = avivo_get_info(output->scrn);
    unsigned int tmp;

    xf86DrvMsg(output->scrn->scrnIndex, X_INFO, "SETUP TMDS2\n");
    if (output->crtc) {
        struct avivo_crtc_private *avivo_crtc = output->crtc->driver_private;

        xf86DrvMsg(output->scrn->scrnIndex, X_INFO,
                   "(TMDS2) %s connector associated to crtc(%d)\n",
                   xf86ConnectorGetName(avivo_output->type),
                   avivo_crtc->crtc_number);
        OUTREG(AVIVO_TMDS2_CRTC_SOURCE, avivo_crtc->crtc_number);
    }
    tmp = INREG(AVIVO_TMDS1_MYSTERY3);
    tmp = (tmp & ~0x3) | 1;
    OUTREG(AVIVO_TMDS1_MYSTERY3, tmp);
    OUTREG(AVIVO_TMDS1_CLOCK_CNTL, 0x1E1F);
    OUTREG(AVIVO_TMDS1_CNTL, 0x1001);
    OUTREG(0x78D0, 0x1);
    OUTREG(AVIVO_TMDS1_MYSTERY3, tmp);
    OUTREG(AVIVO_TMDS1_MYSTERY3, tmp | 0x3);
    OUTREG(AVIVO_TMDS1_MYSTERY3, tmp);
    OUTREG(AVIVO_TMDS1_MYSTERY2, 0x1);
    OUTREG(AVIVO_TMDS1_MYSTERY2, 0x101);
    OUTREG(AVIVO_TMDS1_MYSTERY2, 0x1);
}

static void
avivo_output_dac1_dpms(xf86OutputPtr output, int mode)
{
    struct avivo_info *avivo = avivo_get_info(output->scrn);

    switch(mode) {
    case DPMSModeOn:
        OUTREG(AVIVO_DAC1_CNTL, AVIVO_DAC_EN);
        break;
    case DPMSModeStandby:
    case DPMSModeSuspend:
    case DPMSModeOff:
        OUTREG(AVIVO_DAC1_CNTL, 0);
        break;
    }
}

static void
avivo_output_dac2_dpms(xf86OutputPtr output, int mode)
{
    struct avivo_info *avivo = avivo_get_info(output->scrn);

    switch(mode) {
    case DPMSModeOn:
        OUTREG(AVIVO_DAC2_CNTL, AVIVO_DAC_EN);
        break;
    case DPMSModeStandby:
    case DPMSModeSuspend:
    case DPMSModeOff:
        OUTREG(AVIVO_DAC2_CNTL, 0);
        break;
    }
}

static void
avivo_output_tmds1_dpms(xf86OutputPtr output, int mode)
{
    struct avivo_info *avivo = avivo_get_info(output->scrn);

    switch(mode) {
    case DPMSModeOn:
        OUTREG(AVIVO_TMDS1_CLOCK_ENABLE, 1);
        OUTREG(AVIVO_TMDS1_CLOCK_CNTL, 0x1F);
        break;
    case DPMSModeStandby:
    case DPMSModeSuspend:
    case DPMSModeOff:
        OUTREG(AVIVO_TMDS1_CLOCK_CNTL, 0);
        OUTREG(AVIVO_TMDS1_CLOCK_ENABLE, 0);
        break;
    }
}

static void
avivo_output_tmds2_dpms(xf86OutputPtr output, int mode)
{
    struct avivo_info *avivo = avivo_get_info(output->scrn);

    switch(mode) {
    case DPMSModeOn:
        OUTREG(AVIVO_TMDS2_CLOCK_ENABLE, 1);
        OUTREG(AVIVO_TMDS2_CLOCK_CNTL, 0x1F);
        break;
    case DPMSModeStandby:
    case DPMSModeSuspend:
    case DPMSModeOff:
        OUTREG(AVIVO_TMDS2_CLOCK_CNTL, 0);
        OUTREG(AVIVO_TMDS2_CLOCK_ENABLE, 0);
        break;
    }
}

static void
avivo_output_lvds_dpms(xf86OutputPtr output, int mode)
{
    struct avivo_info *avivo = avivo_get_info(output->scrn);
    int tmp;

    switch(mode) {
    case DPMSModeOn:
        xf86DrvMsg(output->scrn->scrnIndex, X_INFO, "ENABLE TMDS2\n");
        OUTREG(AVIVO_LVDS_CNTL, AVIVO_LVDS_EN | AVIVO_LVDS_MYSTERY);
        OUTREG(AVIVO_TMDS2_CLOCK_CNTL, 0x1E1E);
        break;
    case DPMSModeStandby:
    case DPMSModeSuspend:
    case DPMSModeOff:
        xf86DrvMsg(output->scrn->scrnIndex, X_INFO, "DISABLE TMDS2\n");
        OUTREG(AVIVO_LVDS_CNTL, AVIVO_LVDS_MYSTERY);
        do {
            tmp = INREG(0x7AF4);
            usleep(100);
        } while (tmp != 0x800);
        OUTREG(AVIVO_TMDS2_CLOCK_CNTL, 0);
        OUTREG(AVIVO_TMDS2_CLOCK_ENABLE, 0);
        break;
    }
}

static void
avivo_output_dpms(xf86OutputPtr output, int mode)
{
    struct avivo_output_private *avivo_output = output->driver_private;
    struct avivo_info *avivo = avivo_get_info(output->scrn);
    int tmp, count;

    /* try to grab card lock or at least somethings that looks like a lock
     * if it fails more than 5 times with 1000ms wait btw each try than we
     * assume we can process.
     */
    count = 0;
    tmp = INREG(0x0028);
    while((tmp & 0x100) && (count < 5)) {
        tmp = INREG(0x0028);
        count++;
        usleep(1000);
    }
    if (count >= 5) {
        xf86DrvMsg(output->scrn->scrnIndex, X_INFO,
                   "%s (WARNING) failed to grab card lock process anyway.\n",
                   __func__);
    }
    OUTREG(0x0028, tmp | 0x100);

    if (avivo_output->dpms)
        avivo_output->dpms(output, mode);

    /* release card lock */
    tmp = INREG(0x0028);
    OUTREG(0x0028, tmp & (~0x100));
}

static int
avivo_output_mode_valid(xf86OutputPtr output, DisplayModePtr pMode)
{
    if (pMode->Flags & V_DBLSCAN)
        return MODE_NO_DBLESCAN;

    if (pMode->Clock > 400000 || pMode->Clock < 25000)
        return MODE_CLOCK_RANGE;

    return MODE_OK;
}

static Bool
avivo_output_mode_fixup(xf86OutputPtr output,
                        DisplayModePtr mode,
                        DisplayModePtr adjusted_mode)
{
    return TRUE;
}

static void
avivo_output_prepare(xf86OutputPtr output)
{
    output->funcs->dpms(output, DPMSModeOff);
}

static void
avivo_output_mode_set(xf86OutputPtr output,
                      DisplayModePtr mode,
                      DisplayModePtr adjusted_mode)
{
}

static void
avivo_output_commit(xf86OutputPtr output)
{
    struct avivo_output_private *avivo_output = output->driver_private;

    if (avivo_output->setup)
        avivo_output->setup(output);
    output->funcs->dpms(output, DPMSModeOn);
}

static Bool
avivo_output_detect_ddc(xf86OutputPtr output)
{
    struct avivo_output_private *avivo_output = output->driver_private;

    return xf86I2CProbeAddress(avivo_output->i2c, 0x00A0);
}

static xf86OutputStatus
avivo_output_detect(xf86OutputPtr output)
{
    if (avivo_output_detect_ddc(output))
        return XF86OutputStatusConnected;
    return XF86OutputStatusUnknown;
}

DisplayModePtr
avivo_output_get_modes(xf86OutputPtr output)
{
    struct avivo_output_private *avivo_output = output->driver_private;
    xf86MonPtr edid_mon;
    DisplayModePtr modes;

    edid_mon = xf86OutputGetEDID(output, avivo_output->i2c);
    xf86OutputSetEDID(output, edid_mon);
    modes = xf86OutputGetEDIDModes(output);
    return modes;
}

static void
avivo_output_destroy(xf86OutputPtr output)
{
    struct avivo_output_private *avivo_output = output->driver_private;

    if (avivo_output == NULL)
        return;
    xf86DestroyI2CBusRec(avivo_output->i2c, TRUE, TRUE);
    xfree(avivo_output->name);
    xfree(avivo_output);
}

static const xf86OutputFuncsRec avivo_output_funcs = {
    .dpms = avivo_output_dpms,
    .save = NULL,
    .restore = NULL,
    .mode_valid = avivo_output_mode_valid,
    .mode_fixup = avivo_output_mode_fixup,
    .prepare = avivo_output_prepare,
    .mode_set = avivo_output_mode_set,
    .commit = avivo_output_commit,
    .detect = avivo_output_detect,
    .get_modes = avivo_output_get_modes,
    .destroy = avivo_output_destroy
};

static const xf86OutputFuncsRec avivo_output_lfp_funcs = {
    .dpms = avivo_output_dpms,
    .save = NULL,
    .restore = NULL,
    .mode_valid = avivo_output_mode_valid,
    .mode_fixup = avivo_output_lfp_mode_fixup,
    .prepare = avivo_output_prepare,
    .mode_set = avivo_output_mode_set,
    .commit = avivo_output_commit,
    .detect = avivo_output_detect,
    .get_modes = avivo_output_lfp_get_modes,
    .destroy = avivo_output_destroy
};

Bool
avivo_output_exist(ScrnInfoPtr screen_info, xf86ConnectorType type,
                  int number, unsigned long ddc_reg)
{
    xf86CrtcConfigPtr config = XF86_CRTC_CONFIG_PTR(screen_info);
    int i;

    for (i = 0; i < config->num_output; i++) {
        xf86OutputPtr output = config->output[i];
        struct avivo_output_private *avivo_output = output->driver_private;
        if (avivo_output->number == number && avivo_output->type == type)
            return TRUE;
        /* TMDS2 is shared by LFP & DVI-I */
        if (avivo_output->type == XF86ConnectorLFP && number >= 1)
            return TRUE;
        if (type == XF86ConnectorLFP && avivo_output->number >= 1) {
            avivo_output->type = type;
            avivo_output->i2c->DriverPrivate.uval = ddc_reg;
            return TRUE;
        }
    }
    return FALSE;
}

Bool
avivo_output_init(ScrnInfoPtr screen_info, xf86ConnectorType type,
                  int number, unsigned long ddc_reg)
{
    xf86OutputPtr output;
    struct avivo_output_private *avivo_output;
    int name_size;

    /* allocate & initialize private output structure */
    avivo_output = xcalloc(sizeof(struct avivo_output_private), 1);
    if (avivo_output == NULL)
        return FALSE;
    name_size = snprintf(NULL, 0, "%s connector %d",
                         xf86ConnectorGetName(type), number);
    avivo_output->name = xcalloc(name_size + 1, 1);
    if (avivo_output->name == NULL) {
        xfree(avivo_output);
        xf86DrvMsg(screen_info->scrnIndex, X_ERROR,
                   "Failed to allocate memory for I2C bus name\n");
        return FALSE;
    }
    snprintf(avivo_output->name, name_size + 1, "%s connector %d",
             xf86ConnectorGetName(type), number);
    avivo_output->i2c = xf86CreateI2CBusRec();
    if (!avivo_output->i2c) {
        xfree(avivo_output);
        xf86DrvMsg(screen_info->scrnIndex, X_ERROR,
                   "Couldn't create I2C bus for %s connector %d\n",
                   xf86ConnectorGetName(type), number);
        return FALSE;
    }
    avivo_output->i2c->BusName = avivo_output->name;
    avivo_output->i2c->scrnIndex = screen_info->scrnIndex;
    if (ddc_reg == AVIVO_GPIO_0) {
        avivo_output->i2c->I2CPutBits = avivo_i2c_gpio0_put_bits;
        avivo_output->i2c->I2CGetBits = avivo_i2c_gpio0_get_bits;
    } else {
        avivo_output->i2c->I2CPutBits = avivo_i2c_gpio123_put_bits;
        avivo_output->i2c->I2CGetBits = avivo_i2c_gpio123_get_bits;
    }
    avivo_output->i2c->AcknTimeout = 5;
    avivo_output->i2c->DriverPrivate.uval = ddc_reg;
    if (!xf86I2CBusInit(avivo_output->i2c)) {
        xf86DrvMsg(screen_info->scrnIndex, X_ERROR,
                   "Couldn't initialise I2C bus for %s connector %d\n",
                   xf86ConnectorGetName(type), number);
        return FALSE;
    }
    avivo_output->gpio = ddc_reg;
    avivo_output->type = type;
    avivo_output->number = number;
    avivo_output->output_offset = 0;
    if (number >= 1) {
        switch (avivo_output->type) {
        case XF86ConnectorVGA:
            avivo_output->output_offset = AVIVO_DAC2_CNTL - AVIVO_DAC1_CNTL;
            break;
        case XF86ConnectorLFP:
        case XF86ConnectorDVI_I:
        case XF86ConnectorDVI_D:
        case XF86ConnectorDVI_A:
            avivo_output->output_offset = AVIVO_TMDS2_CNTL - AVIVO_TMDS1_CNTL;
            break;
        default:
            break;
        }
    }
    switch (avivo_output->type) {
    case XF86ConnectorVGA:
        if (!number) {
            avivo_output->setup = avivo_output_dac1_setup;
            avivo_output->dpms = avivo_output_dac1_dpms;
        } else {
            avivo_output->setup = avivo_output_dac2_setup;
            avivo_output->dpms = avivo_output_dac2_dpms;
        }
        break;
    case XF86ConnectorLFP:
        avivo_output->setup = avivo_output_tmds2_setup;
        avivo_output->dpms = avivo_output_lvds_dpms;
        break;
    case XF86ConnectorDVI_I:
    case XF86ConnectorDVI_D:
    case XF86ConnectorDVI_A:
        if (!number) {
            avivo_output->setup = avivo_output_tmds1_setup;
            avivo_output->dpms = avivo_output_tmds1_dpms;
        } else {
            avivo_output->setup = avivo_output_tmds2_setup;
            avivo_output->dpms = avivo_output_tmds2_dpms;
        }
        break;
    default:
        avivo_output->setup = NULL;
        break;
    }
    if (avivo_output->type == XF86ConnectorLFP) {
        avivo_output->output_offset = AVIVO_TMDS2_CNTL - AVIVO_TMDS1_CNTL;
        /* allocate & initialize xf86Output */
        output = xf86OutputCreate (screen_info,
                                   &avivo_output_lfp_funcs,
                                   xf86ConnectorGetName(type));
    } else {
        /* allocate & initialize xf86Output */
        output = xf86OutputCreate (screen_info,
                                   &avivo_output_funcs,
                                   xf86ConnectorGetName(type));
    }

    if (output == NULL) {
        xf86DestroyI2CBusRec(avivo_output->i2c, TRUE, TRUE);
        xfree(avivo_output);
        return FALSE;
    }
    output->driver_private = avivo_output;
    output->interlaceAllowed = FALSE;
    output->doubleScanAllowed = FALSE;
    xf86DrvMsg(screen_info->scrnIndex, X_INFO,
               "added %s connector %d (0x%04lX)\n",
               xf86ConnectorGetName(type), number, ddc_reg);

	return TRUE;
}
