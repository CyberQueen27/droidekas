#include <iostream>
#include <string>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <oath.h>
#include <chrono>
#include <thread>

// Initialize the MongoDB C++ driver instance
mongocxx::instance inst{};

//Connect to the MongoDB server using the client object:
mongocxx::client conn{mongocxx::uri{"mongodb://localhost:27017/local"}};

// Create a new database and collection:
auto db = conn["password_manager"];
auto collection = db["passwords"];

// Define a struct to represent the passwords:
struct Password {
    std::string website;
    std::string username;
    std::string password;
};

// Implement a function to add a new password to the database:
void add_password(const Password& password) {
    collection.insert_one(password);
    std::cout << "Password added successfully!" << std::endl;
}

// Implement a function to retrieve a password from the database:
Password get_password(const std::string& website) {
    auto result = collection.find_one(mongocxx::builder::stream::document{} << "website" << website << mongocxx::builder::stream::finalize);
    if (result) {
        Password password;
        password.website = result->view()["website"].get_utf8().value.to_string();
        password.username = result->view()["username"].get_utf8().value.to_string();
        password.password = result->view()["password"].get_utf8().value.to_string();
        return password;
    } else {
        throw std::runtime_error("Password not found!");
    }
}

// Implement a function to generate a TOTP code:
std::string generate_totp(const std::string& secret) {
    std::string code;

    auto now = std::chrono::system_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    uint8_t hash[SHA1_DIGEST_LENGTH];
    oath_hotp(secret.c_str(), secret.length(), seconds / 30, 6, OATH_HASH_SHA1, hash, sizeof(hash));

    code = std::to_string(oath_hotp_check_with_window(secret.c_str(), secret.length(), seconds / 30, 1, 6, OATH_HASH_SHA1, hash, sizeof(hash)));

    return code;
}

// Implement a main function to test the password manager with TOTP:
int main() {
    Password password1 {"google.com", "johndoe", "password123"};
    Password password2 {"facebook.com", "janedoe", "password456"};

    add_password(password1);
    add_password(password2);

    std::string secret_key = "MY_SECRET_KEY";

    while (true) {
        std::string code = generate_totp(secret_key);
        std::cout << "Enter TOTP code: ";
        std::string input_code;
        std::cin >> input_code;
        if (code == input_code) {
            Password retrieved_password = get_password("google.com")
            std::cout << "Username: " << retrieved_password.username << std::endl;
            std::cout << "Password: " << retrieved_password.password << std::endl;
        } else {
            std::cout << "Invalid TOTP code. Try again." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }

    return 0;
}

