#ifndef __PORTAL_COMPONENT_H__
#define __PORTAL_COMPONENT_H__

// ==========  头部文件 ==========
#include "u8g2.h"
#include "page_stack.h"
#include "screen.h"
#include "ui_toolkit.h"

// ========== 为了Portal组件的通用性，创建组件时需要定义该组件的上下文结构  ==========
// ========== 因为定义了portal_ctx_buffer用于存放上下文，因此可以使用匿名结构体的方式定义  ==========
// 消息框所需数据
typedef struct {
    const char *title;
    const char *msg;
} portal_ctx_message_box_t;

// 数字选择器所需数据
typedef struct {
    const char *title;			
    float *val_ptr;			// 对于需要变化的值，传入它的指针
    float min;
    float max;
    float step;
} portal_ctx_num_t;

typedef struct {
    const char *title;
    uint16_t *val_ptr;
    uint16_t min;
    uint16_t max;
    uint8_t fixed_digit; // 固定显示位数 (1-5)
    uint8_t cursor_pos;  // 内部状态：0是个位, 1是十位...
} portal_ctx_int_precise_t;

// ========== 外部定义  ==========
extern const portal_component_t PORTAL_MESSAGE_BOX;
extern const portal_component_t PORTAL_NUM;
extern const portal_component_t PORTAL_INT_PRECISE;

#endif