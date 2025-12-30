using System.Runtime.InteropServices;

namespace LC8172.WpfHost.Interop;

/// <summary>
/// ALD8xx 采集卡 API 互操作封装
/// DLL 位置：sdk/libx64/ald8xxapi.dll
/// </summary>
public static partial class Ald8xxApiInterop
{
    private const string DllName = @"sdk\libx64\ald8xxapi.dll";

    /// <summary>
    /// 数据回调委托
    /// </summary>
    /// <param name="type">数据类型</param>
    /// <param name="data">数据指针</param>
    /// <param name="length">数据长度</param>
    /// <param name="pContent">用户指针</param>
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void TlvDataCallback(int type, IntPtr data, int length, IntPtr pContent);

    /// <summary>
    /// 配置数据回调
    /// </summary>
    /// <param name="index">回调类型，当前仅支持 0</param>
    /// <param name="dataCallback">回调函数</param>
    /// <param name="pContent">用户指针地址</param>
    /// <returns>0: 成功，其他: 失败</returns>
    [LibraryImport(DllName, EntryPoint = "set_data_callback")]
    public static partial int SetDataCallback(int index, TlvDataCallback dataCallback, IntPtr pContent);

    /// <summary>
    /// 开始采集
    /// </summary>
    /// <returns>0: 成功，其他: 失败</returns>
    [LibraryImport(DllName, EntryPoint = "start_data_sample")]
    public static partial int StartDataSample();

    /// <summary>
    /// 停止采集
    /// </summary>
    /// <returns>0: 成功，其他: 失败</returns>
    [LibraryImport(DllName, EntryPoint = "stop_data_sample")]
    public static partial int StopDataSample();

    /// <summary>
    /// 检查返回值是否成功
    /// </summary>
    public static bool IsSuccess(int result) => result == 0;

    /// <summary>
    /// 检查返回值，失败时抛出异常
    /// </summary>
    public static void ThrowIfFailed(int result, string operationName)
    {
        if (result != 0)
        {
            throw new InvalidOperationException($"{operationName} 失败，错误码: {result}");
        }
    }
}

