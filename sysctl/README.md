
# sysctl

 * http://people.ee.ethz.ch/~arkeller/linux/kernel_user_space_howto.html#ss2.6

## APIs 

 * CTL_UNNUMBERED
 * register_sysctl_table
 * unregister_sysctl_table
 * proc_dointvec_minmax
 * sysctl_intvec

文字列を渡すのにはどの API 使えばよい? 

## struct ctl_table

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

### サブディレクトリ

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