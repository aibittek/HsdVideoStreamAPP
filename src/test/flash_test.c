/*
 * @Author: AIoTMaker kuili@iflytek.com
 * @Date: 2022-11-30 16:09:16
 * @LastEditors: AIoTMaker kuili@iflytek.com
 * @LastEditTime: 2022-12-01 11:56:59
 * @FilePath: \HsdVideoStreamAPP\src\test\flash_test.c
 * @Description: 
 * 
 * Copyright (c) 2022 by AIoTMaker kuili@iflytek.com, All Rights Reserved. 
 */
/*
 * @Author: AIoTMaker kuili@iflytek.com
 * @Date: 2022-11-30 16:09:16
 * @LastEditors: AIoTMaker kuili@iflytek.com
 * @LastEditTime: 2022-11-30 17:28:37
 * @FilePath: \HsdVideoStreamAPP\src\test\flash_test.c
 * @Description: 
 * 
 * Copyright (c) 2022 by AIoTMaker kuili@iflytek.com, All Rights Reserved. 
 */
#include <log.h>
#include <zephyr/fs/fs.h>
#include <flash_core.h>

#define MAX_PATH_LEN            255
#define FLASH_MAX_DATA_LEN      256
#define FLASH_TEST_DATA         "this is a flash read/write test data"
#define FLASH_TEST_FILE_NAME    "/lfs/test.txt"

void flash_test()
{
    int rc;
    struct fs_dirent dirent;
    char* fname = FLASH_TEST_FILE_NAME;
    char test_data[FLASH_MAX_DATA_LEN] = {0};
    struct fs_file_t file = {0};    /*!< 初始化结构体 */

    // 初始化挂载flash设备到/lfs节点
    user_flash_init();

    /* 如果文件先删除 */
    fs_unlink(fname);

    /* 打开文件 */
    rc = fs_open(&file, fname, FS_O_CREATE | FS_O_RDWR);
    if (rc < 0) {
        LOG(EERROR, "FAIL: open %s: %d", fname, rc);
        return ;
    }

    /* 获取文件状态信息 */
    rc = fs_stat(fname, &dirent);
    if (rc < 0) {
        LOG(EERROR, "FAIL: stat %s: %d", fname, rc);
        goto out;
    }

    /* 写文件内容 */
    rc = fs_write(&file, FLASH_TEST_DATA, strlen(FLASH_TEST_DATA));
    if (rc < 0) {
        LOG(EERROR, "FAIL: write %s: %d", fname, rc);
    }

    /* 重定向文件读写指针到开始 */
    rc = fs_seek(&file, 0, FS_SEEK_SET);
    if (rc < 0) {
        LOG(EERROR, "FAIL: seek %s: %d", fname, rc);
        goto out;
    }

    /* 读文件内容 */
    rc = fs_read(&file, test_data, sizeof(test_data));
    if (rc < 0) {
        LOG(EERROR, "FAIL: read %s: [rd:%d]", fname, rc);
        goto out;
    }
    LOG(EDEBUG, "read from %s, data:%s", fname, test_data);

out:
    fs_close(&file);
}
