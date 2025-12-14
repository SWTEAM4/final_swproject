/**
 * 암호화 라이브러리 샘플 코드
 * 
 * 이 파일은 제공된 암호화 라이브러리의 주요 기능을 사용하는 예제를 포함합니다:
 * - AES 블록 암호화/복호화
 * - AES CTR 모드
 * - SHA-512 해시
 * - HMAC-SHA512
 * - PBKDF2-SHA512 (키 파생 함수)
 * - 암호학적으로 안전한 난수 생성
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "crypto_api.h"
#include "aes.h"
#include "sha512.h"
#include "hmac_sha512.h"
#include "kdf.h"

// 헬퍼 함수: 바이트 배열을 16진수 문자열로 출력
void print_hex(const char* label, const uint8_t* data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

// ==================== 예제 1: AES 블록 암호화/복호화 ====================
void example_aes_block_encryption(void) {
    printf("\n========== Example 1: AES Block Encryption/Decryption ==========\n");
    
    // AES 컨텍스트 초기화
    AES_CTX ctx;
    
    // 256비트 키 (32바이트)
    uint8_t key[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };
    
    // 평문 블록 (16바이트)
    uint8_t plaintext[AES_BLOCK_SIZE] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };
    
    uint8_t ciphertext[AES_BLOCK_SIZE];
    uint8_t decrypted[AES_BLOCK_SIZE];
    
    // 키 설정
    CRYPTO_STATUS status = AES_set_key(&ctx, key, 256);
    if (status != CRYPTO_SUCCESS) {
        printf("AES key setup failed: %d\n", status);
        return;
    }
    
    print_hex("Plaintext", plaintext, AES_BLOCK_SIZE);
    
    // 암호화
    status = AES_encrypt_block(&ctx, plaintext, ciphertext);
    if (status != CRYPTO_SUCCESS) {
        printf("Encryption failed: %d\n", status);
        return;
    }
    print_hex("Ciphertext", ciphertext, AES_BLOCK_SIZE);
    
    // 복호화
    status = AES_decrypt_block(&ctx, ciphertext, decrypted);
    if (status != CRYPTO_SUCCESS) {
        printf("Decryption failed: %d\n", status);
        return;
    }
    print_hex("Decrypted", decrypted, AES_BLOCK_SIZE);
    
    // 검증
    if (memcmp(plaintext, decrypted, AES_BLOCK_SIZE) == 0) {
        printf("[OK] Encryption/Decryption successful!\n");
    } else {
        printf("[FAIL] Encryption/Decryption failed!\n");
    }
}

// ==================== 예제 2: AES CTR 모드 ====================
void example_aes_ctr_mode(void) {
    printf("\n========== Example 2: AES CTR Mode ==========\n");
    
    AES_CTX ctx;
    
    // 128비트 키 (16바이트)
    uint8_t key[16] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
    };
    
    // Nonce + Counter (16바이트)
    // 일반적으로 처음 12바이트는 nonce, 마지막 4바이트는 카운터
    uint8_t nonce_counter[AES_BLOCK_SIZE] = {
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
        0xf8, 0xf9, 0xfa, 0xfb, 0x00, 0x00, 0x00, 0x00
    };
    
    // 평문 (임의 길이)
    const char* message = "Hello, AES CTR Mode! This is a test message.";
    size_t message_len = strlen(message);
    uint8_t* plaintext = (uint8_t*)message;
    
    uint8_t* ciphertext = (uint8_t*)malloc(message_len);
    uint8_t* decrypted = (uint8_t*)malloc(message_len + 1); // null terminator 공간 추가
    
    if (!ciphertext || !decrypted) {
        printf("Memory allocation failed\n");
        if (ciphertext) free(ciphertext);
        if (decrypted) free(decrypted);
        return;
    }
    
    // null terminator를 위해 초기화
    memset(decrypted, 0, message_len + 1);
    
    // 키 설정
    CRYPTO_STATUS status = AES_set_key(&ctx, key, 128);
    if (status != CRYPTO_SUCCESS) {
        printf("AES key setup failed: %d\n", status);
        goto cleanup;
    }
    
    printf("Plaintext: %s\n", message);
    print_hex("Plaintext (hex)", plaintext, message_len);
    
    // 암호화 (CTR 모드는 암호화와 복호화가 동일)
    uint8_t nonce_counter_enc[AES_BLOCK_SIZE];
    memcpy(nonce_counter_enc, nonce_counter, AES_BLOCK_SIZE);
    status = AES_CTR_crypt(&ctx, plaintext, message_len, ciphertext, nonce_counter_enc);
    if (status != CRYPTO_SUCCESS) {
        printf("CTR encryption failed: %d\n", status);
        goto cleanup;
    }
    print_hex("Ciphertext (hex)", ciphertext, message_len);
    
    // 복호화 (CTR 모드는 암호화와 복호화가 동일)
    uint8_t nonce_counter_dec[AES_BLOCK_SIZE];
    memcpy(nonce_counter_dec, nonce_counter, AES_BLOCK_SIZE);
    status = AES_CTR_crypt(&ctx, ciphertext, message_len, decrypted, nonce_counter_dec);
    if (status != CRYPTO_SUCCESS) {
        printf("CTR decryption failed: %d\n", status);
        goto cleanup;
    }
    print_hex("Decrypted (hex)", decrypted, message_len);
    
    // 검증
    if (memcmp(plaintext, decrypted, message_len) == 0) {
        printf("[OK] CTR mode encryption/decryption successful!\n");
        printf("Decrypted message: %s\n", (char*)decrypted);
    } else {
        printf("[FAIL] CTR mode encryption/decryption failed!\n");
    }
    
cleanup:
    free(ciphertext);
    free(decrypted);
}

// ==================== 예제 3: SHA-512 해시 ====================
void example_sha512(void) {
    printf("\n========== Example 3: SHA-512 Hash ==========\n");
    
    SHA512_CTX ctx;
    uint8_t hash[SHA512_DIGEST_LENGTH];
    
    // 테스트 메시지
    const char* message = "Hello, SHA-512!";
    size_t message_len = strlen(message);
    
    printf("Input message: %s\n", message);
    
    // 해시 계산
    CRYPTO_STATUS status = sha512_init(&ctx);
    if (status != CRYPTO_SUCCESS) {
        printf("SHA-512 initialization failed: %d\n", status);
        return;
    }
    
    status = sha512_update(&ctx, (const uint8_t*)message, message_len);
    if (status != CRYPTO_SUCCESS) {
        printf("SHA-512 update failed: %d\n", status);
        return;
    }
    
    status = sha512_final(&ctx, hash);
    if (status != CRYPTO_SUCCESS) {
        printf("SHA-512 finalization failed: %d\n", status);
        return;
    }
    
    print_hex("SHA-512 Hash", hash, SHA512_DIGEST_LENGTH);
    
    // 스트리밍 방식 예제 (큰 데이터를 여러 번에 나눠 처리)
    printf("\n--- Streaming Example ---\n");
    const char* chunks[] = {"Hello, ", "SHA-512! ", "Streaming mode."};
    sha512_init(&ctx);
    for (int i = 0; i < 3; i++) {
        sha512_update(&ctx, (const uint8_t*)chunks[i], strlen(chunks[i]));
    }
    sha512_final(&ctx, hash);
    print_hex("SHA-512 Hash (streaming)", hash, SHA512_DIGEST_LENGTH);
}

// ==================== 예제 4: HMAC-SHA512 ====================
void example_hmac_sha512(void) {
    printf("\n========== Example 4: HMAC-SHA512 ==========\n");
    
    // 키
    const char* key_str = "secret_key";
    const uint8_t* key = (const uint8_t*)key_str;
    size_t key_len = strlen(key_str);
    
    // 메시지
    const char* message = "Hello, HMAC-SHA512!";
    size_t message_len = strlen(message);
    
    uint8_t mac[HMAC_SHA512_DIGEST_SIZE];
    
    printf("Key: %s\n", key_str);
    printf("Message: %s\n", message);
    
    // 원샷 HMAC 계산
    hmac_sha512(key, key_len, (const uint8_t*)message, message_len, mac);
    print_hex("HMAC-SHA512 (one-shot)", mac, HMAC_SHA512_DIGEST_SIZE);
    
    // 스트리밍 방식 HMAC 계산
    printf("\n--- Streaming Example ---\n");
    HMAC_SHA512_CTX ctx;
    hmac_sha512_init(&ctx, key, key_len);
    
    const char* chunks[] = {"Hello, ", "HMAC-SHA512! ", "Streaming mode."};
    for (int i = 0; i < 3; i++) {
        hmac_sha512_update(&ctx, (const uint8_t*)chunks[i], strlen(chunks[i]));
    }
    
    uint8_t mac_streaming[HMAC_SHA512_DIGEST_SIZE];
    hmac_sha512_final(&ctx, mac_streaming);
    print_hex("HMAC-SHA512 (streaming)", mac_streaming, HMAC_SHA512_DIGEST_SIZE);
}

// ==================== 예제 5: PBKDF2-SHA512 (키 파생) ====================
void example_pbkdf2(void) {
    printf("\n========== Example 5: PBKDF2-SHA512 (Key Derivation) ==========\n");
    
    // 패스워드
    const char* password = "my_secure_password";
    size_t password_len = strlen(password);
    
    // 솔트 (salt)
    uint8_t salt[] = {0x41, 0x45, 0x53, 0x43, 0x00, 0x01, 0x02, 0x03};
    size_t salt_len = sizeof(salt);
    
    // 반복 횟수
    uint32_t iterations = 10000;
    
    // 출력 키 길이 (예: 32바이트 = 256비트 AES 키)
    size_t output_len = 32;
    uint8_t* derived_key = (uint8_t*)malloc(output_len);
    
    if (!derived_key) {
        printf("Memory allocation failed\n");
        return;
    }
    
    printf("Password: %s\n", password);
    print_hex("Salt", salt, salt_len);
    printf("Iterations: %u\n", iterations);
    printf("Output key length: %zu bytes\n", output_len);
    
    // Derive key using PBKDF2
    pbkdf2_sha512((const uint8_t*)password, password_len,
                   salt, salt_len,
                   iterations,
                   derived_key, output_len);
    
    print_hex("Derived Key", derived_key, output_len);
    
    // 파생된 키를 AES 키로 사용하는 예제
    printf("\n--- Using Derived Key as AES Key ---\n");
    AES_CTX aes_ctx;
    CRYPTO_STATUS status = AES_set_key(&aes_ctx, derived_key, 256);
    if (status == CRYPTO_SUCCESS) {
        printf("[OK] AES context initialization with derived key successful!\n");
    } else {
        printf("[FAIL] AES context initialization failed: %d\n", status);
    }
    
    free(derived_key);
}

// ==================== 예제 6: 난수 생성 ====================
void example_random_bytes(void) {
    printf("\n========== Example 6: Cryptographically Secure Random Number Generation ==========\n");
    
    // 다양한 길이의 난수 생성
    size_t lengths[] = {16, 32, 64};
    int num_lengths = sizeof(lengths) / sizeof(lengths[0]);
    
    for (int i = 0; i < num_lengths; i++) {
        size_t len = lengths[i];
        uint8_t* random_data = (uint8_t*)malloc(len);
        
        if (!random_data) {
            printf("Memory allocation failed\n");
            continue;
        }
        
        CRYPTO_STATUS status = crypto_random_bytes(random_data, len);
        if (status == CRYPTO_SUCCESS) {
            printf("%zu bytes random: ", len);
            print_hex("Random", random_data, len);
        } else {
            printf("Random generation failed (length: %zu): %d\n", len, status);
        }
        
        free(random_data);
    }
    
    // Nonce 생성 예제 (CTR 모드용)
    printf("\n--- Nonce Generation Example (for CTR mode) ---\n");
    uint8_t nonce[AES_BLOCK_SIZE];
    CRYPTO_STATUS status = crypto_random_bytes(nonce, AES_BLOCK_SIZE);
    if (status == CRYPTO_SUCCESS) {
        print_hex("Generated Nonce", nonce, AES_BLOCK_SIZE);
        printf("[OK] Nonce generation successful!\n");
    } else {
        printf("[FAIL] Nonce generation failed: %d\n", status);
    }
}

// ==================== 예제 7: 통합 예제 (실제 사용 시나리오) ====================
void example_integrated(void) {
    printf("\n========== Example 7: Integrated Example (Real-world Scenario) ==========\n");
    printf("Scenario: Derive keys from password, encrypt message, and verify integrity with HMAC\n\n");
    
    // 1. 패스워드로부터 키 파생
    const char* password = "user_password_123";
    uint8_t salt[16];
    crypto_random_bytes(salt, 16); // 랜덤 솔트 생성
    
    uint8_t encryption_key[32]; // 256비트 암호화 키
    uint8_t hmac_key[32];        // 256비트 HMAC 키
    
    pbkdf2_sha512((const uint8_t*)password, strlen(password),
                   salt, sizeof(salt), 10000,
                   encryption_key, sizeof(encryption_key));
    
    pbkdf2_sha512((const uint8_t*)password, strlen(password),
                   salt, sizeof(salt), 10001, // 다른 반복 횟수로 다른 키 생성
                   hmac_key, sizeof(hmac_key));
    
    printf("1. Key derivation completed\n");
    print_hex("   Encryption Key", encryption_key, sizeof(encryption_key));
    print_hex("   HMAC Key", hmac_key, sizeof(hmac_key));
    
    // 2. 메시지 암호화
    const char* message = "This is a secret message that needs to be encrypted.";
    size_t message_len = strlen(message);
    
    AES_CTX aes_ctx;
    AES_set_key(&aes_ctx, encryption_key, 256);
    
    uint8_t nonce[AES_BLOCK_SIZE];
    crypto_random_bytes(nonce, AES_BLOCK_SIZE);
    
    uint8_t* ciphertext = (uint8_t*)malloc(message_len);
    uint8_t nonce_copy[AES_BLOCK_SIZE];
    memcpy(nonce_copy, nonce, AES_BLOCK_SIZE);
    AES_CTR_crypt(&aes_ctx, (const uint8_t*)message, message_len, ciphertext, nonce_copy);
    
    printf("\n2. Message encryption completed\n");
    printf("   Original message: %s\n", message);
    print_hex("   Ciphertext", ciphertext, message_len);
    
    // 3. HMAC 계산 (암호문에 대한 무결성 검증)
    uint8_t mac[HMAC_SHA512_DIGEST_SIZE];
    hmac_sha512(hmac_key, sizeof(hmac_key), ciphertext, message_len, mac);
    
    printf("\n3. HMAC calculation completed\n");
    print_hex("   HMAC", mac, HMAC_SHA512_DIGEST_SIZE);
    
    // 4. 복호화 및 검증
    printf("\n4. Decryption and verification\n");
    uint8_t* decrypted = (uint8_t*)malloc(message_len + 1); // null terminator 공간 추가
    
    if (!decrypted) {
        printf("   [ERROR] Memory allocation failed\n");
        free(ciphertext);
        return;
    }
    
    // null terminator를 위해 초기화
    memset(decrypted, 0, message_len + 1);
    
    memcpy(nonce_copy, nonce, AES_BLOCK_SIZE);
    AES_CTR_crypt(&aes_ctx, ciphertext, message_len, decrypted, nonce_copy);
    
    // HMAC 재계산 및 검증
    uint8_t mac_verify[HMAC_SHA512_DIGEST_SIZE];
    hmac_sha512(hmac_key, sizeof(hmac_key), ciphertext, message_len, mac_verify);
    
    if (memcmp(mac, mac_verify, HMAC_SHA512_DIGEST_SIZE) == 0) {
        printf("   [OK] HMAC verification successful!\n");
        printf("   Decrypted message: %s\n", (char*)decrypted);
    } else {
        printf("   [FAIL] HMAC verification failed! (Data may have been tampered with)\n");
    }
    
    free(ciphertext);
    free(decrypted);
}

/***** 깃허브 주소 https://github.com/SWTEAM4/final_swproject *****/
// ==================== 메인 함수 ====================
int main(void) {
    printf("========================================\n");
    printf("Cryptography Library Sample Code\n");
    printf("========================================\n");
    
    // 각 예제 실행
    example_aes_block_encryption();
    example_aes_ctr_mode();
    example_sha512();
    example_hmac_sha512();
    example_pbkdf2();
    example_random_bytes();
    example_integrated();
    
    printf("\n========================================\n");
    printf("All examples completed!\n");
    printf("========================================\n");
    
    return 0;
}
