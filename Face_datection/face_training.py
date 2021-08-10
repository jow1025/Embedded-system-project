import cv2
import numpy as np
from PIL import Image
import os

path = 'dataset'
recognizer = cv2.face.LBPHFaceRecognizer_create()
detector = cv2.CascadeClassifier("haarcascades/haarcascade_frontalface_default.xml");#디렉토리 주소는 적절히 변경해야함

def getImagesAndLabels(path):
    imagePaths = [os.path.join(path,f) for f in os.listdir(path)] #이미지 파일을 안에 넣음
    faceSamples=[] # 이미지의 face_value를 배열 unit8형태로 저장 -> 딕셔너리로 변환하여 저장
    ids = [] #여러 id값을 배열에 저장
    for imagePath in imagePaths: #이미지 로드
        PIL_img = Image.open(imagePath).convert('L')
        img_numpy = np.array(PIL_img,'uint8') # numpy array로 이미지를 int형으로 변환하여 저장
        id = int(os.path.split(imagePath)[-1].split(".")[1]) # 파일 id 추출
        faces = detector.detectMultiScale(img_numpy) # 얼굴 이미지의 스케일 추출
        for (x,y,w,h) in faces:
            faceSamples.append(img_numpy[y:y+h,x:x+w]) # int형으로 형변환된 샘플을 담음
            ids.append(id) # id 값들도 계속 담겨짐
    return faceSamples,ids
print ("\n [INFO] Training faces. It will take a few seconds. Wait ...")
faces,ids = getImagesAndLabels(path)
recognizer.train(faces, np.array(ids)) # LBP 매트릭스 생성 
recognizer.write('trainer/trainer.yml') # yml파일 형태로 저장
print("\n [INFO] {0} faces trained. Exiting Program".format(len(np.unique(ids)))) #ids 배열에 담긴 요소 수 만큼 훈련됨
