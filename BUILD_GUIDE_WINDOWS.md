# CLI_AES(v1.0), src, test_vectors Build Guide

본 문서는 CLI_AES(v1.0), src, test_vectors 프로젝트의 **Windows** 환경에서의 빌드 방법을 설명한다.
프로젝트 빌드 전, 반드시 해당 방법을 준수하여야 한다.

자세한 내용은 "라이브러리 소스코드 사용설명서"를 참고

---

## BUILD_GUIDE_WINDOWS.md

### 1. 실행 환경 설정

#### 1-a. 사전 준비

* **Visual Studio 2022** (C++ 개발 도구 설치 필수)
* **Git** 설치 필요
  [https://git-scm.com/download/win](https://git-scm.com/download/win)

---

### 1-b. vcpkg 설치

원하는 폴더에서 vcpkg를 다운로드한다.

명령 프롬프트(cmd) 또는 PowerShell 실행 후:

```bat
git clone https://github.com/microsoft/vcpkg
```

예시:

```
C:\dev\vcpkg
```

---

### 1-c. vcpkg 초기 빌드

vcpkg 디렉터리로 이동 후 부트스트랩 실행:

```bat
cd vcpkg
bootstrap-vcpkg.bat
```
📌 실행 완료 후 vcpkg.exe가 생성되면 성공이다.

---
### 1-d. OpenSSL 설치

64비트(권장):

```bat
vcpkg install openssl:x64-windows
```

* 설치에는 약 **10~20분**이 소요될 수 있다.
* OpenSSL, crypto 라이브러리, 헤더 파일이 자동으로 설치된다.

32비트가 필요한 경우:

```bat
vcpkg install openssl:x86-windows
```

---

### 1-e. Visual Studio 자동 연동 설정

```bat
vcpkg integrate install
```

* Visual Studio가 vcpkg 라이브러리를 자동 인식하도록 레지스트리에 등록된다.
* 이후 프로젝트에서 include/lib 경로를 별도로 설정할 필요가 없다.

---

### 1-f. OpenSSL 전처리기 설정

1. Visual Studio에서 프로젝트 열기
2. 상단 메뉴 **프로젝트 → 속성**
3. **C/C++ → 전처리기** 이동
4. 전처리기 정의 항목에서 기존 내용을 제거 후 아래 내용 입력

```
USE_OPENSSL;PLATFORM_WINDOWS
```

5. 적용 후 확인

---

### 2. 컴파일

* 대상 프로젝트: **CLI_AES(v1.0)**
* `cli.c` 파일에 `main()` 함수가 포함되어 있음

Visual Studio에서 **빌드(Build)** 실행

src/sample.c, test_vectors/test.c 에 main 함수 존재

---

### 3. 컴파일 오류 발생 시 (인코딩 문제)

일부 환경에서 소스코드 인코딩 문제로 컴파일 오류가 발생할 수 있다.

#### 해결 방법: UTF-8 인코딩 통일

##### (선택) 자동 변환 도구 사용

(도구 다운로드는 선택사항이나, 다운로드를 하지 않을 경우 모든 소스코드 파일을 직접 UTF-8로 인코딩 해주어야 함)

* Visual Studio 확장 도구 다운로드:
  [https://marketplace.visualstudio.com/items?itemName=Bluehill.EncondingConverter]

##### 사용 방법

1. 솔루션 탐색기에서 **솔루션 이름 우클릭 → 인코딩 변환**
2. 인코딩 **UTF-8** 선택 후 확인
3. **프로젝트 → 속성 → C/C++ → 명령줄** 이동
4. 추가 옵션에 아래 항목 입력

   ```
   /utf-8
   ```
5. 확인 후 다시 빌드

### 참고 사항

* 본 프로젝트는 **OpenSSL libcrypto**에 의존한다.
* `USE_OPENSSL`, `PLATFORM_WINDOWS` 전처리기 정의는 필수이다.
* C 표준은 **C99**를 기준으로 컴파일된다.
