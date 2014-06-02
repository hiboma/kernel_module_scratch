

## kern_path

```c
#include <linux/mount.h>
int kern_path(const char *name, unsigned int flags, struct path *path)
```

ファイルを走査して struct path を取れる

```c
	struct path {
		struct vfsmount *mnt;
		struct dentry *dentry;
	};
```

struct path から

 * struct vfsmount
 * struct dentry

を参照できる。

### 参照カウント

 * kern_path で返ってきた struct path は path_put
 * struct vfsmount は mntget/mntput,
 * struct dentry は dget/dput

でそれぞれ参照カウントを上げ下げする必要がある

### flags

下記のフラグを指定して lookup の動作を変更できる

```c
/*
 * The bitmask for a lookup event:
 *  - follow links at the end
 *  - require a directory
 *  - ending slashes ok even for nonexistent files
 *  - internal "there are more path components" flag
 *  - locked when lookup done with dcache_lock held
 *  - dentry cache is untrusted; force a real lookup
 *  - suppress terminal automount
 */
#define LOOKUP_FOLLOW		 1
#define LOOKUP_DIRECTORY	 2 
#define LOOKUP_CONTINUE		 4 
#define LOOKUP_AUTOMOUNT	 8
#define LOOKUP_PARENT		16
#define LOOKUP_REVAL		64
```

## path_lookup

TODO 

`path_lookup(sunaddr->sun_path, LOOKUP_PARENT, &nd);` 
