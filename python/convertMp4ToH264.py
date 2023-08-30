import os
import shutil
import argparse

def get_file_list(path, video_type):
    """
    获取所有视频文件
    @param path: 视频文件夹路径
    @param video_type: 文件后缀名
    @return: 视频文件列表
    """
    if not os.path.exists(path=path):
        print("视频文件夹路径不存在")

    file_list = os.listdir(path)
    return [os.path.join(path, file) for file in file_list if file.endswith(video_type)]

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="convert mp4 videos to h264 videos")
    parser.add_argument('--input_dir', type=str, default='', help='mp4文件夹路径')
    parser.add_argument('--output_dir', type=str, default='', help='h264文件夹路径')
    args = parser.parse_args()
    input_dir = args.input_dir
    output_dir = args.output_dir

    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    input_file_list = get_file_list(path=input_dir, video_type='mp4')
    # print(input_file_list)
    for file in input_file_list:
        file_name = os.path.basename(file).split('.mp4')[0]
        output_file_path = os.path.join(output_dir, file_name + '.h264')
        os.system(f'ffmpeg -i {file} -c:v libx264 {output_file_path}')