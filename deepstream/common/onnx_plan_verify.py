from __future__ import print_function
import os
import glob
import argparse
import torch
import torch.backends.cudnn as cudnn
import numpy as np
import time
import cv2
import onnxruntime as rt

from sample_onnx._C import Engine
# warnings.filterwarnings('ignore')
PATH = "/workspace/resources/anti_spoof_models/onnx"

def infer_onnx(net, sample):
    # infer onnx
    input_name = net.get_inputs()[0].name
    label_name = net.get_outputs()[0].name
    output_onnx = net.run([label_name], {input_name: sample})[0]
    return output_onnx

def infer_plan(net, sample):
    output_plan = net(sample)
    output_plan = output_plan[0].cpu().detach().numpy()
    return output_plan

def confirm(plan_net, onnx_net, torch_device):
    dummy_input_onnx = np.random.random((1,3,80,80)).astype(np.float32)
    dummy_input_plan = torch.from_numpy(dummy_input_onnx).to(torch_device)

    # infer onnx
    output_onnx = infer_onnx(onnx_net, dummy_input_onnx)

    # infer pth
    output_plan = infer_plan(plan_net, dummy_input_plan)

    # cal diff
    max_dif = ((output_plan)-(output_onnx)).max()
    min_dif = ((output_plan)-(output_onnx)).min()
    mean_dif = ((output_plan)-(output_onnx)).mean()
    sum_dif = (abs(output_plan-output_onnx)).sum()
    print("\n===== -.- ======")
    print(f"[INFO]: max diff: {max_dif}, min diff: {min_dif}, mean diff: {mean_dif}, sum_dif: {sum_dif}")
    print("===== end ======\n")

    return max_dif, min_dif, mean_dif

if __name__ == '__main__':
    torch.set_grad_enabled(False)
    device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
    diff = [[], []]

    for i, model in enumerate(glob.glob(os.path.join(PATH, "*.onnx"))):
        print(f"Verifying {model}") 
        plan_model = Engine.load(model.replace("onnx", "plan"))
        onnx_net = rt.InferenceSession(model)
        
        # load onnx net
        mx, mn, mean = confirm(plan_model, onnx_net, device)
        diff[i].append([mx, mn, mean])



    




