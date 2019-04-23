#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
 
#include <linux/gpio.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>

static struct class *cls;
static int led_gpios[] = {
	EXYNOS4212_GPM4(0),
	EXYNOS4212_GPM4(1),
	EXYNOS4212_GPM4(2),
	EXYNOS4212_GPM4(3),
};

static int leds_open (struct inode *inode, struct file *filp);
static long leds_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

static struct file_operations leds_ops = {
	.owner			= THIS_MODULE,
	.open	= leds_open,
	.unlocked_ioctl = leds_ioctl,
};

static int leds_open (struct inode *inode, struct file *filp)
{
	int i;

	for (i = 0; i < 4; i++)
	{
		s3c_gpio_cfgpin(led_gpios[i], S3C_GPIO_OUTPUT);
	}

	return 0;
}

static long leds_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	/* ���ݲ����������GPIO��ƽ */
	/* cmd: 0-off��1-on */
	/* arg: 0-3��which led */
	if ((cmd != 0) && (cmd != 1))
		return -EINVAL;

	if (arg > 4)
		return -EINVAL;
	
	gpio_set_value(led_gpios[arg], !cmd);

	return 0;
}

static int major;
static int leds_init(void)
{
	major = register_chrdev(0, "leds", &leds_ops);

	/* Ϊ����ϵͳudev��mdev�����Ǵ����豸�ڵ� */
	/* �����࣬�������´����豸�ڵ� */
	cls = class_create(THIS_MODULE, "leds");
	device_create(cls , NULL, MKDEV(major, 0), NULL, "leds");

	return 0;
}

static void leds_exit(void)
{
	device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);
	unregister_chrdev(major, "leds");
}

module_init(leds_init);
module_exit(leds_exit);

MODULE_LICENSE("GPL");

