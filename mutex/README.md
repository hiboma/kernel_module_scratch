# mutex

## API

 * DEFINE_MUTEX
 * mutex_lock
 * mutex_unlock

TASK_UNINTERRUPTIBLE

``` 
vagrant   1228  0.0  0.2  98276  1748 ?        S    16:11   0:00  |   \_ sshd: vagrant@pts/0
vagrant   1229  0.0  0.3  13448  1996 pts/0    Ds+  16:11   0:00  |       \_ -bash
```

```
[vagrant@vagrant-centos65 ~]$ cat /proc/1229/stack 
[<ffffffffa003201d>] mutex_write+0x1d/0x30 [mutex]
[<ffffffff81188f78>] vfs_write+0xb8/0x1a0
[<ffffffff81189871>] sys_write+0x51/0x90
[<ffffffff8100b072>] system_call_fastpath+0x16/0x1b
[<ffffffffffffffff>] 0xffffffffffffffff
```