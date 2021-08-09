# Embedded-system-project

리눅스와 라즈비안의 디바이스 드라이버 연동 및 QT를 활용하여 진행한 프로젝트입니다.  
게임 접속을 위한 로그인을 얼굴인식과 비밀번호로 이중 보안 설정하여 로그인을 마친 뒤 
QT-creator기반의 gui형태로 구현한 테트리스 게임을 진행하며
Achro-em기기에 내장된 다양한 모듈들(LCD, Camera, led, 등등)을 활용합니다.

테트리스의 로그인,게임 시작, 종료 시 화면은 각각 다음과 같습니다.
- 로그인

<img src="https://user-images.githubusercontent.com/67903177/128677143-23cf2d8a-aa08-4ee7-9574-58dacf9efa4e.png" width="400" height
="400"/>

<img src="https://user-images.githubusercontent.com/67903177/128678028-e66c7f92-8c14-43f8-934a-7a2e1412f188.png" width="800" height="450"/>


<h2><br>하드웨어: Achro-em kit</h3>
<img src="https://user-images.githubusercontent.com/67903177/128670056-4fb961a8-ca61-487c-ad7c-ecac8d584b6e.png"

Achro-em은 Hunins가 개발한 rpi기반의 임베디드 시스템 실습 및 교육용 키트입니다.
라즈베리파이4B 기반의 쿼드코어를 탑재한 고효율 개발키트이므로 오픈소스 기반의 다양한 프로젝트를 진행할 수 있습니다.

Rpi와 달리 GPIO핀이 내장되어있지 않고 FPGA에 달려있는 8개의 모듈들(led, lcd, camera, motor 등)을 제어할 수 있습니다.

해당 장비를 사용함으로써 디바이스 드라이버에 대한 이해, 리눅스의 커널 및 시스템 구조에 대해 공부할 수 있었습니다.

자세한 제품 소개 및 사양에 대해서는 아래링크를 참조해주세요.<br>
http://www.huins.com/new/sub/goods_sublist.php?ca_id=7050

<br>프로젝트에 대한 설명입니다.</br>
<br><h3> 1. 설계환경</h3>
 - H/W: Achro-em kit
 - OS: Raspbian, Ubuntu
 - Tool: QT Creator, Samba(파일 공유)
  
<br><h3>2. 구현 내용</b></h3>
 * 얼굴 인식: Opencv의 Haar Cascade Classifier알고리즘
 
    + 인식할 얼굴의 data set만들기
    + data set을 이용해 학습시키기
    + 학습 결과를 바탕으로 얼굴을 보고 누구인지 알아내기
    + 결과적으로 처음 인식한 사람(관리자)에 대해서만 로그인 
 
 * 게임 시작: Qt creator에서 구현한 테트리스(GUI)
    + 시작 레벨에 따른 난이도 조정 옵션
    + 테트리스의 행, 열 조정 옵션
    + 창 사이즈 조절 옵션
  
<br><h3>3. 디바이스 제어</h3>
  - Text LCD: Opencv로 사용자 얼굴인식 후 결과를 LCD에 표현+ QT에서 게임 시작과 레벨업 표현을 표현
  - Push Switch: 테트리스의 퍼즐 움직임에 사용, 상/하/좌/우, 도형의 회전 조절
  - LED: 테트리스에서 한 라인을 클리어 했을 때 짧은 시간동안 깜빡임
  - Dot Matrix: 얼굴 인증 후 3초간의 카운트 다운(로딩) 표현 후, 게임진행+ 게임 스테이지 레벨 표현
  - FND: 테트리스 점수 표현(레벨 별로 클리어 점수를 달리하였지만 실제 게임 난이도는 동일한 어색함 존재)
  - Motor: 얼굴인식/게임이 정상 진행됨을 표현하여 각 단계가 수행되는 동안 모터가 동작함
  - Dip Switch: 1차 얼굴인식과 2차 비밀번호 인증을 위한 입력키로 사용

<br><h3>4. 활동 다이어그램과 순서도</h3>
- 활동 다이어그램
<img src="https://user-images.githubusercontent.com/67903177/128666893-7cf092ff-3732-44be-bdcb-a27b6a63e0b2.png" width="500" height="600"/>

- 순서도
<img src="https://user-images.githubusercontent.com/67903177/128669387-e7ad02ac-4e75-4887-adc9-b07b832521ca.png" width="750" height="450"/>
<br><h3>5. 파일 설명</h3><br>
  파일 저장 폴더에 설명 첨부
