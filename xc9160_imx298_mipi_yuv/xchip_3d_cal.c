/*
 * Copyright (C) 2017 XChip Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
//#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/types.h>
#include "linux/input.h"

#define XCHIP_3D_CAL_LENGTH     4096

static u8 xchip_cal_data[XCHIP_3D_CAL_LENGTH];
static u32 xchip_cal_data_len = 0;
static u32 is_registered = 0;
static ssize_t xchip_3d_cal_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	char info[32];

	if (copy_from_user(info, buf, 32)) {
		pr_err("%s: copy failed", __func__);
		return -1;
	}
	printk("XCHIP Cal: write \n");
	return len;
}

static ssize_t xchip_3d_cal_read(struct file *filep, char __user *ubuf,
				   size_t count, loff_t *ppos)
{
	printk("XCHIP Cal: read \n");
	return simple_read_from_buffer(ubuf, count, ppos, (void*)xchip_cal_data, 
	xchip_cal_data_len);
}


static const struct file_operations xchip_cal_fops = {
	.owner = THIS_MODULE,
	.write = xchip_3d_cal_write,
	.read = xchip_3d_cal_read,
};

static struct miscdevice xchip_3d_cal_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = "xchip_3d_cal",
	.fops  = &xchip_cal_fops,
};

static int xchip_3d_cal_register_fs(void)
{
	int32_t rc;

	printk("XCHIP Cal: register device node \n");
	rc = misc_register(&xchip_3d_cal_misc);
	if (rc < 0) {
		printk("XCHIP Cal: Error，misc_register returned %d", rc);
	}

	return rc;
}

void xchip_3d_cal_init(void* p_data, int len)
{
	int ret = 0;
	if (NULL != p_data && 0 != len && len < XCHIP_3D_CAL_LENGTH) {
		xchip_cal_data_len = len;
		memcpy((void*)xchip_cal_data,  p_data, len);
		if (0 == is_registered) {
			ret = xchip_3d_cal_register_fs();
			if (0 == ret) {
				is_registered = 1;
			} else {
				xchip_cal_data_len = 0;
				printk("XCHIP Cal: No 3D cal data \n");
			}
		} else {
			printk("XCHIP Cal: registered before, no need to re-register \n");
		}
	}
	return;
}




