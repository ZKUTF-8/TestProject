✅ 结论
对于小数据（<10KB）：
双向通讯 → PostMessage 是最优（1-3ms）
JS 调用 C# 同步方法 → HostObject 稍快（<1ms）
但 PostMessage 更灵活，支持双向推送
您的方案完全正确：
✅ 小数据用 PostMessage → 延迟最低、实现最简单
✅ 大数据用 WebSocket → 支持二进制、吞吐量大ni sh