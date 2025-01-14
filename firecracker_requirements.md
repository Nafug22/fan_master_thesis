# Hardware Requirements:
**CPU Architecture**: x86_64 or aarch64.  
**KVM Support**: KVM module enabled with read/write access to the `/dev/kvm`.  

# Software Requirements:
**Operating System**: Ubuntu 24.04, A Linux-based host with a kernel version 5.10, and 6.1  
**Key kernel features**: /dev/net/tun (for networking), and Virtio device support.  

---
Reference: [Firecracker getting-started](https://github.com/firecracker-microvm/firecracker/blob/main/docs/getting-started.md)