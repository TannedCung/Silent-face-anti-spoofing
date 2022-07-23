# -*- coding: utf-8 -*-
# @Time : 20-6-9 下午3:06
# @Author : zhuying
# @Company : Minivision
# @File : test.py
# @Software : PyCharm

import os
import cv2
import numpy as np
import argparse
import warnings
import time
from tqdm import tqdm

from src.anti_spoof_predict import AntiSpoofPredict
from src.generate_patches import CropImage
from src.utility import parse_model_name
warnings.filterwarnings('ignore')


SAMPLE_IMAGE_PATH = "./images/sample/"
SAMPLE_VIDEO = "rtsp://admin:meditech123@192.168.100.90:554/"


# 因为安卓端APK获取的视频流宽高比为3:4,为了与之一致，所以将宽高比限制为3:4
def check_image(image):
    height, width, channel = image.shape
    if width/height != 3/4:
        print("Image is not appropriate!!!\nHeight/Width should be 4/3.")
        return False
    else:
        return True


def test(image_name, model_dir, device_id):
    model_test = AntiSpoofPredict(device_id)
    image_cropper = CropImage()
    image = cv2.imread(SAMPLE_IMAGE_PATH + image_name)
    result = check_image(image)
    # if result is False:
    #     return
    image_bbox = model_test.get_bbox(image)
    prediction = np.zeros((1, 3))
    test_speed = 0
    # sum the prediction from single model's result
    for i, model_name in enumerate(os.listdir(model_dir)):
        h_input, w_input, model_type, scale = parse_model_name(model_name)
        param = {
            "org_img": image,
            "bbox": image_bbox,
            "scale": scale,
            "out_w": w_input,
            "out_h": h_input,
            "crop": True,
        }
        if scale is None:
            param["crop"] = False
        img = image_cropper.crop(**param)
        start = time.time()
        this_predict = model_test.predict(img, os.path.join(model_dir, model_name))
        print(f"Infer {i}: {this_predict}")
        prediction += this_predict
        test_speed += time.time()-start
    print("=============-.-=============")
    # draw result of prediction
    label = np.argmax(prediction)
    value = prediction[0][label]/2
    if label == 1:
        print("Image '{}' is Real Face. Score: {:.2f}.".format(image_name, value))
        result_text = "RealFace Score: {:.2f}".format(value)
        color = (255, 0, 0)
    else:
        print("Image '{}' is Fake Face. Score: {:.2f}.".format(image_name, value))
        result_text = "FakeFace Score: {:.2f}".format(value)
        color = (0, 0, 255)
    print("Prediction cost {:.2f} s".format(test_speed))
    cv2.rectangle(
        image,
        (image_bbox[0], image_bbox[1]),
        (image_bbox[0] + image_bbox[2], image_bbox[1] + image_bbox[3]),
        color, 2)
    cv2.putText(
        image,
        result_text,
        (image_bbox[0], image_bbox[1] - 5),
        cv2.FONT_HERSHEY_COMPLEX, 0.5*image.shape[0]/1024, color)

    format_ = os.path.splitext(image_name)[-1]
    result_image_name = image_name.replace(format_, "_result" + format_)
    cv2.imwrite(SAMPLE_IMAGE_PATH + result_image_name, image)

def _test(image_name, model_dir, device_id):
    model_test_1 = AntiSpoofPredict(device_id)
    model_test_2 = AntiSpoofPredict(device_id)
    models = [model_test_1, model_test_2]
    image_cropper = CropImage()
    cap = cv2.VideoCapture(SAMPLE_VIDEO)
    ret = True
    params = []
    ret, image = cap.read()
    result = check_image(image)
    image_bbox = models[0].get_bbox(image)

    for i, model_name in enumerate(os.listdir(model_dir)):
        fake_img = image
        h_input, w_input, model_type, scale = parse_model_name(model_name)
        param = {
            "org_img": fake_img,
            "bbox": image_bbox,
            "scale": scale,
            "out_w": w_input,
            "out_h": h_input,
            "crop": True,
        }
        if scale is None:
            param["crop"] = False
        fake_img = image_cropper.crop(**param)
        params.append(param)
        this_predict = models[i].predict(fake_img, os.path.join(model_dir, model_name))
    
    out = cv2.VideoWriter('outpy.avi',cv2.VideoWriter_fourcc('M','J','P','G'), 15, (image.shape[1],image.shape[0]))
    while ret:
        ret, image = cap.read()
        if not ret:
            break
        # result = check_image(image)
        # if result is False:
        #     continue
        image_bbox = models[i].get_bbox(image)
        prediction = np.zeros((1, 3))
        test_speed = 0
        # sum the prediction from single model's result
        for i, param in enumerate(params):
            param["org_img"] = image
            param["bbox"] = image_bbox
            img = image_cropper.crop(**param)
            start = time.time()
            this_predict = models[i]._predict(img)
            print(f"Infer {i}: {this_predict}")
            prediction += this_predict
            test_speed += time.time()-start
        print("=============-.-=============")
        # draw result of prediction
        label = np.argmax(prediction)
        value = prediction[0][label]/2
        if label == 1:
            print("Image '{}' is Real Face. Score: {:.2f}.".format(image_name, value))
            result_text = "RealFace Score: {:.2f}".format(value)
            color = (255, 0, 0)
        else:
            print("Image '{}' is Fake Face. Score: {:.2f}.".format(image_name, value))
            result_text = "FakeFace Score: {:.2f}".format(value)
            color = (0, 0, 255)
        print("Prediction cost {:.2f} s".format(test_speed))
        cv2.rectangle(
            image,
            (image_bbox[0], image_bbox[1]),
            (image_bbox[0] + image_bbox[2], image_bbox[1] + image_bbox[3]),
            color, 2)
        cv2.putText(
            image,
            result_text,
            (image_bbox[0], image_bbox[1] - 5),
            cv2.FONT_HERSHEY_COMPLEX, 0.5*image.shape[0]/1024, color)

        format_ = os.path.splitext(image_name)[-1]
        result_image_name = image_name.replace(format_, "_result" + format_)
        cv2.imshow("images", image)
        if cv2.waitKey(1) == ord('q'):
            break
        out.write(image)
    cap.release()
    out.release()



if __name__ == "__main__":
    desc = "test"
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument(
        "--device_id",
        type=int,
        default=0,
        help="which gpu id, [0/1/2/3]")
    parser.add_argument(
        "--model_dir",
        type=str,
        default="./resources/anti_spoof_models/pth",
        help="model_lib used to test")
    parser.add_argument(
        "--image_name",
        type=str,
        default="image_F1.jpg",
        help="image used to test")
    args = parser.parse_args()
    _test(args.image_name, args.model_dir, args.device_id)
