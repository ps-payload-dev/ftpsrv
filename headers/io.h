/* Copyright (C) 2025 John Törnblom
   Copyright (C) 2026 ItsJokerZz

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING. If not, see
<http://www.gnu.org/licenses/>.  */

#pragma once

#include <stddef.h>

#define IO_COPY_BUFSIZE 0x100000

void *mem_alloc(size_t n);

void mem_free(void *ptr);

int io_nread(int fd, void *buf, size_t n);

int io_nwrite(int fd, const void *buf, size_t n);

int io_ncopy(int fd_in, int fd_out, size_t size);

int io_sendfile(int fd, int sock, off_t off, size_t n);

int io_pread(int fd, void *buf, size_t n, off_t off);

int io_pwrite(int fd, const void *buf, size_t n, off_t off);

int io_pcopy(int fd_in, int fd_out, off_t off_in, off_t off_out, size_t n);
