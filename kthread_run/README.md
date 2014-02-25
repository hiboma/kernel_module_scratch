# kthread_run のサンプルコード

http://wiki.bit-hive.com/north/pg/kthread_run%A5%DE%A5%AF%A5%ED のコードを拝借してちょっとだけ変えて載せています

## API

### [kthread_run](http://lxr.free-electrons.com/source/include/linux/kthread.h?v=2.6.32#L21)

 * カーネルスレッドを作成, wakeup する

定期的に起床して何かするタスクの作り方の参考になる

### [schedule_timeout](http://lxr.free-electrons.com/source/kernel/timer.c?v=2.6.32#L1359)

 * ユーザランドでの sleep(3) 的に扱える API
 * タイムアウト値は jiffies で指定
 * 呼び出し側で TASK_INTERRUPTIBLE か TASK_UNINTERRUPTIBLE に set_current_state する必要がある
 
タイマの使い方のサンプルになりそうだ

### [kthread_stop](http://lxr.free-electrons.com/source/kernel/kthread.c?v=2.6.32#L167)

 * todo

## TODO

 * カーネルスレッドの TASK_INTERRUPTIBLE/TASK_UNINTERRUPTIBLE とシグナル
 * ユーザ空間からシグナル送っても応答しないけど、カーネル空間から送ると応答する?

## sandbox

 * `set_current_state(TASK_UNINTERRUPTIBLE)` にして schedule_timeout の数値をめいいっぱい伸ばすと下記ログを出す

```
Feb 12 15:51:55 vagrant-centos65 kernel: INFO: task kthread hiboma&:4082 blocked for more than 120 seconds.
Feb 12 15:51:55 vagrant-centos65 kernel:      Tainted: G           --------------- H  2.6.32-431.el6.x86_64 #1
Feb 12 15:51:55 vagrant-centos65 kernel: "echo 0 > /proc/sys/kernel/hung_task_timeout_secs" disables this message.
Feb 12 15:51:55 vagrant-centos65 kernel: kthread hibom D 0000000000000000     0  4082      2 0x00000080
Feb 12 15:51:55 vagrant-centos65 kernel: ffff880023f6be10 0000000000000046 0000000000000001 ffffffff81645da0
Feb 12 15:51:55 vagrant-centos65 kernel: 0000000000000000 00000000000114c0 000000000000136d ffff880002200000
Feb 12 15:51:55 vagrant-centos65 kernel: ffff880020829098 ffff880023f6bfd8 000000000000fbc8 ffff880020829098
Feb 12 15:51:55 vagrant-centos65 kernel: Call Trace:
Feb 12 15:51:55 vagrant-centos65 kernel: [<ffffffff81528732>] schedule_timeout+0x192/0x2e0
Feb 12 15:51:55 vagrant-centos65 kernel: [<ffffffff81084240>] ? process_timeout+0x0/0x10
Feb 12 15:51:55 vagrant-centos65 kernel: [<ffffffffa003e000>] ? my_kthread+0x0/0x78 [kthread_run]
Feb 12 15:51:55 vagrant-centos65 kernel: [<ffffffffa003e05e>] my_kthread+0x5e/0x78 [kthread_run]
Feb 12 15:51:55 vagrant-centos65 kernel: [<ffffffff8109aef6>] kthread+0x96/0xa0
Feb 12 15:51:55 vagrant-centos65 kernel: [<ffffffff8100c20a>] child_rip+0xa/0x20
Feb 12 15:51:55 vagrant-centos65 kernel: [<ffffffff8109ae60>] ? kthread+0x0/0xa0
Feb 12 15:51:55 vagrant-centos65 kernel: [<ffffffff8100c200>] ? child_rip+0x0/0x20
```

http://ossmpedia.org/messages/linux/2.6.32-279.EL6/2001028.ja のログと同じである

