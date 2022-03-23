#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys

list_file = 'assets_to_downsample.txt'
remote = 'huyuexiang@172.18.97.123:/Users/huyuexiang/workspace/data/makeup_hight_resolution/half_size_xiurong_saihong/magic_assets/origin_assets'

with open(list_file, 'r') as f:
    for line in f.readlines():
        asset = line.split()[0].replace('-unzip', '') + '.zip'
        if os.path.exists(asset):
            parent = asset.split('/')[1]
            dst_dir = '/Users/huyuexiang/workspace/data/makeup_hight_resolution/half_size_xiurong_saihong/magic_assets/origin_assets/' + parent
            cmd_scp = 'scp ' + asset + ' huyuexiang@172.18.97.123:' + dst_dir
            os.system(cmd_scp)
            print(cmd_scp)
