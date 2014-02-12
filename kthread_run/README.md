# kthread_run のサンプルコード

http://wiki.bit-hive.com/north/pg/kthread_run%A5%DE%A5%AF%A5%ED のコードを拝借してちょっとだけ変えて載せています

## [kthread_run](http://lxr.free-electrons.com/source/include/linux/kthread.h?v=2.6.32#L21)

 * カーネルスレッドを作成, wakeup する

定期的に起床して何かするタスクの作り方の参考になる

## [schedule_timeout](http://lxr.free-electrons.com/source/kernel/timer.c?v=2.6.32#L1359)

 * ユーザランドでの sleep(3) 的に扱える API
 * タイムアウト値は jiffies で指定
 * 呼び出し側で TASK_INTERRUPTIBLE か TASK_UNINTERRUPTIBLE に set_current_state する必要がある
 
タイマの使い方のサンプルになりそうだ

## [kthread_stop](http://lxr.free-electrons.com/source/kernel/kthread.c?v=2.6.32#L167)

 * todo