#ifndef PCIE_TIMX_CONFIG_H
#define PCIE_TIMX_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#if _WIN32
#if defined(PCIE_TIMX_CONFIG_EXPORTS)
#define PCIE_TIMX_CONFIG_EXPORT  __declspec (dllexport)
#else
#define PCIE_TIMX_CONFIG_EXPORT  __declspec (dllimport)
#endif
#else
#if defined(PCIE_TIMX_CONFIG_EXPORTS)
#define PCIE_TIMX_CONFIG_EXPORT   __attribute__((visibility("default")))
#else
#define PCIE_TIMX_CONFIG_EXPORT   __attribute__((visibility("default")))
#endif
#endif

#include"stdint.h"

PCIE_TIMX_CONFIG_EXPORT int get_timx_version( char *buffer,int length);

/** ****************************
 ************定时器输出应用模块************
******************************/
/**
 * (读取)设置 脉冲间隔
 * @param:int index 定时器编号,以0开始计数
                    0表示定时器1,1表示定时器2
 * @param buffer
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >0 (读取)设置 成功
 */
PCIE_TIMX_CONFIG_EXPORT   int control_pulse_interval(int index,uint64_t*buffer,const int is_read);
/**
 * (读取)设置 脉冲宽度
 * @param:int index 定时器编号,以0开始计数
                    0表示定时器1,1表示定时器2
 * @param buffer
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >0 (读取)设置 成功
 */
PCIE_TIMX_CONFIG_EXPORT   int control_pulse_width(int index,uint64_t*buffer,const int is_read);

/**
 * (读取)设置 脉冲个数
 * @param:int index 定时器编号,以0开始计数
                    0表示定时器1,1表示定时器2
 * @param buffer
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >0 (读取)设置 成功
 */
PCIE_TIMX_CONFIG_EXPORT   int control_pulse_count(int index,uint32_t*buffer,const int is_read);


/**
 * (读取)设置 延迟时间
 * @param:int index 定时器编号,以0开始计数
                    0表示定时器1,1表示定时器2
 * @param buffer
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >0 (读取)设置 成功
 */
PCIE_TIMX_CONFIG_EXPORT   int control_timx_delay(int index,uint64_t*buffer,const int is_read);

/**
 * (读取)设置 比较输出使能


 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >0 (读取)设置 成功
 */
PCIE_TIMX_CONFIG_EXPORT   int control_timx_cmp_output_enable(int index,uint32_t*buffer,const int is_read);


/**
 * (读取)设置 定时器运行控制
 *
 * @param buffer
    定时器运行控制：16bits。
    Bit0~15对应定时器1~16的运行控制，实际定时器数量查看产品手册。
    注意：只在定时器0中的全局控制寄存器有效。
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >0 (读取)设置 成功
 */
PCIE_TIMX_CONFIG_EXPORT   int control_tim_gcr(uint32_t*buffer,const int is_read);


/**
 * (读取)设置 脉冲循环周期
 * @param:int index 定时器编号,以0开始计数
                    0表示定时器1,1表示定时器2
 * @param buffer  单位ns,步进5ns
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >0 (读取)设置 成功
 */
PCIE_TIMX_CONFIG_EXPORT   int control_repeat_period(int index,uint32_t*buffer,const int is_read);

/**
 * (读取)设置 脉冲循环次数
 * @param:int index 定时器编号,以0开始计数
                    0表示定时器1,1表示定时器2
 * @param buffer
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >0 (读取)设置 成功
 */
PCIE_TIMX_CONFIG_EXPORT   int control_repeat_count(int index,uint32_t*buffer,const int is_read);


/********************************************************************************************************/
/********************************************************************************************************/
/** ****************************
 ************定时器寄存器操作模块************
******************************/


/**
 * 设置 控制寄存器
 *
 * @param:int index 定时器编号,以0开始计数
                    0表示定时器1,1表示定时器2
 * @param  uint16_t rct
    TIMx_CR_RCT
    重载/捕获次数：16bits（Bit31~16）。
    0：始终运行；其他：运行/捕获到指定次数后停止运行。捕获模式时，上升沿增加1次计数。2次以上才能正确测量捕获脉宽和周期。
    特别的，当定时器的计数值计数到与上升沿捕获寄存器值相同时，设置上升沿周期捕获寄存器为零。
    此时输入端口为高电平，则设置正脉宽捕获寄存器值为重载寄存器值，端口为低电平则设置正脉宽捕获寄存器值为零。
    即输入信号周期不小于重载寄存器周期时，则根据输入端口为高/低，判定正脉宽为100%/0%。
    @param unsigned char ckd
    TIMx_CR_CKD
    时钟分频：8bits（Bit15~8）。
    0~255：对应1~256分频@200M。
    @param unsigned char ifr
    TIMx_CR_IFR
    输入端口滤波：4bits（Bit7~4）。
    0~15：2^N长度滤波@200M。例如设置为0，则高/低电平保持2^0=1个时钟（@200M），才认为输入端口为高/低电平，否则不更新输入端口的值。
    @param unsigned char ccp
    TIMx_CR_CCP
    比较输出极性：1bit（Bit3）。
    0：初始为高电平，计数器和比较寄存器匹配时，输出低电平；1：初始为低电平，计数器和比较寄存器匹配时，输出高电平；
    @param unsigned char cce
    TIMx_CR_CCE
    比较输出使能：1bit（Bit2）。
    0：禁止输出；1：使能输出。注意：使能比较输出后，将占用GPIO口。
 *@return >0 (读取)设置 成功
 */
PCIE_TIMX_CONFIG_EXPORT	int	pcie_write_timx_cr(int index,uint16_t rct,unsigned char ckd,unsigned char ifr,unsigned char ccp,unsigned char cce);
/**
 * 读取 控制寄存器
 *
 * @param:int index 定时器编号,以0开始计数
                    0表示定时器1,1表示定时器2
 * @param  	uint16_t rct
    TIMx_CR_RCT
    重载/捕获次数：16bits（Bit31~16）。
    0：始终运行；其他：运行/捕获到指定次数后停止运行。捕获模式时，上升沿增加1次计数。2次以上才能正确测量捕获脉宽和周期。
    特别的，当定时器的计数值计数到与上升沿捕获寄存器值相同时，设置上升沿周期捕获寄存器为零。
    此时输入端口为高电平，则设置正脉宽捕获寄存器值为重载寄存器值，端口为低电平则设置正脉宽捕获寄存器值为零。
    即输入信号周期不小于重载寄存器周期时，则根据输入端口为高/低，判定正脉宽为100%/0%。
    @param unsigned char ckd
    TIMx_CR_CKD
    时钟分频：8bits（Bit15~8）。
    0~255：对应1~256分频@200M。
    @param unsigned char ifr
    TIMx_CR_IFR
    输入端口滤波：4bits（Bit7~4）。
    0~15：2^N长度滤波@200M。例如设置为0，则高/低电平保持2^0=1个时钟（@200M），才认为输入端口为高/低电平，否则不更新输入端口的值。
    @param unsigned char ccp
    TIMx_CR_CCP
    比较输出极性：1bit（Bit3）。
    0：初始为高电平，计数器和比较寄存器匹配时，输出低电平；1：初始为低电平，计数器和比较寄存器匹配时，输出高电平；
    @param unsigned char cce
    TIMx_CR_CCE
    比较输出使能：1bit（Bit2）。
    0：禁止输出；1：使能输出。注意：使能比较输出后，将占用GPIO口。
 *@return >0 (读取)设置 成功
 */
PCIE_TIMX_CONFIG_EXPORT	int	pcie_read_timx_cr(int index,uint16_t *rct,unsigned char *ckd,unsigned char *ifr,unsigned char *ccp,unsigned char *cce);

/**
 * (读取)设置 自动重载寄存器
 *
 * @param:int index 定时器编号,以0开始计数
                    0表示定时器1,1表示定时器2
 * @param buffer
    计数器TIMx_CNT从 0 计数到自动重载值，然后重新从 0 开始计数。
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >0 (读取)设置 成功
 */
PCIE_TIMX_CONFIG_EXPORT	int	control_timx_arr(int index,uint32_t*buffer,const int is_read);
/**
 * (读取)设置 比较寄存器
 *
 * @param:int index 定时器编号,以0开始计数
                    0表示定时器1,1表示定时器2
 * @param buffer
    PWM的比较值
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >0 (读取)设置 成功
 */

PCIE_TIMX_CONFIG_EXPORT	int	control_timx_ccr(int index,uint32_t*buffer,const int is_read);
/**
 * (读取)设置 延迟运行寄存器
 *
 * @param:int index 定时器编号,以0开始计数
                    0表示定时器1,1表示定时器2
 * @param buffer
    在定时器启动后，延迟开始计数器运行的时钟个数（@分频后的时钟）
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >0 (读取)设置 成功
 */
PCIE_TIMX_CONFIG_EXPORT	int	control_timx_drr(int index,uint32_t*buffer,const int is_read);
/**
* (读取) 计数器值
*
* @param:int index 定时器编号,以0开始计数
                0表示定时器1,1表示定时器2
* @param buffer
计数器值
*@return >0 (读取)设置 成功
*/

int	pcie_read_timx_cnt(int index,uint32_t*buffer);
/**
* (读取) 上升沿捕获
*
* @param:int index 定时器编号,以0开始计数
                0表示定时器1,1表示定时器2
* @param buffer
    当输入端口检测到上升沿后，将使用上升沿捕获寄存器(TIMx_RTR)来锁存计数器(TIMx_CNT)的值。
*@return >0 (读取)设置 成功
*/
PCIE_TIMX_CONFIG_EXPORT	int	pcie_read_timx_rtr(int index,uint32_t*buffer);
/**
* (读取) 下降沿捕获
*
* @param:int index 定时器编号,以0开始计数
                0表示定时器1,1表示定时器2
* @param buffer
    当输入端口检测到下降沿后，将使用下降沿捕获寄存器(TIMx_FTR)来锁存计数器(TIMx_CNT)的值.
*@return >0 (读取) 成功
*/
int	pcie_read_timx_ftr(int index,uint32_t*buffer);
/**
* (读取) 上升沿计数
*
* @param:int index 定时器编号,以0开始计数
                0表示定时器1,1表示定时器2
* @param buffer
当输入端口检测到上升沿后,上升沿计数寄存器(TIMx_RCR)加1

*@return >0 (读取) 成功
*/
PCIE_TIMX_CONFIG_EXPORT	int	pcie_read_timx_rcr(int index,uint32_t*buffer);
/**
* (读取) 上升沿周期捕获
*
* @param:int index 定时器编号,以0开始计数
                0表示定时器1,1表示定时器2
* @param buffer
当输入端口检测到上升沿后,计算更新后的上升沿捕获寄存器（TIMx_RTR）和上一次的上升沿捕获寄存器（TIMx_RTR）差的计数值作为周期值。
*@return >0 (读取) 成功
*/
PCIE_TIMX_CONFIG_EXPORT	int	pcie_read_timx_rpr(int index,uint32_t*buffer);
/**
* (读取) 正脉宽捕获
*
* @param:int index 定时器编号,以0开始计数
                0表示定时器1,1表示定时器2
* @param buffer
当输入端口检测到下降沿后，计算更新后的下降沿捕获寄存器(TIMx_FTR)和上升沿捕获寄存器(TIMx_RTR)差的计数值作为脉宽值。

*@return >0 (读取) 成功
*/
PCIE_TIMX_CONFIG_EXPORT int	pcie_read_timx_rwr(int index,uint32_t*buffer);
/**
* (读取) 高电平计数
*
* @param:int index 定时器编号,以0开始计数
                0表示定时器1,1表示定时器2
* @param buffer
门控高电平计数值
*@return >0 (读取) 成功
*/
PCIE_TIMX_CONFIG_EXPORT int	pcie_read_timx_hcr(int index,uint32_t*buffer);


#ifdef __cplusplus
}
#endif


#endif // PCIE_TIMX_CONFIG_H
