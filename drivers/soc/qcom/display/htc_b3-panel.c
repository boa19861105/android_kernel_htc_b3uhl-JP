
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/debug_display.h>
#include <linux/htc_flags.h>

#include "../../../../drivers/video/msm/mdss/mdss_dsi.h"

extern void set_screen_status(bool onoff);

void incell_driver_ready(void (*fn))
{
	return;
}
EXPORT_SYMBOL(incell_driver_ready);

static uint8_t panel_id;
static struct kobject *android_disp_kobj;
struct lcd_vendor_info {
	int idx;
	char vendor[64];
} lcd_name[] = {
	{0x00, "JDI_RES-63423"},
	{0x01, "TIA_HX-8396"},
};

char *disp_vendor(void){
	return lcd_name[panel_id].vendor;
}
EXPORT_SYMBOL(disp_vendor);

static ssize_t disp_vendor_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	ret = snprintf(buf, PAGE_SIZE, "%s\n", lcd_name[panel_id].vendor);
	return ret;
}
static DEVICE_ATTR(vendor, S_IRUGO, disp_vendor_show, NULL);

static int htc_display_sysfs_init(bool status)
{
	if (status) {
		if (android_disp_kobj) {
			PR_DISP_INFO("Already Register the android_display/vendor filenode");
			return 0;
		}
		android_disp_kobj = kobject_create_and_add("android_display", NULL);
		if (!android_disp_kobj) {
			PR_DISP_ERR("%s: subsystem register failed\n", __func__);
			return -ENOMEM;
		}
		if (sysfs_create_file(android_disp_kobj, &dev_attr_vendor.attr)) {
			PR_DISP_ERR("Fail to create sysfs file (vendor)\n");
			return -ENOMEM;
		}
	} else {
		sysfs_remove_file(android_disp_kobj, &dev_attr_vendor.attr);
		kobject_del(android_disp_kobj);
	}
	return 0;
}

static int htc_b3_regulator_deinit(struct platform_device *pdev)
{
	PR_DISP_INFO("%s()\n", __func__);
	htc_display_sysfs_init(0);

	return 0;
}

static int htc_b3_regulator_init(struct platform_device *pdev)
{
	int ret = 0;
	struct mdss_dsi_ctrl_pdata *ctrl_pdata = NULL;

	if (!pdev) {
		PR_DISP_ERR("%s: invalid input\n", __func__);
		return -EINVAL;
	}

	ctrl_pdata = platform_get_drvdata(pdev);
	if (!ctrl_pdata) {
		PR_DISP_ERR("%s: invalid driver data\n", __func__);
		return -EINVAL;
	}

	panel_id = ctrl_pdata->panel_data.panel_info.htc_panel_id;

	htc_display_sysfs_init(1);

	PR_DISP_INFO("== %s( DONE ) ==\n", __func__);
	return ret;
}

static int htc_b3_panel_power_on(struct mdss_panel_data *pdata)
{
	set_screen_status(true);
	return 0;
}

static int htc_b3_panel_power_off(struct mdss_panel_data *pdata)
{
        set_screen_status(false);
	return 0;
}

static struct mdss_dsi_pwrctrl dsi_pwrctrl = {
	.dsi_regulator_init   = htc_b3_regulator_init,
	.dsi_regulator_deinit = htc_b3_regulator_deinit,
	.dsi_power_on         = htc_b3_panel_power_on,
	.dsi_power_off        = htc_b3_panel_power_off,
};

static struct platform_device dsi_pwrctrl_device = {
	.name = "mdss_dsi_pwrctrl",
	.id   = -1,
	.dev.platform_data = &dsi_pwrctrl,
};

int __init htc_8994_dsi_panel_power_register(void)
{
	platform_device_register(&dsi_pwrctrl_device);
	return 0;
}

arch_initcall(htc_8994_dsi_panel_power_register);
