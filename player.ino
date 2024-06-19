#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"


SoftwareSerial mySoftwareSerial(9, 10); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
int count_data;
int currentTrack = 4; // 현재 재생 중인 트랙 번호

void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(10);  //볼륨 설정 (0~30)
  Serial.println(myDFPlayer.readFileCounts()); //SD카드 내의 전체 음악 파일 개수 얻어오기
  count_data = myDFPlayer.readFileCounts() - 1; // 총 파일 수
  
  pinMode(4, INPUT); // 핀 4를 입력으로 설정
  
  myDFPlayer.play(currentTrack); // 초기 트랙 재생
}

void loop()
{
  if (digitalRead(4) == HIGH) { // 핀 4에 신호가 들어왔을 때
    delay(500); // 디바운싱을 위해 잠시 대기
    
    currentTrack++; // 다음 트랙으로 이동
    if (currentTrack > count_data) {
      currentTrack = 1; // 마지막 트랙이면 첫 번째 트랙으로 이동
    }
    
    myDFPlayer.play(currentTrack); // 다음 트랙 재생
    Serial.print("Playing track: ");
    Serial.println(currentTrack);
    
    while (digitalRead(4) == HIGH); // 신호가 꺼질 때까지 대기 (디바운싱)
  }
}