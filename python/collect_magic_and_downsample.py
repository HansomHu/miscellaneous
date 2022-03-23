#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import cv2
import json
import os
import shutil
import sys
import argparse
import numpy as np

TOTAL = 100000
count = 0

# get_file_or_dir: 0 - 文件， 1 - 文件夹
def walk_through(dir, get_file_or_dir = 0):
    results = []
    for filename in os.listdir(dir):
        if get_file_or_dir == 0:
            if os.path.isfile(os.path.join(dir, filename)):
                results.append(os.path.join(dir, filename))
        else:
            if os.path.isdir(os.path.join(dir, filename)):
                results.append(os.path.join(dir, filename))
    return results

# 检查dg_makeup中的素材尺寸，有大尺寸的，则将image和tex文件的相对路径列表返回
def get_large_tex_files(dg_makeup):
    try:
        with open(os.path.join(dg_makeup, 'config.json')) as f:
            config = json.load(f)
            img_and_tex = []
            for filter in config['filters']:
                if 'parts' in filter:
                    if 'INNERFACE' in filter['parts'] or 'OUTERFACE' in filter['parts']:
                        if 'img' in filter:
                            #  if filter['img'].startswith('xr') or filter['img'].startswith('sh'):
                            if not filter['img'].startswith('shang'):
                                img_path = os.path.join(dg_makeup, filter['img'])
                                if os.path.exists(img_path) and os.path.isfile(img_path):
                                    mat = cv2.imread(img_path, cv2.IMREAD_UNCHANGED)
                                    # 考虑到有的素材是单通道的图片，其shape不存在3个值
                                    h, w, c = mat.shape if len(mat.shape) > 2 else [mat.shape[0], mat.shape[1], 1]
                                    if h >= 400 or w >= 400:
                                        parent = dg_makeup[dg_makeup.rfind('dg_makeup'):]
                                        img_and_tex.append(os.path.join(parent, filter['img']))
                                        img_and_tex.append(os.path.join(parent, filter['tex']))
            return img_and_tex
    except FileNotFoundError:
        print(f'Error: {dg_makeup}/config.json 不存在!!')
        return []

# 检查每个魔表素材，输出包含dg_makeup && 素材尺寸超过指定值，则按照该格式返回['素材绝对路径', 'img相对路径', 'tex相对路径', ...]
def analyze_magic_assets(asset):
    # parse params.txt to get dg_makeup list
    record = [asset]
    dg_makeups = []
    try:
        with open(os.path.join(asset, 'params.txt')) as f:
            config = json.load(f)
            for filter in config['filters']:
                if filter.startswith('dg_makeup'):
                    dg_makeups.append(os.path.join(asset, filter))
    except FileNotFoundError:
        print(f'Error: {asset}/params.txt 不存在')

    for dir in dg_makeups:
        files = get_large_tex_files(dir)
        if len(files):
            record += files
    if len(record) == 1:
        record = []
    return record

# records 的格式为['素材绝对路径', 'img相对路径', 'tex相对路径', ...]
def analyze_memory(records, file_path, scale):
    memory_list = []
    for record in records:
        asset = record[0]
        mem = [asset]
        mem_ori, mem_opt = 0, 0
        for _ in record[1:]:
            if _.endswith('.png'):
                mat = cv2.imread(os.path.join(asset, _), cv2.IMREAD_UNCHANGED)
                # 考虑到有的素材是单通道的图片，其shape不存在3个值
                h, w, c = mat.shape if len(mat.shape) > 2 else [mat.shape[0], mat.shape[1], 1]
                mem_ori += w * h * c / 1024 / 1024
        mem_opt = mem_ori * scale * scale
        mem += [mem_ori, mem_opt]
        memory_list.append(mem)
    sum_ori, sum_opt, sum_cnt = 0, 0, 0
    for _ in memory_list:
        sum_ori += _[1]
        sum_opt += _[2]
        sum_cnt += 1
    memory_list.append(['++++ statistic: TOTAL: ',sum_ori, sum_opt, 'AVERAGE OTPIMIZED: ', (sum_ori - sum_opt) / sum_cnt, '++++'])
    with open(file_path, 'w+') as f:
        f.seek(0)
        f.truncate()
        for _ in memory_list:
            line = ' '.join(str(x) for x in _) + '\n'
            f.write(line)

def downscale_img(src_path, dst_path, scale):
    img_src = cv2.imread(src_path, cv2.IMREAD_UNCHANGED)
    img_scale = cv2.resize(img_src, None, fx = scale, fy = scale, interpolation=cv2.INTER_AREA)
    cv2.imwrite(dst_path, img_scale)
    print(img_scale.shape)

def downscale_tex(src_path, dst_path, scale):
    if os.path.exists(src_path):
        line = ''
        with open(src_path, 'r') as f:
            data_str = f.read().replace(',', ' ').replace(';', ' ').replace(':', ' ').split()
            data = [float(i) for i in data_str]
            for i in range(len(data) // 3):
                idx = int(data[i * 3 + 0])
                x = data[i * 3 + 1] * scale
                y = data[i * 3 + 2] * scale
                line += str(idx) + ':' + str(x) + ',' + str(y) + ';'
        with open(dst_path, 'w+') as f:
            f.seek(0)
            f.truncate()
            f.write(line)
            # print(line)

# elements数组内的数据元素格式为：['素材绝对路径', 'img相对路径', 'tex相对路径', ...]
# src_root_path 和 target_root_path是素材包的父路径
def get_dst_config_json_files(elements, src_root_path, target_root_path):
    json_files = []
    for _ in elements[1:]:
        src_path = os.path.join(elements[0], _)
        dst_path = src_path.replace(src_root_path, target_root_path)
        json_path = dst_path[:dst_path.rfind('/')] + "/config.json"
        if json_path not in json_files:
            json_files.append(json_path)
    return json_files

def add_need_scale_to_json_files(json_files):
    for _ in json_files:
        with open(_, 'r+') as f:
            config = json.load(f)
            parent = _[_.rfind('dg_makeup'):]
            parent = parent[:parent.rfind('/')]
            for filter in config['filters']:
                if 'img' in filter:
                    img_relative_path = os.path.join(parent, filter['img'])
                    if img_relative_path in elements:
                        filter['needScale'] = False
            f.seek(0)
            f.truncate()
            # ensure_ascii=False是为了防止输出字符串中汉字变成unicode源码
            json.dump(config, f, indent=4, ensure_ascii=False, sort_keys=True)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="从指定魔表素材库中收集包含美妆，并且包含大尺寸修容、腮红的素材，并将对应png缩小指定倍数")
    parser.add_argument('--update', '-u', help='更新目标素材列表，其内容为尺寸符合要求的魔表素材路径', action='store_true')
    parser.add_argument('--downsample', '-d', help='开始下采样素材', action='store_true')
    parser.add_argument('--copy', '-c', help='执行拷贝源素材动作', action='store_true')
    parser.add_argument('--magic', '-m', help='指定的魔表素材库')
    parser.add_argument('--list', '-l', help='目标素材列表', default='xr_and_sh_assets.txt')
    parser.add_argument('--scale', '-s', help='下采样倍数', type=float, default=0.5)
    parser.add_argument('--target', '-t', help='将目标素材列表中的素材缩小后的新的素材库路径', default='downsampled')
    parser.add_argument('--add_needscale', '-a', help='当启用--downsample参数时，修改config.json文件，在对应的filter中添加needScale字段', action='store_true')
    args = parser.parse_args()
    update = args.update
    downsample = args.downsample
    copy = args.copy
    magic = args.magic
    list_file = args.list
    scale = args.scale
    target = args.target
    add_needscale = args.add_needscale

    if update:
        print(f'开始更新目标素材列表: {list_file}')
        if not magic or not os.path.exists(magic):
            print(f'{magic} 路径不存在，请输入有效路径')
            exit(-1)
        asset_dirs = []
        for _ in walk_through(magic, 1):
            assets = walk_through(_, 1)
            asset_dirs += assets
        #  for _ in walk_through(magic, 1):
           #  asset_dirs += [_]

        records = []
        for asset in asset_dirs:
            if count < TOTAL:
                count += 1
                print(f'Analyzing {asset} ...')
                record = analyze_magic_assets(asset)
                if len(record):
                    records.append(record)
        with open(list_file, 'w+') as f:
            f.seek(0)
            f.truncate()
            for record in records:
                line = str.join(' ', record) + '\n'
                f.write(line)
        analyze_memory(records, list_file.split('.')[0] + '_memory.txt', 0.25)
        exit(0)

    if downsample:
        print(f'开始下采样 ...\n\t素材列表：{list_file}\n\t源素材根目录：{magic}\n\t下采样素材根目录：{target}\n\t下采样倍数：{scale}\n\t')
        try:
            with open(list_file, 'r') as f:
                for line in f.readlines():
                    elements = line.split()
                    ori_dir = elements[0]
                    dst_dir = ori_dir.replace(magic, target)
                    if copy:
                        if os.path.exists(dst_dir): shutil.rmtree(dst_dir)
                        print(f'Copy {ori_dir} to {dst_dir} ...') 
                        shutil.copytree(ori_dir, dst_dir)
                    for _ in elements[1:]:
                        src_path = os.path.join(elements[0], _)
                        dst_path = src_path.replace(magic, target)
                        print(f'Downscale {src_path} to {dst_path} scale: {scale} ...')
                        if src_path.endswith('.tex'):
                            downscale_tex(src_path, dst_path, scale)
                        else:
                            downscale_img(src_path, dst_path, scale)
                    if add_needscale == True:
                        json_files = get_dst_config_json_files(elements, magic, target)
                        add_need_scale_to_json_files(json_files)
        except Exception as e:
            print(repr(e), list_file)
            exit(1)
