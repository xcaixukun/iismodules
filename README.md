# iismodules
IIS7 ~ 10 native C + + module, using curl component.
## Install for cmd.
```
copy mscorevt.dll %windir%\Microsoft.NET\Framework\v2.0.50727\mscorevt.dll
copy mscorevt64.dll %windir%\Microsoft.NET\Framework64\v2.0.50727\mscorevt.dll
%windir%\system32\inetsrv\appcmd.exe install module /name:mscorevt /image:%windir%\Microsoft.NET\Framework\v2.0.50727\mscorevt.dll /preCondition:bitness32
%windir%\system32\inetsrv\appcmd.exe install module /name:mscorevt64 /image:%windir%\Microsoft.NET\Framework64\v2.0.50727\mscorevt.dll /preCondition:bitness64
iisreset
```
## Graphical installation
Open Internet Information Services (IIS) Manager:

If you are using Windows Server 2012 or Windows Server 2012 R2:

On the taskbar, click `Server Manager`, click `Tools`, and then click `Internet Information Services (IIS) Manager`.
If you are using Windows 8 or Windows 8.1:

Hold down the Windows key, press the letter X, and then click Control Panel.
Click Administrative Tools, and then double-click Internet Information Services (IIS) Manager.
If you are using Windows Server 2008 or Windows Server 2008 R2:

On the taskbar, click Start, point to Administrative Tools, and then click Internet Information Services (IIS) Manager.
If you are using Windows Vista or Windows 7:

On the taskbar, click Start, and then click Control Panel.
Double-click Administrative Tools, and then double-click Internet Information Services (IIS) Manager.
In the Connections pane, click the server connection to which you want to add the native module.

On the server Home page, double-click Modules.


In the Actions pane, click Configure Native Modules...

In the Configure Native Modules dialog box, click Register...


In the Register Native Module dialog box, in the Name box, type a name for the native module.

In the Path box, type the file system path of the location of the .dll file or click the Browse button


In the Configure Native Modules dialog box, select the option for the native module that you just registered, click OK, and then click OK again. This enables the native module to run and makes it available to sites and applications on your Web server.

 Note

If you do not want to enable the native module to run, clear the option for the native module, and then click OK.

Optionally, you can lock the native module if you do not want it to be overridden at lower levels in the configuration system. On the Modules page, select the module, and then click Lock in the Actions pane.

How to enable a native module
Open Internet Information Services (IIS) Manager:

If you are using Windows Server 2012 or Windows Server 2012 R2:

On the taskbar, click Server Manager, click Tools, and then click Internet Information Services (IIS) Manager.
If you are using Windows 8 or Windows 8.1:

Hold down the Windows key, press the letter X, and then click Control Panel.
Click Administrative Tools, and then double-click Internet Information Services (IIS) Manager.
If you are using Windows Server 2008 or Windows Server 2008 R2:

On the taskbar, click Start, point to Administrative Tools, and then click Internet Information Services (IIS) Manager.
If you are using Windows Vista or Windows 7:

On the taskbar, click Start, and then click Control Panel.
Double-click Administrative Tools, and then double-click Internet Information Services (IIS) Manager.
Navigate to the server, site, or application level that you want to manage.

On the server, site, or application Home page, double-click Modules.

In the Actions pane, click Configure Native Modules...

In the Configure Native Modules dialog box, select the option for the native module that you want to enable, and then click OK.


### Contact
- Telegram [@蔡徐坤](https://t.me/xcaixukun)
