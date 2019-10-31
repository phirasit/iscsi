#!/bin/bash
sudo iscsiadm -m node --targetname iqn.2019-10.chula.ac.th:iscsi-disk.sn.12345 -p 127.0.0.1:4000 -l
