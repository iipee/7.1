#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <fstream>

class Character {
private:
    std::string name;
    int health;
    int attack;
    int defense;

public:
    Character(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {
        if (health <= 0) {
            throw std::invalid_argument("Health must be positive");
        }
        std::cout << "Character " << name << " created!\n";
    }

    ~Character() {
        std::cout << "Character " << name << " destroyed!\n";
    }

    bool operator==(const Character& other) const {
        return name == other.name && health == other.health;
    }

    friend std::ostream& operator<<(std::ostream& os, const Character& character) {
        os << "Character: " << character.name << ", HP: " << character.health
           << ", Attack: " << character.attack << ", Defense: " << character.defense;
        return os;
    }

    void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health
                  << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    int getHealth() const { return health; }

    // Геттеры для сохранения
    std::string getName() const { return name; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
};

class Monster {
private:
    std::string name;
    int health;
    int attack;
    int defense;

public:
    Monster(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {
        if (health <= 0) {
            throw std::invalid_argument("Health must be positive");
        }
        std::cout << "Monster " << name << " created!\n";
    }

    ~Monster() {
        std::cout << "Monster " << name << " destroyed!\n";
    }

    void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health
                  << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    int getHealth() const { return health; }
};

class Weapon {
private:
    std::string name;
    int damage;
    int weight;

public:
    Weapon(const std::string& n, int d, int w)
        : name(n), damage(d), weight(w) {
        std::cout << "Weapon " << name << " created!\n";
    }

    ~Weapon() {
        std::cout << "Weapon " << name << " destroyed!\n";
    }

    void displayInfo() const {
        std::cout << "Weapon: " << name << ", Damage: " << damage
                  << ", Weight: " << weight << std::endl;
    }

    Weapon operator+(const Weapon& other) const {
        std::string newName = name + other.name;
        int newDamage = damage + other.damage;
        int newWeight = weight + other.weight;
        return Weapon(newName, newDamage, newWeight);
    }

    bool operator>(const Weapon& other) const {
        return damage > other.damage;
    }
};

class Inventory {
private:
    std::unique_ptr<std::string[]> items;
    size_t capacity;
    size_t size;

public:
    Inventory(size_t initialCapacity = 5)
        : capacity(initialCapacity), size(0) {
        items = std::unique_ptr<std::string[]>(new std::string[capacity]);
        std::cout << "Inventory created with capacity " << capacity << "!\n";
    }

    ~Inventory() {
        std::cout << "Inventory destroyed!\n";
    }

    void addItem(const std::string& item) {
        if (size >= capacity) {
            size_t newCapacity = capacity * 2;
            auto newItems = std::unique_ptr<std::string[]>(new std::string[newCapacity]);
            for (size_t i = 0; i < size; ++i) {
                newItems[i] = std::move(items[i]);
            }
            items = std::move(newItems);
            capacity = newCapacity;
            std::cout << "Inventory resized to capacity " << capacity << "!\n";
        }
        items[size] = item;
        ++size;
        std::cout << "Added item: " << item << "\n";
    }

    void displayInventory() const {
        if (size == 0) {
            std::cout << "Inventory is empty.\n";
            return;
        }
        std::cout << "Inventory contents (" << size << " items):\n";
        for (size_t i = 0; i < size; ++i) {
            std::cout << "- " << items[i] << "\n";
        }
    }
};

template <typename T>
class GameManager {
private:
    std::vector<T> entities;

public:
    void addEntity(const T& entity) {
        if (entity->getHealth() <= 0) {
            throw std::invalid_argument("Entity has invalid health");
        }
        entities.push_back(entity);
    }

    void displayAll() const {
        for (const auto& entity : entities) {
            entity->displayInfo();
        }
    }

    // Метод для доступа к entities для сохранения
    const std::vector<T>& getEntities() const { return entities; }
};

template <typename T>
class Queue {
private:
    std::vector<T> items;

public:
    Queue() {
        std::cout << "Queue created!\n";
    }

    ~Queue() {
        std::cout << "Queue destroyed!\n";
    }

    void push(const T& item) {
        items.push_back(item);
        std::cout << "Pushed to queue: " << item << "\n";
    }

    T pop() {
        if (items.empty()) {
            std::cout << "Queue is empty! Cannot pop.\n";
            throw std::runtime_error("Cannot pop from an empty queue");
        }
        T item = items.front();
        items.erase(items.begin());
        std::cout << "Popped from queue: " << item << "\n";
        return item;
    }

    bool empty() const {
        return items.empty();
    }
};

void saveToFile(const GameManager<Character*>& manager, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file for writing.");
    }
    const auto& entities = manager.getEntities();
    for (const auto& entity : entities) {
        file << entity->getName() << " " << entity->getHealth() << " "
             << entity->getAttack() << " " << entity->getDefense() << "\n";
    }
}

void loadFromFile(GameManager<Character*>& manager, const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading.");
    }
    std::string name;
    int health, attack, defense;
    while (file >> name >> health >> attack >> defense) {
        try {
            manager.addEntity(new Character(name, health, attack, defense));
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error loading entity: " << e.what() << std::endl;
        }
    }
}

int main() {
    try {
        // Создание нескольких персонажей
        GameManager<Character*> manager;
        manager.addEntity(new Character("Hero", 100, 20, 10));
        manager.addEntity(new Character("Warrior", 150, 25, 15));
        manager.addEntity(new Character("Mage", 80, 30, 5));

        std::cout << "\nInitial characters:\n";
        manager.displayAll();

        // Сохранение в файл
        saveToFile(manager, "game_save.txt");
        std::cout << "\nCharacters saved to game_save.txt\n";

        // Загрузка из файла
        GameManager<Character*> loadedManager;
        loadFromFile(loadedManager, "game_save.txt");
        std::cout << "\nLoaded characters:\n";
        loadedManager.displayAll();

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << "\nEnd of main() - objects will be destroyed:\n";
    return 0;
}