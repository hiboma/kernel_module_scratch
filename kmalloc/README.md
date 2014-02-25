# kmalloc

## API

 * kmalloc
 * kzalloc

## kmalloc(size_t size, int flags)

 * 任意のサイズで __物理的__ に連続したメモリを確保
 * flags によって動作が異なる
 * 通常のポインタとして操作してよい?

## kzalloc

 * __GFP_ZERO をたてて内部で memset(.., 0,..) でゼロ初期化してくれる

### GFP_KERNEL

 * スリープしうるので、割り込みハンドラでは使用できない

### GFP_ATOMIC

 * スリープしない
 * 割り込みハンドラやスリープできない区間で呼び出す
   * タスクレット
   * タイムアウトハンドラ

