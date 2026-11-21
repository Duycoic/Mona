#include "KEY.h"

static Key_State_t Key_State = KEY_STATE_IDLE;

void KEY_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef KEY_Init = {0};
	
	KEY_Init.Pin  = KEY0_PIN | KEY1_PIN | KEY2_PIN ;
    KEY_Init.Mode = GPIO_MODE_INPUT;
    KEY_Init.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY0_PORT, &KEY_Init);
	
    KEY_Init.Pin  = WKUP_PIN;
    KEY_Init.Mode = GPIO_MODE_INPUT;
    KEY_Init.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(WKUP_PORT, &KEY_Init);
}

/* 读取当前按下的物理按键并映射为逻辑按键码 */
static Key_Code_t Key_Read(void)
{
    if (HAL_GPIO_ReadPin(KEY0_PORT, KEY0_PIN) == KEY_PRESSED)
        return KEY_RIGHT;
    if (HAL_GPIO_ReadPin(KEY1_PORT, KEY1_PIN) == KEY_PRESSED)
        return KEY_DOWN;
    if (HAL_GPIO_ReadPin(KEY2_PORT, KEY2_PIN) == KEY_PRESSED)
        return KEY_LEFT;
    if (HAL_GPIO_ReadPin(WKUP_PORT, WKUP_PIN) == 1)
        return KEY_UP;

    return KEY_NONE;
}

/*
 * 按键扫描状态机：
 * 通过状态迁移完成消抖，避免阻塞 FreeRTOS 调度。
 * 返回 KEY_NONE 表示本次无新按键。
 */
Key_Code_t Key_Scan(void)
{
    Key_Code_t raw_key;
    static Key_Code_t Latch_Key = KEY_NONE;
    Key_Code_t ret = KEY_NONE;

    raw_key = Key_Read();

    switch (Key_State)
    {
        case KEY_STATE_IDLE:
            Latch_Key = KEY_NONE;
            if (raw_key != KEY_NONE)
            {
                Key_State = KEY_STATE_DEBOUNCE;
            }
            break;

        case KEY_STATE_DEBOUNCE:
            raw_key = Key_Read();
            if (raw_key != KEY_NONE)
            {
                Latch_Key = raw_key;
                Key_State = KEY_STATE_WAIT_RELEASE;
            }
            else
            {
                Key_State = KEY_STATE_IDLE;
            }
            break;

        case KEY_STATE_WAIT_RELEASE:
            raw_key = Key_Read();
            if (raw_key == KEY_NONE)
            {
                Key_State = KEY_STATE_IDLE;
            }
            break;

        default:
            Key_State = KEY_STATE_IDLE;
            Latch_Key = KEY_NONE;
            break;
    }

    /* 锁存后立即清除，避免同一按键被重复上报 */
    ret = Latch_Key;
    if (Latch_Key != KEY_NONE)
    {
        Latch_Key = KEY_NONE;
    }

    return ret;
}
