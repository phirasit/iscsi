#!/bin/bash
iscsiadm --mode discovery -t sendtargets -p 127.0.0.1:4000
