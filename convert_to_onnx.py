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

from src.model_lib.MiniFASNet import MiniFASNetV1, MiniFASNetV2,MiniFASNetV1SE,MiniFASNetV2SE
from src.utility import parse_model_name, get_kernel
# warnings.filterwarnings('ignore')

PATH = "/mnt/sda1/HiEveryOneThisIsTannedCung/Silent-Face-Anti-Spoofing/resources/anti_spoof_models/pth"

MODEL_MAPPING = {
    'MiniFASNetV1': MiniFASNetV1,
    'MiniFASNetV2': MiniFASNetV2,
    'MiniFASNetV1SE':MiniFASNetV1SE,
    'MiniFASNetV2SE':MiniFASNetV2SE
}

def load_model(model_name, pretrained_path, device):
    # define model
    h_input, w_input, model_type, _ = parse_model_name(model_name)
    kernel_size = get_kernel(h_input, w_input,)
    model = MODEL_MAPPING[model_type](conv6_kernel=kernel_size).to(device)
    print(f"[INFO]: model_type: {model_type} - kernel_size: {kernel_size}, w_input: {w_input}, h_input: {h_input}")

    # load model weight
    state_dict = torch.load(pretrained_path, map_location=device)
    keys = iter(state_dict)
    first_layer_name = keys.__next__()
    if first_layer_name.find('module.') >= 0:
        from collections import OrderedDict
        new_state_dict = OrderedDict()
        for key, value in state_dict.items():
            name_key = key[7:]
            new_state_dict[name_key] = value
        model.load_state_dict(new_state_dict)
    else:
        model.load_state_dict(state_dict)
    return model

def convert(net, save, device, size=(80,80)):
    net.eval()
    ##################export###############
    print("==> Exporting model to ONNX format at '{}'".format(save))
    input_names = ["input0"]
    output_names = ["output0"]
    inputs = torch.randn(1, 3, size[0], size[1]).to(device)
    dynamic_axes = {'input0': {0: 'batch'},  # size(1,3,640,640)
                    'output0': {0: 'batch'}}
    torch_out = torch.onnx._export(net, inputs, save, export_params=True, verbose=False, input_names=input_names, output_names=output_names, dynamic_axes=dynamic_axes)
    ##################end###############

def confirm(pth_net, onnx_net, torch_device):
    dummy_input_onnx = np.zeros((1,3,80,80), dtype=np.float32)
    dummy_input_torch = torch.from_numpy(dummy_input_onnx).to(torch_device)

    # infer onnx
    input_name = onnx_net.get_inputs()[0].name
    label_name = onnx_net.get_outputs()[0].name
    output_onnx = onnx_net.run([label_name], {input_name: dummy_input_onnx})[0]

    # infer pth
    output_torch = pth_net(dummy_input_torch).cpu().detach().numpy()

    # cal diff
    max_dif = (abs(output_torch - output_onnx)).max()
    min_dif = (abs(output_torch - output_onnx)).min()
    mean_dif = (abs(output_torch - output_onnx)).mean()
    sum_dif = (abs(output_torch - output_onnx)).sum()
    print("\n===== -.- ======")
    print(f"[INFO]: max diff: {max_dif}, min diff: {min_dif}, mean diff: {mean_dif}, sum_dif: {sum_dif}")
    print("===== end ======\n")

if __name__ == '__main__':
    torch.set_grad_enabled(False)
    device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")

    for model in glob.glob(os.path.join(PATH, "*.pth")):
        print(f"Converting {model} to onnx")
        model_name = os.path.basename(model)
        net = load_model(model_name, model, device)
        save = os.path.join(PATH + "/onnx", model_name.replace(".pth", ".onnx").replace(".pt", ".onnx"))
        os.makedirs(os.path.dirname(save), exist_ok=True)
        convert(net, save, device)
        # load onnx net
        onnx_net = rt.InferenceSession(save)
        confirm(net, onnx_net, device)



    




