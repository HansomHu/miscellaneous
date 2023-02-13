# tkinter documentation: https://tkdocs.com/index.html
from tkinter import *
from tkinter import ttk
from tkinter import filedialog
from PIL import ImageTk,Image

def validation_check(coordtype, coords, img):
    # validation check
    if len(coords) != 4:
        print('Error coordinates number is not 4!!!')
        return False
    for x in coords:
        if x < 0:
            return False
    if coordtype.endswith('norm'):
        tmp = list(filter(lambda e: e >= 0 and e <= 1.0, coords))
        if len(tmp) == 0:
            return False
    else:
        coords = [x * scale_ratio for x in coords]
        if coords[0] < 0 or coords[0] >= img.width():
            return False
        if coords[1] < 0 or coords[1] >= img.height():
            return False
    return True


def draw_box(*args):
    global img
    global canvas
    global scale_ratio
    try:
        coordtype = coord_typevar.get()
        coords = [float(x) for x in coord_var.get().replace(',', ' ').split()]
        # print("coordtype: ", coordtype)
        # print("coords: ", coords)
        if not validation_check(coordtype=coordtype, coords=coords, img=img):
            return
        box = []
        if coordtype == xywh_abs:
            box = [coords[0], coords[1], coords[0] + coords[2], coords[1] + coords[3]]
            box = [_ * scale_ratio for _ in box]
        if coordtype == xywh_norm:
            box = [coords[0] * img.width(), coords[1] * img.height(), (coords[0] + coords[2]) * img.width(), (coords[1] + coords[3]) * img.height()]
        if coordtype == ltbr_abs:
            box = [coords[0], coords[1], coords[2], coords[3]]
            box = [_ * scale_ratio for _ in box]
        if coordtype == ltbr_norm:
            box = [coords[0] * img.width(), coords[1] * img.height(), (coords[2]) * img.width(), (coords[3]) * img.height()]
        rect = canvas.create_rectangle(box[0], box[1], box[2], box[3], fill='', outline='#eb2819', width=3) # #eb2819 is red
        canvas.itemconfigure(rect)
    except ValueError:
        pass

def select_image(*args):
    global img
    global canvas
    global content
    global max_size
    global scale_ratio
    filename = filedialog.askopenfilename()
    if len(filename) != 0:
        # img = ImageTk.PhotoImage(Image.open(filename))  
        pil_image = Image.open(filename)
        ratiow = 1.0 if pil_image.width <= max_size else max_size / pil_image.width
        ratioh = 1.0 if pil_image.height <= max_size else max_size / pil_image.height
        scale_ratio = min(ratiow, ratioh)
        img_scaled_size = (int(pil_image.width * scale_ratio), int(pil_image.height * scale_ratio))
        pil_image_scaled = pil_image.resize((img_scaled_size[0], img_scaled_size[1]), Image.Resampling.LANCZOS)
        img = ImageTk.PhotoImage(pil_image_scaled)
        print(f'{img.width()}, {img.height()}, {scale_ratio}')
        canvas = Canvas(content, width = img.width(), height = img.height())
        canvas.grid(column=0, row=0, columnspan=3, rowspan=6)
        canvas.create_image(0, 0, anchor=NW, image=img) 

xywh_abs = 'xywh_abs'
xywh_norm = 'xywh_norm'
ltbr_abs = 'ltbr_abs'
ltbr_norm = 'ltbr_norm'
img = ''
max_size = 960
img_scaled_size = (100, 100)
scale_ratio = 1.0

root = Tk()  
root.title("Draw bbox")
root.columnconfigure(0, weight=1)
root.rowconfigure(0, weight=1)
root.bind("<Return>", draw_box)

content = ttk.Frame(root)
content.grid(column=0, row=0)

# text to display usage
usage_info = """
用法：
1）Coordinates Type坐标类型：
    a) xywh - 左上角坐标 + 宽高；
    b) ltbr - 左上角 + 右下角坐标；
    c) abs  - 绝对坐标，以pixel为单位
    d) norm - 相对坐标，0 ~ 1.0
2）Coordinates坐标数值，必须为4个坐标数值，以空格或逗号分隔
"""
usage = ttk.Label(content, text=usage_info)
usage["font"] = "TkTooltipFont"
usage["foreground"] = "red"
usage.grid(column=3, row=1, sticky=(W, N), columnspan=2)

# button to select image file
button_img = ttk.Button(content, text="Open image", command=select_image)
button_img.grid(column=3, row=2, sticky=(W, N), columnspan=2)

# label
ttk.Label(content, text="Coordinates Type:").grid(column=3, row=3, sticky=(W, N))
# coordinate type: (x, y, width, height) or (left, top, bottom, right)
coord_typevar = StringVar()
coord_type = ttk.Combobox(content, textvariable=coord_typevar)
coord_type.state(['readonly'])
coord_type.set(xywh_abs)
coord_type.bind('<<ComboboxSelected>>', lambda e: print("%s is selected, index: %d" % (coord_typevar.get(), coord_type.current())))
coord_type['values'] = (xywh_abs, xywh_norm, ltbr_abs, ltbr_norm)
coord_type.grid(column=4, row=3, sticky=(W, N))

# label
ttk.Label(content, text="Coordinates:").grid(column=3, row=4,sticky=(W, N))
# coordinate entry
coord_var = StringVar()
coord = ttk.Entry(content, textvariable=coord_var)
# coord.grid()
coord.grid(column=4, row=4, sticky=(W, N), rowspan=2)

# canvas
canvas = Canvas(content, borderwidth=2, background='gray75', width = 200, height = 200)
canvas.grid(column=0, row=0, sticky=(W, N),columnspan=3, rowspan=6)
# canvas.create_image(0, 0, anchor=NW, image=img) 

# # draw line
# id = canvas.create_line(0, 0, 10, 10, fill='red')
# canvas.itemconfigure(id, fill='blue', width=2)

# # draw rectangle
# rect = canvas.create_rectangle(10, 10, 200, 50, fill='', outline='blue')
# canvas.itemconfigure(rect)

root.mainloop()