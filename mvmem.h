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

#ifndef __MVMEM_H__
#define __MVMEM_H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

int mvmem_alloc(size_t len, unsigned int _flags, size_t align);
int mvmem_free(int handle_fd);
int mvmem_set_name(int handle_fd, const char *name);
int mvmem_get_phys(int handle_fd, uint32_t *phys);
int mvmem_get_dma_addr(int handle_fd, uint32_t *dma);
int mvmem_sync(int handle_fd, int fd);
int mvmem_sync_range(int handle_fd, uint32_t offset, uint32_t size, uint32_t note);
void* mvmem_mmap(int fd, size_t length, off_t offset);
int mvmem_munmap(void *addr, size_t length);
int mvmem_notify(int handle_fd, uint32_t note);
int mvmem_set_usage(int fd, uint32_t note);
int mvmem_get_usage(int fd, uint32_t *note);

#ifdef __cplusplus
}
#endif

#endif
