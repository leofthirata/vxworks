### Host (Linux)
```
sudo python3 -m pyftpdlib -p 21 -u target -P vxTarget -d $HOME/<path-to-workspace>
sudo python3 -m pyftpdlib -p 21 -u target -P vxTarget -d $HOME/workspace/vxworks-work/

pyftpdlib: python FTP server library
-p: PORT (21)
-u: username (target)
-P: password (vxTarget)
-d: directory ($HOME/workspace/vxworks-work/)
```
### Target (Raspberry Pi 4B)
#### RTP
```
netDevCreate ("/wrs", "192.168.0.15", 1) -> FTP server is hosted at $HOME/<path-to-workspace>
cmd
cd /wrs
cd <path-to-workspace>/<path-to-project>
project.vxe
```

#### DKM
#### RTP
```
netDevCreate ("/wrs", "192.168.0.15", 1) -> FTP server is hosted at $HOME/<path-to-workspace>
cmd
cd /wrs
cd <path-to-workspace>/<path-to-project>
ld app.o
C main
```

### Read device tree binary file .dtb
sudo apt install device-tree-compiler
fdtdump ./rpi-4b.dtb > test.txt