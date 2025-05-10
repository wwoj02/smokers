#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>

class Semaphore {
private:
    int count;
    std::mutex mtx;
    std::condition_variable cv; 

public:
    Semaphore(int initial_count) : count(initial_count) {}

    void acquire() {
        std::unique_lock<std::mutex> lock(mtx);
        while (count <= 0) {
            cv.wait(lock);
        }
        count--;
    }

    void release() {
        std::unique_lock<std::mutex> lock(mtx);
        count++;
        cv.notify_one();
    }
};

void displayMessage(const std::string& message, std::mutex& screen_mutex) { //komunikaty
    std::lock_guard<std::mutex> lock(screen_mutex);
    std::cout << message << std::endl;
}

void smokerProcess(int smoker_id, Semaphore& tampers, Semaphore& matchboxes, std::mutex& screen_mutex) {
    while (true) {
        displayMessage("Palacz " + std::to_string(smoker_id) + " zada ubijacza.", screen_mutex);
        tampers.acquire();

        displayMessage("Palacz " + std::to_string(smoker_id) + " ubija fajke.", screen_mutex);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        displayMessage("Palacz " + std::to_string(smoker_id) + " zwraca ubijacz.", screen_mutex);
        tampers.release();

        displayMessage("Palacz " + std::to_string(smoker_id) + " zada pudelka zapalek.", screen_mutex);
        matchboxes.acquire();

        displayMessage("Palacz " + std::to_string(smoker_id) + " zapala fajke.", screen_mutex);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        displayMessage("Palacz " + std::to_string(smoker_id) + " zwraca pudelko zapalek.", screen_mutex);
        matchboxes.release();

        displayMessage("Palacz " + std::to_string(smoker_id) + " pali fajke.", screen_mutex);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

int main() {
    const int num_smokers = 3;
    const int num_tampers = 2;
    const int num_matchboxes = 2;

    Semaphore tampers(num_tampers);
    Semaphore matchboxes(num_matchboxes);
    std::mutex screen_mutex;
    std::vector<std::thread> smoker_threads;

    for (int i = 0; i < num_smokers; ++i) {
        smoker_threads.emplace_back(smokerProcess, i + 1, std::ref(tampers), std::ref(matchboxes), std::ref(screen_mutex));
    }
    for (auto& thread : smoker_threads) {
        thread.join();
    }

    return 0;
}