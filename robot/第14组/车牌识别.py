import cv2
import os
import sys
import numpy as np 
import serial
import time
ser=serial.Serial("COM6 ",9600)

def cameraAutoForPictures(saveDir='data/'):
    '''
    调用电脑摄像头来自动获取图片
    '''
    if not os.path.exists(saveDir):
        os.makedirs(saveDir)
    count=1  #图片计数索引
    cap=cv2.VideoCapture(1) # 外接摄像头用1
    width,height,w=640,480,360
    cap.set(cv2.CAP_PROP_FRAME_WIDTH,width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT,height)
    crop_w_start=(width-w)//2
    crop_h_start=(height-w)//2
    print('width: ',width)
    print('height: ',height)
    while True:
        ret,frame=cap.read()  #获取相框
        frame=frame[crop_h_start:crop_h_start+w,crop_w_start:crop_w_start+w]  #展示相框
        # frame=cv2.flip(frame,1,dst=None)  #前置摄像头获取的画面是非镜面的，即左手会出现在画面的右侧，此处使用flip进行水平镜像处理
        cv2.imshow("capture", frame)
        action=cv2.waitKey(1) & 0xFF
        # if action==ord('c'):
        #     saveDir=raw_input(u"请输入新的存储目录：")
        #     if not os.path.exists(saveDir):
        #         os.makedirs(saveDir)
        if action==ord('p'):
            cv2.imwrite("%s/%d.jpg" % (saveDir,count),cv2.resize(frame, (370, 294),interpolation=cv2.INTER_AREA))
            print(u"%s: %d 张图片" % (saveDir,count))
            count+=1
        if action==ord('q'):
            break
    cap.release()  #释放摄像头
    cv2.destroyAllWindows()  #丢弃窗口

def imread_photo(filename, flags = cv2.IMREAD_COLOR):
    return cv2.imread(filename, flags)

def resize_photo(imgArr,MAX_WIDTH = 1000):
    img = imgArr
    rows, cols= img.shape[:2]    
    if cols >  MAX_WIDTH:
        change_rate = MAX_WIDTH / cols
        img = cv2.resize(img ,( MAX_WIDTH ,int(rows * change_rate) ), interpolation = cv2.INTER_AREA)
    return img

def predict(imageArr):
    img_copy = imageArr.copy()
    gray_img = cv2.cvtColor(img_copy , cv2.COLOR_BGR2GRAY)
    gray_img_ = cv2.GaussianBlur(gray_img, (5,5), 0, 0, cv2.BORDER_DEFAULT)
    # kernel = np.ones((23, 23), np.uint8)
    # img_opening = cv2.morphologyEx(gray_img, cv2.MORPH_OPEN, kernel)
    # img_opening = cv2.addWeighted(gray_img, 1, img_opening, -1, 0)
    ret, img_thresh = cv2.threshold(gray_img_, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
    img_edge = cv2.Canny(img_thresh, 100, 200)
    # kernel = np.ones((10, 10), np.uint8)
    # img_edge1 = cv2.morphologyEx(img_edge, cv2.MORPH_CLOSE, kernel)
    # img_edge2 = cv2.morphologyEx(img_edge1, cv2.MORPH_OPEN, kernel)
    image, contours, hierarchy = cv2.findContours(img_edge, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    #print(contours)

    for cnt in contours:
        (x, y, w, h) = cv2.boundingRect(cnt) 
        cv2.rectangle(img_copy, pt1=(x, y), pt2=(x+w, y+h),color=(0, 255, 0), thickness=2)
        
    #cv2.imshow("digit0.jpg", img_copy) 
    #cv2.waitKey(0)

    #cv2.destroyAllWindows()    

    return gray_img_,contours

def chose_licence_plate(contours,Min_Area = 10000,Max_Area = 20000):
    temp_contours = []
    for contour in contours:
        if cv2.contourArea( contour ) > Min_Area and cv2.contourArea( contour ) < Max_Area:
            temp_contours.append(contour)
    print(len(temp_contours))
    print()

    for cnt in temp_contours:
        print(cnt.shape)
        print()
        (x, y, w, h) = cv2.boundingRect(cnt) 
        cv2.rectangle(img, pt1=(x, y), pt2=(x+w, y+h),color=(0, 255, 0), thickness=2)
        
    #cv2.imshow("digit1.jpg", img)
    #cv2.waitKey(0)
    #cv2.destroyAllWindows()


    car_plate = []
    for temp_contour in temp_contours:
        rect_tupple = cv2.minAreaRect( temp_contour )
        rect_width, rect_height = rect_tupple[1]
        if rect_width < rect_height:
            rect_width, rect_height = rect_height, rect_width
        aspect_ratio = rect_width / rect_height
        if aspect_ratio > 2 and aspect_ratio < 5.5:
            car_plate.append( temp_contour )
            rect_vertices = cv2.boxPoints( rect_tupple )
            rect_vertices = np.int0( rect_vertices )   

    for cnt in car_plate:
        (x, y, w, h) = cv2.boundingRect(cnt) 
        cv2.rectangle(img, pt1=(x, y), pt2=(x+w, y+h),color=(0, 0, 255), thickness=2)
    #cv2.imshow("digit2.jpg", img)
    #cv2.waitKey(0)
    #cv2.destroyAllWindows()  

    print(len(car_plate))
    return  car_plate,len(car_plate)
 
def license_segment( car_plates ):
    if len(car_plates)==1:
        for car_plate in car_plates:
            row_min,col_min = np.min(car_plate[:,0,:],axis=0)
            row_max, col_max = np.max(car_plate[:, 0, :], axis=0)
            # cv2.rectangle(img, (row_min,col_min), (row_max, col_max), (0,255,0), 2)
            card_img = img[col_min:col_max,row_min:row_max,:]
            #print(col_min)
            #cv2.imshow("img", img)
        cv2.imwrite( "card_img.jpg", card_img)
        #cv2.imshow("card_img.jpg", card_img)
        #cv2.waitKey(0)
        #cv2.destroyAllWindows()
    return "card_img.jpg"

def find_waves(threshold, histogram):
	up_point = -1
	is_peak = False
	if histogram[0] > threshold:
		up_point = 0
		is_peak = True
	wave_peaks = []
	for i,x in enumerate(histogram):
		if is_peak and x < threshold:
			if i - up_point > 2:
				is_peak = False
				wave_peaks.append((up_point, i))
		elif not is_peak and x >= threshold:
			is_peak = True
			up_point = i
	if is_peak and up_point != -1 and i - up_point > 4:
		wave_peaks.append((up_point, i))
	return wave_peaks
 
def remove_plate_upanddown_border(card_img):
    plate_Arr = cv2.imread(card_img)
    plate_gray_Arr = cv2.cvtColor(plate_Arr, cv2.COLOR_BGR2GRAY)
    # cv2.imshow("plate_gray_Arr", plate_gray_Arr)
    ret, plate_binary_img = cv2.threshold( plate_gray_Arr, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU )
    row_histogram = np.sum(plate_binary_img, axis=1)  
    row_min = np.min( row_histogram )
    row_average = np.sum(row_histogram) / plate_binary_img.shape[0]
    row_threshold = (row_min + row_average) / 2
    wave_peaks = find_waves(row_threshold, row_histogram)
    wave_span = 0.0
    for wave_peak in wave_peaks: 
        span = wave_peak[1]-wave_peak[0]
        if span > wave_span:
            wave_span = span
            selected_wave = wave_peak
    plate_binary_img = plate_binary_img[selected_wave[0]:selected_wave[1], :]
    plate_img = plate_Arr[selected_wave[0]:selected_wave[1], :]
    cv2.imwrite( "plate_binary_img.jpg", plate_binary_img)   
    cv2.imwrite( "plate_img.jpg", plate_img)  
    #cv2.imshow("plate_binary_img", plate_binary_img)
    #cv2.imshow("plate_img.jpg", plate_img)  
    #cv2.waitKey(0)
    #cv2.destroyAllWindows()
    return  plate_img

def getStandardDigit(img):
    STD_WIDTH = 20 
    STD_HEIGHT = 40
    height,width = img.shape
    new_width = int(width * STD_HEIGHT / height)
    if new_width > STD_WIDTH:
        new_width = STD_WIDTH
    resized_num = cv2.resize(img, (new_width,STD_HEIGHT), interpolation = cv2.INTER_NEAREST)
    canvas = np.zeros((STD_HEIGHT, STD_WIDTH))
    x = int((STD_WIDTH - new_width) / 2) 
    canvas[:,x:x+new_width] = resized_num

    return canvas

array_photo = []
def clip_photo(filename):
    img = cv2.imread(filename)
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    blurred = cv2.GaussianBlur(gray, (5,5), 0)
    thresh, lena_thresh = cv2.threshold(blurred, 120, 255, cv2.THRESH_BINARY)
    num_mask = cv2.medianBlur(lena_thresh,3)
    bimg, contours, hier = cv2.findContours(num_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    # print(len(contours))
    minWidth = 10 
    minHeight = 10 
    base = 1000 
    imgIdx = base 

    numbers = []
    for cnt in contours:
        (x, y, w, h) = cv2.boundingRect(cnt)
        if w < minWidth or h < minHeight:
            continue
        else:
            numbers.append((x, y, w, h))

    numbers.sort() 
    for (x, y, w, h) in numbers:
        digit = num_mask[y:y+h, x:x+w]
        digit = getStandardDigit(digit)
        array_photo.append(digit)

        #cv2.imshow("digit.jpg", digit)
        #cv2.waitKey(0)
        #cv2.destroyAllWindows()
        digit*=255.0
        cv2.imwrite('./test/{}.png'.format(imgIdx), digit) 
        imgIdx+=1
        cv2.rectangle(img, pt1=(x, y), pt2=(x+w, y+h),color=(0, 255, 255), thickness=2)

    cv2.imwrite('number_mask_mark_rect.jpg', img)
    # cv2.imshow("img.jpg", img)
    # cv2.waitKey(0)
    # cv2.destroyAllWindows()


def read_directory(directory_name):
    for filename in os.listdir("./"+directory_name):
        label_img.append(filename[0])
        img = cv2.imread(directory_name+'/'+filename, 0)
        _, thresh = cv2.threshold(img, 60, 255, cv2.THRESH_BINARY)
        array_img.append(thresh)


def cal(array):
    for photo in array:
        photo = photo.astype('uint8') # 原为float64，变为uint8
        print(photo.shape)
        sum_dis = [] 
        for i, temp in enumerate(array_img):
            image_dis = photo - temp 
            [rows, cols] = image_dis.shape
            for i in range(rows):
                for j in range(cols):
                    if image_dis[i][j] == 255:
                        image_dis[i][j] = 1
            sum_dis.append(image_dis.sum())
        min_ = min(sum_dis)
        label_ans.append(sum_dis.index(min_))
        # print(min_)
        # print(sum_dis.index(min_))

def cal_distance(directory_name):
    for filename in os.listdir("./"+directory_name):
        img = cv2.imread(directory_name+'/'+filename, 0)
        _, thresh = cv2.threshold(img, 60, 255, cv2.THRESH_BINARY)
        array_test.append(thresh)
        # print(thresh.dtype)
        ans=[]
        sum_dis = [] 
        for i,temp in enumerate(array_img):
            image_dis = thresh - temp
            [rows, cols] = image_dis.shape
            for i in range(rows):
                for j in range(cols):
                    if image_dis[i][j] == 255:
                        image_dis[i][j] = 1
        #     print(image_dis)
            sum_dis.append(image_dis.sum())
        min_ = min(sum_dis)
        label_ans.append(sum_dis.index(min_))
        # print(min_)
        # print(sum_dis.index(min_))
    # print(label_ans)
    # print(ans)
    label_ans_new = [str(x) for x in label_ans]
    ans = "".join(label_ans_new)
    # print(ans)
    return ans

if __name__ == "__main__":
    #cameraAutoForPictures(saveDir='data/')
    #img = imread_photo("./data/1.jpg") 
    #img = imread_photo("111.jpg") 
    while True:
        array_img = []
        label_img = []
        array_test = []
        label_ans = []

        cap = cv2.VideoCapture(1)
        cap.set(3,320)
        cap.set(4,240)
        L=0
        while L!=1:
            k = cv2.waitKey(1) & 0xff
            if k==27:
                break
            ret,img=cap.read()
            img = resize_photo(img) 
            #cv2.imshow('img',img)
            gray_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY) 
            gray_img_, contours = predict(img)
            car_plates,L = chose_licence_plate(contours)
        cap.release()
        #cv2.destroyAllWindows()
        card_img = license_segment(car_plates)
        plate_img = remove_plate_upanddown_border(card_img)

        clip_photo("plate_img.jpg")
        filename1 = "template"
        filename2 = "test"
        read_directory(filename1)
        ans = cal_distance(filename2)

        database = ["4396", "1083"]
        print((ans))
    # print((database[0]))

    # cal(array_photo)
        flag = False
        for data in database:
            if ans == data:
                print("TRUE")
                flag = True   
                break
        if flag == True:
            ser.write(('Y').encode("ascii"))
            time.sleep(13)
            ser.write(('X').encode("ascii"))
        
        if k==27:
            break
    ser.close()
    #cv2.waitKey(0)
    #cv2.destroyAllWindows()
    
    # for i,_ in enumerate(array_test):
    #     print(array_photo[i] == array_test[i])


