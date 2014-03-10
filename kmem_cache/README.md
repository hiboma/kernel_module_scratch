## kmem_cache

 * slab アロケータでの実装
 * kmem_cache_init 汎用キャッシュ
 * kmem_cache_create 特定用途用のキャッシュ

[SReclaimable, SUnreclaim](https://github.com/hiboma/hiboma/blob/master/kernel/SReclaimable.md) についても読もう

## API

 * struct kmem_cache
   * キャッシュデスクリプタ
```c
struct kmem_cache {
/* 1) per-cpu data, touched during every alloc/free */
	struct array_cache *array[NR_CPUS];
/* 2) Cache tunables. Protected by cache_chain_mutex */
	unsigned int batchcount;
	unsigned int limit;
	unsigned int shared;

	unsigned int buffer_size;
	u32 reciprocal_buffer_size;
/* 3) touched by every alloc & free from the backend */

	unsigned int flags;		/* constant flags */
	unsigned int num;		/* # of objs per slab */

/* 4) cache_grow/shrink */
	/* order of pgs per slab (2^n) */
	unsigned int gfporder;

	/* force GFP flags, e.g. GFP_DMA */
	gfp_t gfpflags;

	size_t colour;			/* cache colouring range */
	unsigned int colour_off;	/* colour offset */
	struct kmem_cache *slabp_cache;
	unsigned int slab_size;
	unsigned int dflags;		/* dynamic flags */

	/* constructor func */
	void (*ctor)(void *obj);

/* 5) cache creation/removal */
	const char *name;
	struct list_head next;

/* 6) statistics */
#ifdef CONFIG_DEBUG_SLAB
	unsigned long num_active;
	unsigned long num_allocations;
	unsigned long high_mark;
	unsigned long grown;
	unsigned long reaped;
	unsigned long errors;
	unsigned long max_freeable;
	unsigned long node_allocs;
	unsigned long node_frees;
	unsigned long node_overflow;
	atomic_t allochit;
	atomic_t allocmiss;
	atomic_t freehit;
	atomic_t freemiss;

	/*
	 * If debugging is enabled, then the allocator can add additional
	 * fields and/or padding to every object. buffer_size contains the total
	 * object size including these internal fields, the following two
	 * variables contain the offset to the user object and its size.
	 */
	int obj_offset;
	int obj_size;
#endif /* CONFIG_DEBUG_SLAB */

	/*
	 * We put nodelists[] at the end of kmem_cache, because we want to size
	 * this array to nr_node_ids slots instead of MAX_NUMNODES
	 * (see kmem_cache_init())
	 * We still use [MAX_NUMNODES] and not [1] or [0] because cache_cache
	 * is statically defined, so we reserve the max number of nodes.
	 */
	struct kmem_list3 *nodelists[MAX_NUMNODES];
	/*
	 * Do not add fields after nodelists[]
	 */
};
```
 
 * kmem_cache_create
 * kmem_cache_destroy
   * キャッシュを破棄してから呼ばないと oops
 * kmem_cache_alloc
 * kmem_cache_shrink

## /proc/slabinfo

kmem_cache_create したキャッシュが確認できる

```
# name            <active_objs> <num_objs> <objsize> <objperslab> <pagesperslab> : tunables <limit> <batchcount> <sharedfactor> : slabdata <active_slabs> <num_slabs> <sharedavail>
foo_bar                0      0      8  337    1 : tunables  120   60    0 : slabdata      0      0      0
```

## SReclaimable, SUnreclaim

 * デフォルトでは kmem_cache_alloc したサイズは SUnreclaim に加算される
 * flgas に SLAB_RECLAIM_ACCOUNT を指定すると Reclaimable に加算される
```
# /proc/meminfo
Slab:             153828 kB
SReclaimable:       8468 kB
SUnreclaim:       145360 kB
```

## 間違った kmem_cache_destroy

割り当てたキャッシュを kmem_cache_free せずに kmem_cache_destroy したら下記の通り

```
slab error in kmem_cache_destroy(): cache `foo_bar': Can't free all objects
Pid: 2062, comm: rmmod Tainted: G           --------------- H  2.6.32-431.el6.x86_64 #1
Call Trace:
 [<ffffffff8117203b>] ? kmem_cache_destroy+0xbb/0xf0
 [<ffffffffa0023085>] ? cachep_exit+0x15/0x23 [kmem_cache]
 [<ffffffff810b9454>] ? sys_delete_module+0x194/0x260
 [<ffffffff81006568>] ? __xen_pgd_unpin+0x8/0x100
 [<ffffffff8100b072>] ? system_call_fastpath+0x16/0x1b
SLAB: cache with size 4104 has lost its name
BUG: unable to handle kernel paging request at ffffffffa0023190
IP: [<ffffffff81289bd9>] strnlen+0x9/0x40
PGD 1a87067 PUD 1a8b063 PMD 20bcd067 PTE 0
Oops: 0000 [#1] SMP 
last sysfs file: /sys/devices/system/cpu/online
CPU 0 
Modules linked in: kmem_cache(U) vboxsf(U) ipv6 ppdev parport_pc parport sg i2c_piix4 i2c_core vboxguest(U) virtio_net ext4 jbd2 mbcache sd_mod crc_t10dif ahci virtio_pci virtio_ring virtio dm_mirror dm_region_hash dm_log dm_mod [last unloaded: kmem_cache]

Pid: 1954, comm: slabtop Tainted: G           --------------- H  2.6.32-431.el6.x86_64 #1 innotek GmbH VirtualBox/VirtualBox
RIP: 0010:[<ffffffff81289bd9>]  [<ffffffff81289bd9>] strnlen+0x9/0x40
RSP: 0018:ffff880021407c28  EFLAGS: 00010286
RAX: ffffffff817b55de RBX: ffff880022134000 RCX: 0000000000000005
RDX: ffffffffa0023190 RSI: ffffffffffffffff RDI: ffffffffa0023190
RBP: ffff880021407c28 R08: 0000000000000073 R09: 0000000000001008
R10: ffff880020e8ad98 R11: 0000000000000004 R12: ffff880022133137
R13: ffffffffa0023190 R14: 0000000000000011 R15: 0000000000000010
FS:  00007fdfa9810700(0000) GS:ffff880002200000(0000) knlGS:0000000000000000
CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
CR2: ffffffffa0023190 CR3: 000000002247c000 CR4: 00000000000006f0
DR0: 0000000000000000 DR1: 0000000000000000 DR2: 0000000000000000
DR3: 0000000000000000 DR6: 00000000ffff0ff0 DR7: 0000000000000400
Process slabtop (pid: 1954, threadinfo ffff880021406000, task ffff8800216cd500)
Stack:
 ffff880021407c68 ffffffff8128ae90 ffff880021407c88 ffff880022133137
<d> ffffffff817bfb85 ffffffff817bfb80 ffff880021407d38 ffff880022134000
<d> ffff880021407d08 ffffffff8128c2d8 0000001000000004 0000000a00000011
Call Trace:
 [<ffffffff8128ae90>] string+0x40/0x100
 [<ffffffff8128c2d8>] vsnprintf+0x218/0x5e0
 [<ffffffff811ae10f>] ? seq_open+0x4f/0xb0
 [<ffffffff8114eeef>] ? vma_adjust+0x11f/0x5e0
 [<ffffffff811adb52>] seq_vprintf+0x32/0x60
 [<ffffffff811adbbd>] seq_printf+0x3d/0x40
 [<ffffffff8116f49a>] s_show+0x26a/0x330
 [<ffffffff811adf49>] seq_read+0x289/0x400
 [<ffffffff811f334e>] proc_reg_read+0x7e/0xc0
 [<ffffffff811896a5>] vfs_read+0xb5/0x1a0
 [<ffffffff811897e1>] sys_read+0x51/0x90
 [<ffffffff810e1e5e>] ? __audit_syscall_exit+0x25e/0x290
 [<ffffffff8100b072>] system_call_fastpath+0x16/0x1b
Code: 66 90 48 83 c2 01 80 3a 00 75 f7 48 89 d0 48 29 f8 c9 c3 66 66 66 66 66 66 2e 0f 1f 84 00 00 00 00 00 55 48 85 f6 48 89 e5 74 2e <80> 3f 00 74 29 48 83 ee 01 48 89 f8 eb 12 66 0f 1f 84 00 00 00 
RIP  [<ffffffff81289bd9>] strnlen+0x9/0x40
 RSP <ffff880021407c28>
CR2: ffffffffa0023190
---[ end trace a7919e7f17c0a727 ]---
Kernel panic - not syncing: Fatal exception
Pid: 1954, comm: slabtop Tainted: G      D    --------------- H  2.6.32-431.el6.x86_64 #1
Call Trace:
 [<ffffffff815271fa>] ? panic+0xa7/0x16f
 [<ffffffff8152b534>] ? oops_end+0xe4/0x100
 [<ffffffff8104a00b>] ? no_context+0xfb/0x260
 [<ffffffff8119fd36>] ? free_poll_entry+0x26/0x30
 [<ffffffff8104a295>] ? __bad_area_nosemaphore+0x125/0x1e0
 [<ffffffff811a0e25>] ? do_select+0x5f5/0x6c0
 [<ffffffff8104a363>] ? bad_area_nosemaphore+0x13/0x20
 [<ffffffff8104aabf>] ? __do_page_fault+0x31f/0x480
 [<ffffffff81227850>] ? security_inode_alloc+0x40/0x60
 [<ffffffff811a59ee>] ? inode_init_always+0x11e/0x1c0
 [<ffffffff8109b127>] ? bit_waitqueue+0x17/0xd0
 [<ffffffff8152d45e>] ? do_page_fault+0x3e/0xa0
 [<ffffffff8152a815>] ? page_fault+0x25/0x30
 [<ffffffff81289bd9>] ? strnlen+0x9/0x40
 [<ffffffff8128ae90>] ? string+0x40/0x100
 [<ffffffff8128c2d8>] ? vsnprintf+0x218/0x5e0
 [<ffffffff811ae10f>] ? seq_open+0x4f/0xb0
 [<ffffffff8114eeef>] ? vma_adjust+0x11f/0x5e0
 [<ffffffff811adb52>] ? seq_vprintf+0x32/0x60
 [<ffffffff811adbbd>] ? seq_printf+0x3d/0x40
 [<ffffffff8116f49a>] ? s_show+0x26a/0x330
 [<ffffffff811adf49>] ? seq_read+0x289/0x400
 [<ffffffff811f334e>] ? proc_reg_read+0x7e/0xc0
 [<ffffffff811896a5>] ? vfs_read+0xb5/0x1a0
 [<ffffffff811897e1>] ? sys_read+0x51/0x90
 [<ffffffff810e1e5e>] ? __audit_syscall_exit+0x25e/0x290
 [<ffffffff8100b072>] ? system_call_fastpath+0x16/0x1b
```

/proc/meminfo から seq_printf する途中で page_fault して oops