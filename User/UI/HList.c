#include "hlist.h"
#include "u8g2.h"
#include <string.h>

/**
 * @brief  绘制保护提示弹窗
 *
 * @param[in]  a
 * @param[in]  b
 *
 * @return
 */
static void draw_hlist_alert_window(u8g2_t *u8g2, const Screen_t *screen_cfg,
                                    hlist_t *hl) {
  if (u8g2 == NULL || screen_cfg == NULL || hl == NULL || !hl->alert.active ||
      hl->alert.text == NULL) {
    return;
  }

  // 弹窗尺寸
  int w = 100, h = 35;
  int x = (screen_cfg->width - w) / 2;
  int y = (screen_cfg->height - h) / 2;

  // 绘制弹窗背景和边框
  u8g2_SetDrawColor(u8g2, 0); // 黑色背景
  u8g2_DrawBox(u8g2, x, y, w, h);
  u8g2_SetDrawColor(u8g2, 1); // 白色边框
  u8g2_DrawFrame(u8g2, x, y, w, h);

  // 绘制Alert标题
  u8g2_SetFont(u8g2, screen_cfg->sub_window_font);
  const char *alert_title = "Alert";
  int title_width = u8g2_GetStrWidth(u8g2, alert_title);
  g_screen_cfg.draw_text(u8g2, x + (w - title_width) / 2, y + 12, alert_title);

  u8g2_DrawHLine(u8g2, x + 5, y + 15, w - 10);

  // 绘制提示文本
  uint8_t clip_y1 = y + 16;
  uint8_t clip_y2 = y + 28;
  int text_max_w = w - 10;

  // 滚动文本逻辑
  int text_width = u8g2_GetStrWidth(u8g2, hl->alert.text);
  int text_x = x + 5;
  if (text_width > text_max_w) {
    uint32_t tick = *hl->main_tick;
    int offset = (tick / 5) % (text_width + text_max_w);
    text_x = x + 5 - offset;

    u8g2_SetClipWindow(u8g2, x + 5, clip_y1, x + w - 5, clip_y2);
    g_screen_cfg.draw_text(u8g2, text_x, y + 27, hl->alert.text);
    g_screen_cfg.draw_text(u8g2, text_x + text_width + 10, y + 27, hl->alert.text);
    u8g2_SetMaxClipWindow(u8g2);
  } else {
    // 文本宽度不足，直接居中
    text_x = x + (w - text_width) / 2;
    g_screen_cfg.draw_text(u8g2, text_x, y + 27, hl->alert.text);
  }
}

void hlist_init(hlist_t *hl, uint32_t *tick_ptr) {
  if (hl == NULL || tick_ptr == NULL) {
    return;
  }

  memset(hl, 0, sizeof(hlist_t));
  hl->main_tick = tick_ptr;
  hl->from_index = 0;
  hl->to_index = 0;
  hl->start_tick = *tick_ptr;
  hl->alert.active = false;
  hl->alert.text = NULL;
}

/**
 * @brief  添加XBM位图类型的列表项
 *
 * @param[in]  a
 * @param[in]  b
 *
 * @return
 */
void hlist_add_xbm_item(hlist_t *hl, const char *title, const uint8_t *icon_xbm,
                    const page_component_t *comp, void *ctx) {
  if (hl == NULL) {
    return;
  }
  if (hl->count < 8) {
    hl->items[hl->count].title = title;
    hl->items[hl->count].icon_type = ICON_TYPE_XBM;
    hl->items[hl->count].icon_data.xbm = icon_xbm;
    hl->items[hl->count].comp = comp;
    hl->items[hl->count].ctx = ctx;
    hl->items[hl->count].protect.guard_flag = true;
    hl->items[hl->count].protect.alert_text = NULL;
    hl->count++;
  }
}

/**
 * @brief  添加Glyph字符类型的列表项
 *
 * @param[in]  a
 * @param[in]  b
 *
 * @return
 */
void hlist_add_glyph_item(hlist_t *hl, const char *title, uint16_t glyph,
                    const page_component_t *comp, void *ctx) {
  if (hl == NULL) {
    return;
  }
  if (hl->count < 8) {
    hl->items[hl->count].title = title;
    hl->items[hl->count].icon_type = ICON_TYPE_GLYPH;
    hl->items[hl->count].icon_data.glyph = glyph;
    hl->items[hl->count].comp = comp;
    hl->items[hl->count].ctx = ctx;
    hl->items[hl->count].protect.guard_flag = true;
    hl->items[hl->count].protect.alert_text = NULL;
    hl->count++;
  }
}

/**
 * @brief  添加带保护的XBM位图类型列表项
 *
 * @param[in]  a
 * @param[in]  b
 *
 * @return
 */
void hlist_add_protected_xbm_item(hlist_t *hl, const char *title,
                    const uint8_t *icon_xbm, const page_component_t *comp,
                    void *ctx, bool guard_flag, char *alert_text) {
  if (hl == NULL || alert_text == NULL) {
    return;
  }
  if (hl->count < 8) {
    hl->items[hl->count].title = title;
    hl->items[hl->count].icon_type = ICON_TYPE_XBM;
    hl->items[hl->count].icon_data.xbm = icon_xbm;
    hl->items[hl->count].comp = comp;
    hl->items[hl->count].ctx = ctx;
    hl->items[hl->count].protect.guard_flag = guard_flag;
    hl->items[hl->count].protect.alert_text = alert_text;
    hl->count++;
  }
}

/**
 * @brief  添加带保护的Glyph字符类型列表项
 *
 * @param[in]  a
 * @param[in]  b
 *
 * @return
 */
void hlist_add_protected_glyph_item(hlist_t *hl, const char *title,
                    uint16_t glyph, const page_component_t *comp,
                    void *ctx, bool guard_flag, char *alert_text) {
  if (hl == NULL || alert_text == NULL) {
    return;
  }
  if (hl->count < 8) {
    hl->items[hl->count].title = title;
    hl->items[hl->count].icon_type = ICON_TYPE_GLYPH;
    hl->items[hl->count].icon_data.glyph = glyph;
    hl->items[hl->count].comp = comp;
    hl->items[hl->count].ctx = ctx;
    hl->items[hl->count].protect.guard_flag = guard_flag;
    hl->items[hl->count].protect.alert_text = alert_text;
    hl->count++;
  }
}

// 绘制水平列表
void hlist_draw(u8g2_t *u8g2, void *ctx) {
  if (u8g2 == NULL || ctx == NULL) {
    return;
  }
  hlist_t *hl = (hlist_t *)ctx;
  if (hl->count == 0) {
    return;
  }

  const Screen_t *screen_cfg = &g_screen_cfg;

  int screen_mid = screen_cfg->width / 2;
  int icon_gap = (screen_cfg->width * 2) / 5; // 屏幕宽度的2/5作为图标间距
  int text_base_y =
      screen_cfg->height -
      (screen_cfg->height - screen_cfg->font_height) / 8; // 计算文字Y坐标
  // 动画时长
  const uint32_t animation_ticks = HLIST_ANIM_TICKS;

  uint32_t current_tick = *hl->main_tick;
  uint32_t tick_diff = current_tick - hl->start_tick;
  float p = (float)tick_diff / animation_ticks;
  p = (p < 0.0f) ? 0.0f : (p > 1.0f) ? 1.0f : p;
  bool is_moving = (p < 1.0f);

  float ease_idx =
      hl->from_index + (hl->to_index - hl->from_index) * HLIST_ICON_ANIM(p);

  for (int i = 0; i < hl->count; i++) {
    int x = screen_mid + (int)((i - ease_idx) * icon_gap) - (ICON_WIDTH / 2);

    // 绘制屏幕可见范围内的图标
    if (x > -ICON_WIDTH && x < screen_cfg->width) {
      u8g2_SetDrawColor(u8g2, 1);
      int icon_y = (screen_cfg->height - ICON_HEIGHT - screen_cfg->font_height) / 2;
      
      // 根据图标类型选择绘制方式
      switch (hl->items[i].icon_type) {
        case ICON_TYPE_XBM:
          // XBM位图绘制逻辑
          if (hl->items[i].icon_data.xbm != NULL) {
            u8g2_DrawXBM(u8g2, x, icon_y, ICON_WIDTH, ICON_HEIGHT,
                        hl->items[i].icon_data.xbm);
          }
          break;
          
        case ICON_TYPE_GLYPH:
          // Glyph字符绘制逻辑
          u8g2_SetFont(u8g2, HLIST_ICON_FONT);
          // 计算字符居中显示的偏移
          int glyph_x = x + (ICON_WIDTH - u8g2_GetGlyphWidth(u8g2, hl->items[i].icon_data.glyph)) / 2;
          int glyph_y = icon_y + ICON_HEIGHT - (ICON_HEIGHT - u8g2_GetFontAscent(u8g2)) / 2;
          u8g2_DrawGlyph(u8g2, glyph_x, glyph_y, hl->items[i].icon_data.glyph);
          break;
          
        default:
          break;
      }
    }
  }

  u8g2_SetFont(u8g2, HLIST_TEXT_FONT);
  const char *title = hl->items[hl->to_index].title;
  if (title != NULL) {
    if (is_moving) {
      float text_p = HLIST_TEXT_ANIM(p);
      int text_y_start = screen_cfg->height + screen_cfg->font_height;
      int text_y_target = text_base_y;
      int text_y =
          text_y_start - (int)((text_y_start - text_y_target) * text_p);

      int text_x = (screen_cfg->width - u8g2_GetStrWidth(u8g2, title)) / 2;
      g_screen_cfg.draw_text(u8g2, text_x, text_y, title);
    } else {
      // 动画结束，固定位置显示
      int text_x = (screen_cfg->width - u8g2_GetStrWidth(u8g2, title)) / 2;
      g_screen_cfg.draw_text(u8g2, text_x, text_base_y, title);
    }
  }

  // 绘制保护提示弹窗
  if (hl->alert.active) {
    draw_hlist_alert_window(u8g2, screen_cfg, hl);
  }
}

void hlist_input(int btn, void *ctx) {
  if (ctx == NULL) {
    return;
  }
  hlist_t *hl = (hlist_t *)ctx;
  if (hl->count == 0) {
    return;
  }

  // 优先处理提示弹窗
  if (hl->alert.active) {
    if (btn == BTN_ENTER || btn == BTN_BACK) {
      hl->alert.active = false; // 关闭弹窗
      hl->alert.text = NULL;
    }
    return;
  }

  // 处理按钮事件
  if (btn == BTN_UP && hl->to_index > 0) {
    hl->from_index = hl->to_index;
    hl->to_index--;
    hl->start_tick = *hl->main_tick;
  } else if (btn == BTN_DOWN && hl->to_index < hl->count - 1) {
    hl->from_index = hl->to_index;
    hl->to_index++;
    hl->start_tick = *hl->main_tick;
  } else if (btn == BTN_ENTER) {
    hlist_item_t *it = &hl->items[hl->to_index];
    if (it->comp != NULL) {
      // 检查保护标志
      if (it->protect.guard_flag) {
        // 保护标志为true，直接进入目标页面
        page_stack_push(&g_page_stack, it->comp, it->ctx);
      } else {
        // 保护标志为false，弹出提示弹窗
        hl->alert.active = true;
        hl->alert.text = it->protect.alert_text;
      }
    }
  }
}

const page_component_t HLIST_COMP = {hlist_draw, hlist_input};