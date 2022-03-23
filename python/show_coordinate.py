import cv2
import numpy as np
import sys

file_name=sys.argv[1]
img = cv2.imread(file_name)
WIDTH, HEIGHT = img.shape[0], img.shape[1]
a =[]
b = []
def on_EVENT_LBUTTONDOWN(event, x, y,flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        xy = "%.4f,%.4f - %d,%d" % (x / WIDTH, y / HEIGHT, x, y)
        a.append(x)
        b.append(y)
        cv2.circle(img, (x, y), 1, (255, 0, 0), thickness=-1)
        cv2.putText(img, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
                    1.0, (0, 0, 255), thickness=2)
        cv2.imshow("image", img)

cv2.namedWindow("image")
cv2.setMouseCallback("image", on_EVENT_LBUTTONDOWN)
cv2.imshow("image", img)
cv2.waitKey(0)
print(a[0],b[0])

img[b[0]:b[1],a[0]:a[1],:] = 0   #注意是 行，列（y轴的，X轴）
cv2.imshow("image", img)
cv2.waitKey(0)
print (a,b)
