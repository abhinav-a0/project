#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <random>
#include <limits>
#include <map>
#include <iomanip>

using namespace std;

class HealthSimulator {
private:
    int day;
    int energyLevel;
    int happinessLevel;
    int stressLevel;
    std::string message;

    std::mt19937 rng;
    std::uniform_int_distribution<int> dist_event;

    std::map<std::string, int> activityCounts;

    int clamp(int value) {
        return std::max(0, std::min(100, value));
    }

    struct DailyEvent {
        std::string name;
        std::string (*effect)(HealthSimulator&);
    };

    static std::string unexpectedWorkloadEffect(HealthSimulator& sim) {
        sim.stressLevel = sim.clamp(sim.stressLevel + 20);
        sim.energyLevel = sim.clamp(sim.energyLevel - 15);
        return "Unexpected workload hit! Stress and energy took a dip.";
    }

    static std::string goodNewsEffect(HealthSimulator& sim) {
        sim.happinessLevel = sim.clamp(sim.happinessLevel + 25);
        sim.stressLevel = sim.clamp(sim.stressLevel - 5);
        return "Received some good news! Feeling much happier.";
    }

    static std::string minorIssueEffect(HealthSimulator& sim) {
        sim.stressLevel = sim.clamp(sim.stressLevel + 10);
        return "A minor issue popped up. Feeling a bit more stressed.";
    }

    static std::string relaxingEveningEffect(HealthSimulator& sim) {
        sim.stressLevel = sim.clamp(sim.stressLevel - 15);
        sim.happinessLevel = sim.clamp(sim.happinessLevel + 10);
        return "Had a relaxing evening. Stress eased off.";
    }

    static std::string badSleepEffect(HealthSimulator& sim) {
        sim.energyLevel = sim.clamp(sim.energyLevel - 20);
        sim.happinessLevel = sim.clamp(sim.happinessLevel - 10);
        return "Slept poorly. Energy and mood are low.";
    }

    static std::string noMajorEventEffect(HealthSimulator& sim) {
        return "A calm day. Nothing major happened.";
    }

    std::vector<DailyEvent> events;

public:
    HealthSimulator() : day(1), energyLevel(70), happinessLevel(60), stressLevel(40), message("Welcome to your Health Simulator! Manage your well-being.") {
        std::random_device rd;
        rng.seed(rd());

        events = {
            {"Unexpected Workload", unexpectedWorkloadEffect},
            {"Good News", goodNewsEffect},
            {"Minor Issue", minorIssueEffect},
            {"Relaxing Evening", relaxingEveningEffect},
            {"Bad Sleep", badSleepEffect},
            {"No Major Event", noMajorEventEffect}
        };
        dist_event = std::uniform_int_distribution<int>(0, events.size() - 1);
    }

    void exercise() {
        energyLevel = clamp(energyLevel + 15);
        happinessLevel = clamp(happinessLevel + 10);
        stressLevel = clamp(stressLevel - 10);
        message = "You exercised! Feeling more energetic and happy.";
        activityCounts["Exercise"]++;
    }

    void meditate() {
        stressLevel = clamp(stressLevel - 20);
        happinessLevel = clamp(happinessLevel + 5);
        message = "You meditated. A wave of calm washes over you.";
        activityCounts["Meditate"]++;
    }

    void eatHealthy() {
        energyLevel = clamp(energyLevel + 10);
        happinessLevel = clamp(happinessLevel + 5);
        message = "You ate a healthy meal. Feeling nourished.";
        activityCounts["Eat Healthy"]++;
    }

    void relax() {
        stressLevel = clamp(stressLevel - 15);
        energyLevel = clamp(energyLevel + 5);
        message = "You took time to relax. Stress levels are dropping.";
        activityCounts["Relax"]++;
    }

    void sleep() {
        energyLevel = clamp(energyLevel + 30);
        happinessLevel = clamp(happinessLevel + 5);
        stressLevel = clamp(stressLevel - 10);
        message = "You got good sleep. Ready for a new day!";
        activityCounts["Sleep"]++;
    }

    void newDay() {
        day++;
        int eventIndex = dist_event(rng);
        std::string eventMessage = events[eventIndex].effect(*this);

        if (stressLevel > 70) happinessLevel = clamp(happinessLevel - 5);
        if (energyLevel < 30) stressLevel = clamp(stressLevel + 5);
        if (happinessLevel < 40) energyLevel = clamp(energyLevel - 5);

        message = "Day " + std::to_string(day) + ": " + eventMessage;
    }

    void reset() {
        day = 1;
        energyLevel = 70;
        happinessLevel = 60;
        stressLevel = 40;
        message = "Simulator reset! Starting a new health journey.";
        activityCounts.clear();
    }

    void displayStats() const {
        std::cout << "\n======================================\n";
        std::cout << "           Health Simulator\n";
        std::cout << "======================================\n";
        std::cout << "Day: " << day << "\n";
        std::cout << std::left << std::setw(12) << "Energy:" << energyLevel << "/100\n";
        std::cout << std::left << std::setw(12) << "Happiness:" << happinessLevel << "/100\n";
        std::cout << std::left << std::setw(12) << "Stress:" << stressLevel << "/100\n";
        std::cout << "\nMessage: " << message << "\n";
        std::cout << "--------------------------------------\n";
    }

    void displayActivityStats() const {
        std::cout << "\n--- Activity Log ---\n";
        if (activityCounts.empty()) {
            std::cout << "No activities recorded yet.\n";
        } else {
            std::string mostFrequentActivity = "None";
            int maxCount = 0;

            for (const auto& pair : activityCounts) {
                std::cout << std::left << std::setw(15) << pair.first << ": " << pair.second << " times\n";
                if (pair.second > maxCount) {
                    maxCount = pair.second;
                    mostFrequentActivity = pair.first;
                }
            }
            std::cout << "--------------------\n";
            std::cout << "Most frequent activity: " << mostFrequentActivity << " (" << maxCount << " times)\n";
        }
        std::cout << "--------------------\n";
    }
};

int main() {
    HealthSimulator sim;

    int choice;

    do {
        sim.displayStats();

        std::cout << "\nChoose an action:\n";
        std::cout << "1. Exercise (Boosts Energy, Happiness; Reduces Stress)\n";
        std::cout << "2. Meditate (Reduces Stress; Boosts Happiness)\n";
        std::cout << "3. Eat Healthy (Boosts Energy, Happiness)\n";
        std::cout << "4. Relax (Reduces Stress; Boosts Energy)\n";
        std::cout << "5. Sleep (Major Energy Recovery; Reduces Stress)\n";
        std::cout << "6. Advance to New Day (Experience daily events and changes)\n";
        std::cout << "7. View Activity Stats (See what you've done most)\n";
        std::cout << "8. Reset Simulator (Start Fresh)\n";
        std::cout << "9. Exit Game\n";
        std::cout << "Enter your choice: ";

        while (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: sim.exercise(); break;
            case 2: sim.meditate(); break;
            case 3: sim.eatHealthy(); break;
            case 4: sim.relax(); break;
            case 5: sim.sleep(); break;
            case 6: sim.newDay(); break;
            case 7: sim.displayActivityStats(); break;
            case 8: sim.reset(); break;
            case 9: std::cout << "Exiting Health Simulator. Goodbye!\n"; break;
            default: std::cout << "Invalid choice. Please select a number from the menu.\n"; break;
        }
        std::cout << "\n";

    } while (choice != 9);

    return 0;
}

