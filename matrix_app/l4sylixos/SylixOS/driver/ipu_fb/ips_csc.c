
/*!
 * @file ips_csc.c
 * @brief IPU color space conversion test.
 * @ingroup diag_ipu
 */
#define __SYLIXOS_KERNEL
#include "config.h"
#include <linux/compat.h>
#include "SylixOS.h"

#include <string.h>
#include <stdio.h>

#include "ipu_common.h"
/*
 * IPU 的色域转换，这里不使用  --sylixos
 */
int32_t ips_csc_test(ips_dev_panel_t * panel)
{
    return TRUE;
}
