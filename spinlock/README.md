# spinlock

## API

 * spinlock_t

```c
typedef struct {
	raw_spinlock_t raw_lock;
#ifdef CONFIG_GENERIC_LOCKBREAK
	unsigned int break_lock;
#endif
#ifdef CONFIG_DEBUG_SPINLOCK
	unsigned int magic, owner_cpu;
	void *owner;
#endif
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map dep_map;
#endif
} spinlock_t;

// x86?
typedef struct raw_spinlock {
	unsigned int slock;
} raw_spinlock_t;
```
 
 * spin_lock_init
 * spin_lock, spin_unlock
 * spin_lock_irq, spin_unlock_irq
 * spin_lock_irqsave, spin_unlock_irqrestore

## livelock, ビジーウェイト

二つ目のロックが取れないので livelock になる

```
	spin_lock(&lock);
	spin_lock(&lock);
```

ご覧の通り system 時間が100%で貼り付いている

``` 
top - 12:42:56 up 3 min,  3 users,  load average: 1.45, 0.35, 0.11
Tasks: 116 total,   5 running, 111 sleeping,   0 stopped,   0 zombie
Cpu0  :  0.0%us,  0.0%sy,  0.0%ni, 98.9%id,  0.0%wa,  0.0%hi,  1.1%si,  0.0%st
Cpu1  :  0.0%us,100.0%sy,  0.0%ni,  0.0%id,  0.0%wa,  0.0%hi,  0.0%si,  0.0%st
Cpu2  :  1.1%us, 11.0%sy,  0.0%ni, 87.9%id,  0.0%wa,  0.0%hi,  0.0%si,  0.0%st
Cpu3  :  0.0%us,  0.0%sy,  0.0%ni,100.0%id,  0.0%wa,  0.0%hi,  0.0%si,  0.0%st
Mem:    603168k total,   135768k used,   467400k free,     9852k buffers
Swap:        0k total,        0k used,        0k free,    55496k cached
```

一定時間経つと soft lockup で検知される

```
Mar  1 12:43:53 vagrant-centos65 kernel: BUG: soft lockup - CPU#1 stuck for 67s! [bash:1299]^
Mar  1 12:43:53 vagrant-centos65 kernel: Modules linked in: spinlock(U) vboxsf(U) ipv6 ppdev parport_pc parport sg i2c_piix4 i2c_core virtio_net vboxguest(U) ext4 jbd2 mbcache sd_mod crc_t10dif virtio_pci virtio_ring virtio ahci dm_mirror dm_region_hash dm_log dm_mod [last unloaded: scsi_wait_scan]
Mar  1 12:43:53 vagrant-centos65 kernel: CPU 1
Mar  1 12:43:53 vagrant-centos65 kernel: Modules linked in: spinlock(U) vboxsf(U) ipv6 ppdev parport_pc parport sg i2c_piix4 i2c_core virtio_net vboxguest(U) ext4 jbd2 mbcache sd_mod crc_t10dif virtio_pci virtio_ring virtio ahci dm_mirror dm_region_hash dm_log dm_mod [last unloaded: scsi_wait_scan]
Mar  1 12:43:53 vagrant-centos65 kernel:
Mar  1 12:43:53 vagrant-centos65 kernel: Pid: 1299, comm: bash Tainted: G           --------------- H  2.6.32-431.el6.x86_64 #1 innotek GmbH VirtualBox/VirtualBox
Mar  1 12:43:53 vagrant-centos65 kernel: RIP: 0010:[<ffffffff8152a30e>]  [<ffffffff8152a30e>] _spin_lock+0x1e/0x30
Mar  1 12:43:53 vagrant-centos65 kernel: RSP: 0018:ffff88002163fec8  EFLAGS: 00000297
Mar  1 12:43:53 vagrant-centos65 kernel: RAX: 0000000000000005 RBX: ffff88002163fec8 RCX: ffff88002163ff48
Mar  1 12:43:53 vagrant-centos65 kernel: RDX: 0000000000000004 RSI: 00007f94273af000 RDI: ffffffffa001c430
Mar  1 12:43:53 vagrant-centos65 kernel: RBP: ffffffff8100bb8e R08: ffffffffa001c000 R09: 00007f94273aa700
Mar  1 12:43:53 vagrant-centos65 kernel: R10: 0000000000000000 R11: 0000000000000246 R12: ffff880026286268
Mar  1 12:43:53 vagrant-centos65 kernel: R13: ffff88002615e040 R14: 0000000025c3f032 R15: ffff88002163ff58
Mar  1 12:43:53 vagrant-centos65 kernel: FS:  00007f94273aa700(0000) GS:ffff880002280000(0000) knlGS:0000000000000000
Mar  1 12:43:53 vagrant-centos65 kernel: CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
Mar  1 12:43:53 vagrant-centos65 kernel: CR2: 00000000004696c0 CR3: 0000000023ae2000 CR4: 00000000000006e0
Mar  1 12:43:53 vagrant-centos65 kernel: DR0: 0000000000000000 DR1: 0000000000000000 DR2: 0000000000000000
Mar  1 12:43:53 vagrant-centos65 kernel: DR3: 0000000000000000 DR6: 00000000ffff0ff0 DR7: 0000000000000400
Mar  1 12:43:53 vagrant-centos65 kernel: Process bash (pid: 1299, threadinfo ffff88002163e000, task ffff88002615e040)
Mar  1 12:43:53 vagrant-centos65 kernel: Stack:
Mar  1 12:43:53 vagrant-centos65 kernel: ffff88002163fee8 ffffffffa001c029 00007f94273af000 ffff880025d9fec0
Mar  1 12:43:53 vagrant-centos65 kernel: <d> ffff88002163ff28 ffffffff81188f78 0000000000000014 00000000004696c0
Mar  1 12:43:53 vagrant-centos65 kernel: <d> 0000000000000000 ffff880025d9fec0 00007f94273af000 0000000000000004
Mar  1 12:43:53 vagrant-centos65 kernel: Call Trace:
Mar  1 12:43:53 vagrant-centos65 kernel: [<ffffffffa001c029>] ? spinlock_write+0x29/0x40 [spinlock]
Mar  1 12:43:53 vagrant-centos65 kernel: [<ffffffff81188f78>] ? vfs_write+0xb8/0x1a0
Mar  1 12:43:53 vagrant-centos65 kernel: [<ffffffff81189871>] ? sys_write+0x51/0x90
Mar  1 12:43:53 vagrant-centos65 kernel: [<ffffffff8100b072>] ? system_call_fastpath+0x16/0x1b
Mar  1 12:43:53 vagrant-centos65 kernel: Code: 00 00 00 01 74 05 e8 12 45 d6 ff c9 c3 55 48 89 e5 0f 1f 44 00 00 b8 00 00 01 00 f0 0f c1 07 0f b7 d0 c1 e8 10 39 c2 74 0e f3 90 <0f> b7 17 eb f5 83 3f 00 75 f4 eb df c9 c3 0f 1f 40 00 55 48 89
Mar  1 12:43:53 vagrant-centos65 kernel: Call Trace:
Mar  1 12:43:53 vagrant-centos65 kernel: [<ffffffff81188e5d>] ? rw_verify_area+0x5d/0xc0
Mar  1 12:43:53 vagrant-centos65 kernel: [<ffffffffa001c029>] ? spinlock_write+0x29/0x40 [spinlock]
Mar  1 12:43:53 vagrant-centos65 kernel: [<ffffffff81188f78>] ? vfs_write+0xb8/0x1a0
Mar  1 12:43:53 vagrant-centos65 kernel: [<ffffffff81189871>] ? sys_write+0x51/0x90
Mar  1 12:43:53 vagrant-centos65 kernel: [<ffffffff8100b072>] ? system_call_fastpath+0x16/0x1b
```

kernel/watchdog.c (バニラカーネルには無い)