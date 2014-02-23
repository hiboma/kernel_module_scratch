
## debugfs

debugfs_create_* API が型に応じて様々

 * debugfs_create_atomic_t
 * debugfs_create_dir
 * debugfs_create_netdev_queue_stopped
 * debugfs_create_u32
 * debugfs_create_ul_MAX_STACK_TRACE_DEPTH  
 * debugfs_create_blob
 * debugfs_create_file
 * debugfs_create_size_t
 * debugfs_create_u64
 * debugfs_create_x16                       
 * debugfs_create_bool
 * debugfs_create_i2400m_reset
 * debugfs_create_symlink
 * debugfs_create_u8
 * debugfs_create_x32                       
 * debugfs_create_by_name
 * debugfs_create_i2400m_suspend
 * debugfs_create_u16
 * debugfs_create_ul
 * debugfs_create_x8

## debugfs_create_u8 のテスト

unsigned int 8 な型の変数を読み書きするためのインタフェースをファイルとして提供する

#### try

CentOS6.5 だと debugfs は最初からマウントされてないので mount 作業

```
sudo mount -t debugfs debugfs /mnt/debugfs
```

以下の様なエントリが並ぶ

```
[vagrant@vagrant-centos65 debugfs]$ ls -hal /mnt/debugfs
total 4.0K
drwxr-xr-x  13 root root    0 Feb 20 17:01 .
drwxr-xr-x.  4 root root 4.0K Feb 21 13:19 ..
drwxr-xr-x  29 root root    0 Feb 20 17:01 bdi
drwxr-xr-x   3 root root    0 Feb 20 17:01 boot_params
drwxr-xr-x   2 root root    0 Feb 20 17:01 dynamic_debug
drwxr-xr-x   2 root root    0 Feb 20 17:01 extfrag
-r--r--r--   1 root root    0 Feb 20 17:01 gpio
drwxr-xr-x   2 root root    0 Feb 20 17:01 hid
drwxr-xr-x   2 root root    0 Feb 20 17:01 kprobes
drwxr-xr-x   2 root root    0 Feb 20 17:01 mce
-rw-r--r--   1 root root    0 Feb 20 17:01 sched_features
drwxr-xr-x   6 root root    0 Feb 20 17:01 tracing
-rw-rw-rw-   1 root root    0 Feb 21 13:19 u8
drwxr-xr-x   3 root root    0 Feb 20 17:01 usb
drwxr-xr-x   2 root root    0 Feb 20 17:01 x86
drwxr-xr-x   4 root root    0 Feb 20 17:01 xen
```

値の読み書き

```
$ cat /mnt/debugfs/u8 
0
$ echo 100 > /mnt/debugfs/u8 
$ cat /mnt/debugfs/u8 
100

# オーバーフローする
$ echo 256 > /mnt/debugfs/u8 
$ cat /mnt/debugfs/u8 
0

# unsigned 8bit = 255 が上限ね
$ echo 255 > /mnt/debugfs/u8 
$ cat /mnt/debugfs/u8 
255
```

## debugfs_create_file

 * file_operations でファイルを実装できる

```c
static ssize_t debugfs_test_read(struct file *file, char __user *userbuf,
				 size_t count, loff_t *ppos)
{
	return 0;
} 

static ssize_t debugfs_test_write(struct file *file, const char __user *buf,
				  size_t count, loff_t *ppos)
{
	return count;
}

static struct file_operations fops = {
	.read  = debugfs_test_read,
	.write = debugfs_test_write,
};

// ...

	dentry_file = debugfs_create_file("fuga", 0666, NULL, &file_value, &fops);
```

