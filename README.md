# 2025_Kwangwoon_DS_Project_2

반드시 제안서에 표기되어 있는 **구현 스펙 및 감점 조건**을 확인하고 구현할 것. 이를 확인하지 않고 이의 신청 시 점수 부여 불가능

---

## Update Notes  

업데이트 사항이 있을 시 바로 깃허브에 업로드 예정

---

## How to Clone Repository  

```bash
sudo apt-get install git
git clone https://github.com/evejaeyong/2025_Kwangwoon_DS_Project_2.git
```

---

## How to check memory leak 
- make 이후 생성된 run 파일 실행 전에 valgrind 명령어를 입력하면 메모리 누수를 확인 가능
- 반드시 run 파일이 있는 디렉토리 내에서 수행해야 함. cd(change directory)로 변경하기
```bash
sudo apt-get update
sudo apt-get install valgrind
valgrind ./run
```
- "All heap blocks were freed -- no leaks are possible" 메세지가 출력되어야 누수가 발생하지 않은 것, "LEAK SUMMARY"가 나온다면 누수가 발생
---

## 구현 고려사항

기본적으로 제안서를 확인, 추가적인 사항은 이후 추가될 예정
예시 command.txt 파일은 추후 업로드 예정
