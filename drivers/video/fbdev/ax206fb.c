/*
 *  linux/drivers/video/ax206fb.c -- Virtual frame buffer device
 *
 *      Copyright (C) 2002 James Simmons
 *
 *	Copyright (C) 1997 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>
#include <linux/pid.h>
#include <linux/wait.h>

#include <linux/fb.h>
#include <linux/init.h>

/*
 *  RAM we reserve for the frame buffer. This defines the maximum screen
 *  size
 *
 *  The default can be overridden if the driver is compiled as a module
 */
#define VIDEOMEMSIZE	(2*1024*1024)	/* 2 MB */

static void *videomemory;
static u_long videomemorysize = VIDEOMEMSIZE;
module_param(videomemorysize, ulong, 0);
MODULE_PARM_DESC(videomemorysize, "RAM available to frame buffer (in bytes)");

static char *mode_option = NULL;
module_param(mode_option, charp, 0);
MODULE_PARM_DESC(mode_option, "Preferred video mode (e.g. 640x480-8@60)");

static const struct fb_videomode ax206fb_default = {
	.xres =		128,
	.yres =		128,
	.vmode =	FB_VMODE_NONINTERLACED,
};

static struct fb_fix_screeninfo ax206fb_fix = {
	.id =			"AX206 DPF Frame Buffer",
	.type =			FB_TYPE_PACKED_PIXELS,
	.visual =	FB_VISUAL_PSEUDOCOLOR,
	.xpanstep =	1,
	.ypanstep =	1,
	.ywrapstep =	1,
	.accel =	FB_ACCEL_NONE,
};

static bool ax206fb_enable __initdata = 1;	/* disabled by default */
module_param(ax206fb_enable, bool, 0);
MODULE_PARM_DESC(ax206fb_enable, "Enable Virtual FB driver");
static ssize_t ax206fb_read_sysfs(struct file *file, char __user *user, size_t count, loff_t *ppos);

static int ax206fb_check_var(struct fb_var_screeninfo *var,
			 struct fb_info *info);
static int ax206fb_set_par(struct fb_info *info);
static int ax206fb_setcolreg(u_int regno, u_int red, u_int green, u_int blue,
			 u_int transp, struct fb_info *info);
static int ax206fb_pan_display(struct fb_var_screeninfo *var,
			   struct fb_info *info);
static int ax206fb_mmap(struct fb_info *info,
		    struct vm_area_struct *vma);


struct dentry *ax206fb_debugfs_entry;

static const struct file_operations ax206fb_sysfs_fops = {
	.read = ax206fb_read_sysfs,
};


static DECLARE_WAIT_QUEUE_HEAD(wq);
static int wq_flag = 0;


static ssize_t ax206fb_read_sysfs(struct file *file, char __user *user, size_t count, loff_t *ppos)
{
	char buf[] = "|";

	wait_event_interruptible(wq, wq_flag != 0);
	wq_flag = 0;

	return simple_read_from_buffer(user, count, ppos, buf, 1);
}


static ssize_t ax206fb_write(struct fb_info *info, const char __user *buf,
				size_t count, loff_t *ppos)
{
	unsigned long p = *ppos;
	void *dst;
	int err = 0;
	unsigned long total_size;

	if (info->state != FBINFO_STATE_RUNNING)
		return -EPERM;

	total_size = info->fix.smem_len;

	if (p > total_size)
		return -EFBIG;

	if (count > total_size) {
		err = -EFBIG;
		count = total_size;
	}

	if (count + p > total_size) {
		if (!err)
			err = -ENOSPC;

		count = total_size - p;
	}

	dst = (void __force *) (info->screen_base + p);

	if (copy_from_user(dst, buf, count))
		err = -EFAULT;

	if  (!err)
		*ppos += count;

	/* wake userspace process, so fb can be written to display */
	wq_flag = 1;
	wake_up_interruptible(&wq);

	return (err) ? err : count;
}


static struct fb_ops ax206fb_ops = {
	.fb_read        = fb_sys_read,
	.fb_write       = ax206fb_write,
	.fb_check_var	= ax206fb_check_var,
	.fb_set_par		= ax206fb_set_par,
	.fb_setcolreg	= ax206fb_setcolreg,
	.fb_pan_display	= ax206fb_pan_display,
	.fb_fillrect	= sys_fillrect,
	.fb_copyarea	= sys_copyarea,
	.fb_imageblit	= sys_imageblit,
	.fb_mmap	= ax206fb_mmap,
};

    /*
     *  Internal routines
     */

static u_long get_line_length(int xres_virtual, int bpp)
{
	u_long length;

	length = xres_virtual * bpp;
	length = (length + 31) & ~31;
	length >>= 3;
	return (length);
}

/*
 *  Setting the video mode has been split into two parts.
 *  First part, xxxfb_check_var, must not write anything
 *  to hardware, it should only verify and adjust var.
 *  This means it doesn't alter par but it does use hardware
 *  data from it to check this var.
 */
static int ax206fb_check_var(struct fb_var_screeninfo *var,
			 struct fb_info *info)
{
	u_long line_length;

	/*
	 *  FB_VMODE_CONUPDATE and FB_VMODE_SMOOTH_XPAN are equal!
	 *  as FB_VMODE_SMOOTH_XPAN is only used internally
	 */

	if (var->vmode & FB_VMODE_CONUPDATE) {
		var->vmode |= FB_VMODE_YWRAP;
		var->xoffset = info->var.xoffset;
		var->yoffset = info->var.yoffset;
	}

	/*
	 *  Some very basic checks
	 */
	if (!var->xres)
		var->xres = 1;
	if (!var->yres)
		var->yres = 1;
	if (var->xres > var->xres_virtual)
		var->xres_virtual = var->xres;
	if (var->yres > var->yres_virtual)
		var->yres_virtual = var->yres;
	if (var->bits_per_pixel <= 1)
		var->bits_per_pixel = 1;
	else if (var->bits_per_pixel <= 8)
		var->bits_per_pixel = 8;
	else if (var->bits_per_pixel <= 16)
		var->bits_per_pixel = 16;
	else if (var->bits_per_pixel <= 24)
		var->bits_per_pixel = 24;
	else if (var->bits_per_pixel <= 32)
		var->bits_per_pixel = 32;
	else
		return -EINVAL;

	if (var->xres_virtual < var->xoffset + var->xres)
		var->xres_virtual = var->xoffset + var->xres;
	if (var->yres_virtual < var->yoffset + var->yres)
		var->yres_virtual = var->yoffset + var->yres;

	/*
	 *  Memory limit
	 */
	line_length =
	    get_line_length(var->xres_virtual, var->bits_per_pixel);
	if (line_length * var->yres_virtual > videomemorysize)
		return -ENOMEM;

	/*
	 * Now that we checked it we alter var. The reason being is that the video
	 * mode passed in might not work but slight changes to it might make it 
	 * work. This way we let the user know what is acceptable.
	 */
	switch (var->bits_per_pixel) {
	case 1:
	case 8:
		var->red.offset = 0;
		var->red.length = 8;
		var->green.offset = 0;
		var->green.length = 8;
		var->blue.offset = 0;
		var->blue.length = 8;
		var->transp.offset = 0;
		var->transp.length = 0;
		break;
	case 16:		/* RGBA 5551 */
		if (var->transp.length) {
			var->red.offset = 0;
			var->red.length = 5;
			var->green.offset = 5;
			var->green.length = 5;
			var->blue.offset = 10;
			var->blue.length = 5;
			var->transp.offset = 15;
			var->transp.length = 1;
		} else {	/* RGB 565 */
			var->red.offset = 0;
			var->red.length = 5;
			var->green.offset = 5;
			var->green.length = 6;
			var->blue.offset = 11;
			var->blue.length = 5;
			var->transp.offset = 0;
			var->transp.length = 0;
		}
		break;
	case 24:		/* RGB 888 */
		var->red.offset = 0;
		var->red.length = 8;
		var->green.offset = 8;
		var->green.length = 8;
		var->blue.offset = 16;
		var->blue.length = 8;
		var->transp.offset = 0;
		var->transp.length = 0;
		break;
	case 32:		/* RGBA 8888 */
		var->red.offset = 0;
		var->red.length = 8;
		var->green.offset = 8;
		var->green.length = 8;
		var->blue.offset = 16;
		var->blue.length = 8;
		var->transp.offset = 24;
		var->transp.length = 8;
		break;
	}
	var->red.msb_right = 0;
	var->green.msb_right = 0;
	var->blue.msb_right = 0;
	var->transp.msb_right = 0;

	return 0;
}

/* This routine actually sets the video mode. It's in here where we
 * the hardware state info->par and fix which can be affected by the 
 * change in par. For this driver it doesn't do much. 
 */
static int ax206fb_set_par(struct fb_info *info)
{
	info->fix.line_length = get_line_length(info->var.xres_virtual,
						info->var.bits_per_pixel);
	return 0;
}

/*
 *  Set a single color register. The values supplied are already
 *  rounded down to the hardware's capabilities (according to the
 *  entries in the var structure). Return != 0 for invalid regno.
 */
static int ax206fb_setcolreg(u_int regno, u_int red, u_int green, u_int blue,
			 u_int transp, struct fb_info *info)
{
	if (regno >= 256)	/* no. of hw registers */
		return 1;
	/*
	 * Program hardware... do anything you want with transp
	 */

	/* grayscale works only partially under directcolor */
	if (info->var.grayscale) {
		/* grayscale = 0.30*R + 0.59*G + 0.11*B */
		red = green = blue =
		    (red * 77 + green * 151 + blue * 28) >> 8;
	}

	/* Directcolor:
	 *   var->{color}.offset contains start of bitfield
	 *   var->{color}.length contains length of bitfield
	 *   {hardwarespecific} contains width of RAMDAC
	 *   cmap[X] is programmed to (X << red.offset) | (X << green.offset) | (X << blue.offset)
	 *   RAMDAC[X] is programmed to (red, green, blue)
	 *
	 * Pseudocolor:
	 *    var->{color}.offset is 0 unless the palette index takes less than
	 *                        bits_per_pixel bits and is stored in the upper
	 *                        bits of the pixel value
	 *    var->{color}.length is set so that 1 << length is the number of available
	 *                        palette entries
	 *    cmap is not used
	 *    RAMDAC[X] is programmed to (red, green, blue)
	 *
	 * Truecolor:
	 *    does not use DAC. Usually 3 are present.
	 *    var->{color}.offset contains start of bitfield
	 *    var->{color}.length contains length of bitfield
	 *    cmap is programmed to (red << red.offset) | (green << green.offset) |
	 *                      (blue << blue.offset) | (transp << transp.offset)
	 *    RAMDAC does not exist
	 */
#define CNVT_TOHW(val,width) ((((val)<<(width))+0x7FFF-(val))>>16)
	switch (info->fix.visual) {
	case FB_VISUAL_TRUECOLOR:
	case FB_VISUAL_PSEUDOCOLOR:
		red = CNVT_TOHW(red, info->var.red.length);
		green = CNVT_TOHW(green, info->var.green.length);
		blue = CNVT_TOHW(blue, info->var.blue.length);
		transp = CNVT_TOHW(transp, info->var.transp.length);
		break;
	case FB_VISUAL_DIRECTCOLOR:
		red = CNVT_TOHW(red, 8);	/* expect 8 bit DAC */
		green = CNVT_TOHW(green, 8);
		blue = CNVT_TOHW(blue, 8);
		/* hey, there is bug in transp handling... */
		transp = CNVT_TOHW(transp, 8);
		break;
	}
#undef CNVT_TOHW
	/* Truecolor has hardware independent palette */
	if (info->fix.visual == FB_VISUAL_TRUECOLOR) {
		u32 v;

		if (regno >= 16)
			return 1;

		v = (red << info->var.red.offset) |
		    (green << info->var.green.offset) |
		    (blue << info->var.blue.offset) |
		    (transp << info->var.transp.offset);
		switch (info->var.bits_per_pixel) {
		case 8:
			break;
		case 16:
			((u32 *) (info->pseudo_palette))[regno] = v;
			break;
		case 24:
		case 32:
			((u32 *) (info->pseudo_palette))[regno] = v;
			break;
		}
		return 0;
	}
	return 0;
}

/*
 *  Pan or Wrap the Display
 *
 *  This call looks only at xoffset, yoffset and the FB_VMODE_YWRAP flag
 */
static int ax206fb_pan_display(struct fb_var_screeninfo *var,
			   struct fb_info *info)
{
	if (var->vmode & FB_VMODE_YWRAP) {
		if (var->yoffset < 0
		    || var->yoffset >= info->var.yres_virtual
		    || var->xoffset)
			return -EINVAL;
	} else {
		if (var->xoffset + var->xres > info->var.xres_virtual ||
		    var->yoffset + var->yres > info->var.yres_virtual)
			return -EINVAL;
	}
	info->var.xoffset = var->xoffset;
	info->var.yoffset = var->yoffset;
	if (var->vmode & FB_VMODE_YWRAP)
		info->var.vmode |= FB_VMODE_YWRAP;
	else
		info->var.vmode &= ~FB_VMODE_YWRAP;
	return 0;
}

/*
 *  Most drivers don't need their own mmap function
 */
static int ax206fb_mmap(struct fb_info *info, struct vm_area_struct *vma)
{
	unsigned long start = vma->vm_start;
	unsigned long size = vma->vm_end - vma->vm_start;
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
	unsigned long page, pos;

	if (vma->vm_pgoff > (~0UL >> PAGE_SHIFT))
		return -EINVAL;
	if (size > info->fix.smem_len)
		return -EINVAL;
	if (offset > info->fix.smem_len - size)
		return -EINVAL;

	pos = (unsigned long)info->fix.smem_start + offset;

	pr_notice("mmap() framebuffer addr:%lu size:%lu\n",
		  pos, size);

	while (size > 0) {
		page = vmalloc_to_pfn((void *)pos);
		if (remap_pfn_range(vma, start, page, PAGE_SIZE, PAGE_SHARED))
			return -EAGAIN;

		start += PAGE_SIZE;
		pos += PAGE_SIZE;
		if (size > PAGE_SIZE)
			size -= PAGE_SIZE;
		else
			size = 0;
	}

	return 0;
}

#ifndef MODULE
/*
 * The virtual framebuffer driver is only enabled if explicitly
 * requested by passing 'video=ax206fb:' (or any actual options).
 */
static int __init ax206fb_setup(char *options)
{
	char *this_opt;

	if (!options)
		return 1;

	ax206fb_enable = 1;

	if (!*options)
		return 1;

	while ((this_opt = strsep(&options, ",")) != NULL) {
		if (!*this_opt)
			continue;
		/* Test disable for backwards compatibility */
		if (!strcmp(this_opt, "disable"))
			ax206fb_enable = 0;
		else
			mode_option = this_opt;
	}
	return 1;
}
#endif  /*  MODULE  */

/*
 *  Initialisation
 */
static int __init ax206fb_probe(struct platform_device *dev)
{
	struct fb_info *info;
	unsigned int size = PAGE_ALIGN(videomemorysize);
	int retval = -ENOMEM;

	/*
	 * For real video cards we use ioremap.
	 */
	if (!(videomemory = vmalloc_32_user(size)))
		return retval;

	info = framebuffer_alloc(sizeof(u32) * 256, &dev->dev);
	if (!info)
		goto err;

	info->screen_base = (char __iomem *)videomemory;
	info->fbops = &ax206fb_ops;

	if (!fb_find_mode(&info->var, info, mode_option,
			  NULL, 0, &ax206fb_default, 8)){
		fb_err(info, "Unable to find usable video mode.\n");
		retval = -EINVAL;
		goto err1;
	}

	ax206fb_fix.smem_start = (unsigned long) videomemory;
	ax206fb_fix.smem_len = videomemorysize;
	info->fix = ax206fb_fix;
	info->pseudo_palette = info->par;
	info->par = NULL;
	info->flags = FBINFO_DEFAULT | FBINFO_READS_FAST | FBINFO_VIRTFB | FBINFO_MISC_ALWAYS_SETPAR;

	retval = fb_alloc_cmap(&info->cmap, 256, 0);
	if (retval < 0)
		goto err1;

	retval = register_framebuffer(info);
	if (retval < 0)
		goto err2;
	platform_set_drvdata(dev, info);

	fb_info(info, "ax206fb buffer device, using %ldK of video memory\n",
		videomemorysize >> 10);
	return 0;
err2:
	fb_dealloc_cmap(&info->cmap);
err1:
	framebuffer_release(info);
err:
	vfree(videomemory);
	return retval;
}

static int ax206fb_remove(struct platform_device *dev)
{
	struct fb_info *info = platform_get_drvdata(dev);

	if (info) {
		unregister_framebuffer(info);
		vfree(videomemory);
		fb_dealloc_cmap(&info->cmap);
		framebuffer_release(info);
	}
	return 0;
}




static struct platform_driver ax206fb_driver = {
	.probe	= ax206fb_probe,
	.remove = ax206fb_remove,
	.driver = {
		.name	= "ax206fb",
	},
};

static struct platform_device *ax206fb_device;

static int __init ax206fb_init(void)
{
	int ret = 0;

#ifndef MODULE
	char *option = NULL;

	if (fb_get_options("ax206fb", &option))
		return -ENODEV;
	ax206fb_setup(option);
#endif

	if (!ax206fb_enable)
		return -ENXIO;

	ret = platform_driver_register(&ax206fb_driver);

	if (!ret) {
		ax206fb_device = platform_device_alloc("ax206fb", 0);

		if (ax206fb_device)
			ret = platform_device_add(ax206fb_device);
		else
			ret = -ENOMEM;

		if (ret) {
			platform_device_put(ax206fb_device);
			platform_driver_unregister(&ax206fb_driver);
		}
	}

	/* create debugfs entry */
	ax206fb_debugfs_entry = debugfs_create_file("ax206fb", 0444, NULL, NULL, &ax206fb_sysfs_fops);

	return ret;
}

module_init(ax206fb_init);

#ifdef MODULE
static void __exit ax206fb_exit(void)
{
	/* cleanup and exit */
	platform_device_unregister(ax206fb_device);
	platform_driver_unregister(&ax206fb_driver);

	debugfs_remove(ax206fb_debugfs_entry);

	printk(KERN_INFO "ax206fb module unloaded\n");
}

module_exit(ax206fb_exit);

MODULE_LICENSE("GPL");
#endif				/* MODULE */
