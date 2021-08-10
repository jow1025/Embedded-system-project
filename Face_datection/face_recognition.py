import cv2
import numpy as np
import os
import time

recognizer = cv2.face.LBPHFaceRecognizer_create()

#trainer.yml, haarcascade_frontalface_default.xml의 파일 위치를 본인 환경에 맞게 바꿔주세요.
recognizer.read('trainer/trainer.yml')
cascadePath = "haarcascades/haarcascade_frontalface_default.xml"

faceCascade = cv2.CascadeClassifier(cascadePath);
font = cv2.FONT_HERSHEY_SIMPLEX

id = 0
result = 0
names = ['None', 'Min', 'Jung', 'Jae', 'Nam'] #라벨 지정(이름)


cam = cv2.VideoCapture(0)
cam.set(3, 800)
cam.set(4, 640)

#얼굴로 인식할 최소 창 크기 정의
minW = 0.1*cam.get(3)
minH = 0.1*cam.get(4)

while True:
    ret, img =cam.read() #이미지 로딩
    gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY) 

    # 얼굴 인식
    faces = faceCascade.detectMultiScale( 
        gray,
        scaleFactor = 1.2,
        minNeighbors = 5,
        minSize = (int(minW), int(minH)),
       )
    #얼굴 인식  여부를 판단하기 위해 File 입출력 이용, 인식o: 1(테트리스시작), 인식x: 0(오류문구)

    # 아래 .txt파일이 생성될 위치도 다시 지정해주세요.
    f = open('build-tetris-Desktop-Debug/auth.txt','w')
    f.write('0')
    f.close()

    for(x,y,w,h) in faces:
        # 얼굴인식을 시작하면 모터가 회전함
        os.system('sudo ./fpga_test_step_motor 0 0 10') #motor 드라이버 실행
        #cv2.rectangle(img, (x,y), (x+w,y+h), (255,0,0), 2)
        id, confidence = recognizer.predict(gray[y:y+h,x:x+w]) # 유사도 측정
        result = 100 - confidence

       # 인식률 조정 옵션을 아래 if문 숫자로 조절.  지금은 좀 여유롭게 잡음
        if (40 < (100 - confidence) < 100):
            cv2.rectangle(img,(x,y),(x+w,y+h),(0,255,0),2) 
            id = names[id] #라벨링 배열 속 이름 불러오기
            confidence = "  {0}% Match".format(round(100 - confidence))

            # 아래 if문 result 범위를 조절하여 인식률에 따른 이벤트 실행 조절
            if(result > 60):
           	os.system('sudo ./fpga_test_text_lcd face identifying...') # lcd 실행
            
	# 인식 성공 시 3초 후 게임 시작을 표현하기 위해 3초 카운트 다운(dot matrix로 표현)
           	os.system('sudo ./fpga_test_dot 3') 
           	time.sleep(1)
           	os.system('sudo ./fpga_test_dot 2')
           	time.sleep(1)
           	os.system('sudo ./fpga_test_dot 1')
           	time.sleep(1)
           	os.system('sudo ./fpga_test_dot 0')
            	os.system('sudo ./fpga_test_text_lcd login success')
           	time.sleep(3)
	
	#테트리스 실행하고 파일에 1을 저장(0: 재인증, 1: 게임실행)
            	os.system('sudo ./fpga_test_text_lcd GameList 1.tetris')
		f = open('build-tetris-Desktop-Debug/auth.txt','w')
		f.write('1')
		f.close()
	# 카메라 리소스 해제(종료)
            	cam.release()
            	cv2.destroyAllWindows()
	   #테트리스 실행
                os.system('cd build-tetris-Desktop-Debug && sudo ./tetris-qt')

        #인식 실패 시
        else:
            cv2.rectangle(img,(x,y),(x+w,y+h),(0,0,255),2)
            id = "unknown"
            confidence = "  {0}%".format(round(100 - confidence))
            os.system('sudo ./fpga_test_text_lcd login failed')
        cv2.putText(img, str(id), (x+5,y-5), font, 1, (255,255,255), 2)
        cv2.putText(img, str(confidence), (x+5,y+h-5), font, 1, (255,255,0), 1)
    cv2.imshow('camera',img)
    k = cv2.waitKey(10) & 0xff
    if k == 27:
        break
print("\n [INFO] Exiting Program and cleanup stuff")
cam.release()
cv2.destroyAllWindows()
