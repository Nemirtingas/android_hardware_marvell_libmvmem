/*
 * Copyright (C) 2016 The CyanogenMod Project
 *               2017 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sys/mman.h>
#include <ion/ion.h>
#include <linux/ion.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "mvmem.h"

#ifdef LOG
#define LOG_TAG "MVMEM"
#include <cutils/log.h>
#else
#define ALOGE(...)
#define ALOGI(...)
#define ALOGW(...)
#define ALOGD(...)
#define ALOGF(...)
#endif


static int ion_fd = -1;
static pthread_once_t ion_initialized = PTHREAD_ONCE_INIT;

void ion_init()
{
    ion_fd = ion_open();
}

int ion_open_lock()
{
    pthread_once(&ion_initialized, ion_init);
    return ion_fd;
}

int mvmem_alloc(size_t len, unsigned int _flags, size_t align)
{
    int ion_fd = ion_open_lock();
    int heap_mask;
    int flags = 0;
    int handle_fd;

    ALOGE("%s",__FUNCTION__);
    if( _flags & 2 )
        heap_mask = ION_HEAP_SYSTEM_MASK;
    else
        heap_mask = ION_HEAP_CARVEOUT_MASK;


    if( _flags & 0x20000 )
        flags |= ION_FLAG_CACHED_NEEDS_SYNC;


    if( ion_fd < 0 || ion_alloc_fd(ion_fd, len, align, heap_mask, flags, &handle_fd) )
        handle_fd = -2;

    return handle_fd;
}

int mvmem_free(int handle_fd)
{
    if( handle_fd < 0 )
        return -EINVAL;

    close(handle_fd);
    return 0;
}

int mvmem_set_name(int handle_fd, const char *name)
{
    struct ion_buffer_name_data param;
    int ion_fd = ion_open_lock();

    ALOGE("%s",__FUNCTION__);
    if( ion_fd < 0 )
        return -ENOENT;

    param.fd = handle_fd;
    strncpy(param.name, name, 16);
    return ioctl(ion_fd, ION_IOC_NAME, &param);
}

int mvmem_get_phys(int handle_fd, int *phys)
{
    struct ion_phys_data param;
    int ion_fd = ion_open_lock();
    int result;

    ALOGE("%s",__FUNCTION__);
    if( ion_fd < 0 )
        return -ENOENT;

    param.fd = handle_fd;
    param.flags = ION_BUFFER_TYPE_PHYS;
    result = ioctl(ion_fd, ION_IOC_PHYS, &param);
    if( !result )
        *phys = param.addr;

    return result;
}

int mvmem_get_dma_addr(int handle_fd, int *dma)
{
    int ion_fd = ion_open_lock();
    struct ion_phys_data param;
    int result;

    ALOGE("%s",__FUNCTION__);
    if( ion_fd < 0 )
        return -ENOENT;

    ALOGE("%s",__FUNCTION__);

    param.fd = handle_fd;
    param.flags = ION_BUFFER_TYPE_DMA;
    result = ioctl(ion_fd, ION_IOC_PHYS, &param);
    if( !result )
        *dma = param.addr;

    return result;
}

int mvmem_sync(int handle_fd, int fd)
{
    struct ion_fd_data param;
    int ion_fd = ion_open_lock();

    ALOGE("%s",__FUNCTION__);
    if( ion_fd < 0 )
        return -ENOENT;

    param.handle = handle_fd;
    param.fd = fd;

    return ioctl(ion_fd, ION_IOC_SYNC, &param);
}

int mvmem_sync_range(int handle_fd, int offset, int size, int note)
{
    struct ion_sync_range_data param;
    int ion_fd = ion_open_lock();

    ALOGE("%s",__FUNCTION__);
    if( ion_fd < 0 )
        return -ENOENT;

    param.fd = handle_fd;
    param.offset = offset;
    param.size = size;
    param.note = note;

    return ioctl(ion_fd, ION_IOC_SYNC_RANGE, &param);
}

void* mvmem_mmap(int fd, size_t length, off_t offset)
{
    return mmap(0, length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, offset);
}

int mvmem_munmap(void *addr, size_t length)
{
    return munmap(addr, length);
}

int mvmem_notify(int handle_fd, int note)
{
    int ion_fd = ion_open_lock();
    int result;
    struct ion_notify_data param;

    ALOGE("%s",__FUNCTION__);
    if( ion_fd < 0 )
        return -ENOENT;

    param.fd = handle_fd;
    param.note = note;
    result = ioctl(ion_fd, ION_IOC_NOTIFY, &param);
    if( !result )
        result = param.note;

    return result;
}

int mvmem_set_usage(int fd, int note)
{
    int result = mvmem_notify(fd, note);
    return (result & (result>>31));
}

int mvmem_get_usage(int fd, int *note)
{
    int result;

    result = mvmem_notify(fd, ION_BUFFER_NOTIFY_QUERY);
    if( result >= 0 )
    {
        *note = result;
        result = 0;
    }

    return result;
}
