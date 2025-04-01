### Host (Linux)
```
sudo python3 -m pyftpdlib -p 21 -u target -P vxTarget -d $HOME/<path-to-workspace>
```
### Target (Raspberry Pi 4B)
```
netDevCreate ("/wrs", "192.168.0.15", 1) -> FTP server is hosted at $HOME/<path-to-workspace>
cmd
cd /wrs
cd <path-to-workspace>/<path-to-project>
project.vxe
```
