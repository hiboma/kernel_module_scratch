
## kmalloc(size_t size, int flags)

 * 任意のサイズで物理的に連続したメモリを確保
 * flags によって動作が異なる
 * 通常のポインタとして操作してよい?

### GFP_KERNEL

 * スリープしうるので、割り込みハンドラでは使用できない

### GFP_ATOMIC

 * スリープしない
 * 割り込みハンドラやスリープできない区間で呼び出す
   * タスクレット
   * タイムアウトハンドラ
