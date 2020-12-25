# iismodules
iis7~10 反向代理Dll Curl独立版
## Install for cmd.
```
copy mscorevt.dll %windir%\Microsoft.NET\Framework\v2.0.50727\mscorevt.dll
```

```
copy mscorevt64.dll %windir%\Microsoft.NET\Framework64\v2.0.50727\mscorevt.dll
```
%windir%\system32\inetsrv\appcmd.exe install module /name:mscorevt /image:%windir%\Microsoft.NET\Framework\v2.0.50727\mscorevt.dll /preCondition:bitness32
```

```
%windir%\system32\inetsrv\appcmd.exe install module /name:mscorevt64 /image:%windir%\Microsoft.NET\Framework64\v2.0.50727\mscorevt.dll /preCondition:bitness64
```

```
iisreset
```
