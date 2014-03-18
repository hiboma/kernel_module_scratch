## request_irq

## API

```c
request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
	    const char *name, void *dev)
```

```c
typedef irqreturn_t (*irq_handler_t)(int, void *);
```

```c
void free_irq(unsigned int irq, void *dev_id)
```

```c
#define in_interrupt()		(irq_count())
```

## request_irq の name

 * /proc/interrupts で参照される名前になる。 _oreore_ が登録されている
 * 複数の名前が並んでる IRQ は IRQF_SHARED で登録されたハンドラ

```
[vagrant@vagrant-centos65 request_irq]$ cat /proc/interrupts
           CPU0
  0:        163    XT-PIC-XT        timer
  1:          8    XT-PIC-XT        i8042, oreore
  2:          0    XT-PIC-XT        cascade
  4:          1    XT-PIC-XT      
  8:          0    XT-PIC-XT        rtc0
  9:       1896    XT-PIC-XT        acpi, vboxguest
 10:        381    XT-PIC-XT        virtio0
 11:      15732    XT-PIC-XT        ahci
 12:        136    XT-PIC-XT        i8042
```

## reading

request_irq

```
static inline int __must_check
request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
	    const char *name, void *dev)
{
	return request_threaded_irq(irq, handler, NULL, flags, name, dev);
}
```

request_threaded_irq

 * irq_action の kzalloc での割り当て

```c
/**
 *	request_threaded_irq - allocate an interrupt line
 *	@irq: Interrupt line to allocate
 *	@handler: Function to be called when the IRQ occurs.
 *		  Primary handler for threaded interrupts
 *		  If NULL and thread_fn != NULL the default
 *		  primary handler is installed
 *	@thread_fn: Function called from the irq handler thread
 *		    If NULL, no irq thread is created
 *	@irqflags: Interrupt type flags
 *	@devname: An ascii name for the claiming device
 *	@dev_id: A cookie passed back to the handler function
 *
 *	This call allocates interrupt resources and enables the
 *	interrupt line and IRQ handling. From the point this
 *	call is made your handler function may be invoked. Since
 *	your handler function must clear any interrupt the board
 *	raises, you must take care both to initialise your hardware
 *	and to set up the interrupt handler in the right order.
 *
 *	If you want to set up a threaded irq handler for your device
 *	then you need to supply @handler and @thread_fn. @handler ist
 *	still called in hard interrupt context and has to check
 *	whether the interrupt originates from the device. If yes it
 *	needs to disable the interrupt on the device and return
 *	IRQ_WAKE_THREAD which will wake up the handler thread and run
 *	@thread_fn. This split handler design is necessary to support
 *	shared interrupts.
 *
 *	Dev_id must be globally unique. Normally the address of the
 *	device data structure is used as the cookie. Since the handler
 *	receives this value it makes sense to use it.
 *
 *	If your interrupt is shared you must pass a non NULL dev_id
 *	as this is required when freeing the interrupt.
 *
 *	Flags:
 *
 *	IRQF_SHARED		Interrupt is shared
 *	IRQF_DISABLED	Disable local interrupts while processing
 *	IRQF_SAMPLE_RANDOM	The interrupt can be used for entropy
 *	IRQF_TRIGGER_*		Specify active edge(s) or level
 *
 */
int request_threaded_irq(unsigned int irq, irq_handler_t handler,
			 irq_handler_t thread_fn, unsigned long irqflags,
			 const char *devname, void *dev_id)
{
	struct irqaction *action;
	struct irq_desc *desc;
	int retval;

	/*
	 * handle_IRQ_event() always ignores IRQF_DISABLED except for
	 * the _first_ irqaction (sigh).  That can cause oopsing, but
	 * the behavior is classified as "will not fix" so we need to
	 * start nudging drivers away from using that idiom.
	 */
	if ((irqflags & (IRQF_SHARED|IRQF_DISABLED)) ==
					(IRQF_SHARED|IRQF_DISABLED)) {
		pr_warning(
		  "IRQ %d/%s: IRQF_DISABLED is not guaranteed on shared IRQs\n",
			irq, devname);
	}

#ifdef CONFIG_LOCKDEP
	/*
	 * Lockdep wants atomic interrupt handlers:
	 */
	irqflags |= IRQF_DISABLED;
#endif
	/*
	 * Sanity-check: shared interrupts must pass in a real dev-ID,
	 * otherwise we'll have trouble later trying to figure out
	 * which interrupt is which (messes up the interrupt freeing
	 * logic etc).
	 */
	if ((irqflags & IRQF_SHARED) && !dev_id)
		return -EINVAL;

    // IRQ 番号からデスクリプタを取る
    // irq_desc_ptrs[irq] を返すだけ
	desc = irq_to_desc(irq);
	if (!desc)
		return -EINVAL;

	if (desc->status & IRQ_NOREQUEST)
		return -EINVAL;

	if (!handler) {
		if (!thread_fn)
			return -EINVAL;
		handler = irq_default_primary_handler;
	}

    // irq_action が追加される
	action = kzalloc(sizeof(struct irqaction), GFP_KERNEL);
	if (!action)
		return -ENOMEM;

    // ハンドラ
	action->handler = handler;
    // request_irq を使った場合は thread_fn は NULL 
	action->thread_fn = thread_fn;
	action->flags = irqflags;
	action->name = devname;
    // IRQF_SHARED の場合に free_irq() できるように要指定
	action->dev_id = dev_id;

    // バス?
	chip_bus_lock(irq, desc);
	retval = __setup_irq(irq, desc, action);
	chip_bus_sync_unlock(irq, desc);

	if (retval)
		kfree(action);

    // ハンドラのテスト実行?
#ifdef CONFIG_DEBUG_SHIRQ
	if (irqflags & IRQF_SHARED) {
		/*
		 * It's a shared IRQ -- the driver ought to be prepared for it
		 * to happen immediately, so let's make sure....
		 * We disable the irq to make sure that a 'real' IRQ doesn't
		 * run in parallel with our fake.
		 */
		unsigned long flags;

		disable_irq(irq);
		local_irq_save(flags);

		handler(irq, dev_id);

		local_irq_restore(flags);
		enable_irq(irq);
	}
#endif
	return retval;
}
EXPORT_SYMBOL(request_threaded_irq);
```

__setup_irq

 * irq_desc に irqaction を fugafuga する

```c
/*
 * Internal function to register an irqaction - typically used to
 * allocate special interrupts that are part of the architecture.
 */
static int
__setup_irq(unsigned int irq, struct irq_desc *desc, struct irqaction *new)
{
	struct irqaction *old, **old_ptr;
	const char *old_name = NULL;
	unsigned long flags;
	int nested, shared = 0;
	int ret;

	if (!desc)
		return -EINVAL;

	if (desc->chip == &no_irq_chip)
		return -ENOSYS;
	/*
	 * Some drivers like serial.c use request_irq() heavily,
	 * so we have to be careful not to interfere with a
	 * running system.
	 */
	if (new->flags & IRQF_SAMPLE_RANDOM) {
		/*
		 * This function might sleep, we want to call it first,
		 * outside of the atomic block.
		 * Yes, this might clear the entropy pool if the wrong
		 * driver is attempted to be loaded, without actually
		 * installing a new handler, but is this really a problem,
		 * only the sysadmin is able to do this.
		 */
		rand_initialize_irq(irq);
	}

	/* Oneshot interrupts are not allowed with shared */
	if ((new->flags & IRQF_ONESHOT) && (new->flags & IRQF_SHARED))
		return -EINVAL;

	/*
	 * Check whether the interrupt nests into another interrupt
	 * thread.
	 */
	nested = desc->status & IRQ_NESTED_THREAD;
	if (nested) {
		if (!new->thread_fn)
			return -EINVAL;
		/*
		 * Replace the primary handler which was provided from
		 * the driver for non nested interrupt handling by the
		 * dummy function which warns when called.
		 */
		new->handler = irq_nested_primary_handler;
	}

	/*
	 * Create a handler thread when a thread function is supplied
	 * and the interrupt does not nest into another interrupt
	 * thread.
	 */
     // kthread_fun を指定すると カーネルスレッドを生やす様子
     // どういう使い方になる?
	if (new->thread_fn && !nested) {
		struct task_struct *t;

        // irq スレッド
        // IRQ 番号と name で識別される
		t = kthread_create(irq_thread, new, "irq/%d-%s", irq,
				   new->name);
		if (IS_ERR(t))
			return PTR_ERR(t);
		/*
		 * We keep the reference to the task struct even if
		 * the thread dies to avoid that the interrupt code
		 * references an already freed task_struct.
		 */
		get_task_struct(t);
		new->thread = t;
	}

	/*
	 * The following block of code has to be executed atomically
	 */
	spin_lock_irqsave(&desc->lock, flags);
	old_ptr = &desc->action;
	old = *old_ptr;
	if (old) {
		/*
		 * Can't share interrupts unless both agree to and are
		 * the same type (level, edge, polarity). So both flag
		 * fields must have IRQF_SHARED set and the bits which
		 * set the trigger type must match.
		 */
		if (!((old->flags & new->flags) & IRQF_SHARED) ||
		    ((old->flags ^ new->flags) & IRQF_TRIGGER_MASK)) {
			old_name = old->name;
			goto mismatch;
		}

#if defined(CONFIG_IRQ_PER_CPU)
		/* All handlers must agree on per-cpuness */
		if ((old->flags & IRQF_PERCPU) !=
		    (new->flags & IRQF_PERCPU))
			goto mismatch;
#endif

		/* add new interrupt at end of irq queue */
		do {
			old_ptr = &old->next;
			old = *old_ptr;
		} while (old);
		shared = 1;
	}

	if (!shared) {
		irq_chip_set_defaults(desc->chip);

		init_waitqueue_head(&desc->wait_for_threads);

        // レベル/エッジトリガかどうかでセット
		/* Setup the type (level, edge polarity) if configured: */
		if (new->flags & IRQF_TRIGGER_MASK) {
			ret = __irq_set_trigger(desc, irq,
					new->flags & IRQF_TRIGGER_MASK);

			if (ret)
				goto out_thread;
		} else
			compat_irq_chip_set_default_handler(desc);
#if defined(CONFIG_IRQ_PER_CPU)
		if (new->flags & IRQF_PERCPU)
			desc->status |= IRQ_PER_CPU;
#endif

		desc->status &= ~(IRQ_AUTODETECT | IRQ_WAITING | IRQ_ONESHOT |
				  IRQ_INPROGRESS | IRQ_SPURIOUS_DISABLED);

		if (new->flags & IRQF_ONESHOT)
			desc->status |= IRQ_ONESHOT;

		/*
		 * Force MSI interrupts to run with interrupts
		 * disabled. The multi vector cards can cause stack
		 * overflows due to nested interrupts when enough of
		 * them are directed to a core and fire at the same
		 * time.
		 */
		if (desc->msi_desc)
			new->flags |= IRQF_DISABLED;

		if (!(desc->status & IRQ_NOAUTOEN)) {
			desc->depth = 0;
			desc->status &= ~IRQ_DISABLED;
			desc->chip->startup(irq);
		} else
			/* Undo nested disables: */
			desc->depth = 1;

		/* Exclude IRQ from balancing if requested */
		if (new->flags & IRQF_NOBALANCING)
			desc->status |= IRQ_NO_BALANCING;

		/* Set default affinity mask once everything is setup */
		setup_affinity(irq, desc);

	} else if ((new->flags & IRQF_TRIGGER_MASK)
			&& (new->flags & IRQF_TRIGGER_MASK)
				!= (desc->status & IRQ_TYPE_SENSE_MASK)) {
		/* hope the handler works with the actual trigger mode... */
		pr_warning("IRQ %d uses trigger mode %d; requested %d\n",
				irq, (int)(desc->status & IRQ_TYPE_SENSE_MASK),
				(int)(new->flags & IRQF_TRIGGER_MASK));
	}

	new->irq = irq;
	*old_ptr = new;

	/* Reset broken irq detection when installing new handler */
	desc->irq_count = 0;
	desc->irqs_unhandled = 0;

	/*
	 * Check whether we disabled the irq via the spurious handler
	 * before. Reenable it and give it another chance.
	 */
	if (shared && (desc->status & IRQ_SPURIOUS_DISABLED)) {
		desc->status &= ~IRQ_SPURIOUS_DISABLED;
		__enable_irq(desc, irq, false);
	}

	spin_unlock_irqrestore(&desc->lock, flags);

	/*
	 * Strictly no need to wake it up, but hung_task complains
	 * when no hard interrupt wakes the thread up.
	 */
	if (new->thread)
		wake_up_process(new->thread);

    // proc_mkdir で /proc/irq/<IRQ> のディレクトリを掘る
	register_irq_proc(irq, desc);
	new->dir = NULL;
	register_handler_proc(irq, new);

	return 0;

mismatch:
#ifdef CONFIG_DEBUG_SHIRQ
	if (!(new->flags & IRQF_PROBE_SHARED)) {
		printk(KERN_ERR "IRQ handler type mismatch for IRQ %d\n", irq);
		if (old_name)
			printk(KERN_ERR "current handler: %s\n", old_name);
		dump_stack();
	}
#endif
	ret = -EBUSY;

out_thread:
	spin_unlock_irqrestore(&desc->lock, flags);
	if (new->thread) {
		struct task_struct *t = new->thread;

		new->thread = NULL;
		if (likely(!test_bit(IRQTF_DIED, &new->thread_flags)))
			kthread_stop(t);
		put_task_struct(t);
	}
	return ret;
}
```