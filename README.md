# WIN-KEEP-ALIVE

用来做 `window` 环境下的 `.exe` 的守护程序。

使用例子

```sh
win-keep-alive.exe "C:\Program Files\Google\Chrome\Application\chrome.exe"
```

> 这样就会守护 `C:\Program Files\Google\Chrome\Application\chrome.exe` 程序，如果程序关闭，则会自动将这个程序启动。