# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 建置指令

### CMake Presets（推薦）

```bash
# Desktop SDL2 — x86_64 開發機 debug 建置，輸出至 build-sdl/
cmake --preset desktop-sdl
cmake --build --preset desktop-sdl

# STM32MP157 DRM — Cortex-A7 release 交叉編譯，輸出至 build-a7/
cmake --preset a7-drm
cmake --build --preset a7-drm
```

### 手動 CMake（選擇任意 config）

```bash
cmake -B build [-DCONFIG=<config_name>]   # 省略 CONFIG 則使用 lv_conf.defaults
cmake --build build -j$(nproc)
```

`configs/` 下可用的 config：`sdl`、`drm-egl-2d`、`fbdev`、`wayland`、`vic3da-drm` 等。

### 執行

```bash
./build-sdl/bin/lvglsim [-b sdl] [-W 1280] [-H 720] [-f]
# 透過環境變數覆寫視窗大小：
LV_SIM_WINDOW_WIDTH=1024 LV_SIM_WINDOW_HEIGHT=600 ./build-sdl/bin/lvglsim
```

`-B` 列出已編譯的 backend；`-b <name>` 在執行時選擇特定 backend。

### ASAN 建置

```bash
cmake -B build -DASAN=ON && cmake --build build -j$(nproc)
```

## 架構

### 雙目標設計

| Preset | 目標平台 | Backend | Config |
|--------|--------|---------|--------|
| `desktop-sdl` | x86_64 開發機 | SDL2 | `lv_conf.defaults` |
| `a7-drm` | STM32MP157F-DK2 (Cortex-A7) | DRM/KMS + EVDEV | `configs/vic3da-drm.defaults` |

交叉編譯工具鏈位於 `/home/syuan/STM32MPU_workspace/STM32MP157-DK2_v25.06.11/SDK`（詳見 `cmake/stm32mp157-toolchain.cmake`）。

### lv_conf.h 生成機制

`lv_conf.h` 在 **configure 階段**由 `lvgl/scripts/generate_lv_conf.py` 從 `lv_conf.defaults`（或所選的 `configs/*.defaults`）生成。**絕對不要直接編輯 `build*/lv_conf.h`**，應編輯對應的 `.defaults` 來源檔案。

### Backend 抽象層（`src/lib/`）

`driver_backends.c` 負責註冊所有已編譯的 display 與 input backend、依名稱選擇、初始化（建立 `lv_display`／`lv_indev`），並進入其 run loop。每個 backend 分別位於 `src/lib/display_backends/` 或 `src/lib/indev_backends/`。

### VIC3DA 應用層（`src/app/`）

這是一個自訂的感測器視覺化 UI，用於顯示透過 RPMsg 從 STM32MP157 的 M4 協處理器傳來的 VIC3DA IMU/GNSS 板資料。

**資料流（生產者 → 消費者）：**

```
/dev/ttyRPMSG0
    └─ rpmsg_client.c  （reader thread：阻塞式 read 迴圈）
          └─ packet_parser.c  （65 byte 二進位封包的 byte-stream 組幀器）
                └─ shared_state.c  （mutex 保護的快照；4 種感測器類型）
                      └─ ui_main.c lv_timer @ 16ms  （LVGL 主執行緒輪詢）
                            ├─ ui_accel.c    — 加速度計圖表 + 數值標籤
                            ├─ ui_gnss.c     — GNSS 座標
                            ├─ ui_temp.c     — 溫度儀表
                            ├─ ui_selftest.c — 自我測試結果
                            ├─ ui_statusbar.c — CPU 使用率 + 時鐘
                            └─ ui_control.c  — Start / Stop / Self-Test / Recal 按鈕
```

**執行緒模型：** 單一 LVGL 主執行緒 + 一條 RPMsg reader 執行緒。`shared_state.c` 持有一個 mutex；`shared_state_update_*()` 由 reader 執行緒呼叫，`shared_state_read()` 則只在 LVGL timer callback 中呼叫。

**封包格式：** `packet_parser.h` 中定義 4 種固定 65 byte 的 struct，以 3 byte 的 header magic 區分。傳送給 M4 的指令為單一 byte ASCII：`s`=start、`e`=stop、`r`=recal、`c`=self-test。

**觸控校正：** `ui_main_init()` 為直向 480×800 觸控面板套用 90° CCW 軟體旋轉與軸交換，以對應橫向 800×480 顯示。此段以 `#if LV_USE_EVDEV` 條件編譯保護。

**DRM SW 旋轉：** `src/lib/display_backends/drm.c` 的 `init_drm()` 呼叫 `lv_linux_drm_enable_sw_rotation(disp, false)` 將直向 480×800 物理畫面轉為橫向 800×480 邏輯顯示。此函式原本存在於舊版 LVGL，被移除後已手動移植回 `lvgl/src/drivers/display/drm/lv_linux_drm.c`（同時更新 `include/lvgl/drivers/display/lv_linux_drm.h` 宣告）。**若 LVGL submodule 再次被 bot 更新，此函式會被覆蓋，需重新移植。**

**Chart Auto Scale：** `ui_accel.c` 的 Filtered / Raw 圖表與 `ui_temp.c` 的 Temperature 圖表均實作 auto scale——每次 update 後透過 `lv_chart_get_series_y_array()` 掃描所有點，以 `min/max ± 10% margin`（最小 50 單位）動態更新 Y 軸範圍。

**正常關機流程：** `SIGTERM`／`SIGINT` 將 `g_running` 設為 0（宣告於 `app_lifecycle.h`，定義於 `main.c`）並呼叫 `rpmsg_client_stop()`。DRM run loop 每個 tick 檢查 `g_running`。

## 部署到 STM32MP157F-DK2（192.168.11.13）

### 每次更新 binary

```bash
cmake --build --preset a7-drm
# 必須先 stop 再 scp，否則 binary 正在執行會報 "Text file busy"
ssh root@192.168.11.13 "systemctl stop lvglsim"
scp build-a7/bin/lvglsim root@192.168.11.13:~/
ssh root@192.168.11.13 "systemctl start lvglsim"
```

### 服務管理

```bash
ssh root@192.168.11.13 "systemctl status lvglsim"
ssh root@192.168.11.13 "journalctl -u lvglsim -f"          # 即時 log
ssh root@192.168.11.13 "systemctl restart lvglsim"
```

服務檔位於板子的 `/etc/systemd/system/lvglsim.service`，開機自動啟動（`multi-user.target`）。

**已停用的衝突服務：**
- `ttyrpmsg_reader_mqtt.service` — 原本也讀 `/dev/ttyRPMSG0`，與 `lvglsim` 衝突
- `weston-graphical-session.service` — Weston 與 `lvglsim` 爭奪 DRM master

## 部署到 Ka-Ro TXMP-1570（192.168.11.15）

### U-Boot 前置設定（一次性）

TXMP-1570 的顯示 pipeline 由 U-Boot 在開機時根據 `videomode` 變數動態填入 Device Tree。
**必須先在 U-Boot 設定，否則 Linux 的 stm32-display 驅動找不到 CRTC。**

```
TXMP U-Boot > setenv videomode ET0350
TXMP U-Boot > saveenv
TXMP U-Boot > boot
```

### 每次更新 binary

```bash
cmake --build --preset txmp-drm
ssh root@192.168.11.15 "systemctl stop lvglsim"
scp build-txmp/bin/lvglsim root@192.168.11.15:~/
ssh root@192.168.11.15 "systemctl start lvglsim"
```

### 服務管理

```bash
ssh root@192.168.11.15 "systemctl status lvglsim"
ssh root@192.168.11.15 "journalctl -u lvglsim -f"
ssh root@192.168.11.15 "systemctl restart lvglsim"
```

**Backend：fbdev（`/dev/fb0`）**
TXMP 的 `stm32-display` DRM 驅動在此 BSP 不支援 userspace atomic modesetting，
改用 fbdev backend 直接寫入 `/dev/fb0`（`vic3da-txmp.defaults` 中 `LV_USE_LINUX_FBDEV=1`）。

### 新增 UI 分頁

1. 仿照 `ui_accel.c` 的模式建立 `src/app/ui_foo.c` + `src/app/ui_foo.h`。
2. 在 `shared_state.h` / `shared_state.c` 中新增對應的 `fresh` 欄位並更新 struct。
3. 在 `packet_parser.h` 新增封包類型，並在 `packet_parser.c` 中加入 dispatch。
4. 在 `ui_main_init()` 中呼叫 `ui_foo_create(lv_tabview_add_tab(tv, "Foo"))`。
5. 在 `sensor_poll_timer_cb` 中輪詢 `snap.foo_fresh`。
