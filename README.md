# ftpsrv

This is a simple FTP server that can be executed on a PS4 or PS5 that has been
jailbroken and is running an ELF loader. The FTP server accepts connections on
port 2121.

This project is based on [John Törnblom](https://github.com/john-tornblom)'s
[ftpsrv project][upstream]. It builds with the unified [payload SDK][sdk].

## Features

Client software that has been tested include gFTP, FileZilla, curl, and Thunar.
The payload supports a couple of custom SITE commands specifically for the
PS4 and PS5 (executed without prepending SITE). In particular:

- `KILL` - kill the FTP server.
- `MTRW` - remount `/system` and `/system_ex` with write permissions.
- `SELF` - toggle on-the-fly SELF to ELF decryption (enabled by default).

Note: the SELF command operates on individual connections so use e.g.,

```console
curl -o libkernel.sprx -Q SELF ftp://ps5:2121/system/common/lib/libkernel.sprx
```

## Building for the PS4

Assuming you have the [payload SDK][sdk] installed on a POSIX machine, the FTP
server can be compiled using the following commands:

```console
export PS4_PAYLOAD_SDK=/opt/ps-payload-sdk/ps4
make PLATFORM=ps4
```

The build writes `build/ps4/ftpsrv-ps4.elf` and
`build/ps4/ftpsrv-ps4-install.elf`.

## Building for the PS5

Assuming you have the [payload SDK][sdk] installed on a POSIX machine, the FTP
server can be compiled using the following commands:

```console
export PS5_PAYLOAD_SDK=/opt/ps-payload-sdk/ps5
make PLATFORM=ps5
```

The build writes `build/ps5/ftpsrv-ps5.elf` and
`build/ps5/ftpsrv-ps5-install.elf`.

## Building for GNU/Linux systems

Assuming you have a compiler toolchain installed on your GNU/Linux system, the
FTP server can be compiled using the following command:

```console
make PLATFORM=linux
```

Object files are placed under `build/<platform>/objects/`. One GitHub Actions
workflow builds both consoles with the [payload SDK][sdk] and uploads separate
`ftpsrv-ps4` and `ftpsrv-ps5` artifacts.

## Running

Start an ELF loader on your console, then send the matching ELF to its host and
port (9021 by default). For PS5:

```console
PS5_HOST=ps5
PS5_PORT=9021
nc -q0 "$PS5_HOST" "$PS5_PORT" < build/ps5/ftpsrv-ps5.elf
```

For PS4, use `build/ps4/ftpsrv-ps4.elf`. Once the server is running, connect an
FTP client to port 2121:

```console
curl ftp://ps5:2121/
```

## Installing on the PS4 and PS5

ftpsrv can be installed to disk and then launched from the standard Sony shell
UI assuming [websrv][websrv] is running. To perform the install, deploy the
install payload (`ftpsrv-ps4-install.elf` on PS4, `ftpsrv-ps5-install.elf` on
PS5) to an ELF loader, e.g., on PS5:

```console
PS5_HOST=ps5
PS5_PORT=9021
nc -q0 "$PS5_HOST" "$PS5_PORT" < build/ps5/ftpsrv-ps5-install.elf
```

If the install is successful, a new item appears in the Sony shell UI that can
be used to launch ftpsrv when [websrv][websrv] is running.

## Known issues

- Some PS5 firmwares below version 4 contain a kernel bug where reading from
  some SELF files causes the read syscall to stall.
- PS5 SELF files on the root (e.g., `/SceSysAvControl.elf`) cannot be
  decrypted. To work around this, copy the encrypted file to `/data/` first and
  download it from there instead and it should be decrypted.

## Reporting Bugs

If you encounter problems with ftpsrv, please [file a github issue][issues].
If you plan on sending pull requests which affect more than a few lines of
code, please file an issue before you start to work on your changes. This will
allow us to discuss the solution properly before you commit time and effort.

## License

ftpsrv is licensed under the GPLv3+.

[sdk]: https://github.com/ps-payload-dev/sdk
[websrv]: https://github.com/ps-payload-dev/websrv
[issues]: https://github.com/ps-payload-dev/ftpsrv/issues/new
[upstream]: https://github.com/ps5-payload-dev/ftpsrv
