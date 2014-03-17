## request_irq

```c
request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
	    const char *name, void *dev)
```

```c
typedef irqreturn_t (*irq_handler_t)(int, void *);
```

 * request_irq
 * free_irq
 * in_interrupt
 * IRQF_SHARED
