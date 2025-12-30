#ifndef PCIE_ENCODER_H
#define PCIE_ENCODER_H

#if _WIN32
#if defined(ALD8XXAPI_EXPORTS)
#define ALD8XXAPI_EXPORT  __declspec (dllexport)
#else
#define ALD8XXAPI_EXPORT  __declspec (dllimport)
#endif
#else
#if defined(ALD8XXAPI_EXPORTS)
#define ALD8XXAPI_EXPORT   __attribute__((visibility("default")))
#else
#define ALD8XXAPI_EXPORT   __attribute__((visibility("default")))
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

/**
 * (读取)设置编码器通道选择
 * @param *buffer  数据缓存区
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >=0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int control_encoder_choose(unsigned int *buffer,const int is_read);
/**
 * (读取)设置编码器触发方式
 * @param *buffer  数据缓存区
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >=0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int control_encoder_trigger_type(unsigned int *buffer,const int is_read);


/**
 * (读取)设置编码器计数初始值
 * @param *buffer  数据缓存区
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >=0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int control_encoder_count_start_value(unsigned int *buffer,const int is_read);


/**
 * (读取)设置编码器低限位
 * @param *buffer  数据缓存区
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >=0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int control_encoder_trigger_low_limit(unsigned int *buffer,const int is_read);

/**
 * (读取)设置编码器高限位
 * @param *buffer  数据缓存区
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >=0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int control_encoder_trigger_high_limit(unsigned int *buffer,const int is_read);


/**
 * (读取)设置编码器触发间隔
 * @param *buffer  数据缓存区
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >=0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int control_encoder_trigger_interval(unsigned int *buffer,const int is_read);

/**
 * (读取)设置编码器周向复位值
 * @param *buffer  数据缓存区
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >=0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int control_encoder_circumferential_reset(unsigned int *buffer,const int is_read);

#ifdef __cplusplus
}
#endif
#endif // PCIE_ENCODER_H
