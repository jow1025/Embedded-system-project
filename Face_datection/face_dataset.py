import cv2
import os

cam = cv2.VideoCapture(0)
# 높이, 너비가 너무 크면 좀 끊김, 굳이 해상도 높일 필요는 없음
cam.set(3, 800)
cam.set(4, 640)

#opencv에서 얼굴인식을 위해 제공하는 학습된 xml데이터(깃허브에서 다운)
face_detector = cv2.CascadeClassifier('haarcascades/haarcascade_frontalface_default.xml')

#이 face_id를 다르게 입력하여 다른 사람의 사진 데이터 구분 가능
#최초 1을 입력하면 user1.jpg~30.jpg가 생기고, 또 코드실행해서 1입력하면 user31.jpg~60.jpg 저장
#1입력후 코드 재실행하여 2를 입력하면 user2.1.jpg ~ 2.30.jpg 생성. (즉 숫자를 구분하여 다른 사람얼굴 분류할 때 이용가능)

face_id = input('\n enter user id end press <return> ==>  ')
print("\n [INFO] Initializing face capture. Look the camera and wait ...")

count = 0
while(True):
    ret, img = cam.read()
    #acho-em은 카메라가 정면이기 때문에 상하반전 필요없지만
    # RPI의 경우 상하반전 될 경우 있으므로(어떻게 세우냐에 따라) 상하반전 시 아래 코드한줄 추가
    # img=cv2.flip(img,1)

    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY) #흑색으로 변환해서 저장
    faces = face_detector.detectMultiScale(gray, 1.3, 5) # 얼굴 검출
    
    # 좌표 값(박스의 시작 좌표), 사각형 bounding box의 높이, 너비를 받음
    for (x,y,w,h) in faces:
        cv2.rectangle(img, (x,y), (x+w,y+h), (255,0,0), 2) 
        count += 1 #30장 저장하겠다( 1,2,3 ---29,30) 
        # dataset폴더 아래에 이미지 저장(흑색 이미지)
        cv2.imwrite("dataset/User." + str(face_id) + '.' + str(count) + ".jpg", gray[y:y+h,x:x+w])
        # image라는 이름으로 이미지가 출력됨(프레임에 bounding box 효과)
        cv2.imshow('image', img)
   
    k = cv2.waitKey(100) & 0xff  
    if k == 27:  # 아스키 27번은 ESC를 의미, 즉, ESC를 누르면 종료
        break
    elif count >= 30: #30장저장=>80%이상 정확도가 유지됨, 좀 더 성능을 높이고 싶으면 100장 이상 권장
         break
print("\n [INFO] Exiting Program and cleanup stuff")
cam.release() # 카메라 리소스 해제(종료)
cv2.destroyAllWindows() 
