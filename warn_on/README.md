
## WARN_ON()

引き数が 0 以外の時に下記のようなトレースを出す

```
Feb 14 17:10:41 vagrant-centos65 kernel: ------------[ cut here ]------------
Feb 14 17:10:41 vagrant-centos65 kernel: WARNING: at /vagrant/warn_on/warn_on.c:9 warn_on_init+0x15/0x19 [warn_on]() (Tainted: G           --------------- H )
Feb 14 17:10:41 vagrant-centos65 kernel: Hardware name: VirtualBox
Feb 14 17:10:41 vagrant-centos65 kernel: Modules linked in: warn_on(+)(U) vboxsf(U) ipv6 ppdev parport_pc parport sg i2c_piix4 i2c_core vboxguest(U) virtio_net ext4 jbd2 mbcache sd_mod crc_t10dif ahci virtio_pci virtio_ring virtio dm_mirror dm_region_hash dm_log dm_mod [last unloaded: warn_on]
Feb 14 17:10:41 vagrant-centos65 kernel: Pid: 9294, comm: insmod Tainted: G           --------------- H  2.6.32-431.el6.x86_64 #1
Feb 14 17:10:41 vagrant-centos65 kernel: Call Trace:
Feb 14 17:10:41 vagrant-centos65 kernel: [<ffffffff81071e27>] ? warn_slowpath_common+0x87/0xc0
Feb 14 17:10:41 vagrant-centos65 kernel: [<ffffffffa0044000>] ? warn_on_init+0x0/0x19 [warn_on]
Feb 14 17:10:41 vagrant-centos65 kernel: [<ffffffff81071e7a>] ? warn_slowpath_null+0x1a/0x20
Feb 14 17:10:41 vagrant-centos65 kernel: [<ffffffffa0044015>] ? warn_on_init+0x15/0x19 [warn_on]
Feb 14 17:10:41 vagrant-centos65 kernel: [<ffffffff8100204c>] ? do_one_initcall+0x3c/0x1d0
Feb 14 17:10:41 vagrant-centos65 kernel: [<ffffffff810bc531>] ? sys_init_module+0xe1/0x250
Feb 14 17:10:41 vagrant-centos65 kernel: [<ffffffff8100b072>] ? system_call_fastpath+0x16/0x1b
Feb 14 17:10:41 vagrant-centos65 kernel: ---[ end trace a7919e7f17c0a727 ]---
```
