/* linux/drivers/video/samsung/s3cfb_mdnie.c
 *
 * Register interface file for Samsung mDNIe driver
 *
 * Copyright (c) 2009 Samsung Electronics
 * http://www.samsungsemi.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/


#include "s3cfb.h"
#include "s3cfb_ielcd.h"
#include "s3cfb_mdnie.h"
#include "mdnie.h"


#define s3c_mdnie_readl(addr)			__raw_readl((s3c_mdnie_base + addr))
#define s3c_mdnie_writel(addr, val)		__raw_writel(val, (s3c_mdnie_base + addr))
#define s3c_mdnie_write(addr, val)		__raw_writel(val, (s3c_mdnie_base + addr*4))


static struct resource *s3c_mdnie_mem;
static void __iomem *s3c_mdnie_base;


int mdnie_write(unsigned int addr, unsigned int val)
{
	s3c_mdnie_write(addr, val);

	return 0;
}

int s3c_mdnie_mask(void)
{
	s3c_mdnie_writel(S3C_MDNIE_rR40, 0x7FFF);

	return 0;
}

int s3c_mdnie_unmask(void)
{
	s3c_mdnie_writel(S3C_MDNIE_rR40, 0x0);

	return 0;
}

int s3c_mdnie_set_size(unsigned int hsize, unsigned int vsize)
{
	unsigned int size;

	size = s3c_mdnie_readl(S3C_MDNIE_rR34);
	size &= ~S3C_MDNIE_SIZE_MASK;
	size |= hsize;
	s3c_mdnie_writel(S3C_MDNIE_rR34, size);

	s3c_mdnie_unmask();

	size = s3c_mdnie_readl(S3C_MDNIE_rR35);
	size &= ~S3C_MDNIE_SIZE_MASK;
	size |= vsize;
	s3c_mdnie_writel(S3C_MDNIE_rR35, size);

	s3c_mdnie_unmask();

	return 0;
}

int s3c_mdnie_init_global(struct s3cfb_global *s3cfb_ctrl)
{
	s3c_mdnie_set_size(s3cfb_ctrl->lcd->width, s3cfb_ctrl->lcd->height);
	s3c_ielcd_logic_start();
	s3c_ielcd_init_global(s3cfb_ctrl);

	return 0;
}

void mDNIe_Init_Set_Mode(void)
{
	if (!IS_ERR_OR_NULL(g_mdnie))
		set_mdnie_value(g_mdnie);
}

int s3c_mdnie_start(struct s3cfb_global *ctrl)
{
	s3c_ielcd_start();

	if (!IS_ERR_OR_NULL(g_mdnie))
		g_mdnie->enable = TRUE;

	return 0;
}

int s3c_mdnie_off(void)
{
	if (!IS_ERR_OR_NULL(g_mdnie))
		g_mdnie->enable = FALSE;

	s3c_ielcd_logic_stop();

	return 0;
}

int s3c_mdnie_stop(void)
{
	if (!IS_ERR_OR_NULL(g_mdnie))
		g_mdnie->enable = FALSE;

	return s3c_ielcd_stop();
}

int s3c_mdnie_hw_init(void)
{
	printk(KERN_INFO "%s\n", __func__);

	s3c_mdnie_mem = request_mem_region(S3C_MDNIE_PHY_BASE, S3C_MDNIE_MAP_SIZE, "mdnie");
	if (s3c_mdnie_mem == NULL) {
		printk(KERN_ERR "mDNIe failed to reserved memory region\n");
		return -ENOENT;
	}

	s3c_mdnie_base = ioremap(S3C_MDNIE_PHY_BASE, S3C_MDNIE_MAP_SIZE);
	if (s3c_mdnie_base == NULL) {
		printk(KERN_ERR "mDNIe failed ioremap\n");
		return -ENOENT;
	}

	printk(KERN_INFO "%s : 0x%p\n", __func__, s3c_mdnie_base);

	return 0;
}

int s3c_mdnie_setup(void)
{
	s3c_mdnie_hw_init();
	s3c_ielcd_hw_init();

	return 0;
}

MODULE_DESCRIPTION("mDNIe Device Driver");
MODULE_LICENSE("GPL");
