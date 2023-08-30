# Python 3 server example
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import parse_qs
from cgi import parse_header, parse_multipart
import os
import time
import base64
import json
import shutil

hostName = "0.0.0.0"
serverPort = 9000
alarmDir = './alarmData'

class Param:
    taskList_ = []
    taskUpdateInfo_ = dict()

g_param = Param

class MyServer(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        self.wfile.write(bytes("<html><head><title>https://pythonbasics.org</title></head>", "utf-8"))
        self.wfile.write(bytes("<p>Request: %s</p>" % self.path, "utf-8"))
        self.wfile.write(bytes("<body>", "utf-8"))
        self.wfile.write(bytes("<p>This is an example web server.</p>", "utf-8"))
        self.wfile.write(bytes("</body></html>", "utf-8"))

    def do_POST(self):
        #  print(self.headers)
        #  print(self.path)
        ctype, pdict = parse_header(self.headers['content-type'])
        length = int(self.headers['content-length'])
        if self.path == '/api/heartbeat':
            print(self.rfile.read(length).decode('utf-8'))
        elif self.path == '/api/alarmInfo':
            json_root = json.loads(self.rfile.read(length).decode('utf-8'))
            # print(json.dumps(json_root, ensure_ascii=False, indent=2))
            # print('deviceId: %s, key: %s, type: %s, ts: %d' % (json_root['deviceId'], json_root['key'], json_root['type'], json_root['ts']))
            self.store_alarm_info_new(json_root)
        elif self.path == '/openapi/task/HeartBeat':
            json_root = json.loads(self.rfile.read(length).decode('utf-8'))
            # print(json.dumps(json_root, ensure_ascii=False, indent=2))
            self.check_heartbeat(json_root)
        elif self.path == '/BoxCallBack':
            json_root = json.loads(self.rfile.read(length).decode('utf-8'))
            # print(json.dumps(json_root, ensure_ascii=False, indent=2))
            self.storeFaceVideoStreamResult(json_root)
        #  print("ctype", ctype)
        #  print(pdict)
        #  print(self.rfile.read())
        #  postvars = parse_qs(self.rfile.read(length).decode('utf8'), keep_blank_values=1)
        #  print(postvars)
        self.send_response(200)
        self.send_header("Content-type", "application/json")
        self.end_headers()
        self.wfile.write(bytes("{\"code\": 200, \"message\": \"success!\"}", "utf-8"))

    def storeFaceVideoStreamResult(self, jsonObj):
        dataObj = jsonObj['data']
        taskId = dataObj['taskId'] # indicates the video name
        outputBaseDir = './output_dir'
        curOutputDir = os.path.join(outputBaseDir, taskId)
        if not os.path.exists(curOutputDir):
            os.makedirs(curOutputDir)
        for det in dataObj['detections']:
            trackId = det['trackId']
            for attr in det['attrs']:
                classId = attr['classId']
                if classId == 1: # face
                    smallImage = base64.b64decode(det['smallImage'])
                    backgroundImage = base64.b64decode(det['backGroundImage'])
                    faceAttrs = attr['faceAttrs']
                    downType = faceAttrs['downType']
                    timestamp = faceAttrs['timestamp']
                    flag = 'fir' if downType == 1 else 'sec'
                    smallImageName = os.path.join(curOutputDir, trackId + '_' + str(downType) + '_small.jpg')
                    backgroundImageName = os.path.join(curOutputDir, trackId + '_' + str(downType) + '_back.jpg')
                    with open(smallImageName, 'wb') as f:
                        f.write(smallImage)
                    print(f'store small image {smallImageName}')
                    with open(backgroundImageName, 'wb') as f:
                        f.write(backgroundImage)
                    print(f'store background image {backgroundImageName}')


    def check_heartbeat(self, jsonObj):
        dataObj = jsonObj['data']
        serverId = dataObj['serverId']
        timestamp = dataObj['timestamp']
        taskInfo = dataObj['taskInfo']
        recvTaskList = []
        recvUpdateTime = []
        for info in taskInfo:
            recvTaskList.append(info['taskId'])
            recvUpdateTime.append(info['lastUpdateTime'])
        for task in g_param.taskList_:
            if task not in recvTaskList:
                print(f'ERROR: heartbeat does not contain {task}')
        for task, time in zip(recvTaskList, recvUpdateTime):
            if task not in g_param.taskUpdateInfo_:
                print(f'WARNING: {task} is not in {taskList_}')
            else:
                if time == g_param.taskUpdateInfo_[task]:
                    print(f'ERROR: {task} last update time not change: {time}')
                g_param.taskUpdateInfo_[task] = time
        

    def store_alarm_info_new(self, json_obj):
        record = []
        msgName = json_obj['name']
        seqId = json_obj['seqId']
        dataObj = json_obj['data']
        devId = dataObj['device']['deviceId']
        devMac = dataObj['device']['deviceMac']
        devIp = dataObj['device']['deviceIp']
        devId = 'default' if len(devId) == 0 else devId
        devMac = 'default' if len(devMac) == 0 else devMac
        devIp = 'default' if len(devIp) == 0 else devIp
        root_dir = os.path.join(alarmDir, devId)
        img_dir = os.path.join(root_dir, msgName)
        meta_data = os.path.join(root_dir, 'meta_data.txt')
        # if alarmResult is not None:
        if 'alarmResult' in dataObj:
            alarmResult = dataObj['alarmResult']
            ts = alarmResult['timestamp']
            img = base64.b64decode(alarmResult['img'])
            if not os.path.exists(root_dir):
                os.makedirs(root_dir)
            if not os.path.exists(img_dir):
                os.makedirs(img_dir)
            with open(os.path.join(img_dir, str(ts) + '.jpg'), 'wb') as f:
                f.write(img)
            with open(meta_data, 'a') as f:
                timeArray = time.localtime(int(ts) / 1000 - 8 * 3600) # ts is already CST
                timeStr = time.strftime("%Y-%m-%d %H:%M:%S", timeArray)
                record = [devId, devIp, devMac, msgName, str(ts), timeStr]
                line = str.join(',', record) + '\n'
                f.write(line)
        elif 'personStatResult' in dataObj:
            personStatResult = dataObj['personStatResult']
            ts = personStatResult['timestamp']
            startDate = personStatResult['startDate']
            endDate = personStatResult['endDate']
            incoming = personStatResult['incoming']
            outgoing = personStatResult['outgoing']
            if not os.path.exists(root_dir):
                os.makedirs(root_dir)
            with open(meta_data, 'a') as f:
                timeArray = time.localtime(int(ts) / 1000 - 8 * 3600) # ts is already CST
                timeStr = time.strftime("%Y-%m-%d %H:%M:%S", timeArray)
                record = [devId, devIp, devMac, msgName, str(ts), timeStr, startDate, endDate, str(incoming), str(outgoing)]
                line = str.join(',', record) + '\n'
                f.write(line)
        else:
            print(json.dumps(json_obj, ensure_ascii=False, indent=2))


    def store_alarm_info_old(self, json_obj):
        record = []
        devId = json_obj['deviceId']
        devId = 'default' if len(devId) == 0 else devId
        key = json_obj['key']
        alarmType = json_obj['type']
        ts = json_obj['ts']
        img = base64.b64decode(json_obj['img'])
        root_dir = os.path.join(alarmDir, devId)
        img_dir = os.path.join(root_dir, alarmType)
        meta_data = os.path.join(root_dir, 'meta_data.txt')
        if not os.path.exists(root_dir):
            os.makedirs(root_dir)
        if not os.path.exists(img_dir):
            os.makedirs(img_dir)
        with open(os.path.join(img_dir, str(ts) + '.jpg'), 'wb') as f:
            f.write(img)
        with open(meta_data, 'a') as f:
            timeArray = time.localtime(ts / 1000)
            timeStr = time.strftime("%Y-%m-%d %H:%M:%S", timeArray)
            record = [devId, key, alarmType, str(ts), timeStr]
            line = str.join(',', record) + '\n'
            f.write(line)


if __name__ == "__main__":        
    if not os.path.isdir(alarmDir):
        os.mkdir(alarmDir)

    g_param.taskList_ = ['Face_' + str(x) for x in range(50)]
    # g_param.taskList_ = ['Face_0']
    g_param.taskUpdateInfo_ = dict()
    for task in g_param.taskList_:
        g_param.taskUpdateInfo_[task] = ''
        
    webServer = HTTPServer((hostName, serverPort), MyServer)
    print("Server started http://%s:%s" % (hostName, serverPort))

    try:
        webServer.serve_forever()
    except KeyboardInterrupt:
        pass

    webServer.server_close()
    print("Server stopped.")
