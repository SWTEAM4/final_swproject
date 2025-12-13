/**
 * CLI_AES 라이브러리 통합 샘플 코드
 * 
 * 이 파일은 라이브러리의 모든 주요 기능을 보여주는 예제를 포함합니다:
 * - SHA-512 해시
 * - HMAC-SHA512
 * - AES CTR 암호화/복호화
 * - PBKDF2 키 도출
 * - 키 도출 유틸리티
 * - 파일 암호화/복호화
 * - 통합 암호화 프로세스
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 라이브러리 헤더
#include "crypto_api.h"
#include "sha512.h"
#include "hmac_sha512.h"
#include "aes.h"
#include "kdf.h"
#include "key_derivation.h"
#include "file_crypto.h"

// ============================================================================
// 1. SHA-512 해시 예제
// ============================================================================

void example_sha512_simple(void) {
    printf("\n=== 1. SHA-512 해시 예제 (간단) ===\n");
    
    const char* message = "Hello, World!";
    uint8_t hash[SHA512_DIGEST_LENGTH];
    SHA512_CTX ctx;
    
    // 초기화
    if (sha512_init(&ctx) != CRYPTO_SUCCESS) {
        printf("SHA-512 초기화 실패\n");
        return;
    }
    
    // 데이터 추가
    if (sha512_update(&ctx, (const uint8_t*)message, strlen(message)) != CRYPTO_SUCCESS) {
        printf("SHA-512 업데이트 실패\n");
        return;
    }
    
    // 최종 해시 계산
    if (sha512_final(&ctx, hash) != CRYPTO_SUCCESS) {
        printf("SHA-512 최종화 실패\n");
        return;
    }
    
    // 결과 출력
    printf("입력: %s\n", message);
    printf("SHA-512 해시: ");
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

void example_sha512_streaming(void) {
    printf("\n=== 2. SHA-512 해시 예제 (스트리밍) ===\n");
    
    const char* part1 = "Hello, ";
    const char* part2 = "World!";
    uint8_t hash[SHA512_DIGEST_LENGTH];
    SHA512_CTX ctx;
    
    sha512_init(&ctx);
    sha512_update(&ctx, (const uint8_t*)part1, strlen(part1));
    sha512_update(&ctx, (const uint8_t*)part2, strlen(part2));
    sha512_final(&ctx, hash);
    
    printf("입력: %s%s\n", part1, part2);
    printf("SHA-512 해시: ");
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

// ============================================================================
// 3. HMAC-SHA512 예제
// ============================================================================

void example_hmac_oneshot(void) {
    printf("\n=== 3. HMAC-SHA512 예제 (원샷) ===\n");
    
    const char* key = "secret-key";
    const char* message = "Hello, World!";
    uint8_t mac[HMAC_SHA512_DIGEST_SIZE];
    
    // 원샷 HMAC 계산
    hmac_sha512((const uint8_t*)key, strlen(key),
                (const uint8_t*)message, strlen(message),
                mac);
    
    printf("키: %s\n", key);
    printf("메시지: %s\n", message);
    printf("HMAC-SHA512: ");
    for (int i = 0; i < HMAC_SHA512_DIGEST_SIZE; i++) {
        printf("%02x", mac[i]);
    }
    printf("\n");
}

void example_hmac_streaming(void) {
    printf("\n=== 4. HMAC-SHA512 예제 (스트리밍) ===\n");
    
    const char* key = "secret-key";
    const char* part1 = "Hello, ";
    const char* part2 = "World!";
    uint8_t mac[HMAC_SHA512_DIGEST_SIZE];
    HMAC_SHA512_CTX ctx;
    
    // 스트리밍 HMAC
    hmac_sha512_init(&ctx, (const uint8_t*)key, strlen(key));
    hmac_sha512_update(&ctx, (const uint8_t*)part1, strlen(part1));
    hmac_sha512_update(&ctx, (const uint8_t*)part2, strlen(part2));
    hmac_sha512_final(&ctx, mac);
    
    printf("키: %s\n", key);
    printf("메시지: %s%s\n", part1, part2);
    printf("HMAC-SHA512: ");
    for (int i = 0; i < HMAC_SHA512_DIGEST_SIZE; i++) {
        printf("%02x", mac[i]);
    }
    printf("\n");
}

// ============================================================================
// 5. AES CTR 암호화/복호화 예제
// ============================================================================

void example_aes_ctr_encrypt(void) {
    printf("\n=== 5. AES CTR 암호화/복호화 예제 ===\n");
    
    // AES-256 키 (32 bytes)
    uint8_t key[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };
    
    const char* plaintext = "Hello, World! This is a test message.";
    size_t plaintext_len = strlen(plaintext);
    
    uint8_t nonce_counter[AES_BLOCK_SIZE] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    uint8_t ciphertext[256];
    uint8_t decrypted[256];
    
    AES_CTX ctx;
    
    // 키 설정
    if (AES_set_key(&ctx, key, 256) != CRYPTO_SUCCESS) {
        printf("AES 키 설정 실패\n");
        return;
    }
    
    // 암호화 (CTR 모드는 암호화와 복호화가 동일)
    uint8_t nonce_enc[AES_BLOCK_SIZE];
    memcpy(nonce_enc, nonce_counter, AES_BLOCK_SIZE);
    
    if (AES_CTR_crypt(&ctx, (const uint8_t*)plaintext, plaintext_len,
                      ciphertext, nonce_enc) != CRYPTO_SUCCESS) {
        printf("암호화 실패\n");
        return;
    }
    
    printf("평문: %s\n", plaintext);
    printf("암호문: ");
    for (size_t i = 0; i < plaintext_len; i++) {
        printf("%02x", ciphertext[i]);
    }
    printf("\n");
    
    // 복호화
    uint8_t nonce_dec[AES_BLOCK_SIZE];
    memcpy(nonce_dec, nonce_counter, AES_BLOCK_SIZE);
    
    if (AES_CTR_crypt(&ctx, ciphertext, plaintext_len,
                      decrypted, nonce_dec) != CRYPTO_SUCCESS) {
        printf("복호화 실패\n");
        return;
    }
    
    decrypted[plaintext_len] = '\0';
    printf("복호문: %s\n", decrypted);
    
    if (memcmp(plaintext, decrypted, plaintext_len) == 0) {
        printf("✓ 암호화/복호화 검증 성공!\n");
    } else {
        printf("✗ 암호화/복호화 검증 실패!\n");
    }
}

// ============================================================================
// 6. PBKDF2 키 도출 예제
// ============================================================================

void example_pbkdf2(void) {
    printf("\n=== 6. PBKDF2-SHA512 키 도출 예제 ===\n");
    
    const char* password = "MySecurePassword123!";
    const char* salt_str = "randomsalt1234";
    
    uint8_t salt[16];
    memcpy(salt, salt_str, 16);
    
    uint8_t derived_key[64];  // 512-bit 키
    
    printf("패스워드: %s\n", password);
    printf("솔트: %s\n", salt_str);
    printf("반복 횟수: 10000\n");
    
    // PBKDF2-SHA512로 키 도출 (10000번 반복)
    pbkdf2_sha512((const uint8_t*)password, strlen(password),
                  salt, 16,
                  10000,  // 반복 횟수
                  derived_key, 64);
    
    printf("도출된 키 (512-bit): ");
    for (int i = 0; i < 64; i++) {
        printf("%02x", derived_key[i]);
    }
    printf("\n");
}

// ============================================================================
// 7. 키 도출 유틸리티 예제
// ============================================================================

void example_derive_keys(void) {
    printf("\n=== 7. 키 도출 유틸리티 예제 ===\n");
    
    const char* password = "MySecurePassword123!";
    uint8_t salt[16] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
    };
    
    uint8_t aes_key_128[16];  // AES-128 키
    uint8_t aes_key_256[32];  // AES-256 키
    uint8_t hmac_key[24];     // HMAC 키
    
    printf("패스워드: %s\n", password);
    printf("솔트: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x", salt[i]);
    }
    printf("\n");
    
    // AES-128 키와 HMAC 키 도출
    derive_keys(password, 128, salt, 16, aes_key_128, hmac_key);
    
    printf("\nAES-128 키: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x", aes_key_128[i]);
    }
    printf("\n");
    
    // AES-256 키와 HMAC 키 도출
    derive_keys(password, 256, salt, 16, aes_key_256, hmac_key);
    
    printf("AES-256 키: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", aes_key_256[i]);
    }
    printf("\n");
    
    printf("HMAC 키: ");
    for (int i = 0; i < 24; i++) {
        printf("%02x", hmac_key[i]);
    }
    printf("\n");
}

// ============================================================================
// 8. 통합 암호화 프로세스 예제
// ============================================================================

void example_complete_encryption(void) {
    printf("\n=== 8. 통합 암호화 프로세스 예제 ===\n");
    
    const char* password = "MyPassword123!";
    const char* plaintext = "This is a secret message.";
    size_t plaintext_len = strlen(plaintext);
    
    printf("평문: %s\n", plaintext);
    printf("패스워드: %s\n", password);
    
    // 1. 솔트 생성
    uint8_t salt[16];
    if (crypto_random_bytes(salt, 16) != CRYPTO_SUCCESS) {
        printf("✗ 솔트 생성 실패\n");
        return;
    }
    
    printf("\n1. 생성된 솔트: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x", salt[i]);
    }
    printf("\n");
    
    // 2. 키 도출
    uint8_t aes_key[32];  // AES-256
    uint8_t hmac_key[24];
    derive_keys(password, 256, salt, 16, aes_key, hmac_key);
    
    printf("2. 키 도출 완료\n");
    
    // 3. Nonce 생성
    uint8_t nonce[AES_BLOCK_SIZE];
    if (crypto_random_bytes(nonce, AES_BLOCK_SIZE) != CRYPTO_SUCCESS) {
        printf("✗ Nonce 생성 실패\n");
        return;
    }
    
    printf("3. 생성된 Nonce: ");
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        printf("%02x", nonce[i]);
    }
    printf("\n");
    
    // 4. AES CTR 암호화
    AES_CTX aes_ctx;
    if (AES_set_key(&aes_ctx, aes_key, 256) != CRYPTO_SUCCESS) {
        printf("✗ AES 키 설정 실패\n");
        return;
    }
    
    uint8_t ciphertext[256];
    uint8_t nonce_copy[AES_BLOCK_SIZE];
    memcpy(nonce_copy, nonce, AES_BLOCK_SIZE);
    
    if (AES_CTR_crypt(&aes_ctx, (const uint8_t*)plaintext, plaintext_len,
                      ciphertext, nonce_copy) != CRYPTO_SUCCESS) {
        printf("✗ 암호화 실패\n");
        return;
    }
    
    printf("4. 암호문: ");
    for (size_t i = 0; i < plaintext_len; i++) {
        printf("%02x", ciphertext[i]);
    }
    printf("\n");
    
    // 5. HMAC 계산
    uint8_t hmac[HMAC_SHA512_DIGEST_SIZE];
    hmac_sha512(hmac_key, 24, ciphertext, plaintext_len, hmac);
    
    printf("5. HMAC: ");
    for (int i = 0; i < HMAC_SHA512_DIGEST_SIZE; i++) {
        printf("%02x", hmac[i]);
    }
    printf("\n");
    
    // 6. 복호화 검증
    memcpy(nonce_copy, nonce, AES_BLOCK_SIZE);
    uint8_t decrypted[256];
    
    if (AES_CTR_crypt(&aes_ctx, ciphertext, plaintext_len,
                      decrypted, nonce_copy) != CRYPTO_SUCCESS) {
        printf("✗ 복호화 실패\n");
        return;
    }
    
    decrypted[plaintext_len] = '\0';
    printf("6. 복호문: %s\n", decrypted);
    
    // 7. HMAC 검증
    uint8_t hmac_verify[HMAC_SHA512_DIGEST_SIZE];
    hmac_sha512(hmac_key, 24, ciphertext, plaintext_len, hmac_verify);
    
    if (memcmp(hmac, hmac_verify, HMAC_SHA512_DIGEST_SIZE) == 0) {
        printf("7. ✓ HMAC 검증 성공!\n");
    } else {
        printf("7. ✗ HMAC 검증 실패!\n");
    }
    
    if (memcmp(plaintext, decrypted, plaintext_len) == 0) {
        printf("\n✓ 전체 프로세스 검증 성공!\n");
    } else {
        printf("\n✗ 전체 프로세스 검증 실패!\n");
    }
}

// ============================================================================
// 9. 파일 암호화/복호화 예제 (파일이 존재하는 경우에만 실행)
// ============================================================================

void example_file_crypto(void) {
    printf("\n=== 9. 파일 암호화/복호화 예제 ===\n");
    printf("(주의: 실제 파일이 존재해야 동작합니다)\n");
    
    const char* test_file = "test_sample.txt";
    const char* encrypted_file = "test_sample.enc";
    const char* decrypted_file = "test_sample_decrypted.txt";
    const char* password = "SamplePassword123!";
    
    // 테스트 파일 생성
    FILE* fp = fopen(test_file, "w");
    if (fp) {
        fprintf(fp, "This is a test file for encryption.\n");
        fprintf(fp, "This library supports AES-128, AES-192, and AES-256.\n");
        fclose(fp);
        printf("테스트 파일 생성: %s\n", test_file);
    } else {
        printf("테스트 파일 생성 실패\n");
        return;
    }
    
    // 파일 암호화
    printf("\n파일 암호화 중...\n");
    int result = encrypt_file(test_file, encrypted_file, 256, password);
    
    if (result == FILE_CRYPTO_SUCCESS) {
        printf("✓ 암호화 성공: %s -> %s\n", test_file, encrypted_file);
    } else {
        printf("✗ 암호화 실패: 에러 코드 %d\n", result);
        return;
    }
    
    // 키 길이 확인
    int key_bits = read_aes_key_length(encrypted_file);
    if (key_bits > 0) {
        printf("암호화된 파일의 AES 키 길이: %d bits\n", key_bits);
    }
    
    // 파일 복호화
    printf("\n파일 복호화 중...\n");
    char final_path[512];
    result = decrypt_file(encrypted_file, decrypted_file, password, final_path, sizeof(final_path));
    
    if (result == FILE_CRYPTO_SUCCESS) {
        printf("✓ 복호화 성공: %s -> %s\n", encrypted_file, final_path);
    } else {
        printf("✗ 복호화 실패: 에러 코드 %d\n", result);
    }
}

// ============================================================================
// 진행률 콜백 함수 (파일 암호화용)
// ============================================================================

void progress_callback(long processed, long total, void* user_data) {
    if (total > 0) {
        int percent = (int)((processed * 100) / total);
        printf("\r진행률: %ld / %ld bytes (%d%%)", processed, total, percent);
        fflush(stdout);
    }
}

// ============================================================================
// 메인 함수
// ============================================================================

int main(void) {
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║     CLI_AES 라이브러리 통합 샘플 코드                    ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    
    // 기본 암호화 기능 예제
    example_sha512_simple();
    example_sha512_streaming();
    example_hmac_oneshot();
    example_hmac_streaming();
    example_aes_ctr_encrypt();
    example_pbkdf2();
    example_derive_keys();
    example_complete_encryption();
    
    // 파일 암호화 예제 (선택적)
    // 주석을 해제하면 파일 암호화 예제도 실행됩니다
    // example_file_crypto();
    
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║              모든 예제 실행 완료                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    
    return 0;
}

