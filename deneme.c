#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

// Basit bir XOR şifreleme fonksiyonu
void encrypt(char *data, size_t size, char key) {
    for (size_t i = 0; i < size; i++) {
        data[i] ^= key;
    }
}

// Basit bir XOR şifre çözme fonksiyonu
void decrypt(char *data, size_t size, char key) {
    encrypt(data, size, key); // XOR şifreleme ve çözme aynı işlemi kullanır
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Kullanim: %s <dosya>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Dosya acilamadi");
        return 1;
    }

    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        perror("Dosya durumu alinamadi");
        close(fd);
        return 1;
    }

    size_t file_size = file_stat.st_size;
    char *file_data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_data == MAP_FAILED) {
        perror("Dosya haritalandirma hatasi");
        close(fd);
        return 1;
    }

    close(fd);

    // Basit bir şifreleme anahtarı (gerçek uygulamada daha güvenli bir yöntem kullanılmalı)
    char encryption_key = 0x42;

    // Şifreleme
    encrypt(file_data, file_size, encryption_key);

    // Yeni dosya oluştur ve şifrelenmiş veriyi yaz
    const char *output_filename = "woody";
    int output_fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (output_fd == -1) {
        perror("Çikti dosyasi olusturulamadi");
        munmap(file_data, file_size);
        return 1;
    }

    if (write(output_fd, file_data, file_size) == -1) {
        perror("Çikti dosyaya yazilamadi");
        close(output_fd);
        munmap(file_data, file_size);
        return 1;
    }

    close(output_fd);
    munmap(file_data, file_size);

    printf("Dosya basariyla sifrelendi ve '%s' olarak kaydedildi.\n", output_filename);

    return 0;
}