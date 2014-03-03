# dump_stack

 * Call Trace を出してくれる
 * カーネルパニックを起こす訳ではない
 * デバッグに使っても便利かな?

```
vagrant-centos65.vagrantup.com login: Pid: 4446, comm: insmod Tainted: G           --------------- H  2.6.32-431.el6.x86_64 #1
Call Trace:
 [<ffffffff810a17f5>] ? __blocking_notifier_call_chain+0x65/0x80
 [<ffffffffa0038009>] ? dump_stack_init+0x9/0xd [dump_stack]
 [<ffffffff8100204c>] ? do_one_initcall+0x3c/0x1d0
 [<ffffffff810bc531>] ? sys_init_module+0xe1/0x250
 [<ffffffff8100b072>] ? system_call_fastpath+0x16/0x1b
```


