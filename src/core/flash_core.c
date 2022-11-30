#include <zephyr/zephyr.h>
#include <zephyr/device.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/storage/flash_map.h>
#include <delay.h>
#include <log.h>

#include <flash_core.h>


/// 文件系统设备树信息
#ifdef CONFIG_APP_LITTLEFS_STORAGE_FLASH
static int littlefs_flash_erase(unsigned int id)
{
	const struct flash_area *pfa;
	int rc;

	rc = flash_area_open(id, &pfa);
	if (rc < 0) {
		LOG(EERROR, "FAIL: unable to find flash area %u: %d\n",
			id, rc);
		return rc;
	}

	LOG(EDEBUG, "Area %u at 0x%x on %s for %u bytes\n",
		   id, (unsigned int)pfa->fa_off, pfa->fa_dev_name,
		   (unsigned int)pfa->fa_size);

	/* Optional wipe flash contents */
	if (IS_ENABLED(CONFIG_APP_WIPE_STORAGE)) {
		rc = flash_area_erase(pfa, 0, pfa->fa_size);
		LOG(EERROR, "Erasing flash area ... %d", rc);
	}

	flash_area_close(pfa);
	return rc;
}
#define PARTITION_NODE DT_NODELABEL(lfs1)

#if DT_NODE_EXISTS(PARTITION_NODE)
FS_FSTAB_DECLARE_ENTRY(PARTITION_NODE);
#else /* PARTITION_NODE */
FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(storage);
static struct fs_mount_t lfs_storage_mnt = {
	.type = FS_LITTLEFS,
	.fs_data = &storage,
	.storage_dev = (void *)FLASH_AREA_ID(storage),
	.mnt_point = "/lfs",
};
#endif /* PARTITION_NODE */

	struct fs_mount_t *mp =
#if DT_NODE_EXISTS(PARTITION_NODE)
		&FS_FSTAB_ENTRY(PARTITION_NODE)
#else
		&lfs_storage_mnt
#endif
		;

static int littlefs_mount(struct fs_mount_t *mp)
{
	int rc;

	rc = littlefs_flash_erase((uintptr_t)mp->storage_dev);
	if (rc < 0) {
		return rc;
	}

	/* Do not mount if auto-mount has been enabled */
#if !DT_NODE_EXISTS(PARTITION_NODE) ||						\
	!(FSTAB_ENTRY_DT_MOUNT_FLAGS(PARTITION_NODE) & FS_MOUNT_FLAG_AUTOMOUNT)
	rc = fs_mount(mp);
	if (rc < 0) {
		LOG(EDEBUG, "FAIL: mount id %" PRIuPTR " at %s: %d\n",
		       (uintptr_t)mp->storage_dev, mp->mnt_point, rc);
		return rc;
	}
	LOG(EDEBUG, "%s mount: %d\n", mp->mnt_point, rc);
#else
	LOG(EDEBUG, "%s automounted\n", mp->mnt_point);
#endif

	return 0;
}
#endif /* CONFIG_APP_LITTLEFS_STORAGE_FLASH */

#ifdef CONFIG_APP_LITTLEFS_STORAGE_BLK_SDMMC
struct fs_littlefs lfsfs;
static struct fs_mount_t __mp = {
	.type = FS_LITTLEFS,
	.fs_data = &lfsfs,
	.flags = FS_MOUNT_FLAG_USE_DISK_ACCESS,
};
struct fs_mount_t *mp = &__mp;

static int littlefs_mount(struct fs_mount_t *mp)
{
	static const char *disk_mount_pt = "/"CONFIG_SDMMC_VOLUME_NAME":";
	static const char *disk_pdrv = CONFIG_SDMMC_VOLUME_NAME;

	mp->storage_dev = (void *)disk_pdrv;
	mp->mnt_point = disk_mount_pt;

	return fs_mount(mp);
}
#endif /* CONFIG_APP_LITTLEFS_STORAGE_BLK_SDMMC */


static int lsdir(const char *path)
{
	int res;
	struct fs_dir_t dirp;
	static struct fs_dirent entry;

	fs_dir_t_init(&dirp);

	/* Verify fs_opendir() */
	res = fs_opendir(&dirp, path);
	if (res) {
		LOG(EERROR, "Error opening dir %s [%d]", path, res);
		return res;
	}

	LOG(EDEBUG, "Listing dir %s ...", path);
	for (;;) {
		/* Verify fs_readdir() */
		res = fs_readdir(&dirp, &entry);

		/* entry.name[0] == 0 means end-of-dir */
		if (res || entry.name[0] == 0) {
			if (res < 0) {
				LOG(EERROR, "Error reading dir [%d]", res);
			}
			break;
		}

		if (entry.type == FS_DIR_ENTRY_DIR) {
			LOG(EDEBUG, "[DIR ] %s", entry.name);
		} else {
			LOG(EDEBUG, "[FILE] %s (size = %zu)",
				   entry.name, entry.size);
		}
	}

	/* Verify fs_closedir() */
	fs_closedir(&dirp);

	return res;
}


int user_flash_init()
{
    struct fs_statvfs sbuf;
    int rc;

    /* 挂载文件系统 */
    rc = littlefs_mount(mp);
    if (rc < 0) {
        LOG(EERROR, "fs mount failed.");
        return -1;
    }

    /* 检索文件系统的信息,返回文件系统中的总空间和可用空间。 */
    rc = fs_statvfs(mp->mnt_point, &sbuf);
    if (rc < 0) {
        LOG(EERROR, "FAIL: statvfs: %d", rc);
        return -1;
    }

    LOG(EDEBUG, "%s: bsize = %lu ; frsize = %lu ;"
           " blocks = %lu ; bfree = %lu",
           mp->mnt_point,
           sbuf.f_bsize, sbuf.f_frsize,
           sbuf.f_blocks, sbuf.f_bfree);

    /* 检索文件系统的目录 */
    rc = lsdir(mp->mnt_point);
    if (rc < 0) {
        LOG(EERROR, "FAIL: lsdir %s: %d\n", mp->mnt_point, rc);
    }

    return rc;
}


int user_flash_uninit(void)
{
    /* 卸载文件系统 */
    int rc = fs_unmount(mp);
    LOG(EDEBUG, "%s unmount: %d", mp->mnt_point, rc);

    return rc;
}
