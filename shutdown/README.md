# shutdown

 * http://stackoverflow.com/questions/18296686/shutdown-embedded-linux-from-kernel-space
 * insmod したら shutdown !!!1

 ```
 ＿人人人人人人＿
＞　shutdown　＜
￣Y^Y^Y^Y^Y￣
```

## APIs

 * call_usermodehelper
   * wait に UMH_NO_WAIT を指定するかいなかで GFP_ATOMIC か GFP_KERNEL の違いが出る
   * UMH_NO_WAIT (GFP_ATOMIC) の場合は コマンドを実行しているスレッド(?) の完了を待たない
     * wait_for_completion
 * ユーザランドからの呼び出しとカーネルからの呼び出しではどういった違いが出るのか?
   * 優先度
   * メモリの使用
   * 失敗時のハンドリング
   * stdout, stderr 等々

## call_usermodehelper

```c
static inline int
call_usermodehelper(char *path, char **argv, char **envp, enum umh_wait wait)
{
	struct subprocess_info *info;
	gfp_t gfp_mask = (wait == UMH_NO_WAIT) ? GFP_ATOMIC : GFP_KERNEL;

	info = call_usermodehelper_setup(path, argv, envp, gfp_mask);
	if (info == NULL)
		return -ENOMEM;
	return call_usermodehelper_exec(info, wait);
}
```

```c
/**
 * call_usermodehelper_exec - start a usermode application
 * @sub_info: information about the subprocessa
 * @wait: wait for the application to finish and return status.
 *        when -1 don't wait at all, but you get no useful error back when
 *        the program couldn't be exec'ed. This makes it safe to call
 *        from interrupt context.
 *
 * Runs a user-space application.  The application is started
 * asynchronously if wait is not set, and runs as a child of keventd.
 * (ie. it runs with full root capabilities).
 */
int call_usermodehelper_exec(struct subprocess_info *sub_info,
			     enum umh_wait wait)
{
	DECLARE_COMPLETION_ONSTACK(done);
	int retval = 0;

	BUG_ON(atomic_read(&sub_info->cred->usage) != 1);
	validate_creds(sub_info->cred);

	helper_lock();
	if (sub_info->path[0] == '\0')
		goto out;

	if (!khelper_wq || usermodehelper_disabled) {
		retval = -EBUSY;
		goto out;
	}

	sub_info->complete = &done;
	sub_info->wait = wait;

	queue_work(khelper_wq, &sub_info->work);
	if (wait == UMH_NO_WAIT)	/* task has freed sub_info */
		goto unlock;
	wait_for_completion(&done);
	retval = sub_info->retval;

out:
	call_usermodehelper_freeinfo(sub_info);
unlock:
	helper_unlock();
	return retval;
}
EXPORT_SYMBOL(call_usermodehelper_exec);
```