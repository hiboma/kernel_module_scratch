# spinlock

___Protect data and not code___

 * 複数のCPUからリソース利用の競合を防ぐ場合
   * spinlock, spin_trylock
 * ローカルCPUの割り込みコンテキストでも利用される場合
   * spin_lock_irq
   * spin_lock_irqsave
 * ソフト割り込みハンドラからも利用される場合
   * spin_lock_bh

read_lock, write_lock 属もある   

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

二つ目のロックが取れないので livelock になる例 (カーネルハング, カーネルストール)

```c
	spin_lock(&lock);
	spin_lock(&lock);
```

 * コア数が一個だとそのままCPU専有して何も出来なくなって死亡?
   * spin_lock だと割り込みを許可しているので watchdog が起動する
 * SMPでコア数が複数ある場合ご覧の通り system 時間が100%で貼り付いている

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

spin_lock だとローカル割り込みは許可されているので、 /proc/interrupts の数値は増加する

### /proc/sys/kernel/softlockup_panic = 1 で panic()

watchdog があれこれ出すのは以下のコードを見ると良い

```c
// kernel/watchdog.c (バニラカーネルには無い)

	if (unlikely(duration)) {
		/*
		 * If a virtual machine is stopped by the host it can look to
		 * the watchdog like a soft lockup, check to see if the host
		 * stopped the vm before we issue the warning
		 */
		if (kvm_check_and_clear_guest_paused())
			return HRTIMER_RESTART;

		/* only warn once */
		if (__get_cpu_var(soft_watchdog_warn) == true)
			return HRTIMER_RESTART;

		printk(KERN_EMERG "BUG: soft lockup - CPU#%d stuck for %us! [%s:%d]\n",
			smp_processor_id(), duration,
			current->comm, task_pid_nr(current));
		print_modules();
		print_irqtrace_events(current);
		if (regs)
			show_regs(regs);
		else
			dump_stack();

		if (softlockup_panic)
			panic("softlockup: hung tasks");
```
 
 * ただし コア数=1 で spin_lock_irq を使っているとローカルのタイマ割り込み(?)も無効になるので watchdog も動かない (完全に何も応答しない) ようだ
 * /proc/interrupts の数値も livelock している CPUだけ数値が増えない

## Clocksource tsc unstable

spinlock とは直接関係無いんだけど書いておく

 * sudo sysctl -w kernel.softlockup_panic=1 して
 * livelock を起こしたらホストOS側で下記ログが大量にでている
```
Clocksource tsc unstable (delta = 92016075 ns).  Enable clocksource failover by adding clocksource_failover kernel parameter.
```

 * 対策として http://blog.developerlabs.net/2012/12/kernel-clocksource-tsc-unstable.html を参考にクロックソース? を変更
```
[vagrant@vagrant-centos65 ~]$ cat /sys/devices/system/clocksource/clocksource0/available_clocksource
tsc acpi_pm 

# clocksource_failover=acpi_pm を追加
[vagrant@vagrant-centos65 ~]$ sudo grep clock /etc/grub.conf 
        kernel /boot/vmlinuz-2.6.32-431.el6.x86_64 ro root=UUID=ecd27adf-074b-4511-8cee-4a08a4620a4f rd_NO_LUKS rd_NO_LVM LANG=en_US.UTF-8 rd_NO_MD SYSFONT=latarcyrheb-sun16 crashkernel=auto  KEYBOARDTYPE=pc KEYTABLE=us rd_NO_DM console=ttyS0,9600 clocksource_failover=acpi_pm
```

livelock を起こすとめでたく(?) panic してくれた

```
BUG: soft lockup - CPU#0 stuck for 67s! [bash:1198]
Modules linked in: spinlock(U) vboxsf(U) ipv6 ppdev parport_pc parport sg i2c_piix4 i2c_core vboxguest(U) virtio_net ext4 jbd2 mbcache sd_mod crc_t10dif virtio_pci virtio_ring virtio ahci dm_mirror dm_region_hash dm_log dm_mod [last unloaded: scsi_wait_scan]
CPU 0 
Modules linked in: spinlock(U) vboxsf(U) ipv6 ppdev parport_pc parport sg i2c_piix4 i2c_core vboxguest(U) virtio_net ext4 jbd2 mbcache sd_mod crc_t10dif virtio_pci virtio_ring virtio ahci dm_mirror dm_region_hash dm_log dm_mod [last unloaded: scsi_wait_scan]

Pid: 1198, comm: bash Tainted: G           --------------- H  2.6.32-431.el6.x86_64 #1 innotek GmbH VirtualBox/VirtualBox
RIP: 0010:[<ffffffff8152a30c>]  [<ffffffff8152a30c>] _spin_lock+0x1c/0x30
RSP: 0018:ffff880023f2fec8  EFLAGS: 00000297
RAX: 0000000000000001 RBX: ffff880023f2fec8 RCX: ffff880023f2ff48
RDX: 0000000000000000 RSI: 00007fbe72ad0000 RDI: ffffffffa0023470
RBP: ffffffff8100bb8e R08: ffffffffa0023000 R09: 00007fbe72acb700
R10: 0000000000000000 R11: 0000000000000246 R12: ffff88002128d8a8
R13: ffff880023f2b540 R14: 0000000020be1032 R15: ffff880023f2ff58
FS:  00007fbe72acb700(0000) GS:ffff880002200000(0000) knlGS:0000000000000000
CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
CR2: 00000000004696c0 CR3: 00000000216bd000 CR4: 00000000000006f0
DR0: 0000000000000000 DR1: 0000000000000000 DR2: 0000000000000000
DR3: 0000000000000000 DR6: 00000000ffff0ff0 DR7: 0000000000000400
Process bash (pid: 1198, threadinfo ffff880023f2e000, task ffff880023f2b540)
Stack:
 ffff880023f2fee8 ffffffffa0023029 00007fbe72ad0000 ffff88002148ae40
<d> ffff880023f2ff28 ffffffff81188f78 0000000000000014 00000000004696c0
<d> 0000000000000000 ffff88002148ae40 00007fbe72ad0000 0000000000000004
Call Trace:
 [<ffffffffa0023029>] ? spinlock_write+0x29/0x40 [spinlock]
 [<ffffffff81188f78>] ? vfs_write+0xb8/0x1a0
 [<ffffffff81189871>] ? sys_write+0x51/0x90
 [<ffffffff8100b072>] ? system_call_fastpath+0x16/0x1b
Code: 81 2f 00 00 00 01 74 05 e8 12 45 d6 ff c9 c3 55 48 89 e5 0f 1f 44 00 00 b8 00 00 01 00 3e 0f c1 07 0f b7 d0 c1 e8 10 39 c2 74 0e <f3> 90 0f b7 17 eb f5 83 3f 00 75 f4 eb df c9 c3 0f 1f 40 00 55 
Call Trace:
 [<ffffffff81188e5d>] ? rw_verify_area+0x5d/0xc0
 [<ffffffffa0023029>] ? spinlock_write+0x29/0x40 [spinlock]
 [<ffffffff81188f78>] ? vfs_write+0xb8/0x1a0
 [<ffffffff81189871>] ? sys_write+0x51/0x90
 [<ffffffff8100b072>] ? system_call_fastpath+0x16/0x1b
Kernel panic - not syncing: softlockup: hung tasks
Pid: 1198, comm: bash Tainted: G           --------------- H  2.6.32-431.el6.x86_64 #1
Call Trace:
 <IRQ>  [<ffffffff815271fa>] ? panic+0xa7/0x16f
 [<ffffffff810e6460>] ? watchdog_timer_fn+0x0/0x1e0
 [<ffffffff810e662a>] ? watchdog_timer_fn+0x1ca/0x1e0
 [<ffffffff8109f9ee>] ? __run_hrtimer+0x8e/0x1a0
 [<ffffffff810a6e0f>] ? ktime_get_update_offsets+0x4f/0xd0
 [<ffffffff8109fd56>] ? hrtimer_interrupt+0xe6/0x260
 [<ffffffff81031f1d>] ? local_apic_timer_interrupt+0x3d/0x70
 [<ffffffff815310a5>] ? smp_apic_timer_interrupt+0x45/0x60
 [<ffffffff8100bb93>] ? apic_timer_interrupt+0x13/0x20            #<= 割り込みハンドラ
 <EOI>  [<ffffffffa0023000>] ? spinlock_write+0x0/0x40 [spinlock] # EOI is ?
 [<ffffffff8152a30c>] ? _spin_lock+0x1c/0x30
 [<ffffffff81188e5d>] ? rw_verify_area+0x5d/0xc0
 [<ffffffffa0023029>] ? spinlock_write+0x29/0x40 [spinlock]
 [<ffffffff81188f78>] ? vfs_write+0xb8/0x1a0
 [<ffffffff81189871>] ? sys_write+0x51/0x90
 [<ffffffff8100b072>] ? system_call_fastpath+0x16/0x1b
```