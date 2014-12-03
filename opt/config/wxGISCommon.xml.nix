<?xml version="1.0" encoding="utf-8"?>
<wxGISCommon>
  <splash show="yes" timeout="4000"/>
  <curl proxy="" headers="Accept: */*|Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4|Accept-Encoding: gzip,deflate,sdch|User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/29.0.1547.57 Safari/537.36|Connection: keep-alive" dns_cache_timeout="180" connect_timeout="30" timeout="1000"/>
  <coordinate_mask>
    <mask text="d.dddddd[tab]d.dddddd"/>
    <mask text="dd-mm-ss.ss [W][ ]dd-mm-ss.ss [W]"/>
    <mask text="dd°mm'ss.ss&quot; [W][ ]dd°mm'ss.ss&quot; [W]"/>
    <mask text="dd-mm-ss.ss[ ]dd-mm-ss.ss"/>
    <mask text="ddmmss[ ]ddmmss"/>
    <mask text="d.ddd[tab]d.ddd"/>
    <mask text="d.ddd[ ]d.ddd"/>
  </coordinate_mask>
  <GDAL cachemax="128"/>
  <debug mode="yes"/>
  <libs>
    <lib path="/usr/lib/libwxgisgeoprocessingui.so" name="wxgisgeoprocessingui06"/>
    <lib path="/usr/local/lib/libwxgisgeoprocessingui.so" name="wxgisgeoprocessingui06"/>
    <lib path="/usr/lib/libwxgisgeoprocessing.so" name="wxgisgeoprocessing06"/>
    <lib path="/usr/local/lib/libwxgisgeoprocessing.so" name="wxgisgeoprocessing06"/>
  </libs>
  <taskmngr exe_path="/usr/bin/wxgistaskmanager"/>
</wxGISCommon>
