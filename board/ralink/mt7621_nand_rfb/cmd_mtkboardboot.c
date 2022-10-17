// SPDX-License-Identifier:	GPL-2.0+
/*
 * Copyright (C) 2018 MediaTek Incorporation. All Rights Reserved.
 *
 * Author: Weijie Gao <weijie.gao@mediatek.com>
 */

#include <common.h>
#include <asm/types.h>
#include <linux/mtd/mtd.h>
#include <linux/sizes.h>
#include <jffs2/jffs2.h>

#include "../common/dual_image.h"

static int do_mtkboardboot(cmd_tbl_t *cmdtp, int flag, int argc,
	char *const argv[])
{
	char cmd[128];
	const char *ep;

#ifdef CONFIG_MTK_DUAL_IMAGE_SUPPORT
	dual_image_check();
#endif

	ep = env_get("autostart");
	if (ep)
		ep = strdup(ep);

	env_set("autostart", "yes");

#ifndef CONFIG_ENABLE_NAND_NMBM
	run_command("nboot firmware", 0);

	sprintf(cmd, "nboot 0x%08x nand0 0x%08x",
		CONFIG_SYS_SDRAM_BASE + SZ_32M,
		CONFIG_DEFAULT_NAND_KERNEL_OFFSET);
	run_command(cmd, 0);
#else
	run_command("nmbm nmbm0 boot firmware", 0);

	sprintf(cmd, "nmbm nmbm0 boot 0x%08x 0x%08x",
		CONFIG_SYS_SDRAM_BASE + SZ_32M,
		CONFIG_DEFAULT_NAND_KERNEL_OFFSET);
	run_command(cmd, 0);

#ifdef CONFIG_BOARD_H3C_TX180X	// boot stock firmware and OpenWrt 
	run_command("nmbm nmbm0 read 0x88000000 0x200000 0x800000", 0);
	run_command("bootm 0x880000a0", 0);
#endif

#endif

	if (ep) {
		env_set("autostart", ep);
		free((void *) ep);
	}

#ifndef CONFIG_WEBUI_FAILSAFE_ON_AUTOBOOT_FAIL
	return CMD_RET_FAILURE;
#else
	return run_command("httpd", 0);
#endif
}

U_BOOT_CMD(mtkboardboot, 1, 0, do_mtkboardboot,
	"Boot MT7621 firmware", ""
);