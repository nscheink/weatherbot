apt install -y libnss-resolve
systemctl daemon-reload
systemctl disable --now ifupdown dhcpcd dhcpcd5 isc-dhcp-client isc-dhcp-common rsyslog
apt --autoremove purge -y ifupdown dhcpcd dhcpcd5 isc-dhcp-client isc-dhcp-common rsyslog
rm -r /etc/netw
systemctl disable --now avahi-daemon libnss-mdns
apt --autoremove purge -y avahi-daemon
ln -sf /run/systemd/resolve/stub-resolv.conf /etc/resolv.conf
apt-mark hold avahi-daemon dhcpcd dhcpcd5 ifupdown isc-dhcp-client isc-dhcp-common libnss-mdns openresolv raspberrypi-net-mods rsyslog
systemctl enable systemd-networkd.service systemd-resolved.service

cat > /etc/wpa_supplicant/wpa_supplicant-wlan0.conf <<EOF
country=US
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

network={
    ssid="Weatherbot$RANDOM"
    mode=2
    frequency=2437
    key_mgmt=NONE   # uncomment this for an open hotspot
    # delete next 3 lines if key_mgmt=NONE
    # key_mgmt=WPA-PSK
    # proto=RSN WPA
    # psk="password"
}
EOF

chmod 600 /etc/wpa_supplicant/wpa_supplicant-wlan0.conf
systemctl disable wpa_supplicant.service
systemctl enable wpa_supplicant@wlan0.service
rfkill unblock wlan

cat > /etc/systemd/network/08-wlan0.network <<EOF
[Match]
Name=wlan0
[Network]
Address=192.168.1.1/24
MulticastDNS=yes
DHCPServer=yes
[DHCPServer]
DNS=84.200.69.80 1.1.1.1
EOF

cat > /etc/systemd/network/04-eth0.network <<EOF
[Match]
Name=eth0
[Network]
Address=192.168.2.1/24
MulticastDNS=yes
DHCPServer=yes
[DHCPServer]
DNS=84.200.69.80 1.1.1.1
EOF

shutdown now
