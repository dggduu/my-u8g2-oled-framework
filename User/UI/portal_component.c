#include "portal_component.h"
#include <stdio.h>
#include <math.h>

// --- MessageBox Portal 组件 ---
static void portal_messagebox_draw(u8g2_t *u8g2, int16_t x, int16_t y, uint8_t w, uint8_t h, void *ctx) {
    if (!ctx) return;
    portal_ctx_message_box_t *data = (portal_ctx_message_box_t *)ctx;

    // 绘制背景和外框
    u8g2_SetDrawColor(u8g2, 0);
    u8g2_DrawBox(u8g2, x, y, w, h);
    u8g2_SetDrawColor(u8g2, 1);
    u8g2_DrawFrame(u8g2, x, y, w, h);

    u8g2_SetFont(u8g2, g_screen_cfg.sub_window_font);
    
    // 标题居中绘制
    if (data->title) {
        int title_w = u8g2_GetStrWidth(u8g2, data->title);
        g_screen_cfg.draw_text(u8g2, x + (w - title_w) / 2, y + 12, data->title);
    }
    
    u8g2_DrawHLine(u8g2, x + 5, y + 15, w - 10);

    // 内容绘制
    if (data->msg) {
        g_screen_cfg.draw_text(u8g2, x + 5, y + 27, data->msg);
    }
}

static void portal_mbox_input(int btn, void *ctx) {
    if (btn == BTN_ENTER || btn == BTN_BACK) {
        page_stack_portal_toggle(&g_page_stack, NULL, NULL, 0);
    }
}

const portal_component_t PORTAL_MESSAGE_BOX = {
    .draw = portal_messagebox_draw, 
    .input = portal_mbox_input,
    .w = 100, 
    .h = 35
};

// --- NumSelector Portal 组件 ---
static void portal_num_draw(u8g2_t *u8g2, int16_t x, int16_t y, uint8_t w, uint8_t h, void *ctx) {
    if (!ctx) return;
    portal_ctx_num_t *data = (portal_ctx_num_t *)ctx;
    if (!data->val_ptr) return;

    float val = *(data->val_ptr);
    char buf[32];
    const Screen_t *sc = &g_screen_cfg;
    uint32_t current_tick = g_page_stack.main_tick;

    u8g2_SetDrawColor(u8g2, 0);
    u8g2_DrawBox(u8g2, x, y, w, h);
    u8g2_SetDrawColor(u8g2, 1);
    u8g2_DrawFrame(u8g2, x, y, w, h);

    u8g2_SetFont(u8g2, sc->font);
    draw_scroll_text_with_pause(u8g2, sc, data->title, x + 5, w - 10,
                                y + 12, current_tick, y + 1, y + 14);

    u8g2_SetFont(u8g2, sc->font);
    sprintf(buf, "%.1f", val);
    int val_w = u8g2_GetStrWidth(u8g2, buf);
    u8g2_DrawStr(u8g2, x + (w - val_w) / 2, y + 26, buf);

    int bx = x + 10, by = y + 30, bw = w - 20, bh = 6;
    u8g2_DrawFrame(u8g2, bx, by, bw, bh);
    float ratio = 0.0f;
    if (data->max > data->min) {
        ratio = (val - data->min) / (data->max - data->min);
        ratio = fmaxf(0.0f, fminf(1.0f, ratio));
    }
    if (ratio > 0) {
        u8g2_DrawBox(u8g2, bx + 2, by + 2, (int)((bw - 4) * ratio), bh - 4);
    }

    u8g2_SetFont(u8g2, sc->sub_window_font);
    sprintf(buf, "[%.1f,%.1f,%.1f]", data->min, data->step, data->max);
    int range_w = u8g2_GetStrWidth(u8g2, buf);
    u8g2_DrawStr(u8g2, x + (w - range_w) / 2, y + 45, buf);
}

static void portal_num_input(int btn, void *ctx) {
    if (!ctx) return;
    portal_ctx_num_t *data = (portal_ctx_num_t *)ctx;
    if (!data->val_ptr) return;

    if (btn == BTN_UP) {
        *(data->val_ptr) = fminf(*(data->val_ptr) + data->step, data->max);
    } 
    else if (btn == BTN_DOWN) {
        *(data->val_ptr) = fmaxf(*(data->val_ptr) - data->step, data->min);
    } 
    else if (btn == BTN_ENTER || btn == BTN_BACK) {
        page_stack_portal_toggle(&g_page_stack, NULL, NULL, 0);
    }
}

const portal_component_t PORTAL_NUM = {
    .draw = portal_num_draw, 
    .input = portal_num_input,
    .w = 100,
    .h = 48
};

static void portal_int_precise_draw(u8g2_t *u8g2, int16_t x, int16_t y, uint8_t w, uint8_t h, void *ctx) {
    if (!ctx) return;
    portal_ctx_int_precise_t *data = (portal_ctx_int_precise_t *)ctx;
    const Screen_t *sc = &g_screen_cfg;
    char buf[16];

    u8g2_SetDrawColor(u8g2, 0);
    u8g2_DrawBox(u8g2, x, y, w, h);
    u8g2_SetDrawColor(u8g2, 1);
    u8g2_DrawFrame(u8g2, x, y, w, h);

    u8g2_SetFont(u8g2, sc->font);

    if (data->title) {
        u8g2_DrawStr(u8g2, x + 5, y + 12, data->title);
    }
    sprintf(buf, "[%u,%u]", data->min, data->max);
    int range_w = u8g2_GetStrWidth(u8g2, buf);
    u8g2_DrawStr(u8g2, x + w - range_w - 5, y + 12, buf);

    u8g2_DrawHLine(u8g2, x + 2, y + 15, w - 4);

    char fmt[8];
    sprintf(fmt, "%%0%du", data->fixed_digit);
    sprintf(buf, fmt, *(data->val_ptr));

    int char_w = u8g2_GetMaxCharWidth(u8g2);
    int total_val_w = u8g2_GetStrWidth(u8g2, buf);
    int start_x = x + (w - total_val_w) / 2;
    int start_y = y + 32;
    
    u8g2_DrawStr(u8g2, start_x, start_y, buf);

    int cursor_x = start_x + (data->fixed_digit - 1 - data->cursor_pos) * char_w;
    
    u8g2_SetDrawColor(u8g2, 2);
    u8g2_DrawBox(u8g2, cursor_x, start_y - sc->font_height + 2, char_w, sc->font_height);
    u8g2_SetDrawColor(u8g2, 1);
}

static void portal_int_precise_input(int btn, void *ctx) {
    if (!ctx) return;
    portal_ctx_int_precise_t *data = (portal_ctx_int_precise_t *)ctx;
    uint32_t val = *(data->val_ptr);
    
    // 计算当前位的步进值 (1, 10, 100...)
    uint16_t step = 1;
    for(uint8_t i = 0; i < data->cursor_pos; i++) step *= 10;

    switch (btn) {
        case BTN_LEFT:
            if (data->cursor_pos < data->fixed_digit - 1) data->cursor_pos++;
            break;
        case BTN_RIGHT:
            if (data->cursor_pos > 0) data->cursor_pos--;
            break;
        case BTN_UP:
            if (val + step <= data->max) {
                *(data->val_ptr) = (uint16_t)(val + step);
            } else {
                *(data->val_ptr) = data->max; // 封顶
            }
            break;
        case BTN_DOWN:
            if (val >= (uint32_t)data->min + step) {
                *(data->val_ptr) = (uint16_t)(val - step);
            } else {
                *(data->val_ptr) = data->min; // 兜底
            }
            break;
        case BTN_ENTER:
        case BTN_BACK:
            page_stack_portal_toggle(&g_page_stack, NULL, NULL, 0);
            break;
        default: break;
    }
}

const portal_component_t PORTAL_INT_PRECISE = {
    .draw = portal_int_precise_draw,
    .input = portal_int_precise_input,
    .w = 110,
    .h = 45
};