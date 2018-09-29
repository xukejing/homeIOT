# homeIOT

家庭物联网例子

演示了客户端（esp8266，micropython）通过ADC采集光敏电阻的分压，通过UDP发送给服务端

服务端（linux主机）开启两个线程，一个线程解析来自客户端的UDP数据包，另一个线程与MySQL数据库交互
