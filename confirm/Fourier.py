import cv2
import numpy as np

TEST_IMG = "/workspace/images/sample/image_T1.jpg"

def generate_FT(image):
    image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    f = np.fft.fft2(image)
    fshift = np.fft.fftshift(f)
    fimg = np.log(np.abs(fshift)+1)
    maxx = -1
    minn = 100000
    for i in range(len(fimg)):
        if maxx < max(fimg[i]):
            maxx = max(fimg[i])
        if minn > min(fimg[i]):
            minn = min(fimg[i])
    fimg = (fimg - minn+1) / (maxx - minn+1) # to (0,1)
    return fimg

if __name__ == "__main__":
    test_img = cv2.imread(TEST_IMG)
    fimg = generate_FT(test_img)
    cv2.imwrite("origin.jpg", test_img)
    cv2.imwrite("FT.jpg", fimg*255)
    # cv2.waitKey()