#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>
#include <vector>
#include <SoftwareSerial.h>
SoftwareSerial BTSerial(D2, D1); // RX, TX
SoftwareSerial mySoftwareSerial(D3, D4); // RX, TX ;

//  WiFi 정보
char ssid[32] = "WING";        // Your WiFi SSID
char password[64] = "00000000"; // Your WiFi Password
const char* suno_api_url = "https://studio-api.suno.ai";
const char* clerk_api_url = "https://clerk.suno.com";
const char* audio_douwnlod_url = "https://cdn1.suno.ai/";
const char* new_tokens_api ="/v1/client/sessions/sess_2i0AT1G27anSU8eaputlzF4kxCT/tokens?_clerk_js_version=4.73.3";

String c_stripe_mid = "__stripe_mid=09d738d8-193e-464b-8768-3a594794fb69f8b06d; ";
String c_cfuvid = "_cfuvid=NaXIRb5MH9eqbZXmEZJwcC9zXYOzOtGKi_TjIDyLS34-1718617017586-0.0.1.1-604800000; ";
const char* c_client = "__client=eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJpZCI6ImNsaWVudF8yZlAwZ3FyYVdWWDJlT0FpeUl0WTc0UnJyV3ciLCJyb3RhdGluZ190b2tlbiI6IjdtYjV3amFwZjhvMXEwcnpmOGpibW5wOWIweXk0a3I2cmx2NjRqN2EifQ.cdAqTEsXkmI7YOgOZ6hSq-2uh0jESAZqsHyviaxx6JDdCRKaK7f-nzwUOYgyWFfIPn10ciTfYCCpebJjcYw5qEwE5_Uv1Kpp0Gs01_7HhnB9uXz6iBdQ1M5AOdNhcMntH4YYLnfLcM-BFnKMfyEC_0simRpgkFviuOaIt6K82xI222Ig6oazDCqDulssi4aXAek-ISmw1jamRQtZOuoCqQBvJKKczr0tUqt7H1WRLQgmARJeTsMkLU4SQ5AAQ3LgHkeGs8Ct-J9zEQgULQ6xvc7xlgJKibLlZjX7zKQhftrCG0TM9QY08biI2KsceH9KeulCk-eOl_fOTGa8d4Z14g; ";
String c_client_uat = "__client_uat=1718617124; ";
String c_cf_bm = "__cf_bm=eqEo3fCj2EoqY60ZXMg5D7PZKk2Tw_9UMyMkETAweMY-1718619262-1.0.1.1-lKI0BqOM57thErvVfc7JIZK915IBlty.J5Lk0fNzoQSgf_0RsATii3E6nZ5BN7JG5_yNq7GQzDyH_1H95Y9z1w; ";
const char* c_session = "eyJhbGciOiJSUzI1NiIsImNhdCI6ImNsX0I3ZDRQRDExMUFBQSIsImtpZCI6Imluc18yT1o2eU1EZzhscWRKRWloMXJvemY4T3ptZG4iLCJ0eXAiOiJKV1QifQ.eyJhdWQiOiJzdW5vLWFwaSIsImF6cCI6Imh0dHBzOi8vc3Vuby5jb20iLCJleHAiOjE3MTc1MjA5MTQsImh0dHBzOi8vc3Vuby5haS9jbGFpbXMvY2xlcmtfaWQiOiJ1c2VyXzJmUDBoanZOSUtiN3MyeERjbFlTZ3ZRTkJ2VCIsImh0dHBzOi8vc3Vuby5haS9jbGFpbXMvZW1haWwiOiJiYW5lMDYyNDIyQGdtYWlsLmNvbSIsImlhdCI6MTcxNzUyMDg1NCwiaXNzIjoiaHR0cHM6Ly9jbGVyay5zdW5vLmNvbSIsImp0aSI6ImQ5ODgyMmU4NTg4MzRiOTg5ZGM5IiwibmJmIjoxNzE3NTIwODQ0LCJzaWQiOiJzZXNzXzJoRU5EVVFwcnJEYWV2cVJJTW03TmdUZWxrTiIsInN1YiI6InVzZXJfMmZQMGhqdk5JS2I3czJ4RGNsWVNndlFOQnZUIn0.kVRYdVHoRaUd_Bk4S0bTJeD9KOD6Xdj8uFcI0cNNlLy7kMoOPbkn4WyzbKJ4axX3u80Xu0wC_f1zp_Kuuj1gCSwWBPkRnKcNcqO1vqtKoRad4thjGWN1KMDuG1StxFQyxcf90RsOSbsol6l6LoPX34eePP9f5JQZrBIDasbegkB5RGRj8FUTnDEv7xhifisp8jbuJt9FKu8VD4C35objP8ttipLxNHkK5Ne6biQW25d8d6Cpj5fRnVq6hVrbMKA0qBdeDXadAU0sO5FXjlPQnVWFbNMU7KAn8h6lhRvKjvqz3iFYLk1wdpHPOlhp2UoX6UcuQ_d40swyW3NBunz23w; ";
const char* c_session_cookie = "__session=";
String bearerToken = "";  // 새로운 JWT 토큰을 저장할 변수

const short chipSelect = D8;  

struct Item {
  char id[40];
  char major_model_version[10];
  char tags[128];
  char title[60];
};

struct PageSearchState{
  const char* stringId = "\"id\"";
  const char* stringPrompt = "\"prompt\"";
  const char* stringTags = "\"tags\"";
  const char* stringTitle = "\"title\"";
  bool isId;
  bool isPrompt;
  bool isTags;
  bool isTitle;
};


std::array<Item, 10> Items;
bool debugmode = false;

void sdMountCheck(){
  if (!SD.begin(chipSelect)) {
    Serial.println("Card Mount Failed");
    Serial.println("Card Mount Failed");
    return;
  }
  BTSerial.println("SD Card initialized");
  BTSerial.println("SD Card initialized");
}

void printTab(int count){
  for(int i = 0; i < count ; i++)
    Serial.print("\t");
    BTSerial.print("\t");
  return;
}



String extractValue(const char* jsonString, const char* key) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, jsonString);
    const char* value = doc[key];
    if (value) {
        return String(value);
    }
    return String("");
}

void printCommandList() {
    Serial.println("Available commands:");
    Serial.println("1. LOAD_SONG_LIST_PAGE <page_number>");
    Serial.println("2. SHOW_SONG_LIST");
    Serial.println("3. SEARCH_SONG_ID <id>");
    Serial.println("4. SEARCH_SONG_TITLE <title>");
    Serial.println("5. DOWNLOAD_SONG_ID <id>");
    Serial.println("6. DOWNLOAD_SONG_TITLE <title>");
    Serial.println("7. DOWNLOAD_SONG_NUM <num>");
    Serial.println("8. ? or help command list");
    Serial.println("9. DEBUG_MODE <true, false>");
    Serial.println("10. CREAT_SONG");
    Serial.println("11. MUSIC_PLAY");
    Serial.println("12. WIFI_SET");
    BTSerial.println("Available commands:");
    BTSerial.println("1. LOAD_SONG_LIST_PAGE <page_number>");
    BTSerial.println("2. SHOW_SONG_LIST");
    BTSerial.println("3. SEARCH_SONG_ID <id>");
    BTSerial.println("4. SEARCH_SONG_TITLE <title>");
    BTSerial.println("5. DOWNLOAD_SONG_ID <id>");
    BTSerial.println("6. DOWNLOAD_SONG_TITLE <title>");
    BTSerial.println("7. DOWNLOAD_SONG_NUM <num>");
    BTSerial.println("8. ? or help command list");
    BTSerial.println("9. DEBUG_MODE <true, false>");
    BTSerial.println("10. CREAT_SONG");
    BTSerial.println("11. MUSIC_PLAY");
    BTSerial.println("12. WIFI_SET");
}

int getCommandType(String command) {
    if (command == "LOAD_SONG_LIST_PAGE" || command == "1") {
        return 1;
    } else if (command == "SHOW_SONG_LIST" || command == "2") {
        return 2;
    } else if (command == "SEARCH_SONG_ID" || command == "3") {
        return 3;
    } else if (command == "SEARCH_SONG_TITLE" || command == "4") {
        return 4;
    } else if (command == "DOWNLOAD_SONG_ID" || command == "5") {
        return 5;
    } else if (command == "DOWNLOAD_SONG_TITLE" || command == "6") {
        return 6;
    } else if (command == "DOWNLOAD_SONG_NUM" || command == "7") {
        return 7;
    } else if (command == "?" || command == "help"){
        return 8;
    } else if (command == "DEBUG_MODE" || command == "9"){
        return 9;
    } else if (command == "CREAT_SONG" || command == "10"){
        return 10;
    } else if (command == "MUSIC_PLAY" || command == "11"){
        return 11;
    } else if (command == "WIFI_SET" || command == "12"){
        return 12;
    }
    else {
        return -1; // Unknown command
    }
}

void printItems(byte page_list) {
  Serial.print("page_list : ");
  Serial.println(page_list);
    for (const auto& item : Items) {
        Serial.print("ID: ");
        Serial.println(item.id);
        Serial.print("Major Model Version: ");
        Serial.println(item.major_model_version);
        Serial.print("Tags: ");
        Serial.println(item.tags);
        Serial.print("Title: ");
        Serial.println(item.title);
        Serial.println();
        BTSerial.print("ID: ");
        BTSerial.println(item.id);
        BTSerial.print("Major Model Version: ");
        BTSerial.println(item.major_model_version);
        BTSerial.print("Tags: ");
        BTSerial.println(item.tags);
        BTSerial.print("Title: ");
        BTSerial.println(item.title);
        BTSerial.println();
    }
}

void getNewToken() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();  // 인증서 검증 비활성화 (테스트 목적)
    HTTPClient http;
    String url = String(clerk_api_url) + String(new_tokens_api);
    http.begin(client, url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String cookie_cash = c_stripe_mid + c_cfuvid + String(c_client) + c_client_uat + c_cf_bm + String(c_session_cookie) + String(c_session);
    http.addHeader("Cookie", cookie_cash);
    Serial.println(" ----requset---- ");
    Serial.println("POST");
    Serial.print("url : ");
    Serial.println(url);
    Serial.print("cookie : ");
    Serial.println(cookie_cash);
    BTSerial.println(" ----requset---- ");
    BTSerial.println("POST");
    BTSerial.print("url : ");
    BTSerial.println(url);
    BTSerial.print("cookie : ");
    BTSerial.println(cookie_cash);
    int httpResponseCode = http.POST("");
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(" ----response---- ");
      Serial.println(response);
      BTSerial.println(" ----response---- ");
      BTSerial.println(response);

      // Parse response to get the JWT token
      StaticJsonDocument<256> responseDoc;
      deserializeJson(responseDoc, response);

      bearerToken = responseDoc["jwt"].as<String>();
      Serial.println("New JWT Token: " + bearerToken);
      BTSerial.println("New JWT Token: " + bearerToken);
    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpResponseCode);
      BTSerial.print("Error on HTTP request: ");
      BTSerial.println(httpResponseCode);
    }
    http.end();
  }
}
/**
  버퍼에 데이터를 추가하고 왼쪽으로 미는 함수입니다
*/
char* addDataToBuffer(char buffer[], char newData, int bufferSize) {
     // 왼쪽으로 한 칸씩 밀기
    for (byte i = 0; i < bufferSize - 1; ++i) {
        buffer[i] = buffer[i + 1];
    }
    // 최신 데이터를 마지막 위치에 추가
    buffer[bufferSize - 1] = newData;
    return buffer;
}

void printBuffer(const char buffer[], int bufferSize) {
    for (int i = 0; i < bufferSize; ++i) {
        Serial.print(buffer[i]);
        Serial.print(' ');
        BTSerial.print(buffer[i]);
        BTSerial.print(' ');
    }
    Serial.println();
    BTSerial.println();
}

String getStringBuffer(bool rotat, short startIndex, short endIndex, const char* inputString){
  String cash = "";
  if(!rotat){
    // Serial.print("cash : ");
    for(short i = startIndex; startIndex <= endIndex; startIndex++){
      cash += String(inputString[startIndex]);
    }
    // Serial.print(cash);
    // Serial.println();
  }
  else{
    for(short i = startIndex; startIndex >= endIndex; startIndex--){
      cash += String(inputString[startIndex]);
    }
  }
  return cash;
}

PageSearchState pageCheckAndTrigger(PageSearchState state, const char* inputString) {
      
      String buffer = getStringBuffer(false, 4, 7, inputString);
      if (strcmp(buffer.c_str(), state.stringId) == 0){
          state.isId = true;
          BTSerial.println("Id true");
          // delay(500);
      }
      buffer = getStringBuffer(false, 0, 7, inputString);
      if (strcmp(buffer.c_str(), state.stringPrompt) == 0){
          state.isPrompt = true;
          Serial.println("Prompt true");
          BTSerial.println("Prompt true");
          // delay(200);
      }
      buffer = getStringBuffer(false, 2, 7, inputString);
      if (strcmp(buffer.c_str(), state.stringTags) == 0){
          state.isTags = true;
          Serial.println("Tags true");
          BTSerial.println("Tags true");
          // delay(200);
      }
      buffer = getStringBuffer(false, 1, 7, inputString);
      if (strcmp(buffer.c_str(), state.stringTitle) == 0){
          state.isTitle = true;
          Serial.println("Title true");
          BTSerial.println("Title true");
          // delay(200);
      }
      // delay(500);
    return state;
}

/**
  문자열을 비교해주는 함수 입니다
*/
bool endBufferCheck(char buffer[], const char* currentString, int bufferSize){
  for (short i = 0; i < bufferSize; ++i){
    if(buffer[i] != currentString[i])
      return false;
  }
  return true;
}

void setItemField(char* field, const char* value, size_t fieldSize) {
    strncpy(field, value, fieldSize - 1);
    field[fieldSize - 1] = '\0'; // Null 문자 보장
}

void getFeed(byte page_number = 0) {
  byte itemCount = 0;  // 저장된 item 갯수
  Item currentItem; // Item 구조체
  byte start_page;
  byte end_page;
  if (WiFi.status() == WL_CONNECTED && bearerToken != "") {
    WiFiClientSecure client;
    client.setInsecure();  // 인증서 검증 비활성화 (테스트 목적)
    HTTPClient http;
    String url = String(suno_api_url) + "/api/feed/?page=" + String(page_number/2);
    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + bearerToken);

    if (page_number % 2 == 0){
      start_page = 0;
      end_page = 10;
    }else{
      start_page = 11;
      end_page = 20;
    }

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      WiFiClient* stream = http.getStreamPtr();
      byte bracket_open_count = 0; // [] 즉 json이 끝났는지를 채크
      byte brace_open_count = 0; // 몇번의 {} 깊이가 있는지 체크
      byte brace_end_count = 0;  // 몇번의 {} 가 끝났는지 체크
      byte doubleQuoteCount = 0; // "" 쌍따음표 채크
      byte singleQuoteCount = 0; // '' 작은 따음표 체크
      bool dataFlag = false;
      // std::vector<Item> Items;  // Items 백터
      PageSearchState state;  // state 구조체
      char filterBuffer[8] = {'\0'};  // 필터 버퍼
      char responseBuffer[128] = {'\0'}; // 데이터 버퍼
      short responseBufferIndex = 0; // 데이터 인덱스
      bool capturing = false; // json이 시작되었는지 체크

      char* updatedBuffer;
      char* updatedEndBuffer;

      strcpy(currentItem.id, "12345");
      strcpy(currentItem.major_model_version, "1.0");
      strcpy(currentItem.tags, "example,arduino,project");
      strcpy(currentItem.title, "Arduino Project");
      while (http.connected() || stream->available()) {
        if (stream->available()) {
            char current_char = char(stream->read());

            // Update bracket and brace counts
            if (current_char == '[') {
                bracket_open_count++;
            } else if (current_char == ']') {
                bracket_open_count--;
            } else if (current_char == '{') {
                brace_open_count++;
            } else if (current_char == '}') {
                brace_open_count--;
            } else if (current_char == ',') {
                if (debugmode == true){
                  Serial.print("\n");
                  BTSerial.print("\n");
                  printTab(brace_open_count);
                }
            } else if (current_char == '\"') {
                if (doubleQuoteCount % 2 == 0) {
                    doubleQuoteCount++;
                } else {
                    doubleQuoteCount--;
                }
            } else if (current_char == '\'') {
                if (singleQuoteCount % 2 == 0) {
                    singleQuoteCount++;
                } else {
                    singleQuoteCount--;
                }
            }

            //출력 담당부분
            if (debugmode == true){
              Serial.print(current_char);
              BTSerial.print(current_char);
            }
            // json 시작하고나서 시작
            if (brace_open_count == 0 && capturing && current_char == '}') {
                capturing = false;
                brace_end_count += 1;
                Serial.println();
                Serial.print("id_save : ");
                Serial.println(currentItem.id);
                Serial.println();
                Serial.print("tags_save : ");
                Serial.println(currentItem.tags);
                Serial.println();
                Serial.print("title_save : ");
                Serial.println(currentItem.title);
                Serial.println();
                BTSerial.print("id_save : ");
                BTSerial.println(currentItem.id);
                BTSerial.println();
                BTSerial.print("tags_save : ");
                BTSerial.println(currentItem.tags);
                BTSerial.println();
                BTSerial.print("title_save : ");
                BTSerial.println(currentItem.title);
                // Items.push_back(currentItem);
                if (itemCount < Items.size()) {  // 최대 개수를 초과하지 않도록 확인
                    Items[itemCount++] = currentItem;
                }
            }else if (brace_open_count == 1 && current_char == '{') {
                capturing = true;
                memset(&currentItem, 0, sizeof(Item));
            }else if (bracket_open_count == 0 && current_char == ']') {
                Serial.println("\nBrackets are balanced. Complete JSON received.");
                break;
            }

            if (capturing && brace_end_count > start_page && brace_end_count < end_page) {
              updatedBuffer = addDataToBuffer(filterBuffer, current_char, 8);
              
              if (state.isId || state.isPrompt || state.isTags || state.isTitle){
                if (current_char == ' '){
                  dataFlag = true;
                  // Serial.print("data Flag on ");
                }
                if(dataFlag){
                  if (state.isId) {
                    String cash = getStringBuffer(false, 6, 7, updatedBuffer);
                    // Serial.println();
                    // Serial.print("cash : ");
                    // Serial.println(cash);
                    String check_cash = "\",";
                    if (strcmp(cash.c_str(), check_cash.c_str()) == 0){
                      dataFlag = false;
                      state.isId = false;
                      // Serial.println("data Flag off");
                      // Serial.println(responseBuffer);
                      responseBufferIndex = 0;
                      char cash_id[40] ; 
                      if(responseBufferIndex > 39)
                        Serial.println("------------ERROR responseBufferIndex is id 40 OVER");
                      for(short i = 0 ; i < 40 ; i ++){
                        cash_id[i] = responseBuffer[i];
                        responseBuffer[i] = '\0';
                      }
                      memcpy(currentItem.id, cash_id, sizeof(cash_id));
                    }
                    else if(current_char != '\"'){
                      responseBuffer[responseBufferIndex] = current_char;
                      if (responseBufferIndex < 128){
                        responseBufferIndex += 1;
                      }
                      else{
                        Serial.println("-------------ERROR responseBufferIndex OVER--------");
                        Serial.println(responseBuffer);
                      }
                      
                    }
                  }if (state.isTags) {
                    String cash = getStringBuffer(false, 6, 7, updatedBuffer);
                    // Serial.println();
                    // Serial.print("cash : ");
                    // Serial.println(cash);
                    String check_cash = "\",";
                    if (strcmp(cash.c_str(), check_cash.c_str()) == 0){
                      dataFlag = false;
                      state.isTags = false;
                      // Serial.println("data Flag off");
                      // Serial.println(responseBuffer);
                      responseBufferIndex = 0;
                      char cash_tag[128]; 
                      if(responseBufferIndex > 128)
                        Serial.println("------------ERROR responseBufferIndex is id 128 OVER");
                      
                      for(short i = 0 ; i < 128 ; i ++){
                        cash_tag[i] = responseBuffer[i];
                        responseBuffer[i] = '\0';
                      }
                      memcpy(currentItem.tags, responseBuffer, sizeof(responseBuffer));
                    }
                    else if(current_char != '\"'){
                      responseBuffer[responseBufferIndex] = current_char;
                      if (responseBufferIndex < 128){
                        responseBufferIndex += 1;
                      }
                      else{
                        Serial.println("-------------ERROR responseBufferIndex OVER--------");
                        Serial.println(responseBuffer);
                      }
                      
                    }
                  }if (state.isPrompt) {
                    String cash = getStringBuffer(false, 6, 7, updatedBuffer);
                    // Serial.println();
                    // Serial.print("cash : ");
                    // Serial.println(cash);
                    String check_cash = "\",";
                    if (strcmp(cash.c_str(), check_cash.c_str()) == 0){
                      dataFlag = false;
                      state.isPrompt = false;
                      // Serial.println("data Flag off");
                      // Serial.println(responseBuffer);
                      responseBufferIndex = 0;
                      char cash_prompt[60]; 
                      strcpy(cash_prompt, "(auto create) ");
                      if(responseBuffer == ""){
                        for(short i = 14 ; i < 60 ; i ++){
                          cash_prompt[i] = responseBuffer[i];
                        }
                      }else{
                        for(short i = 0 ; i < 60 ; i ++){
                          cash_prompt[i] = responseBuffer[i];
                        }
                      }
                      
                      memset(responseBuffer, '\0', sizeof(responseBuffer));
                      if(responseBufferIndex > 60)
                        Serial.println("------------ERROR responseBufferIndex is id 60 OVER");
                      
                      memcpy(currentItem.tags, cash_prompt, sizeof(cash_prompt));
                    }
                    else if(current_char != '\"'){
                      responseBuffer[responseBufferIndex] = current_char;
                      if (responseBufferIndex < 60){
                        responseBufferIndex += 1;
                      }
                      else{
                        // Serial.println("-------------ERROR responseBufferIndex OVER--------");
                        // Serial.println(responseBuffer);
                      }
                      
                    }
                  }if (state.isTitle) {
                    String cash = getStringBuffer(false, 6, 7, updatedBuffer);
                    // Serial.println();
                    // Serial.print("cash : ");
                    // Serial.println(cash);
                    String check_cash = "\",";
                    if (strcmp(cash.c_str(), check_cash.c_str()) == 0){
                      dataFlag = false;
                      state.isTitle = false;
                      // Serial.println("data Flag off");
                      // Serial.println(responseBuffer);
                      responseBufferIndex = 0;
                      char cash_title[60]; 
                      if(responseBufferIndex > 60)
                        // Serial.println("------------ERROR responseBufferIndex is id 60 OVER");
                      for(short i = 0 ; i < 60 ; i ++){
                        cash_title[i] = responseBuffer[i];
                        responseBuffer[i] = '\0';
                      }
                      memcpy(currentItem.tags, cash_title, sizeof(cash_title));
                    }
                    else if(current_char != '\"'){
                      responseBuffer[responseBufferIndex] = current_char;
                      if (responseBufferIndex < 60){
                        responseBufferIndex += 1;
                      }
                      else{
                        Serial.println("-------------ERROR responseBufferIndex OVER--------");
                        Serial.println(responseBuffer);
                        BTSerial.println("-------------ERROR responseBufferIndex OVER--------");
                        BTSerial.println(responseBuffer);
                      }
                      
                    }
                  }
                }
              }
              state = pageCheckAndTrigger(state, updatedBuffer);
              // printBuffer(filterBuffer, 6);
              // delay(00);
            }
        }
      }
    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpResponseCode);
      BTSerial.print("Error on HTTP request: ");
      BTSerial.println(httpResponseCode);
    }
    http.end();
  }
}

String getNextTrackFilename(String id) {
  int maxTrackNumber = 0;
  File root = SD.open("/");
  while (true) {
    File entry = root.openNextFile();
    if (!entry) {
      break;
    }
    String fileName = entry.name();
    entry.close();

    if (fileName.endsWith(".mp3")) {
      int trackNumber = fileName.substring(0, fileName.indexOf('_')).toInt();
      if (trackNumber > maxTrackNumber) {
        maxTrackNumber = trackNumber;
      }
    }
  }
  maxTrackNumber++;
  char trackName[30];
  sprintf(trackName, "%04d_%s.mp3", maxTrackNumber, id.c_str());
  return String(trackName);
}

void music_Play(){
  digitalWrite(D0, HIGH);
}
String readInput() {
  Serial.print("exit = ^\n");
  BTSerial.print("exit = ^\n");
  String input = "";
  while (true) {
    if (Serial.available() > 0 || BTSerial.available() > 0) {
    bool bt = BTSerial.available() > 0; // 블루투스 데이터가 먼저 도착한 경우 true

    if (!bt) {
      if (Serial.available()) {
      char c = Serial.read();
      if (c == '^') {
        Serial.println();
        break;
      } else {
        input += c;
        Serial.print(c);
        }
      }
    } else {
      if (BTSerial.available()) {
        char c = BTSerial.read();
        if (c == '^') {
          BTSerial.println();
          break;
        } else {
          input += c;
          BTSerial.print(c);
          }
        }
      }
    }
  
  } 
  return input;
}


void wifiSetting(){
  String newSSID = readInput();
  strcpy(ssid, newSSID.c_str());
  Serial.print("set Ssid : ");
  Serial.println(ssid);
  BTSerial.print("set Ssid : ");
  BTSerial.println(ssid);
  String newPassword = readInput();
  strcpy(password, newPassword.c_str());
  Serial.print("set Password : ");
  Serial.println(ssid);
  BTSerial.print("set Password : ");
  BTSerial.println(ssid);

  WiFi.begin(ssid, password);
  byte i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    BTSerial.println("Connecting to WiFi...");
    i++;
    if (i > 5){
      Serial.println("check ssid and password");
      BTSerial.println("check ssid and password");
      return;
    }
  }
  Serial.println("Connected to WiFi");
  BTSerial.println("Connected to WiFi");
}

void generatorSong(){
  
  WiFiClientSecure  client;
  client.setInsecure();
  Serial.println("Enter prompt: ");
  BTSerial.println("Enter prompt: ");
  String prompt = readInput();
  
  Serial.println("Enter tags (comma separated): ");
  BTSerial.println("Enter tags (comma separated): ");
  String tags = readInput();
  
  Serial.println("Enter title: ");
  BTSerial.println("Enter title: ");
  String title = readInput();

  HTTPClient http;

  getNewToken();

  http.begin(client, "https://studio-api.suno.ai/api/generate/v2/");

  http.addHeader("Content-Type", "text/plain;charset=UTF-8");
  http.addHeader("Authorization", "Bearer "+ String(bearerToken));

  String payload = "{\"prompt\":\"" + prompt + "\",\"tags\":\"" + tags + "\",\"mv\":\"chirp-v3-5\",\"title\":\"" + title + "\",\"continue_clip_id\":null,\"continue_at\":null,\"infill_start_s\":null,\"infill_end_s\":null}";
  
  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
    BTSerial.println(httpResponseCode);
    BTSerial.println(response);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
    BTSerial.print("Error on sending POST: ");
    BTSerial.println(httpResponseCode);
  }
  http.end();
}

void fetchFile(String raw_id, String fileName) {
  sdMountCheck();
  String id = raw_id;
  id.trim();
  HTTPClient http;
  WiFiClientSecure client;
  String url = String(audio_douwnlod_url) + id + String(".mp3");
  client.setInsecure();  // 인증서 검증 비활성화 (테스트 목적)
  Serial.print("URL : ");
  Serial.println(url);
  Serial.println("Starting HTTP GET request...");
  BTSerial.print("URL : ");
  BTSerial.println(url);
  BTSerial.println("Starting HTTP GET request...");
  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.printf("HTTP GET request successful, code: %d\n", httpCode);
    BTSerial.printf("HTTP GET request successful, code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      File file = SD.open("/mp3/"+ fileName + ".mp3", FILE_WRITE);
      if (!file) {
        Serial.println("Failed to open file for writing");
        BTSerial.println("Failed to open file for writing");
        
        return;
      }

      WiFiClient *stream = http.getStreamPtr();
      int len = http.getSize();
      uint8_t buff[128] = { 0 };
      int c = 0;

      Serial.println("Starting file download...");
      Serial.println("BTSerial file download...");
      while (http.connected() && (len > 0 || len == -1)) {
        size_t size = stream->available();
        if (size) {
          int bytes = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
          file.write(buff, bytes);
          if (len > 0) {
            len -= bytes;
          }
          c += bytes;
          Serial.printf("Downloaded %d bytes\n", c);
          BTSerial.printf("Downloaded %d bytes\n", c);
        }
        delay(1);
      }
      file.close();
      Serial.println("File downloaded successfully");
      BTSerial.println("File downloaded successfully");
    }
  } else {
    Serial.printf("GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}


void setup() {
  BTSerial.begin(115200);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinMode(D0, OUTPUT);
  byte loop_count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    BTSerial.println("Connecting to WiFi...");
    loop_count += 1;
  }
  Serial.println("Connected to WiFi");
  BTSerial.println("Connected to WiFi");
}

void loop() {
  // Check if data is available on Serial
   if (Serial.available() > 0 || BTSerial.available() > 0) {
    bool bt = BTSerial.available() > 0; // 블루투스 데이터가 먼저 도착한 경우 true

    // Read the incoming URL from Serial or BTSerial
    String command;
    if (bt) {
      command = BTSerial.readStringUntil('\n');
    } else {
      command = Serial.readStringUntil('\n');
    }
    command.trim(); // 앞뒤 공백 제거
    
    // 명령어와 옵션 분리
    int spaceIndex = command.indexOf(' ');
    String cmd, option;
    if (spaceIndex == -1) {
        cmd = command;
    } else {
        cmd = command.substring(0, spaceIndex);
        option = command.substring(spaceIndex + 1);
    }
    switch (getCommandType(cmd)) {
      case 1:{
          Serial.print("Load song list with page option: ");
          Serial.println(option);
          BTSerial.print("Load song list with page option: ");
          BTSerial.println(option);
          getNewToken();
          byte optionByte = option.toInt();
          getFeed(optionByte);
          break;
      } case 2:{
          Serial.println("Show song list");
          BTSerial.println("Show song list");
          byte optionByte = option.toInt();
          if(option == "");
            optionByte = 0;
          printItems(optionByte);
          break;
      } case 3:{
          Serial.print("Search song by id: ");
          BTSerial.println(option);
          Serial.print("Search song by id: ");
          BTSerial.println(option);
          // Search song by id logic here
          break;
      } case 4:{
          Serial.print("Search song by title: ");
          Serial.println(option);
          BTSerial.print("Search song by title: ");
          BTSerial.println(option);
          // Search song by title logic here
          break;
      } case 5:{
          Serial.print("Download song by id : ");
          Serial.println(option);
          BTSerial.print("Download song by id : ");
          BTSerial.println(option);
          String fileName = getNextTrackFilename(option);
          fetchFile(option, fileName);
          break;
      } case 6:{
          Serial.print("Download song by title: ");
          Serial.println(option);
          BTSerial.print("Download song by title: ");
          BTSerial.println(option);
          
          // Download song by title logic here
          break;
      } case 7:{
          Serial.print("Download song by num: ");
          Serial.println(option);
          BTSerial.print("Download song by num: ");
          BTSerial.println(option);
          int num = option.toInt();
          if (num >= 0 && num < Items.size()) {
            String cash = Items[num].id;
            String fileName = getNextTrackFilename(cash);
            fetchFile(cash, fileName);
          }else {
              Serial.println("Invalid option index");
          }
          // Download song by title logic here
          break;
      } case 8:{
          Serial.print("print command list ");
          BTSerial.print("print command list ");
          printCommandList();
          break;
      } case 9:{
          Serial.print("debugmode : ");
          Serial.println(option);
          BTSerial.print("debugmode : ");
          BTSerial.println(option);
          if (option == "true" or 1)
            debugmode = true;
          else if (option == "false" or 0)
            debugmode = false;
         break;
      }case 10:{
          Serial.print("create song");
          Serial.println(option);
          BTSerial.print("create song");
          BTSerial.println(option);
          generatorSong();
         break;
      }case 11:{
          Serial.print("music play");
          Serial.println(option);
          BTSerial.print("music play");
          BTSerial.println(option);
          music_Play();
         break;
      }case 12:{
          Serial.print("wifi setting");
          Serial.println(option);
          BTSerial.print("wifi setting");
          BTSerial.println(option);
          wifiSetting();
         break;
      }default:{
          Serial.println("Unknown command");
          BTSerial.println("Unknown command");
         break;
      }
    }
    // String url = Serial.readStringUntil('\n');
    // url.trim();  // Remove any leading or trailing whitespace
    // if (url.length() > 0) {
    //   Serial.println("URL received: " + url);
    //   fetchFile(url);
    // } else {
    //   Serial.println("No URL entered. Please try again.");
    // }
  }
}
