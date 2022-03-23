#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import argparse
import zipfile

def unzip_file(filepath, unzip_filepath):
    try:
        if not zipfile.is_zipfile(filepath):
            print("{} is not a zip file".format(filepath))
            return False
        with zipfile.ZipFile(filepath, "r") as zip_file:
            if len(zip_file.namelist()) == 0:
                print("zip file is empty!!!")
                return False
            for file in zip_file.namelist():
                zip_file.extract(file, unzip_filepath)
        return True
    except Exception as e:
        print(repr(e))
        return False

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

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='从指定魔表素材库解压zip包到指定路径')
    parser.add_argument('--zippath', '-z', help='待解压的素材库根目录', required=True)
    parser.add_argument('--unzippath', '-u', help='解压后的素材库根目录', required=True)
    args = parser.parse_args()
    zippath = args.zippath
    unzippath = args.unzippath

    if not os.path.exists(zippath):
        print(f'Error {zippath} 不存在！！！')
        exit(1)
    print(f'开始解压 {zippath} ...')
    for dir in walk_through(zippath, 1):
        for file in walk_through(dir):
            unzip_dir = file.replace(zippath, unzippath).replace('.zip', '')
            print(unzip_dir)
            unzip_file(file, unzip_dir)
    print('解压完毕')
