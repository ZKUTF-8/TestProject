#ifndef MODULE_ULTRASONIC_H
#define MODULE_ULTRASONIC_H

#ifdef __cplusplus
extern "C" {
#endif

#if _WIN32
#if defined(MODULE_ULTRASONIC_EXPORTS)
#define MODULE_ULTRASONIC_EXPORT  __declspec (dllexport)
#else
#define MODULE_ULTRASONIC_EXPORT  __declspec (dllimport)
#endif
#else
#if defined(MODULE_ULTRASONIC_EXPORTS)
#define MODULE_ULTRASONIC_EXPORT   __attribute__((visibility("default")))
#else
#define MODULE_ULTRASONIC_EXPORT   __attribute__((visibility("default")))
#endif
#endif



MODULE_ULTRASONIC_EXPORT int module_us_write_strobe_config(const char *buffer,int length);


MODULE_ULTRASONIC_EXPORT int module_us_fir_complex_write(int channel_index, const int *buffer,const int  count);


MODULE_ULTRASONIC_EXPORT int module_us_control_rectification(int channel_index,unsigned int  *buffer,const int is_read);

MODULE_ULTRASONIC_EXPORT int module_us_control_envelope(int channel_index,unsigned int  *buffer,const int is_read);
//对数使能
MODULE_ULTRASONIC_EXPORT int module_us_control_logarithm(int channel_index,unsigned int  *buffer,const int is_read);

MODULE_ULTRASONIC_EXPORT int module_us_control_adc_start(unsigned int  *buffer,const int is_read);


MODULE_ULTRASONIC_EXPORT int module_us_control_adc_end(unsigned int  *buffer,const int is_read);

//闸门版本
MODULE_ULTRASONIC_EXPORT int module_us_get_strobe_version(unsigned int  *buffer);

//上传类型
MODULE_ULTRASONIC_EXPORT int module_us_control_upload_type(unsigned int  *buffer,const int is_read);


#ifdef __cplusplus
}
#endif

#endif // ULTRASONICAPI_H
