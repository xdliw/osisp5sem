#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>

void readFilePart(const std::string& filename, std::vector<char>& buffer, size_t start, size_t end) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла!" << std::endl;
        return;
    }

    //Чтение из файла в буфер
    file.seekg(start);
    file.read(buffer.data() + start, end - start);
}

int main() {

    std::string filename;
    int numThreads;

    std::cout << "Введите имя файла: ";
    std::cin >> filename;
    std::cout << "Введите количество потоков: ";
    std::cin >> numThreads;

    // Открываем файл для получения его размера
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла!" << std::endl;
        return 1;
    }

    size_t fileSize = file.tellg();
    file.close();

    // Создаем общий буфер для хранения результата
    std::vector<char> buffer(fileSize);

    // Вычисляем размер части файла, которую будет читать каждый поток
    size_t partSize = fileSize / numThreads;

    // Создаем потоки и засекаем время
    std::vector<std::thread> threads;
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numThreads; ++i) {
        size_t start = i * partSize;
        size_t end = (i == numThreads - 1) ? fileSize : (i + 1) * partSize;
        threads.emplace_back(readFilePart, filename, std::ref(buffer), start, end);
    }

    // Дожидаемся завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    std::cout << "результат:\n" << std::string(buffer.begin(), buffer.end()) << std::endl;

    std::cout << "Чтение завершено. Время выполнения: " << duration << " мс" << std::endl;

    return 0;
}