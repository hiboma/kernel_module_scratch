# sysctl

 * http://people.ee.ethz.ch/~arkeller/linux/kernel_user_space_howto.html#ss2.6
 * procfs のインタフェース
 * kernel/sysctl.c

## APIs 

 * CTL_UNNUMBERED
 * register_sysctl_table, unregister_sysctl_table
 * ハンドラー
   * proc_dostring()
   * proc_dointvec()
   * proc_dointvec_jiffies()
   * proc_dointvec_userhz_jiffies()
   * proc_dointvec_minmax(),
   * proc_doulongvec_ms_jiffies_minmax()
   * proc_doulongvec_minmax()
 * ストラテジ
   * sysctl_intvec

## vm.overcommit_ratio のサンプル 

```c
	{
		.ctl_name	= VM_OVERCOMMIT_RATIO,
		.procname	= "overcommit_ratio",
		.data		= &sysctl_overcommit_ratio,
		.maxlen		= sizeof(sysctl_overcommit_ratio),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec,
```

親ディレクトリは下記の様に定義されている

```c
static struct ctl_table root_table[] = {
	{
		.ctl_name	= CTL_KERN,
		.procname	= "kernel",
		.mode		= 0555,
		.child		= kern_table,
	},
	{
		.ctl_name	= CTL_VM,
		.procname	= "vm",    // vm.overcommit_ratio
		.mode		= 0555,
		.child		= vm_table,
	},
	{
		.ctl_name	= CTL_FS,
		.procname	= "fs",
		.mode		= 0555,
		.child		= fs_table,
	},
	{
		.ctl_name	= CTL_DEBUG,
		.procname	= "debug",
		.mode		= 0555,
		.child		= debug_table,
	},
	{
		.ctl_name	= CTL_DEV,
		.procname	= "dev",
		.mode		= 0555,
		.child		= dev_table,
	},
/*
 * NOTE: do not add new entries to this table unless you have read
 * Documentation/sysctl/ctl_unnumbered.txt
 */
	{ .ctl_name = 0 }
};
```

## struct ctl_table, register_sysctl_table

sysctl インタフェースを登録できる 

```
[vagrant@vagrant-centos65 sysctl]$ cat /proc/sys/value 
5
```

```
$ sudo sysctl -a | grep value
value = 5
$ sudo sysctl -w value=10
value = 10
$ sudo sysctl -w value=1000
error: "Invalid argument" setting key "value"
$ sudo sysctl -w value=-1
error: "Invalid argument" setting key "value"
$ sudo sysctl -w value=0
value = 0
```

### サブディレクトリを作りたい場合

struct ctl_table の .child を指定するとサブディレクトリで階層を作成できる

```
$ sudo sysctl -a | grep fuga
fugafuga.value = 5

$ cat /proc/sys/fugafuga/value 
5
```

 * グローバルな階層に値を増やすのは行儀が悪いので、サブディレクトリを掘るべきだろう
 * 単純な数値や文字列の設定をユーザランドから渡すのに便利
   * モジュール内の関数を呼び出すような用途ではない