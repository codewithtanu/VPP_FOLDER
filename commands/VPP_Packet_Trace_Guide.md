# VPP Packet Trace Setup Guide (AF_PACKET - VM Friendly)

## 1️⃣ Enter VPP CLI

``` bash
sudo vppctl   # Open VPP CLI
```

------------------------------------------------------------------------

## 2️⃣ Create Host Interface (AF_PACKET)

``` bash
create host-interface name eth0   # Create VPP host interface from Linux eth0
set interface state host-eth0 up   # Bring VPP interface up
set interface ip address host-eth0 192.168.1.200/24   # Assign IP to VPP interface
```

------------------------------------------------------------------------

## 3️⃣ Verify Interface and IP

``` bash
show interface   # Show all VPP interfaces
show interface address   # Show IP addresses configured in VPP
```

------------------------------------------------------------------------

## 4️⃣ Enable Packet Trace

``` bash
trace add af-packet-input 5   # Trace next 5 packets entering VPP
```

------------------------------------------------------------------------

## 5️⃣ Generate Traffic (Run in Linux Terminal)

``` bash
ping 192.168.1.200   # Send packets to VPP
```

------------------------------------------------------------------------

## 6️⃣ View Packet Path in VPP

``` bash
show trace   # Display node-by-node packet processing path
```

------------------------------------------------------------------------

# 🔥 If You Want dpdk-input Instead

## Required Steps (Linux Terminal)

``` bash
lspci | grep -i ethernet   # Find NIC PCI address
sudo modprobe vfio-pci   # Load DPDK driver module
sudo dpdk-devbind.py --bind=vfio-pci <PCI-ID>   # Bind NIC to DPDK
```

## Edit VPP Config

Edit `/etc/vpp/startup.conf` and add:

    dpdk {
      dev <PCI-ID>   # Tell VPP to use this NIC with DPDK
    }

## Restart VPP

``` bash
sudo systemctl restart vpp   # Restart VPP service
```

Then inside VPP:

``` bash
trace add dpdk-input 5   # Trace packets entering via DPDK
```

------------------------------------------------------------------------

# 🧠 Summary

-   `af-packet-input` → Used when creating host interface (VM friendly)
-   `dpdk-input` → Used when NIC is bound to DPDK driver
-   `show trace` → Shows exact node order packet passed through
