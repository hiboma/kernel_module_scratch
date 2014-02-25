# register_chrdev

 * http://people.ee.ethz.ch/~arkeller/linux/code/ioctl.c

## API

 * register_chrdev, unregister_chrdev
   * major 番号/パスを保持しておく必要がある
   * mknod しないとデバイスを扱えない
   * 0 を渡すと major 番号を動的に割り当て
   * name は /dev/ 以下に作られるファイル名とは関連が無い
     * デバイスのオーナー管理のための名前
```c
static inline int register_chrdev(unsigned int major, const char *name,
				  const struct file_operations *fops)
{
	return __register_chrdev(major, 0, 256, name, fops);
}
```
```
sudo mknod /dev/register_chrdev c 244 0
```

 * simple_read_from_buffer (libfs.c)
   * カーネル空間のバッファからユーザランドのバッファへコピーする際の便利メソッド
   * カーネル空間からカーネル空間でコピーする際は memory_read_from_buffer てのがある

## その他

major 番号の割り当てのAPI間違えてて( return major してた) register_chrdev がずっこけた際の dmesg
 
```
Feb 23 15:11:06 vagrant-centos65 kernel: failed to register_chrdev failed with 248
Feb 23 15:11:06 vagrant-centos65 kernel: sys_init_module: 'register_chrdev'->init suspiciously returned 248, it should follow 0/-E convention
Feb 23 15:11:06 vagrant-centos65 kernel: sys_init_module: loading module anyway...
Feb 23 15:11:06 vagrant-centos65 kernel: Pid: 3760, comm: insmod Tainted: G           --------------- H  2.6.32-431.el6.x86_64 #1
Feb 23 15:11:06 vagrant-centos65 kernel: Call Trace:
Feb 23 15:11:06 vagrant-centos65 kernel: [<ffffffff810bc633>] ? sys_init_module+0x1e3/0x250
Feb 23 15:11:06 vagrant-centos65 kernel: [<ffffffff8100b072>] ? system_call_fastpath+0x16/0x1b
```

 * **0/-E convention** はエラー番号を 0 か負の値で返すべき規約を指している
  * https://www.linux.com/learn/linux-career-center/26428-the-kernel-newbie-corner-loadable-kernel-modules-coming-and-going