#!/bin/bash

cd /home/root/
systemctl stop mrt.base
mv /home/root/lib_mem_tracker.so /usr/lib/fw/RS.bbmpp/
mv /usr/lib/fw/RS.bbmpp/bbmpp /usr/lib/fw/RS.bbmpp/bbmpp.bin
mv /home/root/bbmpp_tracker_script.sh /usr/lib/fw/RS.bbmpp/bbmpp
chmod 755 /usr/lib/fw/RS.bbmpp/bbmpp

systemctl start mrt.base

screen /home/root/mem_monitor -n bbmpp $@
