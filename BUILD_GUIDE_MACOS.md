# CLI_AES(v1.0), src, test_vectors Build Guide

본 문서는 CLI_AES(v1.0), src, test_vectors 프로젝트의 **macOS** 환경에서의 빌드 방법을 설명한다.
프로젝트 빌드 전, 반드시 해당 방법을 준수하여야 한다.
자세한 내용은 "라이브러리 소스코드 사용설명서"를 참고

---

## BUILD_GUIDE_MACOS.md

### 1. OpenSSL 설치

터미널에서 다음 명령어 실행

#### Homebrew 설치 (미설치 시)

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

#### OpenSSL 설치

```bash
brew install openssl
```

설치 확인:

```bash
brew list openssl
```

---

### 2. 프로젝트 디렉터리 이동

```bash
cd /path/to/CLI_AES\(v1.0\)
```

---

### 3. 컴파일

모든 C 소스 파일을 컴파일하여 하나의 실행 파일을 생성한다.

#### Apple Silicon (M1 / M2 등)

```bash
gcc -o cli \
 cli.c \
 aes_ctr.c \
 sha512.c \
 hmac_sha512.c \
 kdf.c \
 key_derivation.c \
 password_utils.c \
 random_utils.c \
 error_utils.c \
 file_path_utils.c \
 platform_utils.c \
 -I/opt/homebrew/opt/openssl/include \
 -L/opt/homebrew/opt/openssl/lib \
 -lcrypto \
 -DUSE_OPENSSL \
 -DPLATFORM_MAC \
 -std=c99 \
 -Wall
```

📌 OpenSSL 경로: `/opt/homebrew/opt/openssl`

---

#### Intel Mac

```bash
gcc -o cli \
 cli.c \
 aes_ctr.c \
 sha512.c \
 hmac_sha512.c \
 kdf.c \
 key_derivation.c \
 password_utils.c \
 random_utils.c \
 error_utils.c \
 file_path_utils.c \
 platform_utils.c \
 -I/usr/local/opt/openssl/include \
 -L/usr/local/opt/openssl/lib \
 -lcrypto \
 -DUSE_OPENSSL \
 -DPLATFORM_MAC \
 -std=c99 \
 -Wall
```

📌 OpenSSL 경로: `/usr/local/opt/openssl`

---

### 4. 실행

```bash
./cli
```
src, test_vectors의 경우 동일한 방법으로 컴파일 후 실행

---

### 참고 사항

* 본 프로젝트는 **OpenSSL libcrypto**에 의존한다.
* `USE_OPENSSL`, `PLATFORM_WINDOWS`, `PLATFORM_MAC` 전처리기 정의는 필수이다.
* C 표준은 **C99**를 기준으로 컴파일된다.
