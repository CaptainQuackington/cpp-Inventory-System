#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>

bool rightClickProcessed = false;

class Item {
public:
    sf::Texture inventoryTexture, equipTexture, removeTexture, cancelTexture;

    Item(float xpos, float ypos, const std::string& itemName)
        : xpos(xpos), ypos(ypos), itemName(itemName) {

        if (!inventoryTexture.loadFromFile("SystemAssets/ItemFrame.png") ||
            !equipTexture.loadFromFile("SystemAssets/Equip.png") ||
            !removeTexture.loadFromFile("SystemAssets/Delete.png") ||
            !cancelTexture.loadFromFile("SystemAssets/Cancel.png")) {
            std::cerr << "Failed to load textures." << std::endl;
        }
    }

    void draw(sf::RenderWindow& window) {
        sf::Sprite inventorySprite(inventoryTexture);
        inventorySprite.setPosition(xpos, ypos);
        window.draw(inventorySprite);

        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Failed to load font." << std::endl;
        }

        sf::Text text(itemName, font, 12);
        text.setFillColor(sf::Color::Black);
        text.setPosition(xpos + 15, ypos + 50);
        window.draw(text);
    }

    bool isPointInside(const sf::Vector2f& point) {
        float x = point.x;
        float y = point.y;
        return xpos <= x && x < xpos + 100 && ypos <= y && y < ypos + 100;
    }

    std::string inspecting(bool Lclicked, const sf::Vector2i& mouse_pos, sf::RenderWindow& window) {
        sf::Sprite equipSprite(equipTexture), removeSprite(removeTexture), cancelSprite(cancelTexture);

        // Adjust positions based on the cursor position
        equipSprite.setPosition(xpos + 5, ypos + 5);  // Adjusted positions
        removeSprite.setPosition(xpos + 5, ypos + 27);
        cancelSprite.setPosition(xpos + 5, ypos + 49);

        // Draw without clearing the window
        window.draw(equipSprite);
        window.draw(removeSprite);
        window.draw(cancelSprite);

        
        //window.display();  // Display immediately after drawing

        if (Lclicked) {
            if (mouse_pos.x >= equipSprite.getPosition().x && mouse_pos.x < equipSprite.getPosition().x + equipTexture.getSize().x &&
                mouse_pos.y >= equipSprite.getPosition().y && mouse_pos.y < equipSprite.getPosition().y + equipTexture.getSize().y) {
                return "Equipped";
            }
            else if (mouse_pos.x >= removeSprite.getPosition().x && mouse_pos.x < removeSprite.getPosition().x + removeTexture.getSize().x &&
                mouse_pos.y >= removeSprite.getPosition().y && mouse_pos.y < removeSprite.getPosition().y + removeTexture.getSize().y) {
                return "Delete";
            }
            else if (mouse_pos.x >= cancelSprite.getPosition().x && mouse_pos.x < cancelSprite.getPosition().x + cancelTexture.getSize().x &&
                mouse_pos.y >= cancelSprite.getPosition().y && mouse_pos.y < cancelSprite.getPosition().y + cancelTexture.getSize().y) {
                return "Cancel";
            }
        }

        return "";
    }




    std::string getName() const {
        return itemName;
    }

private:
    float xpos, ypos;
    std::string itemName;
};

void Inventory(const std::vector<std::string>& inventory, std::string& itemEquipped, int& money, sf::RenderWindow& window) {

    std::unordered_map<Item*, std::size_t> itemObjects;

    bool currentlyInspecting = false;
    Item* inspectingItem = nullptr;
    float xposTemp = 0, yposTemp = 0;

    for (std::size_t i = 0; i < inventory.size(); ++i) {
        float xpos = 215 - 15 + 125 * (i % 3);
        float ypos = 180 + 125 * (i / 3);
        Item* item = new Item(xpos, ypos, inventory[i]);
        itemObjects[item] = i;
    }

    
    

    while (window.isOpen()) {
        window.clear(sf::Color::Black);
        sf::Event event;



        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                // Reset rightClickProcessed for a new right-click event
                rightClickProcessed = false;

                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                for (auto& itemObject : itemObjects) {
                    if (itemObject.first->isPointInside(window.mapPixelToCoords(mousePos))) {
                        currentlyInspecting = true;
                        xposTemp = mousePos.x;
                        yposTemp = mousePos.y;
                        inspectingItem = itemObject.first;
                        break;
                    }
                }
            }
        }


       

        // Draw inventory UI
        sf::Texture inventoryButtonTexture, shopButtonTexture, inventorySpriteTexture;
        if (!inventoryButtonTexture.loadFromFile("SystemAssets/InventoryButton.png") ||
            !shopButtonTexture.loadFromFile("SystemAssets/ShopButton.png") ||
            !inventorySpriteTexture.loadFromFile("SystemAssets/InventorySprite.png")) {
            std::cerr << "Failed to load UI textures." << std::endl;
        }

        sf::Sprite inventoryButtonSprite(inventoryButtonTexture);
        inventoryButtonSprite.setPosition(600, 355);
        window.draw(inventoryButtonSprite);

       

        sf::Sprite inventorySpriteSprite(inventorySpriteTexture);
        inventorySpriteSprite.setPosition(150, 75);
        window.draw(inventorySpriteSprite);

        // Draw each item in the inventory
        for (auto& itemObject : itemObjects) {
            itemObject.first->draw(window);
        }


        for (auto& itemObject : itemObjects) {
            itemObject.first->draw(window);
        }

        // Draw cash text
        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Failed to load font." << std::endl;
        }

        sf::Text moneyText("Cash: " + std::to_string(money), font, 12);
        moneyText.setFillColor(sf::Color::Green);
        moneyText.setPosition(25, 25);
        window.draw(moneyText);

        sf::Text itemEquippedText("Item Equipped: " + itemEquipped, font, 12);
        itemEquippedText.setFillColor(sf::Color::Green);
        itemEquippedText.setPosition(600, 25);
        window.draw(itemEquippedText);

        // Handle item inspection (Equip, Delete, Cancel)
        if (currentlyInspecting) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            std::string option = inspectingItem->inspecting(sf::Mouse::isButtonPressed(sf::Mouse::Left), mousePos, window);
            if (option == "Equipped") {
                itemEquipped = inspectingItem->getName();
                currentlyInspecting = false;
            }
            else if (option == "Cancel") {
                currentlyInspecting = false;
            }
            else if (option == "Delete") {
                // Delete the item from both the inventory list and the item_objects map
                auto iter = itemObjects.find(inspectingItem);
                if (iter != itemObjects.end()) {
                    delete iter->first; // Free memory
                    itemObjects.erase(iter);
                }
                currentlyInspecting = false;

                Item* inspectingItem = nullptr;
                float xposTemp = 0, yposTemp = 0;

                
            }
        }

        window.display();
    }

    // Free memory
    for (auto& itemObject : itemObjects) {
        delete itemObject.first;
    }
}

void MainSystem() {
    // Initial inventory items
    std::vector<std::string> inventory = { "ExampleItem1", "ExampleItem2", "ExampleItem3", "ExampleItem4", "ExampleItem5", "ExampleItem6", "Item 2" };

    // Initial equipped
    std::string itemEquipped = "";

    // Initial money
    int money = 200;

    // SFML window as a global variable
    sf::RenderWindow window(sf::VideoMode(800, 800), "GUI");
    window.setFramerateLimit(60);

    // Run the inventory function
    Inventory(inventory, itemEquipped, money, window);
}