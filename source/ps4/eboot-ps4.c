/* Copyright (C) 2026 ItsJokerZz

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

#define SYS_exit 1
#define SYS_read 3
#define SYS_write 4
#define SYS_open 5
#define SYS_close 6
#define SYS_socket 97
#define SYS_connect 98

#define AF_INET 2
#define SOCK_STREAM 1
#define O_RDONLY 0

#define LOADER_PORT 9021

long
__syscall(long n, ...) {
  long a1 = 0, a2 = 0, a3 = 0, a4 = 0, a5 = 0, a6 = 0;
  __builtin_va_list ap;
  unsigned long ret;
  char iserror;

  __builtin_va_start(ap, n);
  a1 = __builtin_va_arg(ap, long);
  a2 = __builtin_va_arg(ap, long);
  a3 = __builtin_va_arg(ap, long);
  a4 = __builtin_va_arg(ap, long);
  a5 = __builtin_va_arg(ap, long);
  a6 = __builtin_va_arg(ap, long);
  __builtin_va_end(ap);

  register long r10 __asm__("r10") = a4;
  register long r8 __asm__("r8") = a5;
  register long r9 __asm__("r9") = a6;

  __asm__ __volatile__("syscall"
                       : "=a"(ret), "=@ccc"(iserror), "+r"(r10), "+r"(r8),
                         "+r"(r9)
                       : "a"(n), "D"(a1), "S"(a2), "d"(a3)
                       : "rcx", "r11", "memory");

  return iserror ? -ret : ret;
}

static int
klog(const char *s) {
  return __syscall(0x259, 7, s, 0);
}

static int
send_all(int fd, const char *buf, long len) {
  while(len > 0) {
    long n = __syscall(SYS_write, fd, buf, len);
    if(n <= 0) {
      return -1;
    }
    buf += n;
    len -= n;
  }

  return 0;
}

static int
connect_loader(void) {
  struct {
    unsigned char sin_len;
    unsigned char sin_family;
    unsigned short sin_port;
    unsigned int sin_addr;
    unsigned char sin_zero[8];
  } addr;
  unsigned char *p = (unsigned char *)&addr;
  int fd;

  for(unsigned i = 0; i < sizeof(addr); i++) {
    p[i] = 0;
  }

  addr.sin_len = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_port
      = (unsigned short)(((LOADER_PORT & 0xff) << 8) | (LOADER_PORT >> 8));
  addr.sin_addr = 0x0100007f;

  if((fd = __syscall(SYS_socket, AF_INET, SOCK_STREAM, 0)) < 0) {
    return -1;
  }

  if(__syscall(SYS_connect, fd, &addr, sizeof(addr)) < 0) {
    __syscall(SYS_close, fd);
    return -1;
  }

  return fd;
}

static int
open_payload(void) {
  const char *paths[] = {
    "/app0/open.elf",
    "/user/app/" TITLE_ID "/open.elf",
  };
  int fd;

  for(unsigned i = 0; i < sizeof(paths) / sizeof(paths[0]); i++) {
    if((fd = __syscall(SYS_open, paths[i], O_RDONLY, 0)) >= 0) {
      return fd;
    }
  }

  return -1;
}

int
main(void) {
  char buf[4096];
  int in, out;
  long n;

  klog("ftpsrv: eboot started\n");

  if((in = open_payload()) < 0) {
    klog("ftpsrv: unable to open open.elf\n");
    return 1;
  }

  klog("ftpsrv: opened open.elf\n");

  if((out = connect_loader()) < 0) {
    klog("ftpsrv: unable to connect to the ELF loader\n");
    __syscall(SYS_close, in);
    return 1;
  }

  klog("ftpsrv: connected to the ELF loader\n");

  while((n = __syscall(SYS_read, in, buf, sizeof(buf))) > 0) {
    if(send_all(out, buf, n)) {
      klog("ftpsrv: failed to send open.elf\n");
      break;
    }
  }

  __syscall(SYS_close, in);
  __syscall(SYS_close, out);
  klog("ftpsrv: sent open.elf\n");

  return 0;
}

void
_start(void) {
  __syscall(SYS_exit, main());
  __builtin_trap();
}
