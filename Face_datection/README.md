# 코드 설명

사전 준비: 아래 링크에서 haarcascade_frontalface_default.xml파일을 다운 받아주세요.
https://github.com/opencv/opencv/tree/master/data/haarcascade
디렉토리의 위치는 본인의 환경에 맞게 수정해야합니다.

링크에서 다운받은 파일은 opencv홈페이지에서 제공하는 object detection을 위한 이미지 패턴을 학습시켜놓은 xml파일입니다.

우리는 얼굴 인식을 위해 이 파일을 사용합니다.

+ 실행 순서
1. face_dataset.py: 얼굴 인식을 위해 30장의 얼굴 사진을 촬영하여 dataset폴더에 저장합니다.
얼굴을 인식을 위한 분류기로 다운 받은 xml파일을 사용합니다.

2. face_training.py: 이미지를 토대로 학습을 진행합니다. 이미지에서 부분적인 int형의 데이터만을 가져와서 이진수로 변환하여 생성된 LBP매트릭스를 이용해 훈련을 진행하고 훈련결과로 yml파일이 생성됩니다.

3. face_recognition.py: 저장된 이미지 데이터 셋을 토대로 얼굴 인식을 진행합니다. 얼굴 인식 성공률을 조정하여 일정 기준치 만큼 인식에 성공했을 때 테트리스 프로그램을 실행합니다. 파일 입출력을 통해 얼굴 인식 성공 시 1을 저장하고, 실패 시 0을 저장하여 값의 상태에 따라 이벤트 실행이 결정됩니다.
(실패는 0, 에러창이 뜨고 재 인증 시도 / 성공은 1, 테트리스실행)

전체 실행 흐름도는 다음과 같습니다.

<img src="https://user-images.githubusercontent.com/67903177/128912821-b2f159ac-5dd3-4a2e-a56f-da3a714e2582.png" width="750" height="400"/>
출처: https://towardsdatascience.com/real-time-face-recognition-an-end-to-end-project-b738bb0f7348

