import os
import glob
import cv2
import numpy as np
import argparse
import warnings
import time
from tqdm import tqdm
import onnxruntime as rt
import imutils
import torch
from imutils.video import VideoStream

from src.anti_spoof_predict import Detection
from src.generate_patches import CropImage
from src.utility import parse_model_name

from sample_onnx._C import Engine
warnings.filterwarnings('ignore')


SAMPLE_IMAGE_PATH = "./images/sample/"
SAMPLE_VIDEO = "rtsp://admin:meditech123@192.168.100.90:554/"

def parse_plan_model(model_name):
    scale = int(model_name.split(".")[0].replace("spoof", ""))/10
    w_input = 80
    h_input = 80
    model_type = ""
    return h_input, w_input, model_type, scale

class trt_loader:
    def __init__(self, model_dir):
        list_models = glob.glob(os.path.join(model_dir, "*.plan"))
        self.params = []
        self.models = []
        for i, model_name in enumerate(list_models):
            m = Engine.load(model_name)
            h_input, w_input, model_type, scale = parse_plan_model(model_name.split("/")[-1])
            param = {
                "org_img": None,
                "bbox": [],
                "scale": scale,
                "out_w": w_input,
                "out_h": h_input,
                "crop": True,
            }
            if scale is None:
                param["crop"] = False
            self.params.append(param)
            self.models.append(m)
        self.image_cropper = CropImage()
        self.detector = Detection()

    def preprocess(self, img, torch_device):
        img = np.transpose(img, (2,0,1))
        img = np.expand_dims(img, 0).astype(np.float32)
        img = torch.from_numpy(img).to(torch_device)
        img = img.contiguous()
        return img

    @staticmethod
    def infer_plan(net, sample):
        output_plan = net(sample)
        output_plan = output_plan[0].cpu().detach().numpy()
        return output_plan

    def _test_trt(self):
        device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
        cap = VideoStream(SAMPLE_VIDEO).start()
        ret = True
        # width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH ))
        # height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT ))
        width = 1920
        height = 1080

        image = cap.read()
        out = cv2.VideoWriter('outpy.avi',cv2.VideoWriter_fourcc('M','J','P','G'), 15, (width,height))
        while ret:
            image = cap.read()
            if not ret:
                break
            # result = check_image(image)
            # if result is False:
            #     continue
            image_bbox = self.detector.get_bbox(image)
            prediction = np.zeros((1, 3))
            test_speed = 0
            # sum the prediction from single model's result
            for i, param in enumerate(self.params):
                param["org_img"] = image
                param["bbox"] = image_bbox

                img = self.image_cropper.crop(**param)
                img = self.preprocess(img, device)

                start = time.time()

                this_predict = self.infer_plan(self.models[i], (img))
                print(f"Infer {i}: {this_predict}")
                prediction += this_predict
                test_speed += time.time()-start
            print("=============-.-=============")
            # draw result of prediction
            label = np.argmax(prediction)
            value = prediction[0][label]/2
            image_name = "."
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
        default="/workspace/resources/anti_spoof_models/plan",
        help="model_lib used to test")
    parser.add_argument(
        "--image_name",
        type=str,
        default="image_F1.jpg",
        help="image used to test")
    args = parser.parse_args()
    Yuuji_A = trt_loader(args.model_dir)
    Yuuji_A._test_trt()
