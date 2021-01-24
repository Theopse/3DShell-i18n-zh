#include <stdio.h>
#include <string.h>

#include "C2D_helper.h"
#include "common.h"
#include "config.h"
#include "dirbrowse.h"
#include "fs.h"
#include "ftp.h"
#include "menu_main.h"
#include "status_bar.h"
#include "textures.h"
#include "touch.h"
#include "utils.h"

void Menu_DisplayFTP(void) {
	ftp_init();

	Result ret = 0;
	char buf[137], hostname[128];
	u32 wifi_status = 0;

	int pBar = 0, xlim = 270;

	ret = gethostname(hostname, sizeof(hostname));

	if (R_SUCCEEDED(gspLcdInit())) {
		GSPLCD_PowerOffBacklight(GSPLCD_SCREEN_TOP);
		gspLcdExit();
	}

	while(MENU_STATE == MENU_STATE_FTP) {
		ftp_loop();
		
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(RENDER_BOTTOM, config.dark_theme? BLACK_BG : WHITE);
		C2D_SceneBegin(RENDER_BOTTOM);
		Draw_Rect(0, 0, 320, 20, config.dark_theme? STATUS_BAR_DARK : STATUS_BAR_LIGHT); // Status bar
		Draw_Rect(0, 20, 320, 220, config.dark_theme? MENU_BAR_DARK : MENU_BAR_LIGHT); // Menu bar

		ret = ACU_GetWifiStatus(&wifi_status);
			
		Menu_DrawMenuBar();

		if ((wifi_status != 0) && R_SUCCEEDED(ret)) {
			Draw_Text(((320 - Draw_GetTextWidth(0.42f, "FTP 已初始化")) / 2), 37, 0.42f, WHITE, "FTP 已初始化");
			snprintf(buf, 137, "IP: %s:5000", R_FAILED(ret)? "获取 IP 失败" : hostname);

			if (strlen(ftp_accepted_connection) != 0)
				Draw_Text(((320 - Draw_GetTextWidth(0.42f, ftp_accepted_connection)) / 2), 77, 0.42f, WHITE, ftp_accepted_connection);

			Draw_Text(((320 - Draw_GetTextWidth(0.42f, "目前无法访问文件浏览器。")) / 2), 97, 0.42f, WHITE, "目前无法访问文件浏览器。");

			if (strlen(ftp_file_transfer) != 0)
				Draw_Text(((320 - Draw_GetTextWidth(0.42f, ftp_file_transfer)) / 2), 147, 0.42f, WHITE, ftp_file_transfer);

			if (isTransfering) {
				Draw_Rect(50, 140, 220, 3, config.dark_theme? STATUS_BAR_DARK : STATUS_BAR_LIGHT);
				Draw_Rect(pBar, 140, 40, 3, WHITE);

				// Boundary stuff
				Draw_Rect(0, 140, 50, 3, config.dark_theme? MENU_BAR_DARK : MENU_BAR_LIGHT);
				Draw_Rect(270, 140, 50, 3, config.dark_theme? MENU_BAR_DARK : MENU_BAR_LIGHT); 
				pBar += 4;
			
				if (pBar >= xlim)
					pBar = 34;
			}
		}
		else {
			Draw_Text(((320 - Draw_GetTextWidth(0.42f, "初始化 FTP 失败。")) / 2), 37, 0.42f, WHITE, "初始化 FTP 失败。");
			snprintf(buf, 18, "WiFi 未打开。");
		}

		Draw_Text(((320 - Draw_GetTextWidth(0.42f, buf)) / 2), 57, 0.42f, WHITE, buf);
		Draw_Text(((320 - Draw_GetTextWidth(0.42f, "点击 FTP 图标来关闭 FTP 连接。")) / 2), 117, 0.42f, WHITE, "点击 FTP 图标来关闭 FTP 连接。");

		Draw_EndFrame();

		hidScanInput();
		u32 kDown = hidKeysDown();

		if (((kDown & KEY_TOUCH) && (TouchInRect(73, 0, 97, 20))) || (kDown & KEY_SELECT))
			break;
	}

	memset(ftp_accepted_connection, 0, 20); // Empty accepted connection address
	memset(ftp_file_transfer, 0, 50); // Empty transfer status
	ftp_exit();

	if (R_SUCCEEDED(gspLcdInit())) {
		GSPLCD_PowerOnBacklight(GSPLCD_SCREEN_TOP);
		gspLcdExit();
	}

	MENU_STATE = MENU_STATE_HOME;
	Dirbrowse_PopulateFiles(true);
}
