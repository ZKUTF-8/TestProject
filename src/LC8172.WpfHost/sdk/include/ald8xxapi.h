#ifndef ALD8XXAPI_H
#define ALD8XXAPI_H
#ifdef __cplusplus
extern "C" {
#endif

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



/**
* @brief: get_api_version API接口版本读取,
* @param:char *buffer API版本内容数据区
* @param:int length 数据区的长度,块数需要大于6
* @retval: int >0 表示成功，<=0 表示失败
 */
ALD8XXAPI_EXPORT int get_api_version( char *buffer,int length);



/**
  * 打开设备
  *
  * @return 0：打开成功 其他:打开失败
  */
ALD8XXAPI_EXPORT int  device_pcie_init();
/**
  * 关闭设备
  *
  *@return 0：关闭成功
  */
ALD8XXAPI_EXPORT int  device_pcie_deinit();
/** ************************************************************************************
 ADC模块
*****************************************************************************************/
typedef void (*pcie_recv_callback)(  void* data, int length,const void *p_content);
/** ************************************************************************************
  ADC模块  ADC数据区定义
 *****************************************************************************************/
/**
  * @brief: expand_recv_callback  扩展类型数据回调定义,
  * @param:int type 数据类型编码
   * @param:char *expand 扩展数据缓存区
  * @param:int expand_len 数据区的长度，128字节

  * @param:char *adc_data 数据缓存区
     当前系列有4通道和2通道版本，
     2通道版本数据排列方式：CH1，CH2，CH1，CH2，...
     4通道版本数据排列方式：
     CH1，CH2，CH3，CH4，
     CH1，CH2，CH3，CH4，...

  * @param:int adc_len 数据区的长度，
  * @param:const void *p_content 用户自定义指针
  * @retval:
  */
typedef void(*expand_recv_callback)(int type, void *expand,int expand_len,void* adc_data, int adc_len, const void *p_content);

/**
  * ADC最大采样率,最大采样率不超过4G时使用有效
  *
  * @return 0：
  */

ALD8XXAPI_EXPORT unsigned int  get_sampling_rate();

/**
 * (读取)设置 ADC采样率,最大采样率不超过4G时使用有效
 *
 * @param buffer 采样率，单位Hz ,最大不能超过最大采样率,或4G,例如，125MHz ,*buffer=125000000
 * ADC采样率支持的频率为最大采样率的整数倍抽取
     例如：最大采样率为125000000（125M）
    此接口支持的数据可以是 (125000000)125M/1、62500000(125M/2)、41666666(125M/3)....
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 *        =-120 接口选择错误，当前设备使用control_high_sampling_frequency接口进行读取配置
 */
ALD8XXAPI_EXPORT int  control_sampling_frequency(unsigned int  *buffer,const int is_read);
/**
  * ADC最大采样率,采样率
  *
  * @return 有效值
  */
ALD8XXAPI_EXPORT unsigned long long  get_sampling_rate_max();
/**
 * (读取)设置 ADC采样率 control_high_sampling_frequency
 *
 * @param buffer 采样率，单位Hz ,最大不能超过最大采样率,例如，125MHz ,*buffer=125000000
 * ADC采样率支持的频率为最大采样率的整数倍抽取
     例如：最大采样率为125000000（125M）
    此接口支持的数据可以是 (125000000)125M/1、62500000(125M/2)、41666666(125M/3)....
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_high_sampling_frequency(void  *buffer,int size,const int is_read);

/**
 * (读取)设置 采样分频
 *
 * @param buffer
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int control_frequency_division(unsigned int *buffer, const int  is_read);


/**
  * (读取)设置 采集使能 buffer=0:停止采集,buffer=1:启动采集 启动或者停止设备的数据上传
  *
  * @param buffer 采集使能
  * @param is_read is_read=true时为读取,is_read=false时为设置
  *@return >0 (读取)设置 成功
  */
ALD8XXAPI_EXPORT int  control_collect_enable(unsigned int  *buffer,int is_read);
/**
 * (读取)设置 采集模式 buffer=0:触发采集,buffer=1:连续采集
 *
 * @param buffer 采集模式
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_collect_model(unsigned int  *buffer,const int is_read);
/**
 * (读取)设置 时钟源
 *
 * @param buffer 时钟源
 *  0:内部时钟
    1:外部时钟(使用外部时钟10M输入内部PLL)
    2:外部直采(使用外部时钟直接采集)
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int control_collect_clock_source(unsigned int  *buffer,const int is_read);
/**
 * (读取)设置 触发源 buffer=0:软件触发,buffer=1:通道触发,buffer=2:外触发
 *
 * @param buffer 触发源
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_trigger_source(unsigned int  *buffer,const int is_read);
/**
 * (读取)设置 触发边沿 buffer=0:上升沿,buffer=1:下降沿,buffer=2:双边沿
 *
 * @param buffer 触发边沿
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_trigger_clock_edge(unsigned int  *buffer,const int is_read);
/**
 * (读取)设置 软件触发 buffer=0:软件不使能,buffer=1:软件使能
 *
 * @param buffer 软件触发
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_software_trigger(unsigned int  *buffer,const int is_read);
/**
 * (读取)设置 触发延迟(pts)
 *
 * @param buffer 触发延迟
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_remot_trig_delay(unsigned int  *buffer,const int is_read);
/**
 * (读取)设置 预触发长度(pts)
 *
 * @param buffer 预触发长度
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_pretrigger_length(unsigned int  *buffer,const int is_read);
/**
 * (读取)设置 触发长度(pts)
 *
 * @param buffer 触发长度
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_trigger_length(unsigned int  *buffer,const int is_read);

/**
 * (读取)设置 触发使能 buffer=0:禁用,buffer=1:使能
 *
 * @param buffer 触发使能
 * @param channel_num 通道号为（1~8）有效
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_trigger_enable(unsigned int  *buffer,int channel_num,const int is_read);

/**
 * (读取)设置 触发阈值（mV）
 *
 * @param buffer 触发阈值（mV）
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_trigger_threshold( int  *buffer,int channel_num,int bits,const int is_read);
/**
 * (读取)设置 通道量程 buffer=10000:±10V,buffer=5000:±5V,buffer=2000:±2V,buffer=1000:±1V,buffer=500:±0.5V,buffer=200:±0.2V,buffer=100:±0.1V
 * 其他量程按以上规则mV单位取值
 * @param buffer 通道量程
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_channel_range(unsigned int  *buffer,int channel_num,const int is_read);
/**
 * (读取)设置 通道量程特征参数版，
 * @param buffer 通道量程索引，具体索引参数含义参照产品特性
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_property_channel_range(unsigned int  *buffer,int channel_index,const int is_read);
/**
 * (读取)设置 触发阈值（mV）特征参数版
 *
 * @param buffer 触发阈值（mV）
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_property_trigger_threshold( int  *buffer,int channel_index,int bits,const int is_read);

/**
 * (读取)设置 输入阻抗 buffer=0:1MΩ,buffer=1:50Ω
 *
 * @param buffer 输入阻抗
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_input_impedance(unsigned int  *buffer,int channel_num,const int is_read);
/**
 * (读取)设置 耦合方式 buffer=0:AC,buffer=1:DC
 *
 * @param buffer 耦合方式
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_coupling_mode(unsigned int  *buffer,int channel_num,const int is_read);
/**
 * (读取)设置 触发电平
 *buffer= 1520:TTL
 *buffer=1340:LVTTL
 *buffer=1000:模拟触发
 *buffer=其他：无效
 *
 * @param buffer 触发电平
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int control_triggering_level(unsigned int  *buffer,const int is_read);

/**
 * (读取)设置 外部触发阈值
 * @param buffer 阈值，单位mV,范围 (-5000mV,5000mV)
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int control_external_trigger_threshold( int  *buffer,const int is_read);

/**
 * (读取)设置 高低翻转 buffer=0:禁用,buffer=1:启用
 *
 * @param buffer 高低翻转
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int control_trigger_output_polarity(unsigned int  *buffer,const int is_read);
/**
 * (读取)设置 通道触发输出 buffer=
                    1：脉冲计数;
                    3：触发输出
                    9: 时钟输出
 *
 * @param buffer 通道触发输出
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_trigger_output_channel(unsigned int  *buffer,const int is_read);
/**
 * (读取)设置触发输入通道 buffer=0:禁用,buffer=1:模拟触发输入，2,数字触发输入
 *
 * @param buffer 触发输入通道
 * @param is_read is_read=true时为读取,is_read=false时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int control_trigger_input_channel(unsigned int  *buffer, const int  is_read);
/**
 * (读取)设置adc采集使能高性能模式,API 默认高性能模式为关闭状态
 * @param *buffer  数据缓存区，1表示使能高性能模式，其他表示除能
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >=0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int control_adc_high_performance_enable( char  *buffer, const int  is_read);

/**
 * 控制编码器全部复位
 *@return >=0 操作成功 成功
 */
ALD8XXAPI_EXPORT int control_encoder_reset();

/**
 * 控制指定通道编码器复位
 * @param value 每1个Bit控制一个通道
 *              Bit0：编码器通道1 1:表示复位操作，0：无操作
 *              Bit1：编码器通道2 1:表示复位操作，0：无操作
 *              Bit2：编码器通道3 1:表示复位操作，0：无操作
 *              Bits:依次类推
 *
 *@return >=0 操作成功 成功
 */
ALD8XXAPI_EXPORT int control_encoder_channel_reset(unsigned int value);


/** 触发事件上传使能
 * (读取)设置  buffer=0:使能,buffer=1:不上传，其他无效
 * @param buffer 耦合方式
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_trigger_event_enable(unsigned int  *buffer,const int is_read);
/**
 * PPS秒计数
*/
ALD8XXAPI_EXPORT int control_pps_seconds_count(unsigned int  *buffer, const int  is_read);
/**
 * 读取pps计数
*/
ALD8XXAPI_EXPORT int read_pps_count(unsigned int  *seconds_count, unsigned int  * clock_count);
/**
 * (读取)设置聚合个数
 * @param *buffer  数据缓存区，
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >=0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_polymerization_number(unsigned int* buffer, const int is_read);

/** control_upload_channel_num
 * 上传通道数量
 *
 *@return >0
 */
ALD8XXAPI_EXPORT int control_upload_channel_num( void *buffer,const int  is_read);
/** control_upload_channel_mapped
 * 上传通道映射
 *
 *@return >0
 */
ALD8XXAPI_EXPORT int control_upload_channel_mapped( unsigned int *buffer,int length,const int  is_read);


/** control_peripheral_clock (读取)设置外设时钟源
 * @param *buffer  数据缓存区， 0或0x0F 系统时钟，表示外设时钟源有系统时钟倍频到200MHz使用
 *                             1: ADC时钟，表示外设时钟源有ADC时钟源倍频到200MHz使用
 *                              其他：暂不支持
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >=0 (读取)设置 成功
 *@return >0
 */
ALD8XXAPI_EXPORT int control_peripheral_clock( unsigned int *buffer,const int  is_read);


/** **************************************回调模式*************************/
/**
 * 绑定数据回调函数并触发开始采集 开启线程接收解析数据
 *
 * @param _pcie_recv_callback  回调函数的地址
 *@return >0 绑定成功
 */
ALD8XXAPI_EXPORT int  start_sample( pcie_recv_callback _pcie_recv_callback,const void *p_content);
/**
 * 停止采集 停止设备上传数据和停止线程接收解析数据
 *
 *@return >0 停止采集成功
 */
ALD8XXAPI_EXPORT int   stop_sample();


/**
 * 绑定数据回调函数并触发开始采集 开启线程接收解析数据
 *
 * @param expand_recv_callback  回调函数的地址，设置callback_type类型为0的函数地址
 *@return >0 绑定成功
 */
ALD8XXAPI_EXPORT int  expand_recv_start_sample( expand_recv_callback _callback,const void *p_content);


/**
 * 停止采集 停止设备上传数据和停止线程接收解析数据
 *
 *@return >0 停止采集成功
 */
ALD8XXAPI_EXPORT int   expand_recv_stop_sample();




/** ********************************按类型回调数据*******************************************/
typedef void (*tlv_data_callback)(int type, void* data, int length, const void* p_content);
/**
 *配置回调
 * @param  index 回调类型，当前仅支持0,其他无效
 * *其他：无效
 * @param expand_recv_callback  回调函数的地址
 * @param p_content 用户指针地址
 *@return int 0:表示成功，其他：表示失败
 */
ALD8XXAPI_EXPORT int set_data_callback(int index, tlv_data_callback data_callback, const void* p_content);
/**
 *start_data_sample 开始采集
 *@return int 0:表示成功，其他：表示失败
 */
ALD8XXAPI_EXPORT int start_data_sample();
/**
 *stop_data_sample 停止采集
 *@return int 0:表示成功，其他：表示失败
 */
ALD8XXAPI_EXPORT int stop_data_sample();
/**
 * 设置API层线程的CPU亲缘性、优先级
 * @param int index
 * 0:表示接收线程 ，需要设置为最高优先级，
 * 1：表示处理线程，
 * 其他：暂不支持
 * @param int mask_id
 * windows 场景
 * CPU 掩码编号,从0开始编号，按位计算
 * 示例① 配置 CPU 5
 * mask_id= (1 << 5); // CPU 5
 * 示例② 配置 CPU 0 和 CPU 2
 * mask_id= (1 << 0) | (1 << 2); // CPU 0 和 CPU 2
 * linux 场景
 * int mask_id 表示 CPU 编号
 * @param int policy_id  示例：THREAD_PRIORITY_HIGHEST 等
 *@return =0 设置 成功 其他失败
 *
 */
ALD8XXAPI_EXPORT int recv_thread_set_cpu(int index,int mask_id,int policy_id);


/**=============回调版本==============**/
/** **************************************持续采集 RingBuffer*************************/
/**
 * @brief get_ringbuffer_recommend_size 获取产品推荐用户使用最小的缓存大小
 * 前提条件：采集相关参数已配置，例：采集模式，预触发长度、触发长度、通道选择等
 * @param:int trigger_count 触发次数，触发模式下有效,触发模式下：>0有效
 *@return long long  >0:缓存大小，单位字节
 * <0:推荐长度失败
 */
ALD8XXAPI_EXPORT long long get_ringbuffer_recommend_size(unsigned int trigger_count);
/**
 * @brief read_ringbuffer_start 开始读取
 * @param:void*buffer 数据缓存区指针
 * @param:int cache_size 申请缓存大小
 * @param:const void*sync_object_name 完成同步对象名
 * @param:int write_mode  写方式
 * 0：表示RingBuffer覆盖写
 * 1：表示写偏移地址等于用户读偏移地址时，ringbuffer停止写数据，
 *@return >0 表示开始成功，其他：表示失败
 */
ALD8XXAPI_EXPORT int  read_ringbuffer_start(void*buffer, long long cache_size,const char*sync_object_name,int write_mode);
/**
 * @brief read_ringbuffer_offset  读取RingBuffer写偏移地址
 * @return 写偏移地址
 */
ALD8XXAPI_EXPORT long long  read_ringbuffer_woffset();
/**
 * @brief  update_ringbuffer_user_offset 更新用户读偏移地址
 *  @param:long long roffset 用户读偏移地址
 *@return =0 表示成功，其他：表示失败
 */
ALD8XXAPI_EXPORT int  update_ringbuffer_user_roffset(long long roffset);
/**
 * @brief ringbuffer_total 读取RingBuffer累计总量
 *@return 读取API持续采集RingBuffer累计存储字节数
 */
ALD8XXAPI_EXPORT long long ringbuffer_total();
/**
 * @brief ringbuffer_status 读取RingBuffer状态
 *@return  1:表示RingBuffer满状态，已停止写入数据，
 *          0：正常
 */
ALD8XXAPI_EXPORT int ringbuffer_status();
/**
 * @brief read_ringbuffer_stop  停止读取
 */
ALD8XXAPI_EXPORT int  read_ringbuffer_stop();
/** **************************************持续采集 RingBuffer*************************/


/**
 * single_start_sample 单次采集，采集指定数据长度的数据
 *
 * @param  char* buffer 数据缓存区
 * @param int length 数据缓存大小,    连续采集时，长度不能低于1019*4*2，
 *                                    触发采集时，长度不能需要大于等于触发长度+预触发长度
 * @param  timeout 读取数据等待超时时间
 *@return >0返回采集读取的字节数
 */
ALD8XXAPI_EXPORT int   single_start_sample( short*buffer, int length, int timeout);

/**
 * @brief pcie_start_read_adc_data开始读取ADC数据
 * @param:int api_length 申请缓存大小
 *@return 0 表示开始成功，其他：表示失败
 */
ALD8XXAPI_EXPORT int   pcie_start_read_adc_data(unsigned int api_length);
/**
 * @brief pcie_read_adc_data 读取adc数据
 * @param:void*buffer 数据缓存区
 * @param:int length 缓存大小，单位字节数
 *@return >0 表示读取数据字节数，其他读取失败
 */
ALD8XXAPI_EXPORT unsigned int    pcie_read_adc_data( void*buffer,unsigned int length, int timeout);
/**
 * @brief pcie_stop_read_adc_data 结束ADC读取数据
 *@return 0 表示开始成功，其他：表示失败
 */
ALD8XXAPI_EXPORT int   pcie_stop_read_adc_data();

/**
 * @brief expand_read_start启动读取携带扩展信息数
 * @param:int cache_size 申请缓存大小
 *@return >0 表示开始成功，其他：表示失败
 */
ALD8XXAPI_EXPORT int  expand_read_start(long long cache_size);
/**
 * @brief expand_read_data 持续采集方法下读取数据
 * @param:void*buffer 数据缓存区指针
 * @param:  int length 数据缓存数组个数
 *@return 采集字节数
 */
ALD8XXAPI_EXPORT int  expand_read_data(void* buffer,  int length, int timeout);


/**
 * @brief api_transfer_status  API数据传输状态
 * @param:void*buffer 数据缓存区指针
    ***typedef struct _transfer_status_t
    ***{
    ***    int status_length;***状态结构数据长度 ***
    ***    long long all_bytes;***缓存空间总量***
    ***    long long product_bytes;***产生量***
    ***    long long  consumer_bytes;***用量***
    ***    long long lost_count;***丢失统计，block 模式单位为字节，回调模式为包号***
    ***    long long product_speed;***生产速率,单位B/s***
    ***    long long consumer_speed;***消费速率,单位B/s***
    ***
    ***    long long product_count;***生产计数***
    ***    long long consumer_count;***消费计数***
    ***
    ***
    ***    long long software_dma_bytes;***软件缓存未使用空间***
    ***    long long hardware_dma_bytes;***硬件缓存未使用空间***
    ***    int overflow_flag;***溢出标志***
    ***
    ***} transfer_status_t; **** 状态***
 * @param:  int length 数据缓存数据个数
 * @return 采集字节数
*/
ALD8XXAPI_EXPORT int  api_transfer_status(void* buffer,  int length);


/**
 * @brief expand_read_stop 结束持续采集ADC数据
 *@return >=0 成功，其他失败
 */
ALD8XXAPI_EXPORT int  expand_read_stop();

/**
 * (读取)设置触发输出宽度
 * @param *buffer  数据缓存区，单位ns
 * @param is_read is_read=1时为读取,is_read=0时为设置
 *@return >=0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int  control_trigger_output_pulse_width(unsigned long long *buffer,const int is_read);



/** ***************************************************************************************
*** DAC
********************************************************************************************/
/** DAC输出模式 **/
ALD8XXAPI_EXPORT int dac_output_mode(unsigned int *buffer,const int is_read);
/** DDS参数**/ /** 频率 **/
ALD8XXAPI_EXPORT int dac_dds_frequency_para(int index,unsigned long long *buffer,int length,const int is_read);
/** DDS参数**/ /** 相位，幅值，偏移，波形 **/
ALD8XXAPI_EXPORT int dac_dds_para(int index,unsigned int *buffer,int length,const int is_read);
/** AWG运行方式*
 * bit0: 0，实时模式; 1，缓存模式
 * bit1: 0，连续回放模式; 1，触发回放
 * bit3:2:0,DAC 软件触发，1,ADC 同步触发
*/
ALD8XXAPI_EXPORT int dac_awg_para(unsigned int *buffer,int length,const int is_read);

/** DAC使能 **/

ALD8XXAPI_EXPORT int dac_output_ctrl(unsigned int *buffer,int length,const int is_read);

/** AWG 实时模式准备写数据 **/
ALD8XXAPI_EXPORT int dac_dwg_realtime_start_prepare();

/** AWG 实时模式输出开始播放 **/
ALD8XXAPI_EXPORT int dac_dwg_realtime_start();

/** AWG 实时模式传输数据 **/
/**
 * @brief dac_dwg_realtime_write 实时模式传输数据
 * @param:void*buffer 数据缓存区指针
 * @param:  int length 数据缓存数组个数
 * @param:  int time_out 超时时间，单位ms,当设备空间不足时，上位机请求超时时间
 *          <=0 一直等待设备空间
 *          >0:超时时间
 *@return 写入字节数
 */
ALD8XXAPI_EXPORT int dac_dwg_realtime_write(const void*buffer,int length,int time_out);


/** AWG 实时模式输出结束 **/
ALD8XXAPI_EXPORT int dac_dwg_realtime_stop();


/** AWG 缓存模式准备写数据 **/
ALD8XXAPI_EXPORT int dac_dwg_cache_mode_start_prepare();

/** AWG 缓存模式写数据 **/
/**
 * @brief dac_dwg_cache_mode_write 缓存模式写数据
 * @param:void*buffer 数据缓存区指针
 * @param:  int length 数据缓存数组个数
 * @param:  int time_out 超时时间，单位ms,当设备空间不足时，上位机请求超时时间
 *          <=0 一直等待设备空间
 *          >0:超时时间
 *@return 采集字节数
 */
ALD8XXAPI_EXPORT int dac_dwg_cache_mode_write(const void*buffer,int length,int time_out);


/** AWG 缓存模式准备完成，控制设备输出 开始播放**/
ALD8XXAPI_EXPORT int dac_dwg_cache_mode_complete();

/** AWG 缓存模式停止播放 **/
ALD8XXAPI_EXPORT int dac_dwg_cache_mode_stop();


/** AWG 缓存模式 缓存数据清空 **/
ALD8XXAPI_EXPORT int dac_dwg_cache_mode_cache_clear();


/** *缓存模式数据输出*/
ALD8XXAPI_EXPORT int dac_dwg_cache_mode(const void*buffer,int length);

/** DAC 当前数据速率**/
ALD8XXAPI_EXPORT int dac_awg_frequency_para(unsigned int *buffer,int length,const int is_read);
/** dac_get_interpolation_coefficient_range DAC  插值系数范围
 * @param: int device  设备索引编号
 * @param: *max_value 最大插值系数
 * @param: *min_value 	最小插值系数
 *@return >0
 */
ALD8XXAPI_EXPORT int dac_get_interpolation_coefficient_range(unsigned int*max_value,unsigned int*min_value);

/**
 * (读取)设置 DAC插值系数
 * @param: int device  设备索引编号
 * @param buffer缓存区  插值系数 ，
 * 范围：(最小插值系数, 最大插值系数)
 * int length 指针buffer缓存区长度,单位：字节
 * @param is_read is_read=1 时为读取,is_read=0 时为设置
 *@return >0 (读取)设置 成功
 */
ALD8XXAPI_EXPORT int dac_freq_coefficient(unsigned int*buffer,int length,const int is_read);

/** DAC 量程**/
ALD8XXAPI_EXPORT int dac_range_read( unsigned int*buffer,int length);


/** DAC 触发延迟**/
ALD8XXAPI_EXPORT int dac_trigger_delay( unsigned int*buffer,int length,const int is_read);

/** DAC 回放次数**/
ALD8XXAPI_EXPORT int dac_replay_count( unsigned int*buffer,int length,const int is_read);

/** DAC 软件触发**/
ALD8XXAPI_EXPORT int dac_software_trigger();


/** 内部触发周期**/
ALD8XXAPI_EXPORT int control_internal_trigger_period(unsigned int*buffer,const int is_read);

/** DAC芯片采样率**/
ALD8XXAPI_EXPORT int dac_chip_frequency_read(unsigned int *buffer,int length);

/** DAC 最大数据速率**/
ALD8XXAPI_EXPORT int dac_max_data_rate_read(unsigned int *buffer,int length);

/** *获取传感器ADC和DAC数据属性配置**/

ALD8XXAPI_EXPORT int get_device_attribute( void *buffer,int length);

/** ***************************************************************************************
*** 出厂模块
********************************************************************************************/

/** =============出厂使用,不输出==============**/
/** 参数保存*/
ALD8XXAPI_EXPORT int   set_parameter_save();

/** 外触发DAC标定值*/
ALD8XXAPI_EXPORT int   set_dac_calibration(const unsigned int  value);
ALD8XXAPI_EXPORT int   get_dac_calibration(unsigned int  *buffer);

/** 数据模式*/
ALD8XXAPI_EXPORT int   set_data_model(const unsigned int  data_model);
ALD8XXAPI_EXPORT int   get_data_model(unsigned int  *data_model);

/** 触发输入最小宽度*/
ALD8XXAPI_EXPORT int   set_trigger_minimun_input_width(const unsigned int  value);
ALD8XXAPI_EXPORT int   get_trigger_minimun_input_width(unsigned int  *buffer);

/** 触发输出最小宽度 单位ns*/
ALD8XXAPI_EXPORT int   set_trigger_minimun_output_width(const unsigned int  value);
ALD8XXAPI_EXPORT int   get_trigger_minimun_output_width(unsigned int  *buffer);

/** **通道标定K,B值*/

ALD8XXAPI_EXPORT int   write_channel_calibration(int base_address,short*k_value,short*b_value,int size);
ALD8XXAPI_EXPORT int   read_channel_calibration(int base_address,short*k_value,short*b_value,int size);

ALD8XXAPI_EXPORT int control_device_model(void *buffer,int length,const int is_read);

/** =============出厂使用,不输出==============**/

ALD8XXAPI_EXPORT int pcie_registers_read(long base_address, unsigned int *buffer,int length);


ALD8XXAPI_EXPORT int pcie_registers_write(long base_address,const unsigned int *buffer,int length);
/**
*单个寄存器按写位操作
*/
ALD8XXAPI_EXPORT int pcie_register_bits_write(int reg, int pos, int bits,unsigned int value);

ALD8XXAPI_EXPORT int pcie_registers_complex_write_width_id(long address, int id,const int *data, int count, int end_flag);

#ifdef __cplusplus
}
#endif
#endif /** ALD81XAPI_H*/
